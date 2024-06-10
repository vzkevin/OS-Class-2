#include <pthread.h>
#include <atomic>

class Barrier {
public:
  Barrier(int numThreads) : numThreads_(numThreads), waitingCount_(0) {
    pthread_mutex_init(&mutex_, NULL);
    pthread_cond_init(&condVar_, NULL);
  }

  ~Barrier() {
    pthread_cond_destroy(&condVar_);
    pthread_mutex_destroy(&mutex_);
  }

  void wait() {
    pthread_mutex_lock(&mutex_);
    waitingCount_++;
    if (waitingCount_ == numThreads_) {
      // Last thread to arrive, unblock all waiting threads
      waitingCount_ = 0;
      pthread_cond_broadcast(&condVar_);
    } else {
      // Not the last thread, wait for the barrier to be raised
      pthread_cond_wait(&condVar_, &mutex_);
    }
    pthread_mutex_unlock(&mutex_);
  }

private:
  int numThreads_;
  std::atomic<int> waitingCount_;  // Use atomic for thread-safe updates
  pthread_mutex_t mutex_;
  pthread_cond_t condVar_;
};
