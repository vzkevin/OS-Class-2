#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define M 3  // Maximum threads in the m-section
sem_t m_lock; // Shared semaphore for synchronization

int current_count = 0;  // Tracks threads in the m-section (global)

void enter(void) {
    sem_wait(&m_lock);
    current_count++;
    sem_post(&m_lock);
}

void leave(void) {
    sem_wait(&m_lock);
    current_count--;
    if (current_count > 0) {
        sem_post(&m_lock);
    }
}

void doCriticalWork(void) {
    pthread_t tid = pthread_self();
    printf("Thread %lu: In critical section, %d threads here\n", tid, current_count);
}

void *doWork(void *arg) {
    while (1) {
        enter();
        doCriticalWork();
        leave();
        // Add your additional work here (not shown for brevity)
    }
    return NULL;
}

int main(void) {
    int i, n = 10;  // Number of threads (N)
    pthread_t threads[n];

    sem_init(&m_lock, 0, M);  // Initialize semaphore with initial value M

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

    sem_destroy(&m_lock);  // Destroy semaphore
    return 0;
}
