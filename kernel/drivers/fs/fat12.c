/*
    fat12.c

    Implementations of the functions in fat12.h.
*/

#include "drivers/fs/fat12.h"
#include "drivers/ata.h"
#include "klib/logging.h"
#include "klib/memory.h"
#include "klib/string.h"
#include "klib/null.h"
#include "klib/bool.h"
#include "klib/types.h"

static struct DriverFS_FAT12 fs;

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

static u16 get_next_cluster(u16 cluster) {
    u32 fat_offset = cluster + cluster / 2;
    u8 sector[SECTOR_SIZE];
    u32 sector_number = fs.fat_start_sector + (fat_offset / fs.bpb.bytes_per_sector);
    u32 offset = fat_offset % fs.bpb.bytes_per_sector;
    u16 next_cluster;

    get_driver_ata()->read(sector_number, (u16*)sector);

    u16 value = *(u16*)(sector + offset);
    if (cluster & 0x1) {
        next_cluster = value >> 4;
    } else {
        next_cluster = value & 0xfff;
    }

    return next_cluster;
}

static void read_cluster(u16 cluster, u8* buffer) {
    u32 sector1 = fs.data_start_sector + (cluster - 2) * fs.bpb.sectors_per_cluster;

    for (u8 i = 0; i < fs.bpb.sectors_per_cluster; i++) {
        get_driver_ata()->read(sector1 + i, (u16*)(buffer + i * fs.bpb.bytes_per_sector));
    }
}

bool find_entry(bool is_root, u16 cluster, const string formatted, struct FAT12_DirectoryEntry* out) {
    struct FAT12_DirectoryIterator iter;
    struct FAT12_DirectoryEntry e;

    fs.dir_open(is_root, &iter, cluster);
    
    while (fs.dir_next(&iter, &e)) {
        if (memcmp(e.name, formatted, FILENAME_CHARS) == 0) {
            *out = e;
            return true;
        }
    }
    return false;
}

bool resolve_path(const string path, struct FAT12_DirectoryEntry* out) {
    bool is_root = true;
    u16 cluster = 0;

    char part[12];
    string p = path;

    while (*p) {
        string q = part;
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
            if (iter->current_cluster >= END_OF_CHAIN) {
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

static i32 read_file_stream(u16 cluster, u8* buffer, u32 bytes) {
    u32 total = 0;
    u32 cluster_size = fs.bpb.sectors_per_cluster * fs.bpb.bytes_per_sector;
    u8 tmp[cluster_size];

    while (cluster < END_OF_CHAIN && total < bytes) {
        u32 to_copy = bytes - total;
        if (to_copy > cluster_size)
            to_copy = cluster_size;

        read_cluster(cluster, tmp);
        memcpy(buffer + total, tmp, to_copy);

        total += to_copy;
        cluster = get_next_cluster(cluster);
    }
    return total;
}

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

    fs.dir_open(is_root, &iter, cluster);

    out = 0;
    while (fs.dir_next(&iter, &entry)) {
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
    fs.get_next_cluster = get_next_cluster;
    fs.read_cluster = read_cluster;
    fs.find_entry = find_entry;
    fs.resolve_path = resolve_path;
    fs.dir_open = dir_open;
    fs.dir_advance = dir_advance;
    fs.dir_next = dir_next;
    fs.read_file_stream = read_file_stream;
    fs.read_file = read_file;
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