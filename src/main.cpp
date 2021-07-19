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
#include "sht3x-i2c.h"


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


// M8 Temperature - Humidity sensor
Sht3xi2c sht_sensor(Wire3);


static void publish_sht_data(JSONWriter& writer) {
    double temp, humid;

    if (0 == sht_sensor.get_reading(&temp, &humid)) {
        writer.name("i2c_temp").value(temp);
        writer.name("i2c_humid").value(humid);
    }
}

// Called by Tracker instance when it's collecting `loc` json
static void locGenCallback(JSONWriter& writer, LocationPoint& point, const void *context) {

    publish_sht_data(writer);

     writer.name("bcnz").beginObject();
 
    #ifdef SUPPORT_LAIRDBT510
    {
        auto beacons = Scanner.getLairdBt510();
        Log.info("Laird avail: %d", beacons.size());

        if (beacons.size() > 0) {
            for (auto beacon : beacons) {
                Log.info("L beacon: %s", beacon.getAddress().toString().c_str());
                beacon.toJson(&writer);
            }
        }
    }
    #endif //SUPPORT_LAIRDBT510

    #ifdef SUPPORT_EDDYSTONE
    {
        auto beacons = Scanner.getEddystone();
        Log.info("Eddy avail: %d", beacons.size());

        if (beacons.size() > 0) {
            for (auto beacon : beacons) {
                Log.info("E beacon: %s", beacon.getAddress().toString().c_str());
                beacon.toJson(&writer);
            }
        }
    }
    #endif //SUPPORT_EDDYSTONE

     #ifdef SUPPORT_IBEACON
    {
        auto beacons = Scanner.getiBeacons();
        Log.info("ibeacons avail: %d", beacons.size());

        if (beacons.size() > 0) {
            for (auto beacon : beacons) {
                Log.info("I beacon: %s", beacon.getAddress().toString().c_str());
                beacon.toJson(&writer);
            }
        }
    }
    #endif //SUPPORT_IBEACON

    writer.endObject(); //bcnz

}

void setup() {
    Tracker::instance().init();
    delay(500);
        
    // Turn on 5V output on M8 connector
    pinMode(CAN_PWR, OUTPUT);      
    digitalWrite(CAN_PWR, HIGH);  
    delay(500);

    Tracker::instance().location.regLocGenCallback(locGenCallback);
    // enable BLE radio so we can use it for scanning
    if ( SYSTEM_ERROR_NONE != BLE.on()) {
        Log.error("BLE on failed");
    }

    // kick off the M8 sensor reading
    sht_sensor.begin(CLOCK_SPEED_400KHZ);
    sht_sensor.start_periodic(SHT31_ACCURACY_MEDIUM, 1); // 1 Hz measurement

}

void loop() {
    Tracker::instance().loop();
    // scan for nearby BLE beacons advertising
    Scanner.scan(5, SCAN_LAIRDBT510 | SCAN_EDDYSTONE | SCAN_IBEACON );
} 
