/*
 * seq_print.c
 * @author wangguibao (wang_guibao@163.com)
 * @date 2022.09.24 13:16
 * @brief Make two threads run in order
 */
#include <pthread.h>
#include <stdio.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int turn = 0;

void* th_func(void* arg) {
   int* num = (int*)(arg);

   while (1) {
       pthread_mutex_lock(&mutex);

       while (turn != *num-1) {
           pthread_cond_wait(&cond, &mutex);
       }
       printf("%d\n", *num);
       fflush(stdout);
       turn = (turn + 1) % 2;
       pthread_cond_signal(&cond);

       pthread_mutex_unlock(&mutex);
   }

   return NULL;
}

int main() {
    int x = 1;
    int y = 2;

    pthread_t th1;
    pthread_t th2;
    pthread_create(&th1, NULL, th_func, &x);
    pthread_create(&th2, NULL, th_func, &y);

    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    return 0;
}
