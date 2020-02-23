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

#include "lidarRotating.h"

lidarRotating::lidarRotating(I2C *i2c, DigitalOut *motor, InterruptIn *rotationSensor, EventQueue *eventQueue, const Callback<
    void()> *notifyCallback) {
    this->i2c            = i2c;
    this->motor          = motor;
    this->rotationSensor = rotationSensor;
    this->eventQueue     = eventQueue;
    this->notifyCallback = notifyCallback;

    lidarTimer = new Timer();
    lidar      = new LIDARLite_v3HP(i2c);
}

void lidarRotating::start() {
    // setup LIDAR
    lidar->configure(1, 1);
    lidar->resetReferenceFilter();

    // start motor
    motor->write(1);

    // start timer
    lidarTimer->start();

    // setup interrupts
    rotationSensor->fall(callback(this, &lidarRotating::rotationInterrupt));
}

void lidarRotating::rotationInterrupt() {
    rotationPeriod = lidarTimer->read_us();
    lidarTimer->reset();

    rotationFrequency = LIDAR_FREQUENCY_NUMERATOR / rotationPeriod;
    if (rotationPeriod >= MAX_PERIOD || rotationPeriod <= MIN_PERIOD) {
        return;
    }
    uint32_t scanStartTime_us = ((rotationPeriod) * (1 << 8 - LIDAR_ANGLE_RANGE)) >> 9;
    uint32_t scanStopTime_us  = ((rotationPeriod) * (1 << 8 + LIDAR_ANGLE_RANGE)) >> 9;

    scanStartTime_ms = scanStartTime_us / 1000;
    scanStopTime_ms  = scanStopTime_us / 1000;

    eventQueue->call_in(scanStartTime_ms, callback(this, &lidarRotating::scanStart));
    eventQueue->call_in(scanStopTime_ms, callback(this, &lidarRotating::scanStop));
}

void lidarRotating::scanStart() {
    for (uint16_t i; i < LIDAR_STRIPS; i++) {
        distanceBufferNow[i]  = UINT16_MAX;
        distanceBufferPrev[i] = UINT16_MAX;
    }

    scanJobID = eventQueue->call_every(SAMPLE_RATE, callback(this, &lidarRotating::takeReading));
}

void lidarRotating::scanStop() {
    eventQueue->cancel(scanJobID);
}

void lidarRotating::takeReading() {
    if (lidar->getBusyFlag() == 1) {
        return;
    }
    lidar->takeRange();
    uint32_t distance = lidar->readDistance();
    uint32_t time_us  = lidarTimer->read_us();
    uint32_t theta    = (time_us * rotationFrequency) >> (LIDAR_FREQUENCY_NUMERATOR_BITS - TRIG_LUT_SIZE_BITS);

    // rotate reference frame 180°, so that 0° point is in the middle of the scanning range
    theta += (TRIG_LUT_SIZE * 2);
    while (theta >= TRIG_LUT_SIZE * 4) {
        theta -= TRIG_LUT_SIZE * 4; // 0 to 255
    }

    // calculate X and Y coordinates of the LIDAR hit
    int32_t distX = (lut_sin(theta) * distance) >> TRIG_LUT_MAGNITUDE_BITS;
    int32_t distY = (lut_cos(theta) * distance) >> TRIG_LUT_MAGNITUDE_BITS;

    distX += (LIDAR_SCAN_WIDTH >> 1);

    if (distY > LIDAR_SCAN_DEPTH || distX > LIDAR_SCAN_WIDTH || distX < 0) {
        // lidar hit is outside the detection area, so we can ignore it
        return;
    }

    // calculate which "strip" we are looking at
    uint16_t strip = distX / LIDAR_STRIPS;
    if (strip >= LIDAR_STRIPS) {
        return;
    }

    // store distance into that strip if less than what is stored
    if (distY < distanceBufferNow[strip]) {
        distanceBufferNow[strip] = distY;
    }
}