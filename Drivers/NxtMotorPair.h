/*
 * These files have been made available online through a Creative Commons Attribution-ShareAlike 3.0  license.
 * (http://creativecommons.org/licenses/by-sa/3.0/)
 *
 *  This is a library of functions for the RPi to communicate with the BrickPi.
 */
#ifndef __NxtMotorPair_h__
#define __NxtMotorPair_h__

#include "BrickPi.h"

class NxtMotorPair
{
public:
    NxtMotorPair(BrickPi *brickPi, BrickPi_MotorPort left, BrickPi_MotorPort right);
    ~NxtMotorPair();
    void forward(int speed);
    void backward(int speed);
    void stop();
    void left(int speed);
    void right(int speed);
    void setSpeed(int leftSpeed, int rightSpeed);

private:
    BrickPi *brickPi;
    BrickPi_Motor *leftMotor;
    BrickPi_Motor *rightMotor;
};

NxtMotorPair::NxtMotorPair(BrickPi *brickPi, BrickPi_MotorPort left, BrickPi_MotorPort right)
{
    this->brickPi = brickPi;
    this->leftMotor = brickPi->motorAdd(left);
    this->rightMotor = brickPi->motorAdd(right);
}

NxtMotorPair::~NxtMotorPair()
{
    brickPi->motorRemove(leftMotor);
    brickPi->motorRemove(rightMotor);
}

void NxtMotorPair::forward(int speed)
{
    setSpeed(speed, speed);
}

void NxtMotorPair::backward(int speed)
{
    setSpeed(-speed, -speed);
}

void NxtMotorPair::stop()
{
    setSpeed(0, 0);
}

void NxtMotorPair::left(int speed)
{
    setSpeed(speed, -speed);
}

void NxtMotorPair::right(int speed)
{
    setSpeed(-speed, speed);
}

void NxtMotorPair::setSpeed(int leftSpeed, int rightSpeed)
{
    leftMotor->setSpeed(leftSpeed);
    rightMotor->setSpeed(rightSpeed);
}

#endif // __NxtMotorPair_h__
