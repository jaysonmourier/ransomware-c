#ifndef H_SYNC
#define H_SYNC

#include <windows.h>

extern HANDLE semaphore;
extern HANDLE mutex;

// sem
int init_sem();
void release_sem();
void wait_sem();
void cleanup_sem();

// mutex
int init_mutex();
void lock_mutex();
void unlock_mutex();
void cleanup_mutex();
#endif