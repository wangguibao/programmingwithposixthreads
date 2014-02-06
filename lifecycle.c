#include <pthread.h>
#include "errors.h"

void *thread_routine(void *arg)
{
    return arg;
}

int main(int argc, char **argv)
{
    pthread_t thread_id;
    void *thread_result;
    int status;

    status = pthread_create(&thread_id, NULL, thread_routine, NULL);
    if (status != 0) {
	err_abort(status, "pthread_create() error");
    }

    status = pthread_join(thread_id, &thread_result);
    if (status != 0) {
	err_abort(status, "pthread_join() error");
    }

    return !(thread_result == NULL);
}
