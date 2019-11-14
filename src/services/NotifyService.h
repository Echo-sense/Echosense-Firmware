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

#ifndef ECHOSENSE_FIRMWARE_NOTIFYSERVICE_H
#define ECHOSENSE_FIRMWARE_NOTIFYSERVICE_H

#include <mbed.h>
#include "ble/BLE.h"
#include "ble/Gap.h"

class NotifyService {
public:
    static const uint16_t NOTIFY_SERVICE_UUID              = 0xA000;
    static const uint16_t NOTIFY_STATE_CHARACTERISTIC_UUID = 0xA001;

    NotifyService(BLE &_ble);

    void sendNotification(bool newState);

    bool readnotificationState();

private:
    BLE                              *ble;
    ReadOnlyGattCharacteristic<bool> *notificationState;
    bool                             valueState;
};

#endif //ECHOSENSE_FIRMWARE_NOTIFYSERVICE_H
