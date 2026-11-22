#include <video/vbe.h>
#include <stdint.h>

// these are the addresses in memory which we need
#define PDPT_ADDR   0x2000UL
#define FB_PD_ADDR  0x4000UL

// 1 GiB is 1,073,741,824 bytes
const uint64_t one_gib = 1073741824;
// 2 MiB is 2,097,152 bytes
const uint64_t two_mib = 2097152;

uint16_t compute_pdpt(uint32_t framebuffer_addr) {
    // here we compute the pdpt index by dividing the address by 1 GiB
    return (addr / one_gib) % 512;
}

uint16_t compute_dir(uint32_t framebuffer_addr) {
    // in this function we compute the directory index which we get by dividing the addr by 2 MiB and then doing the modulo 512
    return (framebuffer_addr / two_mib) % 512;
}

uint64_t get_pages_amount(uint16_t height, uint16_t pitch, uint32_t framebuffer_addr) {
    uint64_t size = (uint64_t)height * (uint64_t)pitch;
    uint64_t offset = framebuffer_addr % two_mib;

    // here we just calculate how many pages we will have and since normally it would round down we 
    // add 2 MiB to the size so it rounds up instead
    // we also add an offset in case there are already some byte in the first page
    return (offset + size + two_mib - 1) / two_mib;
}

void fill_dir_entries(uint16_t pdpt_entrie_idx, uint16_t pd_first_entry, vbe_mode_info_t *vbe) {
    // these are the two pointers to the address of the pd and pdpt
    volatile uint64_t *pdpt = (volatile uint64_t *)PDPT_ADDR;
    volatile uint64_t *pd   = (volatile uint64_t *)FB_PD_ADDR;

    // we need these flags later
    uint8_t present_flag = 1;
    uint8_t writeable_flag = 2;

    // the amount of pages we need to write to
    uint64_t amount_of_pages = get_pages_amount(vbe->height, vbe->pitch, vbe->framebuffer);
    // this grows by 2 MiB per step
    uint64_t addr = vbe->framebuffer_addr % two_mib;

    for (uint64_t i = pd_first_entry; i < pd_first_entry + amount_of_pages; i++) {
        // then we write to the pd entry
        pd[i] = addr | present_flag | writeable_flag;

        // here we add 2 MiB to the addr
        addr += two_mib;
    }

    // then we only need to do one assign to the pdpt entry idx
    pdpt[pdpt_entrie_idx] = FB_PD_ADDR | present_flag | writeable_flag;
}