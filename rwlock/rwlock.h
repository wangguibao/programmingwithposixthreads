#ifndef RWLOCK_H
#define RWLOCK_H
#include <pthread.h>

#define LOCK_INVALID (0)
#define LOCK_VALID (1)

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t read_cv;
    pthread_cond_t write_cv;

    int writers;
    int readers;

    int write_waiters;
    int read_waiters;

    int valid;
} rwlock_t;

int rwlock_init(rwlock_t* lock);
int rwlock_destroy(rwlock_t* lock);

int rwlock_readlock(rwlock_t* lock);
int rwlock_readtrylock(rwlock_t* lock);
int rwlock_readunlock(rwlock_t* lock);

int rwlock_writelock(rwlock_t* lock);
int rwlock_writetrylock(rwlock_t* lock);
int rwlock_writeunlock(rwlock_t* lock);
#endif // RWLOCK_H
