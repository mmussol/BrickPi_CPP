/*
 * These files have been made available online through a Creative Commons Attribution-ShareAlike 3.0  license.
 * (http://creativecommons.org/licenses/by-sa/3.0/)
 *
 *  This is a library of functions for the RPi to communicate with the BrickPi.
 */
#ifndef __PspController_h_
#define __PspController_h_

// Mindsensors PSP Controller

#include "BrickPi.h"

class PspController {
public:
    PspController(BrickPi *brickPi, BrickPi_SensorPort port, unsigned char addr = 0x02);
    ~PspController();
    void update();

    // Buttons
    unsigned char l1;
    unsigned char l2;
    unsigned char r1;
    unsigned char r2;
    unsigned char a;
    unsigned char b;
    unsigned char c;
    unsigned char d;
    unsigned char tri;
    unsigned char sqr;
    unsigned char cir;
    unsigned char cro;
    unsigned char ljb;  // joystick button state
    unsigned char rjb;  // joystick button state

    int ljx;   // analog value of joystick scaled from -127 to 127
    int ljy;   // analog value of joystick scaled from -127 to 127
    int rjx;   // analog value of joystick scaled from -127 to 127
    int rjy;   // analog value of joystick scaled from -127 to 127

private:
    BrickPi *brickPi;
    BrickPi_SensorPort port;
    BrickPi_Sensor *sensor;
    BrickPi_I2C_Device *device;
};

PspController::PspController(BrickPi *brickPi, BrickPi_SensorPort port, unsigned char addr)
{
    unsigned char speed = 6;
    unsigned char settings = 0;

    this->brickPi = brickPi;
    this->port = port;
    sensor = brickPi->sensorAdd(port, SensorType_I2C);
    sensor->setI2CSpeed(speed);
    device = sensor->addI2CDevice(addr, settings);
}

PspController::~PspController()
{
    sensor->removeI2CDevice(device);
    brickPi->sensorRemove(sensor);
}

void PspController::update()
{
    device->write = 1; // number of bytes to write
    device->read = 6; // number of bytes to read
    device->out[0] = 0x42; // byte to write

    brickPi->updateValues();

    // Left and right joystick button press
    ljb = ~(device->in[0] >> 1) & 0x01;
    rjb = ~(device->in[0] >> 2) & 0x01;

    // For buttons a,b,c,d
    d = ~(device->in[0] >> 4) & 0x01;
    c = ~(device->in[0] >> 5) & 0x01;
    b = ~(device->in[0] >> 6) & 0x01;
    a = ~(device->in[0] >> 7) & 0x01;

    // For buttons l1,l2,r1,r2
    l2 = ~(device->in[1] ) & 0x01;
    r2 = ~(device->in[1] >> 1) & 0x01;
    l1 = ~(device->in[1] >> 2) & 0x01;
    r1 = ~(device->in[1] >> 3) & 0x01;

    // For buttons square,triangle,cross,circle
    tri = ~(device->in[1] >> 4) & 0x01;
    cir = ~(device->in[1] >> 5) & 0x01;
    cro = ~(device->in[1] >> 6) & 0x01;
    sqr = ~(device->in[1] >> 7) & 0x01;

    // Left joystick x and y , -127 to 127
    ljx = -1*((device->in[5] & 0xff) - 128);
    ljy = (device->in[4] & 0xff) - 128;

    // Right joystick x and y , -127 to 127
    rjx = -1*((device->in[3] & 0xff) - 128);
    rjy = (device->in[2] & 0xff) - 128;
}

#endif // __PspController_h_
