#include <pthread.h>
#include <atomic>
#include <iostream> // For std::cout

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
            // Last thread to arrive, unblock all waiting threads
            waiting_count_ = 0;
            pthread_cond_broadcast(&cond_var_);
        } else {
            // Not the last thread, wait for the barrier to be raised
            pthread_cond_wait(&cond_var_, &mutex_);
        }
        pthread_mutex_unlock(&mutex_);
    }

private:
    int num_threads_;
    std::atomic<int> waiting_count_;  // Use atomic for thread-safe updates
    pthread_mutex_t mutex_;
    pthread_cond_t cond_var_;
};

// Barrier object must be declared before threadFun
Barrier barrier(4); // Assuming 4 threads (adjust as needed)

void* threadFun(void* arg) {
    int thread_id = *reinterpret_cast<int*>(arg);

    for (int i = 0; i < 3; i++) {
        std::cout << "Thread " << thread_id << " before barrier " << i << std::endl;
        barrier.wait();
        std::cout << "Thread " << thread_id << " after barrier " << i << std::endl;
    }

    pthread_exit(NULL);
}

int main() {
    const int num_threads = 4;

    pthread_t threads[num_threads];
    int thread_ids[num_threads];

    for (int i = 0; i < num_threads; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, threadFun, &thread_ids[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
