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

#include "NotifyService.h"

// totally random UUIDs I swear
const UUID NotifyService::NOTIFY_SERVICE_UUID              = UUID{"2020A000-5543-4945-43484-F53454E5345"};
const UUID NotifyService::NOTIFY_STATE_CHARACTERISTIC_UUID = UUID{"2020A001-5543-4945-43484-F53454E5345"};

NotifyService::NotifyService(BLE &_ble) {
    ble               = &_ble;
    notificationState = new ReadOnlyGattCharacteristic<bool>(
        NOTIFY_STATE_CHARACTERISTIC_UUID, 0,
        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);

    GattCharacteristic *charTable[]   = {};
    GattService        *buttonService = new GattService(NOTIFY_SERVICE_UUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));

    ble->gattServer().addService(*buttonService);
}

void NotifyService::sendNotification() {
}
