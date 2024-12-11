#ifndef H_THREAD
#define H_THREAD

#include <windows.h>
#include "stack.h"

#define N_THREADS 100

extern volatile int thread_job_running;

struct Threads {
    HANDLE threads[N_THREADS];
};

struct ThreadParams {
   struct Stack *stack;
};

struct Threads *thread_create(struct Stack *stack, struct ThreadParams **thread_params);
void thread_free(struct Threads **threads);
void thread_terminate();
void thread_wait_all(struct Threads *threads);
DWORD WINAPI thread_job(LPVOID param);
#endif