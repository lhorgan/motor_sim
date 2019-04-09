#ifndef BACKLIGHT_H_INCLUDED
#define BACKLIGHT_H_INCLUDED

#include <chrono>
#include <ctime>
#include <stdlib.h> 
#include <atomic>
#include <thread>

class Backlight {
public:
    Backlight(double refreshRate, double refreshNoise=0) {
        this->refreshRate = refreshRate;
        this->refreshNoise = refreshNoise;
        startFrameFreq = 16;

        white = true;
        startFrame = true;

        running = true;
        runThread.reset(new std::thread(&Backlight::run, this));
    }

    ~Backlight() {
        runThread->join();
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
        double effectiveFPS = refreshRate;

        while(running) {
            auto elapsed = std::chrono::high_resolution_clock::now() - start;
            long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

            if(microseconds >= (long long)((double)usToS / effectiveFPS)) {
                //printf("NEW FRAME!\n");
                start = std::chrono::high_resolution_clock::now();
                
                double noise = (double)((double)(rand() % 10000) / 10000.0) * refreshNoise;
                noise = rand() % 2 == 0 ? -1 * noise : noise;
                effectiveFPS = refreshRate + noise;

                white = !white;
                if(++currFrame == startFrameFreq) {
                    startFrame = true;
                    currFrame = 0;
                }
                else {
                    startFrame = false;
                }
            }

            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    }
    std::unique_ptr<std::thread> runThread;

private:
    double refreshRate; // fps
    double refreshNoise; // also fps (ie +/- 0.1fps)
    int startFrameFreq;
    std::atomic<bool> white; // is the frame black or white?
    std::atomic<bool> startFrame; // should the motor be home at the start of this frame?
    
    bool running;
};

#endif