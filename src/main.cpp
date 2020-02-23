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
#include "bleSetup.h"
#include "trig.h"
#include <LIDARLite_v3HP.h>

//using namespace std;

#define EVENT_QUEUE_DEPTH 16
#define MAX_DIST = 175 /* cm */

//IO
I2C        i2c(I2C_SDA0, I2C_SCL0);
Serial     pc(USBTX, USBRX);
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);

DigitalOut  motor(A3);
InterruptIn rotation(A2);

//Peripherals
LIDARLite_v3HP lidar(&i2c);
InterruptIn    lidarInterrupt(D9);

EventQueue eventQueue(32 * EVENTS_EVENT_SIZE);
Ticker     ticker;

uint16_t lidarSampleCount = 0;

Timer timer;

bool atSpeed = 0;

void resetNotification() {
    notifyService->sendNotification(0);
    led2 = 0;
}

void sendNotification() {
    if (notifyService->readnotificationState()) {
        return;
    }
    pc.printf("VEHICLE APPROACHING!\n");
    led2 = 1;
    notifyService->sendNotification(1);
    eventQueue.call_in(3000, &resetNotification);
}

int main() {
    pc.baud(115200);

    // setup LIDAR
    lidar.configure(1, 1);
    lidar.resetReferenceFilter();

    // setup BLE
    BLE &ble = BLE::Instance();

    ble.onEventsToProcess(scheduleBleEventsProcessing);
    ble.init(bleInitComplete);

    notifyService->sendNotification(0);

    // setup Ticker
    pc.printf("starting event loop\r\n");
    //eventQueue.call_every(SAMPLE_RATE, &tick);
    // eventQueue.call_every(1000, &test);
    timer.start();
    eventQueue.dispatch_forever();
}

