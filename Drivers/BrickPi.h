/*
 * These files have been made available online through a Creative Commons Attribution-ShareAlike 3.0  license.
 * (http://creativecommons.org/licenses/by-sa/3.0/)
 *
 *  This is a library of functions for the RPi to communicate with the BrickPi.
 */
#ifndef __BrickPi_h_
#define __BrickPi_h_

#include <stdio.h>
#include <string.h>
#include "BrickPi_Motor.h"
#include "BrickPi_Sensor.h"
#include "BrickPi_Msg.h"
#include "tick.h"

class BrickPi
{
public:
    BrickPi(unsigned char addr0 = 1, unsigned char addr1 = 2);
    ~BrickPi();

    int setTimeout(unsigned long timeout);
    int setupSensors(); 
    int updateValues(); 

    BrickPi_Motor *motorAdd(BrickPi_MotorPort port);
    void motorRemove(BrickPi_Motor *motor);

    BrickPi_Sensor *sensorAdd(BrickPi_SensorPort port, BrickPi_SensorType type);
    void sensorRemove(BrickPi_Sensor *sensor);

private:
    unsigned char address[2]; // Communication address
    BrickPi_Msg *msg;
    BrickPi_Motor *motors[MotorPort_Max];
    BrickPi_Sensor *sensors[SensorPort_Max];
};

BrickPi::BrickPi(unsigned char addr0, unsigned char addr1)
{
    address[0] = addr0;
    address[1] = addr1;
    msg = new BrickPi_Msg();
    for (int port = 0; port < MotorPort_Max; port++) {
        motors[port] = new BrickPi_Motor(this);
    }
    for (int port = 0; port < SensorPort_Max; port++) {
        sensors[port] = new BrickPi_Sensor(this);
    }
    ClearTick();
}

BrickPi::~BrickPi()
{
    if (msg) {
        delete msg;
    }
    for (int port = 0; port < MotorPort_Max; port++) {
        delete motors[port];
    }
    for (int port = 0; port < SensorPort_Max; port++) {
        delete sensors[port];
    }
}

int BrickPi::setTimeout(unsigned long timeout)
{
    for (int i = 0; i < 2; i++) {
        msg->reset();
        msg->addByte(MsgType_Timeout);
        msg->addByte( timeout             & 0xFF);
        msg->addByte((timeout / 256     ) & 0xFF);
        msg->addByte((timeout / 65536   ) & 0xFF);
        msg->addByte((timeout / 16777216) & 0xFF);

        msg->tx(address[i]);
        int bytesReceived = msg->rx(2500);
        if (bytesReceived != 1 || (msg->msgBuffer[0] != MsgType_Timeout)) {
            printf("%s %d : Rx failed %d\n", __FUNCTION__, __LINE__, bytesReceived);
            return -1;
        }
    }
    return 0;
}

int BrickPi::setupSensors()
{
    for (int i = 0; i < 2; i++) {
        msg->reset();
        msg->addByte(MsgType_SensorType);
        if (i == 0) {
            msg->addByte(sensors[SensorPort_1]->getType());
            msg->addByte(sensors[SensorPort_2]->getType());
            sensors[SensorPort_1]->setup(msg);
            sensors[SensorPort_2]->setup(msg);
        } else {
            msg->addByte(sensors[SensorPort_3]->getType());
            msg->addByte(sensors[SensorPort_4]->getType());
            sensors[SensorPort_3]->setup(msg);
            sensors[SensorPort_4]->setup(msg);
        }

        msg->tx(address[i]);
        int bytesReceived = msg->rx(5000000);
        if (bytesReceived != 1 || (msg->msgBuffer[0] != MsgType_SensorType)) {
            printf("%s %d : Rx failed %d\n", __FUNCTION__, __LINE__, bytesReceived);
            return -1;
        }
    }
    return 0;
}

int BrickPi::updateValues()
{
    int bytesReceived, retryCount = 0;
    BrickPi_Sensor *sensor;
    BrickPi_I2C_Device *device;

retry:
    for (int i = 0; i < 2; i++) {
        msg->reset();
        msg->addByte(MsgType_Values);
        msg->addBits(2, 0); // disable encoder offset
        // TODO : encoder offsets

        if (i == 0) {
            motors[MotorPort_A]->addBits(msg);
            motors[MotorPort_B]->addBits(msg);
        } else {
            motors[MotorPort_C]->addBits(msg);
            motors[MotorPort_D]->addBits(msg);
        }

        if (i == 0) {
            sensors[SensorPort_1]->addBits(msg);
            sensors[SensorPort_2]->addBits(msg);
        } else {
            sensors[SensorPort_3]->addBits(msg);
            sensors[SensorPort_4]->addBits(msg);
        }

        msg->tx(address[i]);
        bytesReceived = msg->rx(7500);
        if (bytesReceived < 0 || (msg->msgBuffer[0] != MsgType_Values)) {
            if (retryCount < 3) {
                retryCount++;
                goto retry;
            } else {
                printf("%s %d : Rx failed\n", __FUNCTION__, __LINE__);
                return -1;
            }
        }

        (void) msg->getBits(8); // message type

        unsigned char bitsUsed[2] = {0, 0}; // Used for encoder values
        bitsUsed[0] = msg->getBits(5);
        bitsUsed[1] = msg->getBits(5);
        unsigned long Temp_EncoderVal;

        int ii = 0;
        while (ii < 2) {
            Temp_EncoderVal = msg->getBits(bitsUsed[ii]);
#if 0 // TODO
            if(Temp_EncoderVal & 0x01){
                Temp_EncoderVal /= 2;
                BrickPi.Encoder[ii + (i * 2)] = Temp_EncoderVal * (-1);}
            else{
                BrickPi.Encoder[ii + (i * 2)] = (Temp_EncoderVal / 2);}
#endif
            ii++;
        }

        if (i == 0) {
            sensors[SensorPort_1]->getBits(msg);
            sensors[SensorPort_2]->getBits(msg);
        } else {
            sensors[SensorPort_3]->getBits(msg);
            sensors[SensorPort_4]->getBits(msg);
        }
    }
}

BrickPi_Motor * BrickPi::motorAdd(BrickPi_MotorPort port)
{
    motors[port]->enable();
    return motors[port];
}

void BrickPi::motorRemove(BrickPi_Motor *motor)
{
    for (int port = 0; port < MotorPort_Max; port++) {
        if (motors[port] == motor) {
            motors[port]->disable();
            break;
        }
    }
}

BrickPi_Sensor * BrickPi::sensorAdd(BrickPi_SensorPort port, BrickPi_SensorType type)
{
    sensors[port]->setType(type);
    return sensors[port];
}

void BrickPi::sensorRemove(BrickPi_Sensor *sensor)
{
    for (int port = 0; port < SensorPort_Max; port++) {
        if (sensors[port] == sensor) {
            sensors[port]->setType(SensorType_Raw);
            break;
        }
    }
}

#endif // __BrickPi_h_
