#include <fs/fs.h>
#include <stdint.h>

void fs_init(uint64_t initrd_addr) {
    // this function basically inits the whole file system stuff
    // it will also only mount if the first header has the correct magic
    // and after that check we set the amount of files we have
    // then we set where the entries start
    // and also set where the raw file bytes start

    
}

uint64_t fs_read(char[32] name) {

}