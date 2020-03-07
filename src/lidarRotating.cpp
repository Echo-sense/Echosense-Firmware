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

extern DigitalOut led1;

lidarRotating::lidarRotating(I2C *i2c, DigitalOut *motor, InterruptIn *rotationSensor, EventQueue *eventQueue, Callback<
    void()> notifyCallback) {
    this->i2c            = i2c;
    this->motor          = motor;
    this->rotationSensor = rotationSensor;
    this->eventQueue     = eventQueue;
    this->notifyCallback = notifyCallback;


    lidarTimer = new Timer();
    lidar      = new LIDARLite_v3HP(i2c);

    for (uint16_t i = 0; i < LIDAR_STRIPS; i++) {
        distanceBufferNow[i]  = UINT16_MAX;
    }
	for (uint16_t i = 0; i < LIDAR_PAST; i++){
		for (uint16_t j = 0; j < LIDAR_STRIPS; j++){
			distanceBufferPrev[i][j] = UINT16_MAX;
		}
	}
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

    eventQueue->call(callback(this, &lidarRotating::printRotationTime));
    scanStartTime_us = ((rotationPeriod) * ((1 << 8) - LIDAR_ANGLE_RANGE)) >> 9;
    scanStopTime_us  = ((rotationPeriod) * ((1 << 8) + LIDAR_ANGLE_RANGE)) >> 9;
//
//    scanStartTime_ms = scanStartTime_us / 1000;
//    scanStopTime_ms  = scanStopTime_us / 1000;

    eventQueue->call(callback(this, &lidarRotating::scanStop));
    eventQueue->call(callback(this, &lidarRotating::scanStart));
}

void lidarRotating::scanStart() {
    led1 = 1;
    lidar->takeRange();

    scanJobID = eventQueue->call_every(SAMPLE_RATE, callback(this, &lidarRotating::takeReading));
}

void lidarRotating::scanStop() {
    led1 = 0;
    eventQueue->cancel(scanJobID);

    printf("Frame Start\r\n");
    for (uint16_t i = 0; i < LIDAR_STRIPS; i++) {
        uint16_t nElements = LIDAR_PAST + 1;
		uint16_t distanceNow  = distanceBufferNow[i];
        //uint16_t distancePrev = distanceBufferPrev[i];
        //int32_t  velocity     = ((int32_t) distancePrev - (int32_t) distanceNow) * 1000000 / (int32_t) rotationPeriod;
        uint16_t rotationTime = 1000000 / (int32_t) rotationPeriod;
		uint16_t sumXY = 0;
		uint16_t sumX = 0;
		uint32_t sumY = 0;
		uint32_t sumX2 = 0;
		for (uint16_t x = 0; x < LIDAR_PAST; x++){
			uint16_t distanceP = distanceBufferPrev[x][i];
			sumXY += distanceP * rotationTime * x;
			sumX += rotationTime * x;
			sumY += distanceP;
			sumX2 += rotationTime * rotationTime * x * x;
		}
		sumXY += distanceNow * rotationTime * LIDAR_PAST;
		sumX += LIDAR_PAST;
		sumY += distanceNow;
		sumX2 += LIDAR_PAST * LIDAR_PAST;
		
		int32_t velocity = (nElements * sumXY - sumX * sumY) / (nElements * sumX2 - sumX * sumX);
		
		distanceBufferNow[i]  = UINT16_MAX;
        //distanceBufferPrev[i] = distanceNow;
		for (uint16_t x = 0 ; x < LIDAR_PAST - 1 ; x++){
			distanceBufferPrev[x][i] = distanceBufferPrev[x+1][i];
		}
		distanceBufferPrev[LIDAR_PAST - 1][i] = distanceNow;

        if (velocity > TRIGGER_SPEED && velocity < MAX_SPEED) {
            eventQueue->call(notifyCallback);
        }

        printf("[Porty-A]%d[END]\r\n", distanceNow);
        printf("[Porty-B]%d[END]\r\n", distancePrev);
        printf("[Porty-C]%d[END]\r\n", velocity);
    }
}

void printData(uint32_t a, int32_t b, int32_t c, int32_t d) {
    printf("[Porty-A]%d[END]\r\n", a);
    printf("[Porty-B]%d[END]\r\n", b);
    printf("[Porty-C]%d[END]\r\n", c);
    printf("[Porty-D]%d[END]\r\n", d);
}

void lidarRotating::takeReading() {
    if (lidar->getBusyFlag() == 1) {
        return;
    }

    lidar->takeRange();
    int32_t  distance = lidar->readDistance();
    uint32_t time_us  = lidarTimer->read_us();
    if (time_us < scanStartTime_us || time_us > scanStopTime_us) {
        return;
    }
    uint32_t theta = (time_us * rotationFrequency) >> (LIDAR_FREQUENCY_NUMERATOR_BITS - (TRIG_LUT_SIZE_BITS + 2));


    // rotate reference frame 180°, so that 0° point is in the middle of the scanning range
    theta += (TRIG_LUT_SIZE * 2);
//    while (theta >= TRIG_LUT_SIZE * 4) {
//        theta -= TRIG_LUT_SIZE * 4; // 0 to 255
//    }


    if (distance < LIDAR_SCAN_MINIMUM) {
        distance = LIDAR_SCAN_MINIMUM;
    }

    // calculate X and Y coordinates of the LIDAR hit
    int32_t distX = (lut_sin(theta) * distance) >> TRIG_LUT_MAGNITUDE_BITS;
    int32_t distY = (lut_cos(theta) * distance) >> TRIG_LUT_MAGNITUDE_BITS;

    //eventQueue->call(&printData, theta, time_us, distX, distance);

    distX += (int32_t) (LIDAR_SCAN_WIDTH / 2);

    if (distY > LIDAR_SCAN_DEPTH || distX > LIDAR_SCAN_WIDTH || distX < 0) {
        // lidar hit is outside the detection area, so we can ignore it
        return;
    }

    // calculate which "strip" we are looking at
    uint16_t strip = (distX * LIDAR_STRIPS) / LIDAR_SCAN_WIDTH;


    if (strip >= LIDAR_STRIPS) {
        return;
    }


    // store distance into that strip if less than what is stored
    if (distY < distanceBufferNow[strip]) {
        distanceBufferNow[strip] = distY;
    }
}

void lidarRotating::printRotationTime() {
    printf("rotation time: %d\r\n", rotationPeriod);
    printf("rotation frequency: %d\r\n", rotationFrequency);
    printf("start time: %d µs\r\n", scanStartTime_us);
    printf("stop time: %d µs\r\n", scanStopTime_us);
}

