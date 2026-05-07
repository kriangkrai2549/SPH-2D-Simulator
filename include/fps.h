#ifndef FPS_H
#define FPS_J


#include<chrono>
#include<deque>



using Clock = std::chrono::high_resolution_clock; 
using timePoint = std::chrono::time_point<Clock>;


class Fps{
  private:
    std::deque<double> frame_time;
    timePoint start_time;
    timePoint end_time;
    const int SAMPLE_SIZE = 60;
  public:
  void start();
  void end();
  double get_fps();
};

#endif