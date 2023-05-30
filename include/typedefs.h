/**
 * @file       typedefs.h
 * @author     Fa-b
 * @license    This project is released under the MIT License (MIT)
 * @date       Sep 2021
 * @brief      Some print macros. also applies terminal features for bridge
 */

#include <BlynkSimpleEsp8266.h>
#include "privates.h"

#ifndef typedefs_h
#define typedefs_h

class BridgeTerminal
    : public WidgetTerminal
{
public:
    BridgeTerminal(uint8_t vPin)
        : WidgetTerminal(vPin)
    {}

    void setAuthToken(const char* token) {
        char mem[BLYNK_MAX_SENDBYTES];
        BlynkParam cmd(mem, 0, sizeof(mem));
        cmd.add(mPin);
        cmd.add("i");
        cmd.add(token);
        Blynk.sendCmd(BLYNK_CMD_BRIDGE, 0, cmd.getBuffer(), cmd.getLength()-1);
    }

    virtual void flush() {
        if (mOutQty) {
            char mem[8];
            BlynkParam cmd(mem, 0, sizeof(mem));
            cmd.add(mPin);
            cmd.add("vw");
            cmd.add(mPin);
            Blynk.sendCmd(BLYNK_CMD_BRIDGE, 0, cmd.getBuffer(), cmd.getLength(), mOutBuf, mOutQty);
            mOutQty = 0;
        }
    }
    
    void clear() {
        flush();
        char mem[BLYNK_MAX_SENDBYTES];
        BlynkParam cmd(mem, 0, sizeof(mem));
        cmd.add(mPin);
        cmd.add("vw");
        cmd.add(mPin);
        cmd.add("clr");
        Blynk.sendCmd(BLYNK_CMD_BRIDGE, 0, cmd.getBuffer(), cmd.getLength()-1);
    }
};

extern WidgetTerminal terminal;
extern BridgeTerminal bridge;
#ifdef DEBUG_TERMINAL
#define DEBUG_PRINT(x) do { terminal.print(String(name) + String("/> ") + String(x)); terminal.flush(); } while(0)
#define DEBUG_PRINTLN(x) do{ terminal.println(String(name) + String("/> ") + String(x)); terminal.flush(); } while(0)
#define DEBUG_PRINTF(x, ...) do{ terminal.print(String(name) + String("/> ")); terminal.printf(x, ## __VA_ARGS__); terminal.flush(); } while(0)
#elif DEBUG_BRIDGE
#define DEBUG_PRINT(x) do { bridge.print(String(name) + String("/> ") + String(x)); bridge.flush(); } while(0)
#define DEBUG_PRINTLN(x) do{ bridge.println(String(name) + String("/> ") + String(x)); bridge.flush(); } while(0)
#define DEBUG_PRINTF(x, ...) do{ bridge.print(String(name) + String("/> ")); bridge.printf(x, ## __VA_ARGS__); bridge.flush(); } while(0)
#elif DEBUG_SERIAL
#define DEBUG_PRINT(x) do { Serial.print(String(name) + String("/> ") + String(x)); Serial.flush(); } while(0)
#define DEBUG_PRINTLN(x) do{ Serial.println(String(name) + String("/> ") + String(x)); Serial.flush(); } while(0)
#define DEBUG_PRINTF(x, ...) do{ Serial.print(String(name) + String("/> ")); Serial.printf(x, ## __VA_ARGS__); Serial.flush(); } while(0)
#else
#define DEBUG_PRINT(x) { }
#define DEBUG_PRINTLN(x) { }
#define DEBUG_PRINTF(x, ...) { }
#endif

#ifdef INFO_TERMINAL
#define INFO_PRINT(x) do { terminal.print(String(name) + String("/> ") + String(x)); terminal.flush(); } while(0)
#define INFO_PRINTLN(x) do{ terminal.println(String(name) + String("/> ") + String(x)); terminal.flush(); } while(0)
#define INFO_PRINTF(x, ...) do{ terminal.print(String(name) + String("/> ")); terminal.printf(x, ## __VA_ARGS__); terminal.flush(); } while(0)
#elif INFO_BRIDGE
#define INFO_PRINT(x) do { bridge.print(String(name) + String("/> ") + String(x)); bridge.flush(); } while(0)
#define INFO_PRINTLN(x) do{ bridge.println(String(name) + String("/> ") + String(x)); bridge.flush(); } while(0)
#define INFO_PRINTF(x, ...) do{ bridge.print(String(name) + String("/> ")); bridge.printf(x, ## __VA_ARGS__); bridge.flush(); } while(0)
#elif INFO_SERIAL
#define INFO_PRINT(x) do { Serial.print(String(name) + String("/> ") + String(x)); Serial.flush(); } while(0)
#define INFO_PRINTLN(x) do{ Serial.println(String(name) + String("/> ") + String(x)); Serial.flush(); } while(0)
#define INFO_PRINTF(x, ...) do{ Serial.print(String(name) + String("/> ")); Serial.printf(x, ## __VA_ARGS__); Serial.flush(); } while(0)
#else
#define INFO_PRINT(x) { }
#define INFO_PRINTLN(x) { }
#define INFO_PRINTF(x, ...) { }
#endif

#ifdef WARNING_TERMINAL
#define WARNING_PRINT(x) do { terminal.print(String(name) + String("/> ") + String(x)); terminal.flush(); } while(0)
#define WARNING_PRINTLN(x) do{ terminal.println(String(name) + String("/> ") + String(x)); terminal.flush(); } while(0)
#define WARNING_PRINTF(x, ...) do{ terminal.print(String(name) + String("/> ")); terminal.printf(x, ## __VA_ARGS__); terminal.flush(); } while(0)
#elif WARNING_BRIDGE
#define WARNING_PRINT(x) do { bridge.print(String(name) + String("/> ") + String(x)); bridge.flush(); } while(0)
#define WARNING_PRINTLN(x) do{ bridge.println(String(name) + String("/> ") + String(x)); bridge.flush(); } while(0)
#define WARNING_PRINTF(x, ...) do{ bridge.print(String(name) + String("/> ")); bridge.printf(x, ## __VA_ARGS__); bridge.flush(); } while(0)
#elif WARNING_SERIAL
#define WARNING_PRINT(x) do { Serial.print(String(name) + String("/> ") + String(x)); Serial.flush(); } while(0)
#define WARNING_PRINTLN(x) do{ Serial.println(String(name) + String("/> ") + String(x)); Serial.flush(); } while(0)
#define WARNING_PRINTF(x, ...) do{ Serial.print(String(name) + String("/> ")); Serial.printf(x, ## __VA_ARGS__); Serial.flush(); } while(0)
#else
#define WARNING_PRINT(x) { }
#define WARNING_PRINTLN(x) { }
#define WARNING_PRINTF(x, ...) { }
#endif

#ifdef ERROR_TERMINAL
#define ERROR_PRINT(x) do { terminal.print(String(name) + String("/> ") + String(x)); terminal.flush(); } while(0)
#define ERROR_PRINTLN(x) do{ terminal.println(String(name) + String("/> ") + String(x)); terminal.flush(); } while(0)
#define ERROR_PRINTF(x, ...) do{ terminal.print(String(name) + String("/> ")); terminal.printf(x, ## __VA_ARGS__); terminal.flush(); } while(0)
#elif ERROR_BRIDGE
#define ERROR_PRINT(x) do { bridge.print(String(name) + String("/> ") + String(x)); bridge.flush(); } while(0)
#define ERROR_PRINTLN(x) do{ bridge.println(String(name) + String("/> ") + String(x)); bridge.flush(); } while(0)
#define ERROR_PRINTF(x, ...) do{ bridge.print(String(name) + String("/> ")); bridge.printf(x, ## __VA_ARGS__); bridge.flush(); } while(0)
#elif ERROR_SERIAL
#define ERROR_PRINT(x) do { Serial.print(String(name) + String("/> ") + String(x)); Serial.flush(); } while(0)
#define ERROR_PRINTLN(x) do{ Serial.println(String(name) + String("/> ") + String(x)); Serial.flush(); } while(0)
#define ERROR_PRINTF(x, ...) do{ Serial.print(String(name) + String("/> ")); Serial.printf(x, ## __VA_ARGS__); Serial.flush(); } while(0)
#else
#define ERROR_PRINT(x) { }
#define ERROR_PRINTLN(x) { }
#define ERROR_PRINTF(x, ...) { }
#endif

#endif /* typedefs_h */