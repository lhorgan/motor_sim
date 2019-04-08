#include <chrono>
#include <ctime>
#include <stdlib.h> 
#include <atomic>
#include <thread>

class Backlight {
public:
    Backlight() {
        white = true;
    }

    bool isWhite() {
        return white;
    }

    bool isStartFrame() {
        return startFrame;
    }

    void run() {
        long long usToS = 1000000;
        unsigned long long currFrame = 0;

        auto start = std::chrono::high_resolution_clock::now();
        while(running) {
            auto elapsed = std::chrono::high_resolution_clock::now() - start;
            long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

            double noise = (double)((rand() % 10000) / 10000) * refreshNoise;
            noise = rand() % 2 == 0 ? -1 : 1;
            double effectiveFPS = refreshRate + noise;

            if(microseconds >= (long long)((double)usToS / effectiveFPS)) {
                start = std::chrono::high_resolution_clock::now();
                white = !white;
                if(currFrame % startFrameFreq == 0) {
                    startFrame = true;
                }
                currFrame++;
            }

            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    }

private:
    double refreshRate; // fps
    double refreshNoise; // also fps (ie +/- 0.1fps)
    int startFrameFreq;
    std::atomic<bool> white; // is the frame black or white?
    std::atomic<bool> startFrame; // should the motor be home at the start of this frame?
    
    bool running;
};