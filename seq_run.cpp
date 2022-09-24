 * seq_run.cpp
 * @author wangguibao (wang_guibao@163.com)
 *
 * @brief Make threads run in order
 */
#include <pthread.h>
#include <iostream>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int turn = 0;
int N = 0;

void* th_func(void* arg) {
    int* num = (int*)(arg);
    while (1) {
        pthread_mutex_lock(&mutex);

        while (turn != *num) {
            pthread_cond_wait(&cond, &mutex);
        }
        std::cout << *num << std::endl;
        turn = (turn + 1) % N;

        // It's crutial to use pthread_cond_broadcast instead of
        // pthread_cond_signal here, since there are N-1 threads waiting on the
        // cond, and pthread_cond_singal randomly select one thread to wake up,
        // who might not be the right one
        pthread_cond_broadcast(&cond);

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: seq_run N" << std::endl;
        std::cout << "  where N is thread number" << std::endl;
        return -1;
    }

    int thread_num = atoi(argv[1]);
    N = thread_num;

    if (thread_num <= 0) {
        std::cout << "N is invalid" << std::endl;
        return -1;
    }

    int *tokens = new int[thread_num];
    for (size_t i = 0; i < thread_num; ++i) {
        tokens[i] = i;
    }

    pthread_t* ths = new pthread_t[thread_num];
    for (size_t i = 0;i < thread_num; ++i) {
        pthread_create(&ths[i], NULL, th_func, &tokens[i]);
    }

    for (size_t i = 0;i < thread_num; ++i) {
        pthread_join(ths[i], NULL);
    }

    return 0;
}
