#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define M 3  // Maximum threads in the m-section

pthread_mutex_t m_mutex;  // Mutex for synchronization
pthread_cond_t m_cond;    // Condition variable to signal waiting threads

int current_count = 0;  // Tracks threads in the m-section

void enter(void *arg) {
    pthread_mutex_lock(&m_mutex);
    while (current_count >= M) {
        pthread_cond_wait(&m_cond, &m_mutex);
    }
    current_count++;
    pthread_mutex_unlock(&m_mutex);
}

void leave(void *arg) {
    pthread_mutex_lock(&m_mutex);
    current_count--;
    if (current_count < M) {
        pthread_cond_signal(&m_cond);
    }
    pthread_mutex_unlock(&m_mutex);
}

void doCriticalWork(void) {
    pthread_t tid = pthread_self();
    printf("Thread %lu: In critical section, %d threads here\n", tid, current_count);
}

void *doWork(void *arg) {
    while (1) {
        enter(arg);
        doCriticalWork();
        leave(arg);
        // Add your additional work here (not shown for brevity)
    }
    return NULL;
}

int main(void) {
    int i, n = 10;  // Number of threads (N)
    pthread_t threads[n];

    pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_cond, NULL);

    for (i = 0; i < n; i++) {
        if (pthread_create(&threads[i], NULL, doWork, NULL) != 0) {
            perror("pthread_create failed");
            return 1;
        }
    }

    for (i = 0; i < n; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join failed");
            return 1;
        }
    }

    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_cond);
    return 0;
}
