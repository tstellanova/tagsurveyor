/*
 * Copyright (c) 2020 Particle Industries, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef BEACON_SCAN_CONFIG_H
#define BEACON_SCAN_CONFIG_H

#include "Particle.h"

/**
 * Configure support for scanning various kinds of Beacons. 
 * If you would like to reduce the amount of flash that the application uses,
 * you can remove some beacon types.
 */

// #define SUPPORT_IBEACON

// #define SUPPORT_KONTAKT

// #if SYSTEM_VERSION >= SYSTEM_VERSION_DEFAULT(3, 0, 0)
// #define SUPPORT_LAIRDBT510
// #endif

#define SUPPORT_EDDYSTONE
// KKM SMART requires support for Eddystone as well
// #define SUPPORT_KKMSMART


#endif //BEACON_SCAN_CONFIG_H