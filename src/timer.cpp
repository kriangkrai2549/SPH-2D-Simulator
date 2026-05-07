#include<iostream>
#include <iomanip>
#include"timer.h"






using Clock = std::chrono::high_resolution_clock; 
using timePoint = std::chrono::time_point<Clock>;
using Pair = std::pair<const std::string, ProcessTimer>;
using Duration_ms = std::chrono::duration<double ,std::milli>;


std::ostream& operator<<(std::ostream &os, const ProcessTimer &p) {
    Duration_ms ms = p.end_time - p.start_time;
    os << std::fixed << std::setprecision(4);
    os << std::left  << std::setw(20) << p.name
       << std::right << std::setw(10) << ms.count() << " ms";
    return os;
}


Timer::Timer(int max_process){
  this->max_process = max_process;
  processes.reserve(max_process);
}


void Timer::start(const std::string& name){
   if(!processes.count(name)){
     ProcessTimer proc;
     proc.name = name;
     processes[name] = proc;
    }
    processes[name].start_time = Clock::now();
    processes[name].running = true;
}

void Timer::stop(const std::string& name){
   if(!processes.count(name)){return;}
   processes[name].end_time = Clock::now();
   processes[name].running = false;
   processes[name].count += 1;
}

void Timer::show_report(){
    Duration_ms total(0.0);
    for(Pair& p : processes){
        Duration_ms delta = p.second.end_time - p.second.start_time; 
        total += delta;
        p.second.total += delta;
    }

    for(const Pair& p : processes){
        Duration_ms dura = p.second.end_time - p.second.start_time;
        double percentage = (dura.count() / total.count()) * 100.0;

        std::cout << std::fixed << std::setprecision(4)
                  << std::left  << std::setw(20) << p.second.name
                  << std::right << std::setw(10) << dura.count()  << " ms "
                  << std::right << std::setw(8)  << percentage    << "%\n";
    }
    std::cout << std::left  << std::setw(20) << "Total"
              << std::right << std::setw(10) << total.count() << " ms\n";
}


void Timer::show_final_report(){
    Duration_ms total(0.0);
    for(Pair& p : processes){
        total += p.second.total/p.second.count;
    }

    for(const Pair& p : processes){
        Duration_ms avg = p.second.total/p.second.count;
        double percentage = ((avg.count())/total.count()) * 100.0;

        std::cout << std::fixed << std::setprecision(4)
                  << std::left  << std::setw(20) << p.second.name
                  << std::right << std::setw(10) << avg.count()  << " ms "
                  << std::right << std::setw(8)  << percentage    << "%\n";
    }
    std::cout << std::left  << std::setw(20) << "Total"
              << std::right << std::setw(10) << total.count() << " ms\n";
}


