#ifndef H_FILE_UTILS
#define H_FILE_UTILS

#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include "stack.h"

bool should_ignore(const WIN32_FIND_DATAW *find_file_data);
bool build_path(wchar_t *folder_path, const size_t size, const wchar_t *root_path, const wchar_t *file_name);
void add_to_stack(WIN32_FIND_DATAW *file_data, wchar_t *path, struct Stack *folders_stack, struct Stack *files_stack);
wchar_t *duplicate_path(const wchar_t *path);
void populate_stacks_from_directory(wchar_t *root_path, struct Stack *folders_stack, struct Stack *files_stack);

#endif