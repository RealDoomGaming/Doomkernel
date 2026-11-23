#include <video/vbe.h>
#include <stdint.h>
#include <fonts/fonts.h>

// these are the addresses in memory which we need
#define PDPT_ADDR   0x2000UL
#define FB_PD_ADDR  0x4000UL

// 1 GiB is 1,073,741,824 bytes
const uint64_t one_gib = 1073741824;
// 2 MiB is 2,097,152 bytes
const uint64_t two_mib = 2097152;

uint16_t compute_pdpt(uint32_t framebuffer_addr) {
    // here we compute the pdpt index by dividing the address by 1 GiB
    return (framebuffer_addr / one_gib) % 512;
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

static void reload_cr3() {
    uint64_t cr3;
    __asm__ volatile ("mov %%cr3, %0\n\t mov %0, %%cr3" : "=r"(cr3) : : "memory");
}

void fill_dir_entries(uint16_t pdpt_entrie_idx, uint16_t pd_first_entry, vbe_mode_info_t *vbe) {
    // these are the two pointers to the address of the pd and pdpt
    volatile uint64_t *pdpt = (volatile uint64_t *)PDPT_ADDR;
    volatile uint64_t *pd   = (volatile uint64_t *)FB_PD_ADDR;

    // we need these flags later
    uint8_t present_flag = 1;
    uint8_t writeable_flag = 2;
    uint8_t page_size_flag = 128;
    // so the cpu doesnt chache video memory we need these two flags
    uint8_t pwt_flag = 8;
    uint8_t pcd_flag = 16;
    // we combine all flags here
    uint8_t flags = present_flag | writeable_flag | page_size_flag | pwt_flag | pcd_flag;

    // the amount of pages we need to write to
    uint64_t amount_of_pages = get_pages_amount(vbe->height, vbe->pitch, vbe->framebuffer);
    // this grows by 2 MiB per step
    uint64_t addr = vbe->framebuffer - (vbe->framebuffer % two_mib);

    for (uint64_t i = pd_first_entry; i < pd_first_entry + amount_of_pages; i++) {
        // then we write to the pd entry
        pd[i] = addr | flags;

        // here we add 2 MiB to the addr
        addr += two_mib;
    }

    // then we only need to do one assign to the pdpt entry idx
    pdpt[pdpt_entrie_idx] = FB_PD_ADDR | present_flag | writeable_flag;

    // then we reload the cr3
    reload_cr3();
}

void put_pixel(const vbe_mode_info_t *vbe, uint32_t x, uint32_t y, uint32_t color) {
    // Skip anything off-screen so we never touch unmapped memory.
    if (x >= vbe->width || y >= vbe->height) {
        return;
    }

    // Byte address is base + row * pitch + column * 4 bytes per pixel.
    // The (uintptr_t) casts make the math 64-bit.
    uintptr_t addr = (uintptr_t)vbe->framebuffer
                   + (uintptr_t)y * vbe->pitch
                   + (uintptr_t)x * 4;

    *(volatile uint32_t *)addr = color;
}

void draw_character(const glyph *character, const vbe_mode_info_t *vbe, uint32_t x, uint32_t y, uint32_t font_color, uint32_t background_color) {
    for (uint32_t row = 0; row < 8; row++) {
        for (uint32_t col = 0; col < 8; col++) {
            uint8_t shifted = character->data[row] >> col;
            uint8_t bit = shifted & 1;

            if (bit != 0) {
                // write this pixel
                put_pixel(vbe, x + col, y + row, font_color);
            }
        }
    }
}

void fill_rect(const vbe_mode_info_t *vbe, uint32_t x, uint32_t y,
                uint32_t w, uint32_t h, uint32_t color) {
    for (uint32_t row = y; row < y + h; row++) {        // outer loop rows
        for (uint32_t col = x; col < x + w; col++) {    // inner loop columns
            put_pixel(vbe, col, row, color);
        }
    }
}