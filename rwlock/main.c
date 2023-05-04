#include "rwlock.h"
#include "utility.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define THREADS 5
#define DATASIZE 15
#define ITERATIONS 10000

typedef struct {
    int thread_num;
    pthread_t thread_id;
    int updates;
    int reads;
    int interval;
} thread_t;

typedef struct {
    rwlock_t lock;
    int data;
    int updates;
} data_t;

thread_t threads[THREADS];
data_t data[DATASIZE];

void* thread_routine(void* arg) {
    thread_t* self = (thread_t *)arg;
    int element = 0;
    int repeats = 0;

    for (int i = 0; i < ITERATIONS; ++i) {
        if ((i % self->interval) == 0) {
            int ret = rwlock_writelock(&data[element].lock);
            if (ret != 0) {
                ERROR_RETURN("rwlock_writelock fail", NULL);
            }

            data[element].data = self->thread_num;
            data[element].updates++;
            self->updates++;

            ret = rwlock_writeunlock(&data[element].lock);
            if (ret != 0) {
                ERROR_RETURN("rwlock_writeunlock fail", NULL);
            }
        } else {
            int ret = rwlock_readlock(&data[element].lock);
            if (ret != 0) {
                ERROR_RETURN("rwlock_readlock fail", NULL);
            }

            self->reads++;
            if (data[element].data == self->thread_num) {
                repeats++;
            }

            ret = rwlock_readunlock(&data[element].lock);
            if (ret != 0) {
                ERROR_RETURN("rwlock_readunlock fail", NULL);
            }
        }

        ++element;
        if (element >= DATASIZE) {
            element = 0;
        }
    }

    printf("Thread %d found unchanged elements %d times\n",
            self->thread_num,
            repeats);

    return NULL;
}

int main()
{
    unsigned int seed = time(NULL);
    printf("seed = %d\n", seed);

    int thread_updates = 0;
    int data_updates = 0;

    for (int i = 0; i < DATASIZE; ++i) {
        data[i].data = 0;
        data[i].updates = 0;
        int ret = rwlock_init(&data[i].lock);
        if (ret != 0) {
            ERROR_RETURN("rwlock_init fail", -1);
        }
    }

    for (int i = 0; i < THREADS; ++i) {
        threads[i].thread_num = i;
        threads[i].updates = 0;
        threads[i].reads = 0;
        threads[i].interval = rand_r(&seed) % 71;
        int ret = pthread_create(
                &threads[i].thread_id,
                NULL,
                thread_routine,
                (void*)(&threads[i]));
        if (ret != 0) {
            ERROR_RETURN("pthread_create fail", -1);
        }
    }

    for (int i = 0; i < THREADS; ++i) {
        int ret = pthread_join(threads[i].thread_id, NULL);
        if (ret != 0) {
            ERROR_RETURN("pthread_join fail", -1);
        }

        thread_updates += threads[i].updates;
        printf("Thread %02d: interval %d, updates %d, reads %d\n",
               i,
               threads[i].interval,
               threads[i].updates,
               threads[i].reads);
    }

    for (int i = 0; i < DATASIZE; ++i) {
        data_updates += data[i].updates;
        printf("Data %d: value %d, updates %d\n",
               i,
               data[i].data,
               data[i].updates);
        int ret = rwlock_destroy(&data[i].lock);
        if (ret != 0) {
            ERROR_RETURN("rwlock_destroy fail", -1);
        }
    }

    return 0;
}
