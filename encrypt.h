#ifndef H_ENCRYPT
#define H_ENCRYPT

#include <windows.h>
#include "stack.h"

#define EXT L".lock"

wchar_t *add_extension(wchar_t *file_path);
void encrypt_data(LPCVOID p_src, LPVOID p_dest, DWORD file_size);
#endif