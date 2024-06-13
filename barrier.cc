#include <pthread.h>
#include <atomic>

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
    std::atomic<int> waiting_count_;
    pthread_mutex_t mutex_;
    pthread_cond_t cond_var_;
};