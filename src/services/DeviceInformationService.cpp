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

#include "DeviceInformationService.h"

DeviceInformationService::DeviceInformationService(
    BLE &_ble,
    const char *_manufacturersName,
    const char *_modelNumber,
    const char *_serialNumber,
    const char *_hardwareRevision,
    const char *_firmwareRevision,
    const char *_softwareRevision) {

    ble               = &_ble;
    manufacturersName = _manufacturersName;
    modelNumber       = _modelNumber;
    serialNumber      = _serialNumber;
    hardwareRevision  = _hardwareRevision;
    firmwareRevision  = _firmwareRevision;
    softwareRevision  = _softwareRevision;

    uint8_t            charCount = 0;
    GattCharacteristic *charTable[6];


    if (modelNumber != NULL) {
        uint8_t length = strlen(modelNumber);

        modelNumberStringCharacteristic = new GattCharacteristic(
            GattCharacteristic::UUID_MODEL_NUMBER_STRING_CHAR,
            (uint8_t *) modelNumber,
            length, /* min length */
            length, /* max length */
            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ,
            NULL, 0, false);

        charTable[charCount++] = modelNumberStringCharacteristic;
    }

    if (manufacturersName != NULL) {
        uint8_t length = strlen(manufacturersName);

        manufacturersNameStringCharacteristic = new GattCharacteristic(
            GattCharacteristic::UUID_MANUFACTURER_NAME_STRING_CHAR,
            (uint8_t *) manufacturersName,
            length, /* min length */
            length, /* max length */
            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ,
            NULL, 0, false);

        charTable[charCount++] = manufacturersNameStringCharacteristic;
    }

    if (serialNumber != NULL) {
        uint8_t length = strlen(serialNumber);

        serialNumberStringCharacteristic = new GattCharacteristic(
            GattCharacteristic::UUID_SERIAL_NUMBER_STRING_CHAR,
            (uint8_t *) serialNumber,
            length, /* min length */
            length, /* max length */
            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ,
            NULL, 0, false);

        charTable[charCount++] = serialNumberStringCharacteristic;
    }

    if (hardwareRevision != NULL) {
        uint8_t length = strlen(hardwareRevision);

        hardwareRevisionStringCharacteristic = new GattCharacteristic(
            GattCharacteristic::UUID_HARDWARE_REVISION_STRING_CHAR,
            (uint8_t *) hardwareRevision,
            length, /* min length */
            length, /* max length */
            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ,
            NULL, 0, false);

        charTable[charCount++] = hardwareRevisionStringCharacteristic;
    }

    if (firmwareRevision != NULL) {
        uint8_t length = strlen(firmwareRevision);

        firmwareRevisionStringCharacteristic = new GattCharacteristic(
            GattCharacteristic::UUID_FIRMWARE_REVISION_STRING_CHAR,
            (uint8_t *) firmwareRevision,
            length, /* min length */
            length, /* max length */
            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ,
            NULL, 0, false);

        charTable[charCount++] = firmwareRevisionStringCharacteristic;
    }

    if (softwareRevision != NULL) {
        uint8_t length = strlen(softwareRevision);

        softwareRevisionStringCharacteristic = new GattCharacteristic(
            GattCharacteristic::UUID_SOFTWARE_REVISION_STRING_CHAR,
            (uint8_t *) softwareRevision,
            length, /* min length */
            length, /* max length */
            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ,
            NULL, 0, false);

        charTable[charCount++] = softwareRevisionStringCharacteristic;
    }

    GattService deviceInformationService(
        GattService::UUID_DEVICE_INFORMATION_SERVICE,
        charTable,
        charCount);

    ble->gattServer().addService(deviceInformationService);
}

