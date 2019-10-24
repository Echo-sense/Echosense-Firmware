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

#define SAMPLE_RATE 10

//IO
I2C i2c(I2C_SDA, I2C_SCL);
DigitalOut powerLed(POWER_LED);

//Peripherals
LIDARLite_v3HP lidar(&i2c);

Ticker ticker;

void tick() {
    // do LIDAR sensing
}

int main() {
    ticker.attach(&tick, SAMPLE_RATE);
}

