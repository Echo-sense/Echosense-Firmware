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

#define SAMPLE_RATE 50 //ms
#define EVENT_QUEUE_DEPTH 16

#define TRIGGER_SPEED_KPH 5 /* km/h */
#define TRIGGER_SPEED (1000 / 36) * TRIGGER_SPEED_KPH /* cm/s */

#define MAX_SPEED_KPH 60 /* km/h */
#define MAX_SPEED (1000 / 36) * MAX_SPEED_KPH /* cm/s */

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

uint16_t inverseDeltaTime   = 1000 / SAMPLE_RATE;
uint16_t dist               = 0;
bool     notificationSignal = 0;

uint16_t lidarSampleCount = 0;

Timer timer;
uint16_t totalRotationTime = 0;

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

void tick() {
    if (lidar.getBusyFlag() == 1) {
        led1 = 0;
        return;
    }
    led1 = 1;

    uint16_t oldDist = dist;
    uint16_t newDist = lidar.readDistance();

    int16_t velocity_check = ((oldDist - newDist)) * inverseDeltaTime;
    if (velocity_check > MAX_SPEED) {
        dist    = newDist;
        oldDist = newDist;
    } else {
        dist = (dist + newDist) / 2;
    }

    int16_t velocity = ((oldDist - dist)) * inverseDeltaTime;
    //led1 = (dist < 10) ? 0 : 1;
    //pc.printf("[Porty-A]%d[END]\r\n", dist);
    //pc.printf("[Porty-B]%d[END]\r\n", newDist);
    //pc.printf("[Porty-C]%d[END]\r\n", velocity);

    if (velocity > TRIGGER_SPEED) {
        sendNotification();
    }

    lidar.takeRange();
}

void getTotalRotationTime() {
    totalRotationTime = timer.read_ms();
    timer.stop();
    pc.printf("total rotation time is %d\r\n", totalRotationTime);
}

void test() {
    //motor = !motor;
    lidarSampleCount++;
    if (lidarSampleCount != 1000) {
        eventQueue.call(&test);
    } else {
        uint16_t time = timer.read_ms();
        pc.printf("%d\r\n", time);
        timer.stop();
    }
    lidar.readDistance();
    //pc.printf("interrupt!\n");

    //motorSpeed += 100;
    //motorSpeed %= 32768;
    //motor.pulsewidth_us(motorSpeed);
}

void lidarInterruptFn() {
    lidar.takeRange();
    led1 = !led1;
}

int main() {
    led1 = 1;
    pc.baud(115200);

    //motor.period_us(32768);
    motor = 0;

    // setup LIDAR
    lidar.configure(1, 1);
    lidar.resetReferenceFilter();


    for (uint16_t x = 0; x < 256; x++) {
        pc.printf("[Porty-A]%d[END]\r\n", lut_cos(x));
    }
    //print_memory_info();

    // setup BLE
    BLE &ble = BLE::Instance();

    ble.onEventsToProcess(scheduleBleEventsProcessing);
    ble.init(bleInitComplete);

    notifyService->sendNotification(0);

    lidarInterrupt.fall(&lidarInterruptFn);
    


    // setup Ticker
    pc.printf("starting event loop\r\n");
    //eventQueue.call_every(SAMPLE_RATE, &tick);
    // eventQueue.call_every(1000, &test);
    timer.start();
    //eventQueue.call(&test);
    rotation.fall(&getTotalRotationTime);
    eventQueue.dispatch_forever();
}

