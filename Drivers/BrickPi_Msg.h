/*
 * These files have been made available online through a Creative Commons Attribution-ShareAlike 3.0  license.
 * (http://creativecommons.org/licenses/by-sa/3.0/)
 *
 *  This is a library of functions for the RPi to communicate with the BrickPi.
 */
#ifndef __BrickPi_Msg_h_
#define __BrickPi_Msg_h_

#include <stdio.h>
#include <stdexcept>
#include <wiringSerial.h>
#include "tick.h"

using std::cout;
using std::endl;

typedef enum {
    MsgType_ChangeAddr = 1, // Change the UART address
    MsgType_SensorType = 2, // Change/set the sensor type
    MsgType_Values = 3, // Set the motor speed/direction, return sensors/encoders
    MsgType_Stop = 4, // Float motors immediately
    MsgType_Timeout = 5 // Set the timeout
} MsgType;

class BrickPi_Msg {
public:
    BrickPi_Msg();
    ~BrickPi_Msg();
    void reset();
    void addByte(unsigned char val);
    void addBits(int numBits, unsigned long val);
    unsigned long getBits(int numBits);
    int tx(unsigned char addr);
    int rx(long timeout);
    void print();
    unsigned char msgBuffer[256];
private:
    int uartFd;
    unsigned char txBuffer[256];
    unsigned char rxBuffer[256];
    int bitCount;
};

BrickPi_Msg::BrickPi_Msg()
{
    uartFd = serialOpen("/dev/ttyAMA0", 500000);
    if (uartFd == -1) {
        throw std::runtime_error("Failed to open serial port");
    }
    reset();
}

BrickPi_Msg::~BrickPi_Msg()
{
    if (uartFd >= 0) {
        serialClose(uartFd);
    }
}

void BrickPi_Msg::reset()
{
    for (int i = 0; i < 256; i++) {
        msgBuffer[i] = 0;
    }
    bitCount = 0;
}

void BrickPi_Msg::addByte(unsigned char val)
{
    msgBuffer[(bitCount / 8)] = val;
    bitCount += 8;
}

void BrickPi_Msg::addBits(int numBits, unsigned long val)
{
    for (int i = 0; i < numBits; i++) {
        if (val & 0x1) {
            msgBuffer[((bitCount + i) / 8)] |= (0x1 << ((bitCount + i) % 8));
        }
        val /= 2;
    }
    bitCount += numBits;
}

unsigned long BrickPi_Msg::getBits(int numBits)
{
    unsigned long result = 0;
    char i = numBits;
    while(i){
        result *= 2;
        result |= ((msgBuffer[((bitCount + (i - 1)) / 8)] >> ((bitCount + (i - 1)) % 8)) & 0x01);
        i--;
    }
    bitCount += numBits;
    return result;
}

int BrickPi_Msg::tx(unsigned char addr)
{
    int i;
    unsigned char byteCount = ((bitCount + 7) / 8);
    if (byteCount >= 253) return -1;

    txBuffer[0] = addr;
    txBuffer[1] = addr + byteCount;
    txBuffer[2] = byteCount;

    for (i = 0; i < byteCount; i++) {
        txBuffer[1] += msgBuffer[i];
        txBuffer[i + 3] = msgBuffer[i];
    }
    for (i = 0; i < (byteCount + 3); i++) {
        serialPutchar(uartFd, txBuffer[i]);
    }
}

int BrickPi_Msg::rx(long timeout)
{
    int i, result;
    unsigned char checkSum, rxBytes = 0;
    unsigned long originalTick = CurrentTickUs();

    while (serialDataAvail(uartFd) <= 0) {
        if (timeout && ((CurrentTickUs() - originalTick) >= timeout)) {
            return -2;
        }
        usleep(50);
    }

    while (rxBytes < serialDataAvail(uartFd)) {
        // If it's been 1 ms since the last data was received,
        // assume it's the end of the message.
        rxBytes = serialDataAvail(uartFd);
        usleep(75);
    }

    for (i = 0; i < rxBytes; i++) {
        result = serialGetchar(uartFd);
        if (result < 0) {
            return -1;
        } else {
            rxBuffer[i] = result;
        }
    }

    if (rxBytes < 2) return -4;
    if (rxBytes < (rxBuffer[1] + 2)) return -6;

    checkSum = rxBuffer[1];
    for (i = 0; i < (rxBytes - 2); i++) {
        checkSum += rxBuffer[i + 2];
        msgBuffer[i] = rxBuffer[i + 2];
    }

    if (checkSum != rxBuffer[0]) return -5;
    bitCount = 0;
    return (rxBytes - 2);
}

void BrickPi_Msg::print()
{
    int i;
    unsigned char byteCount = ((bitCount + 7) / 8);

    printf("--- %d bytes ---\n", byteCount);

    for (i = 0; i < byteCount; i++) {
        printf(" %d : 0x%02x\n", i, msgBuffer[i]);
    }
}

#endif // __BrickPi_Msg_h_
