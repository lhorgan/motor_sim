#ifndef ARDUINO_H_INCLUDED
#define ARDUINO_H_INCLUDED

#include <chrono>
#include <ctime>
#include <stdlib.h> 
#include <atomic>
#include <thread>

#include "Backlight.h"
#include "Motor.h"

class Arduino {
public:
    Arduino() {
        this->motor.reset(new Motor(10, 0.01, 0, 1.0));
        this->backlight.reset(new Backlight(240, 0));

        start = std::chrono::high_resolution_clock::now();

        setup();

        running = true;
        runThread.reset(new std::thread(&Arduino::run, this));
    }

    ~Arduino() {
        runThread->join();
    }

private:
    std::unique_ptr<std::thread> runThread;
    std::unique_ptr<Backlight> backlight;
    std::unique_ptr<Motor> motor;
    bool running;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;

    unsigned long long micros() { // simulated Arduino micros function
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    }

    void run() {
        while(running) {
            loop();
            std::this_thread::sleep_for(std::chrono::nanoseconds(63)); // Arduino runs at 12.5Mhz
        }
    }    

    long long lastMotorHomeTime;
    long long lastFrameChangeTime;
    long long lastMonitorHomeTime;
    bool motorHome;
    long long monitorHomeTimes[5];
    bool monitorHome;
    long long motorHomeTimes[5];

    int homeTol; // how long do we allow between motor home and monitor home?
    double motorSpeed; // duh
    double speedDelta; // how much to increase the speed by
    double usToS;

    void setup() {
        usToS = 1000000;
        lastMotorHomeTime = 0;
        lastMonitorHomeTime = 0;
        lastFrameChangeTime = 0;
        lastMonitorHomeTime = 0;
        motorHome = false;
        monitorHome = false;

        for(int i = 0; i < 5; i++) {
            monitorHomeTimes[i] = -1;
        }
    }

    void loop() {
        long long currTime = micros();
        
        bool currMonitorHome = isMonitorHome();
        if(currMonitorHome != monitorHome) {
            monitorHome = currMonitorHome;
            if(monitorHome) {
                //printf("THE MONITOR IS HOME!\n");
                lastMonitorHomeTime = currTime;
                addMonitorSpeedMeasurement(currTime);
            }
        }

        bool currMotorHome = isMotorHome();
        if(currMotorHome != motorHome) {
            motorHome = currMotorHome;
            if(motorHome) { // well, now we know the motor IS home
                lastMotorHomeTime = currTime;
                addMotorSpeedMeasurement(currTime);
                double motorSpeed = getMotorSpeed();
                double monitorSpeed = getMonitorSpeed();
                if(motorSpeed > 0 && monitorSpeed > 0) {
                    if(habs(motorSpeed - monitorSpeed) > 0.05) {
                        motor->setSpeed(monitorSpeed);
                    }
                    else { // speeds are synced
                        /*if(habs(lastMotorHomeTime - lastMonitorHomeTime) < (monitorSpeed / 100)) {
                            
                        }*/
                    }
                }
                printf("MONITOR SPEED: %f\n", monitorSpeed);
                printf("MOTOR SPEED: %f\n\n", motorSpeed);
            }
        }
    }

    long long habs(long long i) {
        if(i < 0) {
            return -1 * i;
        }
        return i;
    }

    double habs(double i) {
        if(i < 0) {
            return -1 * i;
        }
        return i;
    }

    double addMonitorSpeedMeasurement(long long time) {
        for(int i = 1; i < 5; i++) {
            monitorHomeTimes[i - 1] = monitorHomeTimes[i];
        }
        monitorHomeTimes[4] = time;
    }

    double getMonitorSpeed() { // homes/sec
        double avgInterval = 0;
        for(int i = 0; i < 5; i++) {
            if(monitorHomeTimes[i] == -1) {
                return -1;
            }
            else if(i >= 1) {
                avgInterval += monitorHomeTimes[i] - monitorHomeTimes[i - 1];
            }
        }
        avgInterval = avgInterval / 4;
        return usToS / avgInterval;
    }

    double addMotorSpeedMeasurement(long long time) {
        for(int i = 1; i < 5; i++) {
            motorHomeTimes[i - 1] = motorHomeTimes[i];
        }
        motorHomeTimes[4] = time;
    }

    double getMotorSpeed() {
        double avgInterval = 0;
        for(int i = 0; i < 5; i++) {
            if(motorHomeTimes[i] == -1) {
                return -1;
            }
            else if(i >= 1) {
                avgInterval += motorHomeTimes[i] - motorHomeTimes[i - 1];
            }
        }
        avgInterval = avgInterval / 4;
        return usToS / avgInterval;
    }

    /**
    * these will all need to be implemented in real Arduino code
    **/
    bool isMotorHome() { // rps (Hz)
        return this->motor->isHome();
    }

    bool isFrameWhite() {
        return this->backlight->isWhite();
    }

    bool isMonitorHome() {
        //printf("%i\n", this->backlight->isStartFrame());
        return this->backlight->isStartFrame();
    }
};

#endif