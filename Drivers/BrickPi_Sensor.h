/*
 * These files have been made available online through a Creative Commons Attribution-ShareAlike 3.0  license.
 * (http://creativecommons.org/licenses/by-sa/3.0/)
 *
 *  This is a library of functions for the RPi to communicate with the BrickPi.
 */
#ifndef __BrickPi_Sensor_h_
#define __BrickPi_Sensor_h_

#include <stdio.h>
#include <list>
#include "BrickPi_Msg.h"

using std::list;

typedef enum {
    SensorPort_1,
    SensorPort_2,
    SensorPort_3,
    SensorPort_4,
    SensorPort_5,
    SensorPort_Max
} BrickPi_SensorPort;

typedef enum {
    SensorType_Raw = 0,
    SensorType_Touch = 32,
    SensorType_UltraSonicCont = 33,
    SensorType_UltraSonicSS = 34,
    SensorType_RcxLight = 35,
    SensorType_ColorFull = 26,
    SensorType_ColorRed = 37,
    SensorType_ColorGreen = 38,
    SensorType_ColorBlue = 39,
    SensorType_ColorNone = 40,
    SensorType_I2C = 41,
    SensorType_I2C_9V = 42
} BrickPi_SensorType;

#define BIT_I2C_MID  0x01  // Do one of those funny clock pulses between writing and reading.
#define BIT_I2C_SAME 0x02  // The transmit data, and the number of bytes to read and write isn't going to change.

#define INDEX_RED   0
#define INDEX_GREEN 1
#define INDEX_BLUE  2
#define INDEX_BLANK 3

class BrickPi_I2C_Device {
public:
    unsigned char addr;     // The I2C address of each device on each bus.  
    unsigned char settings; // I2C settings
    unsigned char write;    // How many bytes to write
    unsigned char read;     // How many bytes to read
    unsigned char out[16];  // The I2C bytes to write
    unsigned char in[16];   // The I2C input buffers
};

class BrickPi;

class BrickPi_Sensor {
public:
    BrickPi_Sensor(BrickPi *brickPi);
    ~BrickPi_Sensor();

    void setType(BrickPi_SensorType type);
    void setI2CSpeed(unsigned char i2cSpeed);
    BrickPi_SensorType getType();
    long getValue();
    long getValue(int index);

    void setup(BrickPi_Msg *msg);
    void addBits(BrickPi_Msg *msg);
    void getBits(BrickPi_Msg *msg);

    BrickPi_I2C_Device *addI2CDevice(unsigned char addr, unsigned char settings);
    void removeI2CDevice(BrickPi_I2C_Device *device);

private:
    BrickPi *brickPi;
    BrickPi_SensorType type;
    long value;
    long values[4];
    unsigned char speed; // I2C speed.
    list <BrickPi_I2C_Device *> devices;
};

BrickPi_Sensor::BrickPi_Sensor(BrickPi *brickPi)
{
    this->brickPi = brickPi;
    type = SensorType_Raw;
    speed = 0;
    memset(&values, 0, sizeof(values));
}

BrickPi_Sensor::~BrickPi_Sensor()
{
}

void BrickPi_Sensor::setType(BrickPi_SensorType type)
{
    this->type = type;
}

void BrickPi_Sensor::setI2CSpeed(unsigned char i2cSpeed)
{
    speed = i2cSpeed;
}

BrickPi_SensorType BrickPi_Sensor::getType()
{
    return type;
}

long BrickPi_Sensor::getValue()
{
    return value;
}

long BrickPi_Sensor::getValue(int index)
{
    if (index < 4) {
        return values[index];
    }
}

BrickPi_I2C_Device * BrickPi_Sensor::addI2CDevice(unsigned char addr, unsigned char settings)
{
    if ((type == SensorType_I2C || type == SensorType_I2C_9V) && (devices.size() < 8)) {
        BrickPi_I2C_Device *device = new BrickPi_I2C_Device();
        device->addr = addr;
        device->settings = settings;
        devices.push_back(device);
        return device;
    }
    return NULL;
}

void BrickPi_Sensor::removeI2CDevice(BrickPi_I2C_Device *device)
{
    if ((type == SensorType_I2C || type == SensorType_I2C_9V) && devices.size()) {
        devices.remove(device);
        delete device;
    }
}

void BrickPi_Sensor::setup(BrickPi_Msg *msg)
{
    if (type == SensorType_I2C || type == SensorType_I2C_9V) {
        BrickPi_I2C_Device *device;

        msg->addBits(8, speed);
        msg->addBits(3, (devices.size() - 1));

        for (list<BrickPi_I2C_Device *>::iterator it = devices.begin();
             it != devices.end(); it++) {
            device = *it;
            msg->addBits(7, (device->addr >> 1));
            msg->addBits(2, device->settings);
            if (device->settings & BIT_I2C_SAME) {
                msg->addBits(4, device->write);
                msg->addBits(4, device->read);
                unsigned char outByte = 0;
                while (outByte < device->write) {
                    msg->addBits(8, device->out[outByte]);
                    outByte++;
                }
            }
        }
    }
}

void BrickPi_Sensor::addBits(BrickPi_Msg *msg)
{
    if (type == SensorType_I2C || type == SensorType_I2C_9V) {
        BrickPi_I2C_Device *device;

        for (list<BrickPi_I2C_Device *>::iterator it = devices.begin();
             it != devices.end(); it++) {
            device = *it;
            if (!(device->settings & BIT_I2C_SAME)) {
                msg->addBits(4, device->write);
                msg->addBits(4, device->read);
                unsigned char outByte = 0;
                while (outByte < device->write) {
                    msg->addBits(8, device->out[outByte]);
                    outByte++;
                }
            }
        }
    }
}

void BrickPi_Sensor::getBits(BrickPi_Msg *msg)
{
    if (type == SensorType_I2C || type == SensorType_I2C_9V) {
        BrickPi_I2C_Device *device;

        int index = 0;
        long val = msg->getBits(devices.size());
        for (list<BrickPi_I2C_Device *>::iterator it = devices.begin();
             it != devices.end(); it++) {
            device = *it;
            if (val & (0x01 << index++)) {
                unsigned char inByte = 0;
                while (inByte < device->read) {
                    device->in[inByte] = msg->getBits(8);
                    inByte++;
                }
            }
        }
    }
#if 0 // TODO : untested
      else if (type == SensorType_Touch) {
        value = msg->getBits(1);
    } else if (type == SensorType_UltraSonicCont ||
               type == SensorType_UltraSonicSS) {
        value = msg->getBits(8);
    } else if (type == SensorType_ColorFull) {
        value = msg->getBits(3);
        values[INDEX_BLANK] = msg->getBits(10);
        values[INDEX_RED] = msg->getBits(10);
        values[INDEX_GREEN] = msg->getBits(10);
        values[INDEX_BLUE] = msg->getBits(10);
    } else {
        value = msg->getBits(10);
    }
#endif
}

#endif // __BrickPi_Sensor_h_
