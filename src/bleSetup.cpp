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
#include "services/NotifyService.h"
#include "bleSetup.h"

const static char     DEVICE_NAME[] = "EchoSense";
const static uint16_t uuid16_list[] = {NotifyService::NOTIFY_SERVICE_UUID.getShortUUID()};
const static uint8_t* uuid128_list = NotifyService::NOTIFY_SERVICE_UUID.getBaseUUID();
NotifyService *notifyService;

void onBleInitError(BLE &ble, ble_error_t error) {
    // handle initialization error
    printf("error occur");
}

void bleDisconnectCallback(const Gap::DisconnectionCallbackParams_t *params) {
}

void blePrintMacAddress() {
    // TODO: setup serial console code
    /* Print out device MAC address to the console*/
    Gap::AddressType_t addr_type;
    Gap::Address_t address;
    BLE::Instance().gap().getAddress(&addr_type, address);
    printf("DEVICE MAC ADDRESS: ");
    for (int i = 5; i >= 1; i--){
        printf("%02x:", address[i]);
    }
    printf("%02x\r\n", address[0]);
}

void bleInitComplete(BLE::InitializationCompleteCallbackContext *params) {
    BLE         &ble  = params->ble;
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

    // create services
    notifyService = new NotifyService(ble);

    // TODO: handle callbacks for EchoSense here

    // Start advertising
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *) uuid16_list, sizeof(uuid16_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS, uuid128_list, sizeof(uuid128_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *) DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::APPEARANCE, (uint8_t *) GapAdvertisingData::GENERIC_CYCLING, sizeof(GapAdvertisingData::Appearance_t));
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(1000); /* 1000ms. */
    ble.gap().startAdvertising();
    ble::AdvertisingDataBuilder builder();
    //builder.addOrAppendData();
    blePrintMacAddress();
}

void scheduleBleEventsProcessing(BLE::OnEventsToProcessCallbackContext *context) {
    BLE &ble = BLE::Instance();
    eventQueue.call(Callback<void()>(&ble, &BLE::processEvents));
}

