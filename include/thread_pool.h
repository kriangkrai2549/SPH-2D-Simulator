#ifndef THREAD_POOL
#define THREAD_POOL

#include<thread>
#include<queue>
#include<functional>
#include<mutex>
#include<condition_variable>
#include<atomic>



class ThreadPool{
  private:
   std::vector<std::thread> threads;
   std::queue<std::function<void()>> taskQueue;
   std::mutex mtx;
   std::condition_variable cv;
   std::condition_variable cv_main;
   std::atomic<int> activate_task{0};
   bool run = true;

  void worker(){
    while(true){
      std::function<void()> task;
      {
        std::unique_lock<std::mutex> lock(mtx);

        cv.wait(lock,[this]{
          return !run || !taskQueue.empty();
        });

        //The while loop exist way
        if(!run && taskQueue.empty()){
          return;
        }

        task = taskQueue.front();
        taskQueue.pop();
      }
      task();
      activate_task--;
      if(activate_task == 0){
        cv_main.notify_all();
      }
    }
  }
  public:
   ThreadPool(int number_thread);
   ~ThreadPool();
   void add_task(std::function<void()> task);
   void wait();
};

#endif