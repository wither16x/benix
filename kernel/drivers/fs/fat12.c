/*
    fat12.c

    Implementations of the functions in fat12.h.
*/

#include "drivers/fs/fat12.h"
#include "drivers/ata.h"
#include "klib/memory.h"
#include "klib/string.h"
#include "klib/null.h"
#include "klib/bool.h"
#include "klib/types.h"

static struct DriverFS_FAT12 fs;

// PRIVATE FUNCTIONS
// =============================================================================
static void format_filename(const string input, string out) {
    memset(out, ' ', FILENAME_CHARS);

    const string dot = strchr(input, '.');
    size_t name_len = dot ? (size_t)(dot - input) : strlen(input);
    size_t ext_len  = dot ? strlen((string)dot + 1) : 0;

    if (name_len > 8) name_len = NAME_CHARS;
    if (ext_len  > 3) ext_len  = EXTENSION_CHARS;

    for (size_t i = 0; i < name_len; i++) {
        out[i] = toupper(input[i]);
    }

    for (size_t i = 0; i < ext_len; i++) {
        out[8 + i] = toupper(dot[1 + i]);
    }
}

static u16 find_free_cluster(void) {
    u8 sector[SECTOR_SIZE];
    u16 val, cluster_val;
    u32 fat_size = fs.bpb.fat_size;
    u32 clusters = fs.bpb.total_sectors16 / fs.bpb.sectors_per_cluster;
    u32 fat_offset, sectorno, offset;

    for (u32 cluster = 2; cluster < clusters; cluster++) {
        fat_offset = cluster + cluster / 2;
        sectorno = fs.fat_start_sector + (fat_offset / SECTOR_SIZE);
        offset = fat_offset % SECTOR_SIZE;

        get_driver_ata()->read(sectorno, (u16*)sector);

        val = *(u16*)(sector + offset);
        cluster_val = (cluster & 1) ? (val >> 4) & 0xfff : val & 0xfff;

        if (cluster_val == 0) {
            return cluster;
        }
    }

    return 0;
}

static u16 get_next_cluster(u16 cluster) {
    u32 fat_offset = cluster + cluster / 2;
    u8 sector[SECTOR_SIZE];
    u32 sector_number = fs.fat_start_sector + (fat_offset / fs.bpb.bytes_per_sector);
    u32 offset = fat_offset % fs.bpb.bytes_per_sector;
    u16 next_cluster, value;

    get_driver_ata()->read(sector_number, (u16*)sector);

    value = *(u16*)(sector + offset);
    next_cluster = (cluster & 1) ? (value >> 4) & 0xfff : value & 0xfff;

    return next_cluster;
}

static void set_cluster_end_of_chain(u16 cluster) {
    u8 sector[SECTOR_SIZE];
    u32 fat_offset = cluster + cluster / 2;
    u32 sector_number = fs.fat_start_sector + (fat_offset / fs.bpb.bytes_per_sector);
    u32 offset = fat_offset % SECTOR_SIZE;

    get_driver_ata()->read(sector_number, (u16*)sector);

    u16 value = *(u16*)(sector + offset);
    if (cluster & 1) {
        value = (value & 0xf) | (0xfff << 4);
    } else {
        value = (value & 0xf000) | 0xfff;
    }

    *(u16*)(sector + offset) = value;
    get_driver_ata()->write(sector_number, (u16*)sector);
}

static void set_cluster_next(u16 cluster, u16 next) {
    u8 sector[SECTOR_SIZE];
    u16 value;
    u32 fat_offset = cluster + cluster / 2;
    u32 sectorno = fs.fat_start_sector + (fat_offset / fs.bpb.bytes_per_sector);
    u32 offset = fat_offset % fs.bpb.bytes_per_sector;

    get_driver_ata()->read(sectorno, (u16*)sector);
    value = *(u16*)(sector + offset);

    if (cluster & 1) {
        value = (value & 0xf) | (next << 4);
    } else {
        value = (value & 0xf000) | (next & 0xfff);
    }

    *(u16*)(sector + offset) = value;
    get_driver_ata()->write(sectorno, (u16*)sector);
}

static void read_cluster(u16 cluster, u8* buffer) {
    u32 sector1 = fs.data_start_sector + (cluster - 2) * fs.bpb.sectors_per_cluster;

    for (u8 i = 0; i < fs.bpb.sectors_per_cluster; i++) {
        get_driver_ata()->read(sector1 + i, (u16*)(buffer + i * fs.bpb.bytes_per_sector));
    }
}

static void free_cluster_chain(u16 cluster) {
    u16 next;

    while (cluster >= 2 && cluster < END_OF_CHAIN) {
        next = get_next_cluster(cluster);
        set_cluster_next(cluster, 0);
        cluster = next;
    }
}

static void dir_open(bool is_root, struct FAT12_DirectoryIterator* iter, u16 first_cluster) {
    iter->is_root = is_root;
    iter->current_cluster = first_cluster;

    if (is_root)
        iter->sector = fs.rootdir_start_sector;
    else
        iter->sector = fs.data_start_sector + (first_cluster - 2) * fs.bpb.sectors_per_cluster;

    iter->offset = 0;
    get_driver_ata()->read(iter->sector, (u16*)iter->sector_buffer);
}

static bool dir_advance(struct FAT12_DirectoryIterator* iter) {
    u32 max_sectors, rel;

    iter->offset += ENTRY_SIZE;
    if (iter->offset < SECTOR_SIZE) {
        return true;
    }

    iter->offset = 0;
    iter->sector++;

    if (iter->is_root) {
        max_sectors = ((fs.bpb.rootdir_entry_count * ENTRY_SIZE) + SECTOR_SIZE - 1) / SECTOR_SIZE;
        if (iter->sector >= fs.rootdir_start_sector + max_sectors) {
            return false;
        }
    } else {
        rel = iter->sector - (fs.data_start_sector + (iter->current_cluster - 2) * fs.bpb.sectors_per_cluster);
        if (rel >= fs.bpb.sectors_per_cluster) {
            iter->current_cluster = get_next_cluster(iter->current_cluster);
            if (iter->current_cluster < 2 || iter->current_cluster >= END_OF_CHAIN) {
                return false;
            }
            iter->sector = fs.data_start_sector + (iter->current_cluster - 2) * fs.bpb.sectors_per_cluster;
        }
    }

    get_driver_ata()->read(iter->sector, (u16*)iter->sector_buffer);

    return true;
}

static bool dir_next(struct FAT12_DirectoryIterator* iter, struct FAT12_DirectoryEntry* out) {
    while (true) {
        struct FAT12_DirectoryEntry* e = (struct FAT12_DirectoryEntry*)(iter->sector_buffer + iter->offset);
        
        if (e->name[0] == 0) {
            return false;
        }
        if (e->name[0] != ATTR_REMOVED && (e->attributes & ATTR_LFN) != ATTR_LFN) {
            *out = *e;
            dir_advance(iter);
            return true;
        }

        if (!dir_advance(iter)) {
            return false;
        }
    }
}

static bool find_entry(bool is_root, u16 cluster, const string formatted, struct FAT12_DirectoryEntry* out) {
    struct FAT12_DirectoryIterator iter;
    struct FAT12_DirectoryEntry e;

    dir_open(is_root, &iter, cluster);
    
    while (dir_next(&iter, &e)) {
        if (memcmp(e.name, formatted, FILENAME_CHARS) == 0) {
            *out = e;
            return true;
        }
    }
    return false;
}

static bool add_entry(bool is_root, u16 cluster, struct FAT12_DirectoryEntry* entry) {
    struct FAT12_DirectoryIterator iter;
    struct FAT12_DirectoryEntry* e;
    dir_open(is_root, &iter, cluster);

    do {
        e = (struct FAT12_DirectoryEntry*)(iter.sector_buffer + iter.offset);
        if (e->name[0] == 0 || e->name[0] == ATTR_REMOVED) {
            memmove(e, entry, sizeof(struct FAT12_DirectoryEntry));
            get_driver_ata()->write(iter.sector, (u16*)iter.sector_buffer);
            return true;
        }
    } while (dir_advance(&iter));

    return false;
}

static bool resolve_path(const string path, struct FAT12_DirectoryEntry* out) {
    bool is_root = true;
    u16 cluster = 0;
    char part[12];
    string p = path;
    string q;

    while (*p) {
        q = part;
        while (*p && *p != PATH_SEPARATOR) {
            *q++ = *p++;
        }
        *q = 0;
        if (*p == PATH_SEPARATOR) {
            p++;
        }

        char formatted[FILENAME_CHARS];
        format_filename(part, formatted);

        if (!find_entry(is_root, cluster, formatted, out)) {
            return false;
        }

        is_root = false;
        cluster = out->first_cluster_low;
    }

    return true;
}

static bool resolve_path_handle(const string path, struct FAT12_PathHandle* out) {
    bool is_root = true;
    bool found = false;
    u16 cluster = 0;
    char part[12];
    char formatted[FILENAME_CHARS];
    string p = path;
    string q;
    struct FAT12_DirectoryIterator iterator;
    struct FAT12_DirectoryEntry entry;

    while (*p) {
        q = part;

        while (*p && *p != PATH_SEPARATOR) {
            *q++ = *p++;
        }
        *q = 0;
        if (*p == PATH_SEPARATOR) {
            p++;
        }

        format_filename(part, formatted);
        dir_open(is_root, &iterator, cluster);

        while (dir_next(&iterator, &entry)) {
            if (!memcmp(entry.name, formatted, FILENAME_CHARS)) {
                out->entry = entry;
                out->iterator = iterator;
                out->iterator.offset -= ENTRY_SIZE;
                is_root = false;
                cluster = entry.first_cluster_low;
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }

    return true;
}

static i32 read_file_stream(u16 cluster, u8* buffer, u32 bytes) {
    u32 total = 0;
    u32 cluster_size = fs.bpb.sectors_per_cluster * fs.bpb.bytes_per_sector;
    u32 base_sector, remaining, to_copy;
    u8 sector[SECTOR_SIZE];

    while (cluster >= 2 && cluster < END_OF_CHAIN && total < bytes) {
        base_sector = fs.data_start_sector + (cluster - 2) * fs.bpb.sectors_per_cluster;

        for (u32 s = 0; s < fs.bpb.sectors_per_cluster && total < bytes; s++) {
            get_driver_ata()->read(base_sector + s, (u16*)sector);

            remaining = bytes - total;
            to_copy = remaining < SECTOR_SIZE ? remaining : SECTOR_SIZE;

            memmove(buffer + total, sector, to_copy);
            total += to_copy;
        }

        cluster = get_next_cluster(cluster);
    }
    return total;
}

static i32 write_file_stream(u16* first_cluster, const u8* buffer, u32 bytes) {
    u8 sector[SECTOR_SIZE];
    u32 cluster_size = fs.bpb.sectors_per_cluster * fs.bpb.bytes_per_sector;
    u32 written = 0;
    u16 previous = 0;
    u16 current = 0;
    u16 free_cluster;
    u32 base, to_copy;

    while (written < bytes) {
        free_cluster = find_free_cluster();
        if (!free_cluster) {
            return -1;
        }

        set_cluster_end_of_chain(free_cluster);

        if (previous) {
            set_cluster_next(previous, free_cluster);
        } else {
            *first_cluster = free_cluster;
        }

        base = fs.data_start_sector + (free_cluster - 2) * fs.bpb.sectors_per_cluster;

        for (u32 s = 0; s < fs.bpb.sectors_per_cluster && written < bytes; s++) {
            to_copy = bytes - written;
            if (to_copy > SECTOR_SIZE) {
                to_copy = SECTOR_SIZE;
            }
            memset(sector, 0, SECTOR_SIZE);
            memmove(sector, (voidptr)buffer + written, to_copy);
            get_driver_ata()->write(base + s, (u16*)sector);
            written += to_copy;
        }
        previous = free_cluster;
        current = free_cluster;
    }

    return written;
}

// PUBLIC FUNCTIONS
// =============================================================================

/*
    -1: file not found
    -2: is a directory
*/
static i32 read_file(const string filename, u8* buffer, u32 buffer_size) {
    struct FAT12_DirectoryEntry entry;

    if (!resolve_path(filename, &entry)) {
        return -1;
    }
    if (entry.attributes & ATTR_DIRECTORY) {
        return -2;
    }

    u32 to_read = entry.file_size;
    if (to_read > buffer_size) {
        to_read = buffer_size;
    }

    u32 read = read_file_stream(entry.first_cluster_low, buffer, to_read);
    return (i32)read;
}

/*
    0: success
    -1: file not found
    -2: is a directory
    -3: failed to write
*/
static i32 write_file(const string filename, const u8* buffer, u32 buffer_size) {
    struct FAT12_PathHandle handle;
    u16 first_cluster = 0;
    u8 sector[SECTOR_SIZE];

    if (!resolve_path_handle(filename, &handle)) {
        return -1;
    }
    if (handle.entry.attributes & ATTR_DIRECTORY) {
        return -2;
    }
    if (handle.entry.first_cluster_low) {
        free_cluster_chain(handle.entry.first_cluster_low);
    }
    if (buffer_size) {
        if (write_file_stream(&first_cluster, buffer, buffer_size) < 0) {
            return -3;
        }
    }

    handle.entry.first_cluster_low = first_cluster;
    handle.entry.file_size = buffer_size;

    get_driver_ata()->read(handle.iterator.sector, (u16*)sector);
    memmove(sector + handle.iterator.offset, &handle.entry, sizeof(handle.entry));
    get_driver_ata()->write(handle.iterator.sector, (u16*)sector);

    return 0;
}

/*
    0: success
    -1: failed to resolve parent path
    -2: parent path does not lead to a directory
    -3: file already exists
    -4: no free cluster found
    -5: failed to add entry
*/
static i32 create_file(const string filename) {
    char parent[256], file[256], formatted[FILENAME_CHARS];
    string p = strchr(filename, PATH_SEPARATOR);
    struct FAT12_DirectoryEntry dir_parent, new_entry;
    bool is_root;
    u16 cluster = 0;
    u16 free_cluster;

    if (p) {
        i32 length = p - filename;
        strncpy(parent, filename, length);
        parent[length] = 0;
        strcpy(file, p + 1);
    } else {
        strcpy(parent, DIR_ROOT);
        strcpy(file, filename);
    }

    is_root = strcmp(parent, DIR_ROOT) == 0;
    if (!is_root) {
        if (!resolve_path(parent, &dir_parent)) {
            return -1;
        }
        cluster = dir_parent.first_cluster_low;
        if (!(dir_parent.attributes & ATTR_DIRECTORY)) {
            return -2;
        }
    }

    format_filename(file, formatted);
    if (find_entry(is_root, cluster, formatted, &new_entry)) {
        return -3;
    }

    free_cluster = find_free_cluster();
    if (free_cluster == 0) {
        return -4;
    }

    memset(&new_entry, 0, sizeof(new_entry));
    memmove(new_entry.name, formatted, FILENAME_CHARS);
    new_entry.attributes = 0;
    new_entry.first_cluster_low = free_cluster;
    new_entry.file_size = 0;

    if (!add_entry(is_root, cluster, &new_entry)) {
        return -5;
    }

    set_cluster_end_of_chain(free_cluster);

    return 0;
}

/*
    0: success
    -1: directory not found
    -2: is not a directory
*/
static i32 read_dir(const string path, string buffer) {
    struct FAT12_DirectoryEntry dir;
    struct FAT12_DirectoryIterator iter;
    struct FAT12_DirectoryEntry entry;
    u32 out;

    bool is_root = false;
    u16 cluster = 0;

    if (strcmp(path, DIR_ROOT) == 0) {
        is_root = true;
    } else {
        if (!resolve_path(path, &dir)) {
            return -1;
        }
        if (!(dir.attributes & ATTR_DIRECTORY)) {
            return -2;
        }
        cluster = dir.first_cluster_low;
    }

    dir_open(is_root, &iter, cluster);

    out = 0;
    while (dir_next(&iter, &entry)) {
        if (entry.name[0] == ATTR_REMOVED) {
            continue;
        }

        for (u32 i = 0; i < FILENAME_CHARS; i++) {
            buffer[out++] = entry.name[i];
        }
        buffer[out++] = '\n';
    }

    buffer[out] = '\0';
    return (i32)out;
}

static i32 lookup(const string filename) {
    struct FAT12_DirectoryEntry entry;
    return resolve_path(filename, &entry) ? 1 : 0;
}

void init_fsdriver_fat12(void) {
    fs.read_file = read_file;
    fs.write_file = write_file;
    fs.create_file = create_file;
    fs.read_dir = read_dir;
    fs.lookup = lookup;

    u8 sector[SECTOR_SIZE];
    get_driver_ata()->read(0, (u16*)sector);
    memcpy(&fs.bpb, sector, sizeof(fs.bpb));

    fs.fat_start_sector = fs.bpb.reserved_sector_count;
    fs.rootdir_start_sector = fs.fat_start_sector + fs.bpb.number_of_fats * fs.bpb.fat_size;
    fs.data_start_sector = fs.rootdir_start_sector + ((fs.bpb.rootdir_entry_count * 32) + (fs.bpb.bytes_per_sector - 1)) / fs.bpb.bytes_per_sector;
}

struct DriverFS_FAT12* get_fsdriver_fat12(void) {
    return &fs;
}