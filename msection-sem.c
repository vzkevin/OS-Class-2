#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

int M = 3;  // Global maximum threads
sem_t m_lock; // For sync

int current_count = 0;  // Thread counter

// Function to enter the critical section
void enter(void) {
    sem_wait(&m_lock);
    current_count++;
    sem_post(&m_lock);
}

// Function to leave the critical section
void leave(void) {
    sem_wait(&m_lock);
    current_count--;
    if (current_count > 0) {
        sem_post(&m_lock);
    }
}

// Function to perform critical work
void doCriticalWork(void) {
    pthread_t tid = pthread_self();
    printf("Thread %lu: In critical section, %d threads here\n", tid, current_count);
}

// Function executed by each thread
void *doWork(void *arg) {
    while (1) {
        enter();
        doCriticalWork();
        leave();
    }
    return NULL;
}

int main(void) {
    long i;
    long n = 10;  // Threads
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
