#include"thread_pool.h"
#include<queue>
#include<functional>
#include<mutex>
#include<condition_variable>



ThreadPool::ThreadPool(int number_thread){
       threads.reserve(number_thread);
       for(int i = 0 ;i < number_thread;i++){
           threads.emplace_back(&ThreadPool::worker, this);
       }
}

ThreadPool::~ThreadPool(){
      {
       std::unique_lock<std::mutex> lock(mtx);
       run = false;
      }
      
      cv.notify_all();

       for(auto& t : threads){
        t.join();
       }
}

void ThreadPool::add_task(std::function<void()> task){
      {
       std::unique_lock<std::mutex> lock(mtx);
       taskQueue.push(task);
       activate_task++;
      }
      cv.notify_one();
}


void ThreadPool::wait(){
      {
       std::unique_lock<std::mutex> lock(mtx);
       cv_main.wait(lock ,[this]{return activate_task == 0;});
      }
}
