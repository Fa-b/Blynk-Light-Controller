
#include <Arduino.h>

#ifndef OTA_updater_h
#define OTA_updater_h

int versionCompare(String, String);
void checkForUpdates(void);

#endif /* OTA_updater_h */