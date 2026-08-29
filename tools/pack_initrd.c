// what this does: 
// it basically glues together files into a big blob (a bin file) with a table at the front saying where each file starts and how big it is
// this then becomes the file system
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#pragma pack(push, 1)
typedef struct {
    char magic[4];       
    uint32_t file_count;
} initrd_header_t;

typedef struct {
    char name[32];
    uint32_t offset;  
    uint32_t size;
} initrd_entry_t;
#pragma pack(pop)

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "usage: %s output.bin file1 [file2 ...]\n", argv[0]);
        return 1;
    }

    const char *out_path = argv[1];
    int file_count = argc - 2;

    FILE *out = fopen(out_path, "wb");
    if (!out) {
        perror("fopen output");
        return 1;
    }

    initrd_header_t header;
    memcpy(header.magic, "DFS1", 4);
    header.file_count = (uint32_t)file_count;
    fwrite(&header, sizeof(header), 1, out);

    long *sizes = malloc(sizeof(long) * file_count);
    uint32_t running_offset = 0;

    for (int i = 0; i < file_count; i++) {
        FILE *f = fopen(argv[i + 2], "rb");
        if (!f) {
            fprintf(stderr, "couldn't open %s\n", argv[i + 2]);
            return 1;
        }
        fseek(f, 0, SEEK_END);
        sizes[i] = ftell(f);
        fclose(f);
    }

    for (int i = 0; i < file_count; i++) {
        initrd_entry_t entry;
        memset(&entry, 0, sizeof(entry));

        const char *slash = strrchr(argv[i + 2], '/');
        const char *base = slash ? slash + 1 : argv[i + 2];
        strncpy(entry.name, base, sizeof(entry.name) - 1);

        entry.offset = running_offset;
        entry.size = (uint32_t)sizes[i];

        fwrite(&entry, sizeof(entry), 1, out);
        running_offset += entry.size;
    }

    for (int i = 0; i < file_count; i++) {
        FILE *f = fopen(argv[i + 2], "rb");
        char buf[4096];
        size_t n;
        while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
            fwrite(buf, 1, n, out);
        }
        fclose(f);
    }

    free(sizes);
    fclose(out);

    printf("packed %d files into %s\n", file_count, out_path);
    return 0;
}