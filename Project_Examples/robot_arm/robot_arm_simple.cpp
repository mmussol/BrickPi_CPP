/*
 * These files have been made available online through a Creative Commons Attribution-ShareAlike 3.0  license.
 * (http://creativecommons.org/licenses/by-sa/3.0/)
 *
 */
#include <iostream>
#include "BrickPi.h"

using namespace std;

/*********************************
 * Robot arm using NXT motors.
 * See this link for assembly instructions:
 *
 * www.nxtprograms.com/robot_arm/steps.html
 */
 * g++ -o robot_arm_simple robot_arm_simple.cpp -lrt -lm -L/usr/local/lib -lwiringPi -I../../Drivers
 *
 *********************************/

int main()
{
    char inp;
    int result;
    BrickPi brickPi;
    BrickPi_Motor *leftRightMotor = brickPi.motorAdd(MotorPort_A);
    BrickPi_Motor *upDownMotor = brickPi.motorAdd(MotorPort_B);
    BrickPi_Motor *clawMotor = brickPi.motorAdd(MotorPort_C);
    result = brickPi.setupSensors();
    if (result) {
        cout << "Setup sensors failed." << endl;
        return -1;
    }
    brickPi.setTimeout(2000);
    while (true) {
        cout << "Enter value [l=left, r=right, u=up, d=down, o=open, c=close] : ";
        cin >> inp; // Take input from the terminal
        // Move the bot
        if (inp == 'u') {
            cout << "Moving Up" << endl;
            upDownMotor->setSpeed(150);
        } else if (inp == 'd') {
            cout << "Moving Down" << endl;
            upDownMotor->setSpeed(-100);
        } else if (inp == 'l') {
            cout << "Moving Left" << endl;
            leftRightMotor->setSpeed(100);
        } else if (inp == 'r') {
            cout << "Moving Right" << endl;
            leftRightMotor->setSpeed(-100);
        } else if (inp == 'o') {
            cout << "Open" << endl;
            clawMotor->setSpeed(50);
        } else if (inp == 'c') {
            cout << "Close" << endl;
            clawMotor->setSpeed(-50);
        }
        result = brickPi.updateValues();
        if (result) {
            cout << "Update values failed." << endl;
            return -1;
        }
        usleep(500000);
        upDownMotor->setSpeed(0);
        leftRightMotor->setSpeed(0);
        clawMotor->setSpeed(0);
        result = brickPi.updateValues();
    }
    return 0;
}
