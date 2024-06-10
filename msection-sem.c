#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// global maximum threads
int max = 3;
sem_t mutex;

// Thread counter
int counter = 0;

// Function to enter
void enter(void) {
    sem_wait(&mutex);
    counter++;
    sem_post(&mutex);
}

// Function to leave
void leave(void) {
    sem_wait(&mutex);
    counter--;
    if (counter > 0) {
        sem_post(&mutex);
    }
}

// Function do critical work
void do_critical_work(void) {
    pthread_t tid = pthread_self();
    printf("Thread %lu: In critical section, %d threads here\n", tid, counter);
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
    // Number of threads
    long n = 10;
    pthread_t threads[n];

    // Initialize semaphore with initial value max threads
    sem_init(&mutex, 0, max); 

    // Create threads
    for (i = 0; i < n; i++) {
        if (pthread_create(&threads[i], NULL, do_work, NULL) != 0) {
            printf("pthread_create failed");
            return 1;
        }
    }

    // now wait for the threads to complete
    for (i = 0; i < n; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            printf("pthread_join failed");
            return 1;
        }
    }

    // Destroy semaphore
    sem_destroy(&mutex);  // Destroy semaphore
    return 0;
}
