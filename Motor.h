#ifndef MOTOR_H_INCLUDED
#define MOTOR_H_INCLUDED

#include <chrono>
#include <ctime>
#include <stdlib.h> 
#include <atomic>
#include <thread>

class Motor {
public:
    Motor(double targetRPS, double a, double aNoise, double startAngle=0, double homeTol=1) {
        this->angle = startAngle;
        this->rps = 0;
        this->targetRPS = rps;
        this->targetAcceleration = a;
        this->aNoise = aNoise;
        this->homeTol = homeTol;
        
        home = false;
        if(startAngle >= 360 - homeTol || startAngle < homeTol) {
            home = true;
        }

        runThread.reset(new std::thread(&run, this));
    }

    void setSpeed(double targetRPS) {
        this->targetRPS = targetRPS;
    }

    ~Motor() {
        runThread->join();
    }

private:
    double angle;
    double rps;
    double targetRPS;
    double targetAcceleration;
    double aNoise;
    double homeTol;

    bool running;
    std::atomic<bool> home;

    double getCurrentA() {
        double noiseDir = rand() % 2 == 0 ? -1.0 : 1.0;
        return targetAcceleration + (aNoise * noiseDir);
    }

    double getIntrinsicA() {
        return 1.0; // 1 rps/s
    }

    void run() {
        long long usToS = 1000000;
        
        auto start = std::chrono::high_resolution_clock::now();
        while(running) {
            auto elapsed = std::chrono::high_resolution_clock::now() - start;
            long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            start = std::chrono::high_resolution_clock::now();

            double newAngle = ((this->rps * microseconds) / usToS) * 360.0;
            double currentSpeed = (newAngle - angle) / (microseconds / usToS);
            
            if(currentSpeed >= targetRPS) {
                this->targetAcceleration = -getIntrinsicA();
            }
            else {
                this->targetAcceleration = getIntrinsicA();
            }

            this->rps += getCurrentA() * (microseconds / usToS);

            if(angle >= 360.0) {
                angle -= 360.0;
            }

            if(angle >= 360 - homeTol || angle < homeTol) {
                home = true;
            }

            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    }
    std::unique_ptr<std::thread> runThread;
};

#endif