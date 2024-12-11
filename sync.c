#include <stdio.h>
#include <windows.h>
#include "sync.h"

HANDLE semaphore;
HANDLE mutex;

// sem
int sync_create_semaphore() {
    if((semaphore = CreateSemaphore(
        NULL,
        0,
        1000000,
        NULL
    )) == NULL) {
        printf("(%s:%d) Error creating semaphore: %ld\n", __FILE__, __LINE__, GetLastError());
        return 1;
    }
    printf("Semaphore créé avec succès : %p\n", semaphore);
    return 0;
}

void sync_release_semaphore() {
    if(!ReleaseSemaphore(semaphore, 1, NULL)) {
        printf("(%s:%d) Error: unable to release semaphore (%ld)\n", __FILE__, __LINE__, GetLastError());
    }
}

void sync_wait_semaphore() {
    if(WaitForSingleObject(
        semaphore,
        INFINITE
    ) != WAIT_OBJECT_0) {
        printf("(%s:%d) Error WaitForSingleObject : %ld\n", __FILE__, __LINE__, GetLastError());
    }
}

void sync_cleanup_semaphore() {
    if(!CloseHandle(semaphore)) {
        printf("(%s:%d) Error clean up semaphore : %ld\n", __FILE__, __LINE__, GetLastError());
    }
}

// mutex
int sync_create_mutex() {
    if((mutex = CreateMutex(
        NULL,
        FALSE,
        NULL
    )) == NULL) {
        printf("(%s:%d) Error creating mutex: %ld\n", __FILE__, __LINE__, GetLastError());
        return 1;
    }
    printf("Mutex créé avec succès : %p\n", mutex);
    return 0;
}

void sync_lock_mutex() {
    WaitForSingleObject(mutex, INFINITE);
}

void sync_unlock_mutex() {
    ReleaseMutex(mutex);
}

void sync_cleanup_mutex() {
    CloseHandle(mutex);
}