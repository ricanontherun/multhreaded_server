#ifndef THREAD_SAFE_QUEUE_INCLUDED
#define THREAD_SAFE_QUEUE_INCLUDED

#include <queue>
#include <mutex>
#include <iostream>
#include <condition_variable>

template <typename T>
class ThreadSafeQueue
{
  public:
    void push(const T & item)
    {
      std::unique_lock<std::mutex> lock(this->mutex);
      this->queue.push(item);
      lock.unlock();

      // Tell a blocked thread there's something for it do process.
      this->condition.notify_one();
    }

    T pop()
    {
      std::unique_lock<std::mutex> lock(this->mutex);

      while ( this->queue.empty() ) {
        this->condition.wait(lock);
      }

      auto item = this->queue.front();
      this->queue.pop();
      return item;
    }

    bool empty()
    {
      std::unique_lock<std::mutex> lock(this->mutex);
      return this->queue.empty();
    }

  private:
    std::queue<T> queue;
    std::mutex mutex;
    std::condition_variable condition;
};

#endif
