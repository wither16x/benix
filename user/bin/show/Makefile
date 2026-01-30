NASM        := nasm
CC			:= gcc
LD          := ld

ASMFLAGS    := -f elf32
CFLAGS		:= -m32 -ffreestanding -nostdlib -fno-pic -fno-pie -mno-sse -mno-mmx -mno-sse2 -fno-tree-vectorize \
				 -g -c -I. -I../../../benlibc
LDFLAGS     := -m elf_i386 -Ttext 0x400000 -entry main --oformat binary

SRC_DIR     := .
OBJ_DIR     := ../../../obj/user
TARGET      := ../../../binaries/user/bin/show

ASM_FILES   := $(shell find $(SRC_DIR) -name '*.asm')
C_FILES	:= $(shell find $(SRC_DIR) -name '*.c')

ASM_OBJ := $(patsubst $(SRC_DIR)/%.asm,$(OBJ_DIR)/%.o,$(ASM_FILES))
C_OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_FILES))

OBJ_FILES := $(ASM_OBJ) $(C_OBJ)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(LD) $(LDFLAGS) -o $@ $^ ../../../obj/benlibc/*.o

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.asm
	@mkdir -p $(dir $@)
	$(NASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)