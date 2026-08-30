#include <fs/fs.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

static initrd_entry_t  *fs_entries;
static uint8_t *fs_data;
static uint32_t fs_file_count;

void fs_init(uint64_t initrd_addr) {
    // this function basically inits the whole file system stuff
    // it will also only mount if the first header has the correct magic
    // and after that check we set the amount of files we have
    // then we set where the entries start
    // and also set where the raw file bytes start

    initrd_header_t *initrd_header = (initrd_header_t *) initrd_addr;

    if (memcmp(initrd_header->magic, "DSF1", 4) != 0) {
        // the magic wasnt the same so we dont mount the file system
        printf("[fs] invalid magic for initrd so the fs isnt going to be mounted\n");
        fs_file_count = 0;
        
        return;
    }

    fs_file_count = initrd_header->file_count;
    fs_entries = (initrd_entry_t *)(initrd_addr + sizeof(initrd_header_t));
    fs_data = (uint8_t *)(fs_entries + fs_file_count);
}

initrd_entry_t *fs_read(char *name) {
    // in this function we just loop through all files until we find one
    // which has the matching name
    
    uint32_t count = 0;
    initrd_entry_t *current = fs_entries;

    while (count < fs_file_count) {
        if (memcmp(current->name, name, 32) == 0) {
            return current;
        }

        count++;
        current++;
    }

    return NULL;
}

void *fs_get_data(initrd_entry_t *entry) {
    return fs_data + entry->offset;
}