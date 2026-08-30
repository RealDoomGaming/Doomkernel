#ifndef _FS_H
#define _FS_H

#include <stdint.h>

typedef struct {
    char magic[4];
    uint32_t file_count;
} initrd_header_t;

typedef struct {
    char name[32];
    uint32_t offset;
    uint32_t size;
} initrd_entry_t;

void fs_init(uint64_t);
uint64_t fs_read(char[32]);

#endif