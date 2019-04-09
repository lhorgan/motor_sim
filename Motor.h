#ifndef MOTOR_H_INCLUDED
#define MOTOR_H_INCLUDED

#include <chrono>
#include <ctime>
#include <stdlib.h> 
#include <stdio.h>
#include <atomic>
#include <thread>

class Motor {
public:
    Motor(double targetRPS, double aNoise, double startAngle=0, double homeTol=1) {
        printf("MOTOR INITIALIZED\n");
        this->angle = startAngle;
        this->rps = 0;
        this->targetRPS = targetRPS;
        this->targetAcceleration = getIntrinsicA();
        this->aNoise = 0;
        this->homeTol = homeTol;
        
        home = false;

        if(startAngle >= 360 - homeTol || startAngle < homeTol) {
            home = true;
        }

        running = true;
        runThread.reset(new std::thread(&Motor::run, this));
    }

    void setSpeed(double targetRPS) {
        this->targetRPS = targetRPS;
    }

    bool isHome() {
        return home;
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
        printf("MOTOR RUNNING\n");
        long long usToS = 1000000;
        
        auto start = std::chrono::high_resolution_clock::now();
        while(running) {
            auto elapsed = std::chrono::high_resolution_clock::now() - start;
            long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            start = std::chrono::high_resolution_clock::now();

            double newAngle = ((double)(this->rps * (double)microseconds) / (double)usToS) * 360.0 + angle;

            //printf("%i\n", microseconds);
            //printf("%f\n", newAngle);
            //printf("curr rps %f\n", rps);
            //printf("%f\n\n", this->rps);

            double currentSpeed = ((newAngle - angle) / ((double)microseconds / (double)usToS)) / 360;
            //printf("cs, : %f\n", newAngle - angle);
            //printf("CURRENT SPEED: %f rps\n", currentSpeed);
            angle = newAngle;

            if(currentSpeed >= targetRPS) {
                this->targetAcceleration = -getIntrinsicA();
            }
            else {
                this->targetAcceleration = getIntrinsicA();
            }

            this->rps += getCurrentA() * (double)((double)microseconds / (double)usToS);
            //printf("%f\n", getCurrentA());
            //printf("%f\n", (double)((double)microseconds / (double)usToS));

            if(angle >= 360.0) {
                angle -= 360.0;
            }

            if(angle >= 360 - homeTol || angle <= homeTol) {
                //printf("HOME!\n");
                home = true;
            }
            else {
                home = false;
            }

            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }
    std::unique_ptr<std::thread> runThread;
};

#endif