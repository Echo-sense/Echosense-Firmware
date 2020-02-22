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

lidarRotating::lidarRotating(I2C *i2c, DigitalOut *motor, InterruptIn *rotationSensor, const Callback<void()> *notifyCallback) {
    this->i2c            = i2c;
    this->motor          = motor;
    this->rotationSensor = rotationSensor;
    this->notifyCallback = notifyCallback;

    lidarTimer = new Timer();
    lidar      = new LIDARLite_v3HP(i2c);
}

void lidarRotating::start() {
    motor->write(1); // start motor
}
