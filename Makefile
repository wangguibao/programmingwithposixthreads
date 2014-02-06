# Digital UNIX 4.0 compilation flags:
CFLAGS=-pthread -g $(DEBUGFLAGS)
RTFLAGS=-lrt

# Solaris 2.5 compilation flags:
#CFLAGS=-D_POSIX_C_SOURCE=199506 -D_REENTRANT -Xa -lpthread -g $(DEBUGFLAGS)
#RTFLAGS=-lposix4

SOURCES=alarm_fork.c alarm_mutex.c alarm_thread.c thread_error.c \
	lifecycle.c mutex_dynamic.c mutex_static.c trylock.c

PROGRAMS=$(SOURCES:.c=)
all:	${PROGRAMS}
alarm_mutex:
	${CC} ${CFLAGS} ${RTFLAGS} ${LDFLAGS} -o $@ alarm_mutex.c
backoff:
	${CC} ${CFLAGS} ${RTFLAGS} ${LDFLAGS} -o $@ backoff.c
sched_attr:
	${CC} ${CFLAGS} ${RTFLAGS} ${LDFLAGS} -o $@ sched_attr.c
sched_thread:
	${CC} ${CFLAGS} ${RTFLAGS} ${LDFLAGS} -o $@ sched_thread.c
semaphore_signal:
	${CC} ${CFLAGS} ${RTFLAGS} ${LDFLAGS} -o $@ semaphore_signal.c
semaphore_wait:
	${CC} ${CFLAGS} ${RTFLAGS} ${LDFLAGS} -o $@ semaphore_wait.c
sigev_thread:
	${CC} ${CFLAGS} ${RTFLAGS} ${LDFLAGS} -o $@ sigev_thread.c
susp:
	${CC} ${CFLAGS} ${RTFLAGS} ${LDFLAGS} -o $@ susp.c
rwlock_main: rwlock.c rwlock.h rwlock_main.c
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ rwlock_main.c rwlock.c
rwlock_try_main: rwlock.h rwlock.c rwlock_try_main.c
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ rwlock_try_main.c rwlock.c
barrier_main: barrier.h barrier.c barrier_main.c
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ barrier_main.c barrier.c
workq_main: workq.h workq.c workq_main.c
	${CC} ${CFLAGS} ${RTFLAGS} ${LDFLAGS} -o $@ workq_main.c workq.c
clean:
	@rm -rf $(PROGRAMS) *.o
recompile:	clean all
