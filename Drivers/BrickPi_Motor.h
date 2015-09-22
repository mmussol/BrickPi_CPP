/*
 * These files have been made available online through a Creative Commons Attribution-ShareAlike 3.0  license.
 * (http://creativecommons.org/licenses/by-sa/3.0/)
 *
 *  This is a library of functions for the RPi to communicate with the BrickPi.
 */
#ifndef __BrickPi_Motor_h_
#define __BrickPi_Motor_h_

#include "BrickPi_Msg.h"

typedef enum {
    MotorPort_A,
    MotorPort_B,
    MotorPort_C,
    MotorPort_D,
    MotorPort_Max
} BrickPi_MotorPort;

class BrickPi;

class BrickPi_Motor {
public:
    BrickPi_Motor(BrickPi *brickPi);
    ~BrickPi_Motor();
    void enable();
    void disable();
    void setSpeed(int speed);
    void addBits(BrickPi_Msg *msg);
private:
    BrickPi *brickPi;
    bool enabled;
    bool backwards;
    int speed;
};

BrickPi_Motor::BrickPi_Motor(BrickPi *brickPi)
{
    this->brickPi = brickPi;
}

BrickPi_Motor::~BrickPi_Motor()
{
}

void BrickPi_Motor::enable()
{
   enabled = true;
}

void BrickPi_Motor::disable()
{
   enabled = false;
}

void BrickPi_Motor::setSpeed(int speed)
{
    if (speed < 0) {
        backwards = true;
        speed *= (-1);
    } else {
        backwards = false;
    }
    if (speed > 255) {
        speed = 255;
    }
    this->speed = speed;
}

/* Motor bits (10-bit value)
 *   SSSSSSSSDE
 *   S = speed (0 - 255)
 *   D = direction (1 = backwards, 0 = forwards)
 *   E = enable (1 = enabled, 0 = disabled)
 */
void BrickPi_Motor::addBits(BrickPi_Msg *msg)
{
    unsigned long val;
    val = (speed & 0xFF) << 2;
    val |= (backwards ? 1 : 0) << 1;
    val |= (enabled ? 1 : 0);
    msg->addBits(10, (val & 0x3FF));
}

#endif // __BrickPi_Motor_h_
