/*
 * Copyright 2020 Team EchoSense 
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ECHOSENSE_FIRMWARE_LIDARROTATING_H
#define ECHOSENSE_FIRMWARE_LIDARROTATING_H

#include <mbed.h>
#include <LIDARLite_v3HP.h>

#define TRIGGER_SPEED_KPH 5 /* km/h */
#define TRIGGER_SPEED (1000 / 36) * TRIGGER_SPEED_KPH /* cm/s */

#define MAX_SPEED_KPH 60 /* km/h */
#define MAX_SPEED (1000 / 36) * MAX_SPEED_KPH /* cm/s */

#define MAX_PERIOD 1000000
#define MIN_PERIOD 100000

#define FREQUENCY_NUMERATOR (1<<28)

#define LIDAR_STRIPS 25 // number of strips to do detection calculations on
#define LIDAR_DETECTION_WIDTH 500 // width of detection area in cm

extern EventQueue eventQueue;

class lidarRotating {
public:
    lidarRotating(I2C *i2c, DigitalOut *motor, InterruptIn *rotationSensor, const Callback<void()> *notifyCallback);

    void start();

private:
    LIDARLite_v3HP *lidar;
    Timer *lidarTimer;

    I2C *i2c;
    DigitalOut *motor;
    InterruptIn *rotationSensor;
    const Callback<void()> *notifyCallback;

    uint32_t rotationPeriod    = 0; // time it takes for the lidar sensor to make one rotation
    uint32_t rotationFrequency = 0; // fixed point rotation frequency, FREQUENCY_NUMERATOR / rotationPeriod

    uint16_t distanceBufferNow[LIDAR_STRIPS];
    uint16_t distanceBufferPrev[LIDAR_STRIPS];

    void takeReading();
    void printRotationTime();
};


#endif //ECHOSENSE_FIRMWARE_LIDARROTATING_H
