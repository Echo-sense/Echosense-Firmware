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

#include <cmath>
#include <mbed.h>
#include "trig.h"

using namespace std;

template<typename T>
constexpr T look_up_table_elem(int i) {
    return {};
}

template<>
constexpr uint16_t look_up_table_elem(int i) {
    return round(cos(static_cast <long double>(i) / TRIG_LUT_SIZE * 3.14159 / 2) * TRIG_LUT_MAGNITUDE);
}

template<typename T, int... N>
struct lookup_table_expand {};

template<typename T, int... N>
struct lookup_table_expand<T, 1, N...> {
    static constexpr std::array<T, sizeof...(N) + 1> values = {{look_up_table_elem<T>(0), N...}};
};

template<typename T, int L, int... N>
struct lookup_table_expand<T, L, N...> : lookup_table_expand<T, L - 1, look_up_table_elem<T>(L - 1), N...> {};

template<typename T, int... N>
constexpr std::array<T, sizeof...(N) + 1> lookup_table_expand<T, 1, N...>::values;

const std::array<uint16_t, TRIG_LUT_SIZE + 1> cosineLUT = lookup_table_expand<uint16_t, TRIG_LUT_SIZE + 1>::values;

int16_t lut_cos(uint16_t theta) {
    while (theta >= TRIG_LUT_SIZE * 4) {
        theta -= TRIG_LUT_SIZE * 4; // 0 to 255
    }
    int16_t sign = 1;
    if (theta >= TRIG_LUT_SIZE * 2) {
        theta = TRIG_LUT_SIZE * 4 - theta; // 128 to 1
    }
    if (theta >= TRIG_LUT_SIZE) {
        theta = TRIG_LUT_SIZE * 2 - theta; // 64 to 1
        sign  = -1;
    }

    return cosineLUT[theta] * sign;
}

int16_t lut_sin(uint16_t theta) {
    while (theta >= TRIG_LUT_SIZE * 4) {
        theta -= TRIG_LUT_SIZE * 4; // 0 to 255
    }
    int16_t sign = 1;
    if (theta >= TRIG_LUT_SIZE * 2) {
        theta = TRIG_LUT_SIZE * 4 - theta; // 128 to 1
        sign  = -1;
    }
    if (theta >= TRIG_LUT_SIZE) {
        theta = TRIG_LUT_SIZE * 2 - theta; // 64 to 1
    }

    return cosineLUT[TRIG_LUT_SIZE - theta] * sign;
}
