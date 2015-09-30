/*
 * These files have been made available online through a Creative Commons Attribution-ShareAlike 3.0  license.
 * (http://creativecommons.org/licenses/by-sa/3.0/)
 *
 */
#include <iostream>
#include <string>
#include "BrickPi.h"

using namespace std;

// g++ -o LEGO_ColorSensor LEGO_ColorSensor.cpp -lrt -lm -L/usr/local/lib -lwiringPi -I../Drivers

int main()
{
    char inp;
    int result;
    string color[] = { "t", "Black", "Blue", "Green", "Yellow", "Red", "White" };
    BrickPi brickPi;
    // Connect color sensor to PORT_3
    BrickPi_Sensor *colorSensor = brickPi.sensorAdd(SensorPort_3, SensorType_ColorFull);
    // Work-around (see www.dexterindustries.com/topic/problems-with-lego-color-sensor/
    BrickPi_Sensor *dummyColorSensor = brickPi.sensorAdd(SensorPort_4, SensorType_ColorFull);

    result = brickPi.setupSensors();
    if (result) {
        cout << "Setup sensors failed." << endl;
        return -1;
    }
    while (true) {
        result = brickPi.updateValues();
        if (!result) {
            int value = (int) colorSensor->getValue();
            // BrickPi.Sensor returns a number from 1 to 6.
            // The color array contains the corresponding color names
            printf("Results: %d (%s)\n", value, color[value].c_str());
        }
        usleep(500000);
    }
    return 0;
}
