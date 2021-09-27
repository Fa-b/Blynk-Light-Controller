/**
 * @file       main.cpp
 * @author     Fa-b
 * @license    This project is released under the MIT License (MIT)
 * @date       Sep 2021
 * @brief      Entry, not much done here yet
 */

#include <Arduino.h>
#include <BlynkSimpleEsp8266.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>

#include "OTA_updater.h"
#include "privates.h"
#include "typedefs.h"

#define LED D2

#define PWM_FREQUENCY 1000   // 1 kHz
#define SAMPLE_INTERVAL 100  // 100 milliseconds
#define SAMPLE_COUNT 8       // 32 samples

// extern in privates.h
DEVICE_INFO_T device_info;
const char name[] = DEVICE_NAME;
const char auth[] = AUTH_TOKEN;
const char version[] = VERSION;
char hostname[] = HOSTNAME;
char ssid[] = SSID;
char pass[] = WIFI_PASS;
char remoteAuth[] = REMOTE_AUTH;

// extern in typedefs.h
#ifdef ERROR_TERMINAL
WidgetTerminal terminal(DEBUG_TERMINAL);
#elif ERROR_BRIDGE
BridgeTerminal bridge(DEBUG_BRIDGE);
#endif

// Modifying lib to avoid linker errors (multiple file projects)
static WiFiClient _blynkWifiClient;
static BlynkArduinoClient _blynkTransport(_blynkWifiClient);
BlynkWifi Blynk(_blynkTransport);

// lib instances
Ticker updater;

// Global vars
int brightness = 0;
int avg_current = 0;
int state = 0;
char *timerParam = nullptr;

char Date[16];
char Time[16];

static void setLED(int);
static void measure_ADC(void);

void setup() {
    // Debug console
    //Serial.begin(115200);
    pinMode(LED, OUTPUT);
    Blynk.begin(auth, ssid, pass, HOSTNAME, 8080);

    analogWriteFreq(PWM_FREQUENCY);
}

BLYNK_CONNECTED() {
    Blynk.syncVirtual(V3);
    Blynk.syncVirtual(V0);

    bridge.setAuthToken(remoteAuth);
    INFO_PRINT("Just connected.\n");
    DEBUG_PRINT("Debug mode is on which is why I will spam here :-)\n\n");

    updater.attach_ms(SAMPLE_INTERVAL, measure_ADC);
    // OTA Server update controller
    checkForUpdates();
}

void loop() {
    Blynk.run();
}

BLYNK_WRITE(V0) {
    brightness = param.asInt();
    if (state == 1) {
        analogWrite(LED, brightness);
    } else {
        digitalWrite(LED, LOW);
    }
}

BLYNK_WRITE(V3) {
    setLED(param.asInt());
}

BLYNK_WRITE(V4) {  // Time Input as Schedule see here: https://community.blynk.cc/t/automatic-scheduler-esp-01-with-4-time-input-widgets/10658
    size_t len = param.getLength();
    timerParam = new char[len];
    strncpy(timerParam, (char *)param.getBuffer(), len);

    DEBUG_PRINT("Buffer:\n");
    for (unsigned int i = 0; i < len; i++)
        DEBUG_PRINTF("%x", timerParam[i]);
    //setLED(param.asInt());
}

static void setLED(int value) {
    state = value;
    if (state == 1) {
        analogWrite(LED, brightness);
    } else {
        digitalWrite(LED, LOW);
    }
}

static void measure_ADC() {
    static int n_samples = 0;
    avg_current = (avg_current * (SAMPLE_COUNT - 1) + analogRead(A0)) / SAMPLE_COUNT;

    if (n_samples++ >= SAMPLE_COUNT) {
        n_samples = 0;
        Blynk.virtualWrite(V1, avg_current);
        Blynk.virtualWrite(V2, state ? ((MAXPOWERCORRECTION_PERCENT * avg_current) / 1023) : 0);
    }
}
