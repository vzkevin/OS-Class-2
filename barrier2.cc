#include <pthread.h>
#include <atomic>
#include <iostream> // for std::cout

class Barrier {
public:
  Barrier(int numThreads) : numThreads_(numThreads), waitingCount_(0) {
    pthread_mutex_init(&mutex_, NULL);
    pthread_cond_init(&cond_, NULL);
  }

  ~Barrier() {
    pthread_cond_destroy(&cond_);
    pthread_mutex_destroy(&mutex_);
  }

  void wait() {
    pthread_mutex_lock(&mutex_);
    waitingCount_++;
    if (waitingCount_ == numThreads_) {
      // Last thread to arrive, unblock all waiting threads
      waitingCount_ = 0;
      pthread_cond_broadcast(&cond_);
    } else {
      // Not the last thread, wait for the barrier to be raised
      pthread_cond_wait(&cond_, &mutex_);
    }
    pthread_mutex_unlock(&mutex_);
  }

private:
  int numThreads_;
  std::atomic<int> waitingCount_;  // Use atomic for thread-safe updates
  pthread_mutex_t mutex_;
  pthread_cond_t cond_;
};

// **Barrier object must be declared before thread_fun**
Barrier barrier(4); // Assuming 4 threads (adjust as needed)

void* thread_fun(void* arg) {
  int threadId = *reinterpret_cast<int*>(arg);

  for (int i = 0; i < 3; i++) {
    std::cout << "Thread " << threadId << " before barrier " << i << std::endl;
    barrier.wait();
    std::cout << "Thread " << threadId << " after barrier " << i << std::endl;
  }

  return NULL;
}

int main() {
  const int numThreads = 4;

  pthread_t threads[numThreads];
  int threadIds[numThreads];

  for (int i = 0; i < numThreads; i++) {
    threadIds[i] = i;
    pthread_create(&threads[i], NULL, thread_fun, &threadIds[i]);
  }

  for (int i = 0; i < numThreads; i++) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}
