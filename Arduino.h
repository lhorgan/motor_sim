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
    long long lastStartFrameTime;
    bool motorHome;

    void setup() {
        lastMotorHomeTime = 0;
        lastFrameChangeTime = 0;
        lastStartFrameTime = 0;
        motorHome = false;
    }

    void loop() {
        long long currTime = micros();

        if(isMotorHome() != motorHome) {
            if(!motorHome) { // well, now we know the motor IS home
                int timeDelta = currTime - lastMotorHomeTime;
                
            }
            motorHome = !motorHome;
        }
    }

    /**
    * these will all need to be implemented in real Arduino code
    **/
    bool isMotorHome() {
        return this->motor->isHome();
    }

    bool isFrameWhite() {
        return this->backlight->isWhite();
    }

    bool isStartFrame() {
        return this->backlight->isStartFrame();
    }
};

#endif