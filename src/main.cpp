/**
 * @file       main.cpp
 * @author     Fa-b
 * @license    This project is released under the MIT License (MIT)
 * @date       Sep 2021
 * @brief      Entry, not much done here yet
 */

#include <Arduino.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <map>

#include "privates.h"
#include "typedefs.h"
#include "command_parser.h"

#define LED D2

#define PWM_FREQUENCY 10000   // 10000 Hz
#define PWM_RESOLUTION 1023  // 10 bit
#define SAMPLE_INTERVAL 50  // 50 milliseconds
#define SAMPLE_COUNT 8       // 8 samples

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

// Multiple devices sharing a single auth token act as a single device and therefore have to share data using bridge
WidgetBridge self(SELF_BRIDGE);

// Modifying lib to avoid linker errors (multiple file projects)
static WiFiClient _blynkWifiClient;
static BlynkArduinoClient _blynkTransport(_blynkWifiClient);
BlynkWifi Blynk(_blynkTransport);

// lib instances
Ticker updater;

// Global vars
int brightness = 0;
unsigned long avg_current_samples = 0;
double avg_current = 0.0;
std::map<String, double> cur_list;
int state = 0;
int strobe = 0;
char *timerParam = nullptr;
bool master = true;
String self_ip = "";

char Date[16];
char Time[16];

static void setLED(int);
static void measure_ADC(void);

void setup() {
    pinMode(LED, OUTPUT);
    setLED(0);
    // builtin LED
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    analogWriteFreq(PWM_FREQUENCY);
    analogWriteRange(PWM_RESOLUTION);

    // Debug console
    //Serial.begin(115200);
    Blynk.begin(auth, ssid, pass, HOSTNAME, 8080);

    Blynk.syncVirtual(V3);
    Blynk.syncVirtual(V0);
    Blynk.syncVirtual(V4);
}

BLYNK_CONNECTED() {
    self_ip = _blynkWifiClient.localIP().toString();
    self.setAuthToken(AUTH_TOKEN);
    self.virtualWrite(SELF_BRIDGE, self_ip);
    bridge.setAuthToken(remoteAuth);
    INFO_PRINT("Just connected.\n");
    DEBUG_PRINT("Debug mode is on which is why I will spam here :-)\n\n");

    // OTA Server update controller
    checkForUpdates();

    updater.attach_ms(SAMPLE_INTERVAL, measure_ADC);
}

void loop() {
    Blynk.run();
}

// first connected device will become master, all slaves send values to master for accumulation, only master sends data to app
BLYNK_WRITE(SELF_BRIDGE) { 
    // TODO: make sure to re-attempt discovery/detect failure whatever..
    if(master) {
        // I'm Master
        const char* data = param.asString();
        if(!param[1].isValid() && IPAddress().isValid(data)) {
            // Registration packet
            if(strcmp(data, self_ip.c_str()) == 0) {
                // Received my IP, another master was faster
                DEBUG_PRINTF("I'm slave: %s\n", self_ip.c_str());
                master = false;
            } else {
                // Apparently I was first, echo data and store it to dict
                cur_list.insert(std::make_pair(String(data), 0));
                self.virtualWrite(SELF_BRIDGE, data);
                DEBUG_PRINTF("I'm master: %s\n", self_ip.c_str());
            }
        } else if(param[1].isValid()) {
            auto it = cur_list.find(String(data));
            // Store current average to dict
            if(it != cur_list.end()) {
                it->second = param[1].asDouble();
            }
            
        } 
    } else {
        // I'm Slave
        // Don't do anything
    }
}

BLYNK_WRITE(V0) {
    // will be reset when current ADC sample interval expires
    digitalWrite(LED_BUILTIN, LOW);
    brightness = (int)(BRIGHTNESS_MULTIPLIER * param.asInt());
    setLED(state);
}

BLYNK_WRITE(V3) {
    // will be reset when current ADC sample interval expires
    digitalWrite(LED_BUILTIN, LOW);
    setLED(param.asInt());
}

BLYNK_WRITE(V4) {
    // will be reset when current ADC sample interval expires
    digitalWrite(LED_BUILTIN, LOW);
    strobe = param.asInt();

    // Reset to original state
    if(strobe == 0) {
        Blynk.syncVirtual(V3);
    }
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
    avg_current_samples = ((unsigned long)avg_current_samples * (SAMPLE_COUNT - 1) + analogRead(A0)) / SAMPLE_COUNT;

    // max. ON time of LED is coupled to sample interval
    digitalWrite(LED_BUILTIN, HIGH);
    if (strobe == 1) {
        setLED(state^1);
    }
    if (n_samples++ >= SAMPLE_COUNT) {
        n_samples = 0;
        // Fix Android scaling bug
        avg_current = ((double)avg_current_samples * (MAX_MEASURE_HARDWARE_CURRENT_A / 1023));
        if(master) {
            unsigned int cnt = 1;
            for (auto it = cur_list.begin(); it != cur_list.end(); ++it) {
                avg_current += it->second;
                cnt++;
            }
            Blynk.virtualWrite(V1, avg_current);
            Blynk.virtualWrite(V2, (state || strobe) ? ((POWER_PERCENT / cnt) * avg_current) : 0);
        } else {
            self.virtualWrite(SELF_BRIDGE, self_ip, avg_current);
        }
    }
}
