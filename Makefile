CC=gcc
CFLAGS=-pthread -g $(DEBUGFLAGS)
RTFLAGS=-lrt

PROGRAMS=alarm_fork alarm_mutex alarm_thread backoff \
    cond cond_dynamic cond_static lifecycle mutex_dynamic \
    mutex_static thread_error trylock

all:	${PROGRAMS}
clean:
	@rm -rf $(PROGRAMS) *.o
recompile:	clean all

