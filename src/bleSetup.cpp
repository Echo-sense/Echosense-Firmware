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

const static char     DEVICE_NAME[] = "EchoSense";
const static char     MANUFACTURER_NAME[] = "Team EchoSense";
const static uint16_t uuid16_list[] = {NotifyService::NOTIFY_SERVICE_UUID};
/* Device Information Service should not be advertised */

NotifyService            *notifyService;
DeviceInformationService *deviceInformationService;

void onBleInitError(BLE &ble, ble_error_t error) {
    printf("BLUETOOTH FAILED TO INITIALIZE\r\n");
}

void bleDisconnectCallback(const Gap::DisconnectionCallbackParams_t *params) {
    printf("BLUETOOTH DISCONNECTED, RESUMING ADVERTISEMENT\r\n");

    BLE::Instance().gap().startAdvertising(); // restart advertising
}

void blePrintMacAddress() {
    /* Print out device MAC address to the console*/
    Gap::AddressType_t addr_type;
    Gap::Address_t     address;
    BLE::Instance().gap().getAddress(&addr_type, address);
    printf("DEVICE MAC ADDRESS: ");
    for (int i = 5; i >= 1; i--) {
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

    printf("BLUETOOTH INITIALIZATION COMPLETE\n");

    // add disconnect callback
    ble.gap().onDisconnection(bleDisconnectCallback);

    // create services
    notifyService = new NotifyService(ble);
    deviceInformationService = new DeviceInformationService(ble, MANUFACTURER_NAME);


    // TODO: handle callbacks for EchoSense here

    // Start advertising
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *) uuid16_list, sizeof(uuid16_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *) DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::APPEARANCE, (uint8_t *) GapAdvertisingData::GENERIC_CYCLING, sizeof(GapAdvertisingData::Appearance_t));
    ble.gap().accumulateAdvertisingPayloadTxPower(0);
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(760); /* ms */
    ble.gap().startAdvertising();
    blePrintMacAddress();
}

void scheduleBleEventsProcessing(BLE::OnEventsToProcessCallbackContext *context) {
    BLE &ble = BLE::Instance();
    eventQueue.call(Callback<void()>(&ble, &BLE::processEvents));
}

