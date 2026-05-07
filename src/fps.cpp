#include <iostream>
#include <chrono>
#include <deque>
#include <numeric>
#include "fps.h"

using Clock = std::chrono::high_resolution_clock; 
using timePoint = std::chrono::time_point<Clock>;


void Fps::start() {
    start_time = Clock::now();
}


void Fps::end() {
    end_time = Clock::now();
}


double Fps::get_fps() {
    std::chrono::duration<double> delta = end_time - start_time;
    frame_time.push_back(delta.count());

    if (frame_time.size() > SAMPLE_SIZE)
        frame_time.pop_front();

    double avg_delta = std::accumulate(frame_time.begin(), frame_time.end(), 0.0) / frame_time.size();
    return 1.0 / avg_delta;
}
