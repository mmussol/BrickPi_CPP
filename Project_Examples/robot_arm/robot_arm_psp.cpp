/*
 * These files have been made available online through a Creative Commons Attribution-ShareAlike 3.0  license.
 * (http://creativecommons.org/licenses/by-sa/3.0/)
 *
 */
#include <iostream>
#include "BrickPi.h"
#include "PspController.h"

using namespace std;

/*********************************
 * Robot arm using NXT motors. Uses Mindsensor PSP controller.
 * See this link for assembly instructions:
 *
 * www.nxtprograms.com/robot_arm/steps.html
 *
 * g++ -o robot_arm_psp robot_arm_psp.cpp -lrt -lm -L/usr/local/lib -lwiringPi -I../../Drivers
 *
 *********************************/

int main()
{
    char inp;
    int result;
    BrickPi brickPi;
    BrickPi_Motor *leftRightMotor = brickPi.motorAdd(MotorPort_A);
    PspController psp(&brickPi, SensorPort_1);
    BrickPi_Motor *upDownMotor = brickPi.motorAdd(MotorPort_B);
    BrickPi_Motor *clawMotor = brickPi.motorAdd(MotorPort_C);
    result = brickPi.setupSensors();
    if (result) {
        cout << "Setup sensors failed." << endl;
        return -1;
    }
    brickPi.setTimeout(2000);
    while (true) {
        psp.update();
        upDownMotor->setSpeed(psp.rjx);
        leftRightMotor->setSpeed(-psp.rjy);
        clawMotor->setSpeed(-psp.ljy);
        result = brickPi.updateValues();
        if (result) {
            cout << "Update values failed." << endl;
            return -1;
        }
        usleep(10000); // Sleep for 10 msec
    }
    return 0;
}
