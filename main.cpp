/*
monitor changes frames at 60 Hz
motor spins at 15 Hz
signal home every 4th (60/15) MONITOR frame to keep motor and monitor in sync

monitor changes frames at 240 Hz
motor spins at 15 Hz
signal home every 16th (240/15) MONITOR frame to keep motor and monitor in sync

THE ACTUAL ALGORITHM
PART A:
-if homes are ALMOST synced and speed is within tolerance:
    -If motor homed AFTER the monitor home signal, speed up
    -If motor homed BEFORE the monitor home signal, slow down
    -If time between motor home and monitor home is too great, do part B
-if speed is not within tolerance, do Part B

PART B:
    -measure interval between homes from monitor
    -measure interval between from motor
    -speed up or slow down accordingly, until the intervals are the same
    -once the intervals match, slow down slighly so that they're off
*/

#include <iostream>

#include "Arduino.h"

using namespace std;

int main(void) {
    //Motor motor(15, 1, 0.01, 0, 1.0);
    Arduino ard;
}