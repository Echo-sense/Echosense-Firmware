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
#include <LIDARLite_v3HP.h>

#define SAMPLE_RATE 1000000
#define EVENT_QUEUE_DEPTH 16

//IO
I2C i2c(I2C_SDA, I2C_SCL);
Serial serial(p20, p18);
DigitalOut powerLed1(POWER_LED1);
DigitalOut powerLed2(POWER_LED2);

//Peripherals
LIDARLite_v3HP lidar(&i2c);

Ticker ticker;

EventQueue eventQueue(EVENTS_EVENT_SIZE * EVENT_QUEUE_DEPTH);
Thread eventThread;

void tick() {
    // do LIDAR sensing
	powerLed1=!powerLed1;
    powerLed2=!powerLed2;
}

int main() {
    ticker.attach_us(eventQueue.event(tick), SAMPLE_RATE);

    BLE &ble = BLE::Instance();
    ble.onEventsToProcess(scheduleBleEventsProcessing);
    ble.init(bleInitComplete);

    eventThread.start(callback(&eventQueue, &EventQueue::dispatch_forever));
}

