#include <chrono>
#include <ctime>
#include <stdlib.h> 
#include <atomic>

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
    }

    void setSpeed(double targetRPS) {
        this->targetRPS = targetRPS;
    }

    void run() {
        long long usToS = 1000000;

        while(running) {
            auto start = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::high_resolution_clock::now() - start;
            long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

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
        }
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
};