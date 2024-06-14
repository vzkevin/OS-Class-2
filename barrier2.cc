#include <pthread.h>
#include <iostream>

class Barrier {
public:
    Barrier(int num_threads) : num_threads_(num_threads), waiting_count_(0) {
        pthread_mutex_init(&mutex_, NULL);
        pthread_cond_init(&cond_var_, NULL);
    }

    ~Barrier() {
        pthread_cond_destroy(&cond_var_);
        pthread_mutex_destroy(&mutex_);
    }

    void wait() {
        pthread_mutex_lock(&mutex_);
        waiting_count_++;
        if (waiting_count_ == num_threads_) {
            // Last thread arrives so unblock all
            waiting_count_ = 0;
            pthread_cond_broadcast(&cond_var_);
        } else {
            // Still more wait for threads
            pthread_cond_wait(&cond_var_, &mutex_);
        }
        pthread_mutex_unlock(&mutex_);
    }

private:
    int num_threads_;
    int waiting_count_;
    pthread_mutex_t mutex_;
    pthread_cond_t cond_var_;
};

// Create a Barrier object with 4 threads
Barrier barrier(4); 

void* threadFun(void* arg) {
    int thread_id = *reinterpret_cast<int*>(arg);

    for (int i = 0; i < 3; i++) {
        printf("Thread %d before barrier %d\n", thread_id, i);
        barrier.wait();
        printf("Thread %d after barrier %d\n", thread_id, i);
    }

    pthread_exit(NULL);
}

int main() {
    const int num_threads = 4;

    // Array to hold thread ids
    pthread_t threads[num_threads];

    // Ids as integers
    int thread_ids[num_threads];

    // Create threads
    for (int i = 0; i < num_threads; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, threadFun, &thread_ids[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
