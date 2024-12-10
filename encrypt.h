#ifndef H_ENCRYPT
#define H_ENCRYPT

#include <windows.h>
#include "stack.h"

wchar_t *add_extension(wchar_t *file_path);
void encrypt_stack(struct Stack *files_stack);

#endif