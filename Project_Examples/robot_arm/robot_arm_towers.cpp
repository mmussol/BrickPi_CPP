/*
 * These files have been made available online through a Creative Commons Attribution-ShareAlike 3.0  license.
 * (http://creativecommons.org/licenses/by-sa/3.0/)
 *
 */
#include <iostream>
#include <string>
#include "BrickPi.h"

using namespace std;

/*********************************
 * Robot arm using NXT motors.
 * See this link for assembly instructions:
 *
 * www.nxtprograms.com/robot_arm/steps.html
 *
 * g++ -o robot_arm_towers robot_arm_towers.cpp -lrt -lm -L/usr/local/lib -lwiringPi -I../../Drivers
 *
 *********************************/

typedef enum {
    Color_Black = 1,
    Color_Blue,
    Color_Green,
    Color_Yellow,
    Color_Red,
    Color_White
} color_t;

static string colorName[] = 
    { "t", "Black", "Blue", "Green", "Yellow", "Red", "White" };

class Robot
{
public:
    Robot(BrickPi *bp);
    ~Robot();
    int setup(color_t initColor);
    int move(color_t newColor);

private:
    BrickPi *brickPi;
    color_t currColor;
    BrickPi_Motor *leftRightMotor;
    BrickPi_Motor *upDownMotor;
    BrickPi_Motor *clawMotor;
    BrickPi_Sensor *colorSensor;
    BrickPi_Sensor *dummyColorSensor;
};

Robot::Robot(BrickPi *bp)
{
    brickPi = bp;
    currColor = Color_White;
}

Robot::~Robot()
{
    if (leftRightMotor) brickPi->motorRemove(leftRightMotor);
    if (upDownMotor) brickPi->motorRemove(upDownMotor);
    if (clawMotor) brickPi->motorRemove(clawMotor);
    if (colorSensor) brickPi->sensorRemove(colorSensor);
    if (dummyColorSensor) brickPi->sensorRemove(dummyColorSensor);
}

int Robot::setup(color_t initColor)
{
    currColor = initColor;
    leftRightMotor = brickPi->motorAdd(MotorPort_A);
    upDownMotor = brickPi->motorAdd(MotorPort_B);
    clawMotor = brickPi->motorAdd(MotorPort_C);
    // Connect color sensor to PORT_3
    colorSensor = brickPi->sensorAdd(SensorPort_3, SensorType_ColorFull);
    // Work-around (see www.dexterindustries.com/topic/problems-with-lego-color-sensor/
    dummyColorSensor = brickPi->sensorAdd(SensorPort_4, SensorType_ColorFull);

    if (brickPi->setupSensors()) {
        cout << "Setup sensors failed." << endl;
        return -1;
    }
    brickPi->setTimeout(5000);

    return 0;
}

/**
 * Move to the specified color.
 * From the robot's perspective, the colors from left to right are:
 *   BLACK -> RED -> GREEN -> BLUE -> YELLOW
 */
int Robot::move(color_t newColor)
{
    int result = 0;
    bool goLeft = false;
    bool goRight = false;

    printf("Moving to %s\n", colorName[newColor].c_str());

    if (newColor == currColor) return 0;
    switch (currColor) {
        case Color_Blue:
        case Color_Yellow:
            goLeft = true;
            break;
        case Color_Black:
        case Color_Red:
            goRight = true;
            break;
        case Color_Green:
            if (newColor == Color_Red) goLeft = true;
            else goRight = true;
            break;
        default:
            return -1;
            break;
    }

    if (goRight) {
        leftRightMotor->setSpeed(-80);
    } else if (goLeft) {
        leftRightMotor->setSpeed(80);
    }
    result = brickPi->updateValues();
    if (result) {
        cout << "Update values failed." << endl;
        return -1;
    }
    while (true) {
        result = brickPi->updateValues();
        if (!result) {
            color_t value = (color_t) colorSensor->getValue();
            if (value == newColor) {
                leftRightMotor->setSpeed(0);
                result = brickPi->updateValues();
                break;
            }
        }
        usleep(5000); // Sleep 5 msec
    }
    if (result == 0) {
        currColor = newColor;
    }
    return result;
}

int main()
{
    char inp;
    int result;
    BrickPi brickPi;
    Robot robot(&brickPi);
    color_t initColor = Color_Black;

    cout << "Enter starting color [k=black, r=red, g=green, b=blue, y=yellow] : ";
    cin >> inp; // Take input from the terminal
    if (inp == 'k') initColor = Color_Black;
    else if (inp == 'r') initColor = Color_Red;
    else if (inp == 'g') initColor = Color_Green;
    else if (inp == 'b') initColor = Color_Blue;
    else if (inp == 'y') initColor = Color_Yellow;
    else {
        printf("Invalid color\n");
        return -1;
    }
    result = robot.setup(initColor);
    if (result) {
        cout << "Setup failed." << endl;
        return -1;
    }
    while (true) {
        cout << "Enter value [r=red, g=green, b=blue] : ";
        cin >> inp; // Take input from the terminal
        // Move the bot
        if (inp == 'r') {
            cout << "Moving to red" << endl;
            result = robot.move(Color_Red);
        } else if (inp == 'g') {
            cout << "Moving to green" << endl;
            result = robot.move(Color_Green);
        } else if (inp == 'b') {
            cout << "Moving to blue" << endl;
            result = robot.move(Color_Blue);
        }
        if (result) {
            cout << "Move failed." << endl;
        }
    }
    return 0;
}

