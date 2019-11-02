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

#ifndef ECHOSENSE_FIRMWARE_BLESETUP_H
#define ECHOSENSE_FIRMWARE_BLESETUP_H

#include <mbed.h>
#include "ble/BLE.h"
#include "ble/Gap.h"

extern EventQueue *eventQueue;

void onBleInitError(BLE &ble, ble_error_t error);

void bleDisconnectCallback(const Gap::DisconnectionCallbackParams_t *params);

void bleInitComplete(BLE::InitializationCompleteCallbackContext *params);

void blePrintMacAddress();

void scheduleBleEventsProcessing(BLE::OnEventsToProcessCallbackContext *context);

#endif //ECHOSENSE_FIRMWARE_BLESETUP_H
