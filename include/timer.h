#ifndef TIMER_H
#define TIMER_H


#include<iostream>
#include<chrono>
#include<unordered_map>



using Clock = std::chrono::high_resolution_clock; 
using timePoint = std::chrono::time_point<Clock>;
using Duration_ms = std::chrono::duration<double ,std::milli>;

struct ProcessTimer{
                  std::string name;
                  Clock::time_point start_time;
                  Clock::time_point end_time;
                  Duration_ms total;
                  int count = 0;
                  bool running = false;
};

class Timer{
  private:
    std::unordered_map<std::string,ProcessTimer> processes;
    int max_process;
  public:
  Timer(int max_process);
  void start(const std::string& precess_name);
  void stop(const std::string& precess_name);
  void show_report();
  void show_final_report();

};


#endif