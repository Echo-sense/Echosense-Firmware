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

#ifndef ECHOSENSE_FIRMWARE_DEVICEINFORMATIONSERVICE_H
#define ECHOSENSE_FIRMWARE_DEVICEINFORMATIONSERVICE_H

#include <mbed.h>
#include <BLE.h>

class DeviceInformationService {
public:
    const char *manufacturersName;
    const char *modelNumber;
    const char *serialNumber;
    const char *hardwareRevision;
    const char *firmwareRevision;
    const char *softwareRevision;

    DeviceInformationService(
        BLE &_ble,
        const char *_manufacturersName = NULL,
        const char *_modelNumber = NULL,
        const char *_serialNumber = NULL,
        const char *_hardwareRevision = NULL,
        const char *_firmwareRevision = NULL,
        const char *_softwareRevision = NULL);

protected:
    /**
     * A pointer to the BLE instance object to which the services and
     * characteristics will be added.
     */
    BLE                *ble;
    /**
     * BLE characterising to allow BLE peers access to the manufacturer's name.
     */
    GattCharacteristic *manufacturersNameStringCharacteristic;
    /**
     * BLE characterising to allow BLE peers access to the model number.
     */
    GattCharacteristic *modelNumberStringCharacteristic;
    /**
     * BLE characterising to allow BLE peers access to the serial number.
     */
    GattCharacteristic *serialNumberStringCharacteristic;
    /**
     * BLE characterising to allow BLE peers access to the hardware revision string.
     */
    GattCharacteristic *hardwareRevisionStringCharacteristic;
    /**
     * BLE characterising to allow BLE peers access to the firmware revision string.
     */
    GattCharacteristic *firmwareRevisionStringCharacteristic;
    /**
     * BLE characterising to allow BLE peers access to the software revision string.
     */
    GattCharacteristic *softwareRevisionStringCharacteristic;
};


#endif //ECHOSENSE_FIRMWARE_DEVICEINFORMATIONSERVICE_H
