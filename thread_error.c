#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main()
{
    pthread_t thread;
    int status;

    if ((status = pthread_join(thread, NULL)) != 0) {
	fprintf(stderr, "error %d: %s\n", status, strerror(status));
    }

    return 0;
}

