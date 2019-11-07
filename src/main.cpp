/*
 * Copyright 2019 Team EchoSense
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

#include "mbed.h"
#include "pins.h"
#include <LIDARLite_v3HP.h>

#define SAMPLE_RATE 500 //ms

//IO
I2C        i2c(I2C_SDA0, I2C_SCL0);
Serial     pc(USBTX, USBRX);
DigitalOut led1(p8);
DigitalOut led2(p5);
DigitalOut led3(p3);

//Peripherals
LIDARLite_v3HP lidar(&i2c);

EventQueue eventQueue(32 * EVENTS_EVENT_SIZE);
Thread     eventThread;
Ticker     ticker;

uint16_t dist = 0;

void tick() {
    led3 = !led3;

    lidar.takeRange();

    lidar.waitForBusy();
    dist = lidar.readDistance();
    led1 = (dist < 10) ? 0 : 1;
    pc.printf("[Porty-A]%d[END]\r\n", dist);
}

int main() {
    pc.printf("starting event loop\r\n");

    lidar.configure();
    lidar.resetReferenceFilter();

    eventQueue.call_every(SAMPLE_RATE, &tick);
    eventQueue.dispatch_forever();
}

