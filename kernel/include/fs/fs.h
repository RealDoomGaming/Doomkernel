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
initrd_entry_t *fs_read(char*);
void *fs_get_data(initrd_entry_t *entry);

#endif