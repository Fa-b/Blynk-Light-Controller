/**
 * @file       OTA_updater.h
 * @author     Fa-b
 * @license    This project is released under the MIT License (MIT)
 * @date       Sep 2021
 * @brief      Checks for updates on a dedicated OTA Server (Port 8070 atm), compares versions and downloads
 */

#include "OTA_updater.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ArduinoOTA.h> // not working: https://github.com/jandrassy/ArduinoOTA
// Alternative: https://arduino-esp8266.readthedocs.io/en/3.0.2/ota_updates/readme.html

#include "privates.h"
#include "typedefs.h"

// compare versions strings of the form "1.2.3a"
int versionStringCompare(String v1, String v2) {
    int vnum1 = 0, vnum2 = 0;
    int vrev1 = 0, vrev2 = 0;

    for (unsigned int i = 0, j = 0; (i < v1.length() || j < v2.length());) {
        while (i < v1.length() && v1[i] != '.') {
            vnum1 = vnum1 * 10 + (v1[i] - '0');
            i++;
            if(v1[i] >= 'A' && v1[i] <= 'Z') {
                vrev1 = vrev1 * 10 + (v1[i] - 'A' + 1);
                i++;
                break;
            } else if(v1[i] >= 'a' && v1[i] <= 'z') {
                vrev1 = vrev1 * 10 + (v1[i] - 'a' + 1);
                i++;
                break;
            }
        }

        while (j < v2.length() && v2[j] != '.') {
            vnum2 = vnum2 * 10 + (v2[j] - '0');
            j++;
            if(v2[j] >= 'A' && v2[j] <= 'Z') {
                vrev2 = vrev2 * 10 + (v2[j] - 'A' + 1);
                j++;
                break;
            } else if(v2[j] >= 'a' && v2[j] <= 'z') {
                vrev2 = vrev2 * 10 + (v2[j] - 'a' + 1);
                j++;
                break;
            }
        }

        if (vnum1 > vnum2)
            return 1;
        if (vnum2 > vnum1)
            return -1;
        if (vrev1 > vrev2)
            return 1;
        if (vrev2 > vrev1)
            return -1;

        vnum1 = vnum2 = 0;
        i++;
        j++;
    }
    return 0;
}

void loadFirmware(String ver) {
    WiFiClient client;
    HTTPClient http;

    INFO_PRINTF("Downloading Firmware version: %s\n", ver.c_str());

    // Prepare path
    char path[64];
    snprintf(path, sizeof(path), "/%s/%s", auth, ver.c_str());
    http.begin(client, hostname, 8070, path);  // HTTP

    int statusCode = http.GET();
    if (statusCode != HTTP_CODE_OK) {
        http.end();
        return;
    }

    long length = http.getSize();
    if (length == HTTPC_ERROR_CONNECTION_FAILED) {
        http.end();
        WARNING_PRINT("Server didn't provide Content-length header. Can't continue with update.\n");
        return;
    }
    DEBUG_PRINTF("Server returned update file size of %lu bytes\n", length);

    if (!InternalStorage.open(length)) {
        http.end();
        WARNING_PRINTLN("There is not enough space to store the update. Can't continue with update.\n");
        return;
    }

    byte b;
    while (length > 0) {
        if (!http.getStream().readBytes(&b, 1))  // reading a byte with timeout
            break;
        InternalStorage.write(b);
        length--;
    }
    InternalStorage.close();
    http.end();
    if (length > 0) {
        WARNING_PRINTF("Timeout downloading update file after %lu bytes. Can't continue with update.\n", length);
        return;
    }

    DEBUG_PRINT("Sketch update apply and reset.\n");
    InternalStorage.apply();  // this doesn't return
}

void checkForUpdates() {
    WiFiClient client;
    HTTPClient http;

    // Prepare path
    char path[64];
    snprintf(path, sizeof(path), "/OTA/%s", auth);

    http.begin(client, hostname, 8070, path);  // HTTP
    int statusCode = http.GET();  // Send GET request

    if (statusCode == HTTP_CODE_OK) {
        String newFWVersion = http.getString();

        DEBUG_PRINTF("Current firmware version: %s\n", version);
        DEBUG_PRINTF("Available firmware version: %s\n", newFWVersion.c_str());

        if (versionStringCompare(newFWVersion, version) > 0) {
            INFO_PRINTF("Firmware Update Available %s -> %s\n", version, newFWVersion.c_str());

            loadFirmware(newFWVersion);
        } else {
            INFO_PRINTF("Already on latest version: %s\n", version);
        }
    } else {
        WARNING_PRINTF("Firmware version check failed, got HTTP response code %d\n", statusCode);
        http.end();
    }
}