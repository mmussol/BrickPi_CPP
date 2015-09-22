/*
 * These files have been made available online through a Creative Commons Attribution-ShareAlike 3.0  license.
 * (http://creativecommons.org/licenses/by-sa/3.0/)
 *
 *  This is a library of functions for the RPi to communicate with the BrickPi.
 */
#include <iostream>
#include "BrickPi.h"
#include "NxtMotorPair.h"

using namespace std;

// g++ -o simplebot simplebot_simple.cpp -lrt -lm -L/usr/local/lib -lwiringPi -I../../Drivers

int main()
{
    char inp;
    int result, speed = 200;
    BrickPi brickPi;
    NxtMotorPair motorPair(&brickPi, MotorPort_B, MotorPort_C);
    result = brickPi.setupSensors();
    if (result) {
        cout << "Setup sensors failed." << endl;
        return -1;
    }
    brickPi.setTimeout(3000);
    while (true) {
        cout << "Enter value [w=fwd, a=left, d=right, s=back, x=stop] : ";
        cin >> inp; // Take input from the terminal
        // Move the bot
        if (inp == 'w') {
            cout << "Moving Forward" << endl;
            motorPair.forward(speed);
        } else if (inp == 'a') {
            cout << "Moving Left" << endl;
            motorPair.left(speed);
        } else if (inp == 'd') {
            cout << "Moving Right" << endl;
            motorPair.right(speed);
        } else if (inp == 's') {
            cout << "Moving Backwards" << endl;
            motorPair.backward(speed);
        } else if (inp == 'x') {
            cout << "Stop" << endl;
            motorPair.stop();
        }
        result = brickPi.updateValues();
        if (result) {
            cout << "Update values failed." << endl;
            return -1;
        }
        usleep(10000); // Sleep for 10 msec
    }
    return 0;
}
