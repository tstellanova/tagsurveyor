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

#include "Particle.h"

#include "tracker_config.h"
#include "tracker.h"
#include "BeaconScanner.h"

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(SEMI_AUTOMATIC);

PRODUCT_ID(TRACKER_PRODUCT_ID);
PRODUCT_VERSION(TRACKER_PRODUCT_VERSION);

STARTUP(
    Tracker::startup();
);

SerialLogHandler logHandler(115200, LOG_LEVEL_INFO, {
    { "app.gps.nmea", LOG_LEVEL_INFO },
    { "comm.protocol", LOG_LEVEL_WARN},
    { "mux", LOG_LEVEL_WARN},
    { "app.gps.ubx",  LOG_LEVEL_WARN },
    { "ncp.at", LOG_LEVEL_INFO },
    { "net.ppp.client", LOG_LEVEL_INFO },
});


// Called by Tracker instance when it's collecting `loc` json
static void locGenCallback(JSONWriter &writer, LocationPoint &point, const void *context) {
    auto beacons = Scanner.getLairdBt510();
    if (beacons.size() > 0) {
        Log.info("beacons avail: %d", beacons.size());

        writer.name("bcnz").beginObject();
        for (auto beacon : beacons) {
            Log.info("beacon: %s", beacon.getAddress().toString().c_str());
            beacon.toJson(&writer);
        }
        writer.endObject(); //bcnz
    }
}

void setup() {
    Tracker::instance().init();
    delay(500);
    Tracker::instance().location.regLocGenCallback(locGenCallback);

    BLE.on();
}

void loop() {
    Tracker::instance().loop();
    Scanner.scan(5, SCAN_LAIRDBT510 );
    delay(500);
}