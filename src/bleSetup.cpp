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

#include "bleSetup.h"

EventQueue *eventQueue = new EventQueue(EVENTS_EVENT_SIZE * 10);

const static char     DEVICE_NAME[] = "EchoSense";
static const uint16_t uuid16_list[] = {};

void onBleInitError(BLE &ble, ble_error_t error) {
    // handle initialization error
}

void bleDisconnectCallback(const Gap::DisconnectionCallbackParams_t *params) {
}

void blePrintMacAddress() {
    // TODO: setup serial console code
}

void bleInitComplete(BLE::InitializationCompleteCallbackContext *params) {
    BLE &ble = params->ble;
    ble_error_t error = params->error;

    if (error != BLE_ERROR_NONE) {
        // forward error to error handling code
        onBleInitError(ble, error);
        return;
    }

    // make sure this is the default instance of the BLE object
    if (ble.getInstanceID() != BLE::DEFAULT_INSTANCE) {
        // handle not-default-instance error
        return;
    }

    // add disconnect callback
    ble.gap().onDisconnection(bleDisconnectCallback);

    // TODO: handle callbacks for EchoSense here

    // Start advertising
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *) uuid16_list, sizeof(uuid16_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *) DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(1000); /* 1000ms. */
    ble.gap().startAdvertising();

    blePrintMacAddress();
}

void scheduleBleEventsProcessing(BLE::OnEventsToProcessCallbackContext *context) {
}

