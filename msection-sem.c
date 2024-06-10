#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

int max_threads = 3;  // Global maximum threads
sem_t mutex_lock; // For sync

int current_count = 0;  // Thread counter

// Function to enter the critical section
void enter(void) {
    sem_wait(&mutex_lock);
    current_count++;
    sem_post(&mutex_lock);
}

// Function to leave the critical section
void leave(void) {
    sem_wait(&mutex_lock);
    current_count--;
    if (current_count > 0) {
        sem_post(&mutex_lock);
    }
}

// Function to perform critical work
void do_critical_work(void) {
    pthread_t tid = pthread_self();
    printf("Thread %lu: In critical section, %d threads here\n", tid, current_count);
}

// Function executed by each thread
void *do_work(void *arg) {
    while (1) {
        enter();
        do_critical_work();
        leave();
    }
    return NULL;
}

int main(void) {
    long i;
    long num_threads = 10;  // Number of threads
    pthread_t threads[num_threads];

    sem_init(&mutex_lock, 0, max_threads);  // Initialize semaphore with initial value max_threads

    for (i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], NULL, do_work, NULL) != 0) {
            perror("pthread_create failed");
            return 1;
        }
    }

    for (i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join failed");
            return 1;
        }
    }

    sem_destroy(&mutex_lock);  // Destroy semaphore
    return 0;
}
