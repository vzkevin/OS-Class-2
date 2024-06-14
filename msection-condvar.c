#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREADS 3  // Maximum threads in the critical section

pthread_mutex_t mutex_lock;  // Mutex for synchronization
pthread_cond_t cond_var;    // Condition variable to signal waiting threads

int current_count = 0;  // Tracks threads in the critical section

void enter(void *arg) {
    pthread_mutex_lock(&mutex_lock);
    while (current_count >= MAX_THREADS) {
        pthread_cond_wait(&cond_var, &mutex_lock);
    }
    current_count++;
    pthread_mutex_unlock(&mutex_lock);
}

void leave(void *arg) {
    pthread_mutex_lock(&mutex_lock);
    current_count--;
    if (current_count < MAX_THREADS) {
        pthread_cond_signal(&cond_var);
    }
    pthread_mutex_unlock(&mutex_lock);
}

void do_critical_work(void) {
    pthread_t tid = pthread_self();
    printf("Thread %lu: In critical section, %d threads here\n", tid, current_count);
}

void *do_work(void *arg) {
    while (1) {
        enter(arg);
        do_critical_work();
        leave(arg);
        // Add your additional work here (not shown for brevity)
    }
    return NULL;
}

int main(void) {
    int i, num_threads = 10;  // Number of threads
    pthread_t threads[num_threads];

    pthread_mutex_init(&mutex_lock, NULL);
    pthread_cond_init(&cond_var, NULL);

    // Create threads
    for (i = 0; i < n; i++) {
        if (pthread_create(&threads[i], NULL, do_work, NULL) != 0) {
            printf("create failed");
            return 1;
        }
    }

    // now wait for the threads to complete
    for (i = 0; i < n; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            printf("join failed");
            return 1;
        }
    }

    pthread_mutex_destroy(&mutex_lock);
    pthread_cond_destroy(&cond_var);
    return 0;
}
