/*
 * These files have been made available online through a Creative Commons Attribution-ShareAlike 3.0  license.
 * (http://creativecommons.org/licenses/by-sa/3.0/)
 *
 *  This is a library of functions for the RPi to communicate with the BrickPi.
 */
#include <iostream>
#include "BrickPi.h"
#include "NxtMotorPair.h"
#include "PspController.h"

using namespace std;

// g++ -o simplebot_psp simplebot_psp.cpp -lrt -lm -L/usr/local/lib -lwiringPi -I../../Drivers

int main()
{
    int result, speedLeft, speedRight;
    BrickPi brickPi;
    NxtMotorPair motorPair(&brickPi, MotorPort_B, MotorPort_C);
    PspController psp(&brickPi, SensorPort_1);
    result = brickPi.setupSensors();
    if (result) {
        cout << "Setup sensors failed." << endl;
        return -1;
    }
    brickPi.setTimeout(3000);
    while (true) {
        psp.update();
        speedLeft = psp.ljy + psp.ljx;
        speedRight = psp.rjy + psp.rjx;
        motorPair.setSpeed(speedLeft * 2, speedRight * 2); // Move the bot
        result = brickPi.updateValues();
        if (result) {
            cout << "Update values failed." << endl;
            return -1;
        }
        usleep(10000); // Sleep for 10 msec
    }
    return 0;
}
