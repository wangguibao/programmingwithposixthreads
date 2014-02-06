#include <pthread.h>
#include "errors.h"

/*
 * Declare a structure, with a mutex, statically initialized. This
 * is the same as using pthread_mutex_init, with the default attributes
 */
typedef struct my_struct_tag {
    pthread_mutex_t mutex;	/* Protects access to value */
    int value;			/* Access protected by mutex */
} my_struct_t;


int main(int argc, char **argv)
{
    my_struct_t *data;
    int status;

    if ((data = malloc(sizeof(my_struct_t))) == NULL) {
	errno_abort("Allocate structure");
    }

    if ((status = pthread_mutex_init(&data->mutex, NULL)) != 0) {
	err_abort(status, "Init mutex");
    }

    if ((status = pthread_mutex_destroy(&data->mutex)) != 0) {
	err_abort(status, "Destroy mutex");
    }

    free(data);

    return status;
}

