#ifndef privates_h
#define privates_h

// Network credentials
#define     HOSTNAME        "hostname"
#define     SSID            "ABCDEFG"
#define     WIFI_PASS       "xxxxxxxx"  /* Set password to "" for open networks. */

// Device Info
#define     DEVICE_NAME     "My Device"
#define     AUTH_TOKEN      "0123456789abcdefghijZYXWVUTSRQPO"
#define     VERSION         "1.0.0"

#define     REMOTE_AUTH     "xxxx"

// Whether or not to print debugging messages (either directly or to bridge)
//#define DEBUG_TERMINAL    V100
#define DEBUG_BRIDGE        V5
//#define INFO_TERMINAL    V100
#define INFO_BRIDGE         V5
//#define WARNING_TERMINAL  V100
#define WARNING_BRIDGE      V5
//#define ERROR_TERMINAL    V100
#define ERROR_BRIDGE        V5

// ADC hardware configuration
#define MAX_MEASURE_HARDWARE_CURRENT_A  ((double)1.500)
#define POWER_PERCENT                   ((double)100 / MAX_CURRENT_DESIRED_A)
#define BRIGHTNESS_MULTIPLIER           ((double)MAX_CURRENT_DESIRED_A / FULL_ON_HARWARE_CURRENT_A * (2.5 / 3.4))  // PWM -> DAC output limitation (0.4 .. 2.5 VDC for AL8805)

// Don't touch from here:
typedef struct {
    const char name[64] = DEVICE_NAME;
    const char auth[64] = AUTH_TOKEN; // 32 bytes not enough for 32 byte token plus \0
    const char version[16] = VERSION;
} DEVICE_INFO_T;

/* The following has to go to ld file directly beyond '*(.ver_number)':

	. = ALIGN(16);
	KEEP(*(.versionSection))

    This should locate the contents in the .bin output file at offset @ 0x1020
*/
__attribute__((section (".versionSection")))
extern DEVICE_INFO_T device_info;

extern const char name[];
extern const char auth[];
extern const char version[];

extern char hostname[];
extern char ssid[];
extern char pass[];

extern char remoteAuth[];

#endif /* privates_h */