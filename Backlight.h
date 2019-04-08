#include <chrono>
#include <ctime>
#include <stdlib.h> 
#include <atomic>

class Backlight {
public:
    Backlight() {

    }

private:
    double refreshRate; // fps
    double refreshNoise; // also fps (ie +/- 0.1fps)
    std::atomic<bool> white; // is the frame black or white?
    std::atomic<bool> startFrame; // should the motor be home at the start of this frame?
};