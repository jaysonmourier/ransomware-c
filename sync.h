#ifndef H_SYNC
#define H_SYNC

#include <windows.h>

extern HANDLE semaphore;
extern HANDLE mutex;

// sem
int sync_create_semaphore();
void sync_release_semaphore();
void sync_wait_semaphore();
void sync_cleanup_semaphore();

// mutex
int sync_create_mutex();
void sync_lock_mutex();
void sync_unlock_mutex();
void sync_cleanup_mutex();
#endif