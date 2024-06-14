#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int max = 3;

// Mutex for sync
pthread_mutex_t mutex;

// Condition var for waiting threads
pthread_cond_t cond_var;

// Thread counter
int counter = 0; 

// Function to enter
void enter(void *arg) {
    pthread_mutex(&mutex);
    pthread_mutex_lock(&mutex);
    while (counter >= max) {
        pthread_cond_wait(&cond_var, &mutex);
    }
    counter++;
    pthread_mutex_unlock(&mutex);
}

// Function to leave
void leave(void *arg) {
    pthread_mutex_lock(&mutex);
    counter--;
    if (counter < max) {
        pthread_cond_signal(&cond_var);
    }
    pthread_mutex_unlock(&mutex);
}

// Function do critical work
void do_critical_work(void) {
    pthread_t tid = pthread_self();
    printf("Thread %lu: In critical section, %d threads here\n", tid, counter);
}

// Function executed by each thread
void *do_work(void *arg) {
    while (1) {
        enter(arg);
        do_critical_work();
        leave(arg);
    }
    return NULL;
}

int main(void) {
    long i;

    // Number of threads
    long n = 10;
    pthread_t threads[n];

    pthread_mutex_init(&mutex, NULL);
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

    // Destroy
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_var);
    return 0;
}
