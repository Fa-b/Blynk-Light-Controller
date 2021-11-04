modify `privates_template.h` according to requirements and rename to `privates.h`

for `typedefs.h` to work as intended, Blynk libs `WidgetTerminal.h` has to be modified to make the `private` properties `protected` instead.

Additionally `WidgetTerminal.h` the `mOutBuf` property should be adjusted to use a buffer size of 128 instead of the 64 bytes default.
Otherwise messages that excceed 63 characters might appear sliced across terminal output:
```js
protected:
    uint8_t mOutBuf[128];
    uint8_t mOutQty;
};
```
You could just as well copy over the whole class and reimplement instead of inheriting.. this is a matter of laziness.

In order for `firmware.bin` output to contain device information, the linker file has to be manipulated.
While this will certainly work well also for ArduinoIDE, I haven't tested it there so you will have to find your way.
It would be desireable to automate this step in the future. When the project is rebuilt from scratch (for example after a clean) you will have to manually edit the linker file again.
Here's a snapshot that you can find in the`privates_template.h` too.. for your convenience though:
```cpp
/* This has to go to ld file directly beyond '*(.ver_number)':
	. = ALIGN(16);
	KEEP(*(.versionSection))

    This should locate the contents in the .bin output file at offset @ 0x1020
*/
__attribute__((section (".versionSection")))
extern DEVICE_INFO_T device_info;
```

The linker file is located in `.pio/build/d1_mini/ld/local.eagle.app.v6.common.ld` (for PlatformIO).
It is generated via the global header located in `%HOMEPATH%\.platformio\packages\framework-arduinoespressif8266\tools\sdk\ld\eagle.app.v6.common.ld.h`.
And the place where I put it is here:
```cpp
    ...
    /* Stuff the CRC in well known symbols at a well known location */
    __crc_len = ABSOLUTE(.);
    LONG(0x00000000);
    __crc_val = ABSOLUTE(.);
    LONG(0x00000000);
    *(.ver_number)
	. = ALIGN(16);
	KEEP(*(.versionSection))
    *.c.o(.literal*, .text*)
    *.cpp.o(EXCLUDE_FILE (umm_malloc.cpp.o) .literal*, EXCLUDE_FILE (umm_malloc.cpp.o) .text*)
    ...
```
Obviously `d1_mini` will depend on your selected env. If you decide to only locally add the section, you will have to redo the change whenever the build process decides to previously clean (or obviously when you manually clean before compilation). Adding it to the Arduino Framework's header file will automagically add that section for you.

`BlynkSimpleEsp8266.h` initializes globals which is bad practice and will lead to errors in multi-file projects. Hence initialization is carried to main.cpp here.
The last few lines in that lib header should read similar to the following and you shall be fine:
```cpp
};

extern BlynkWifi Blynk;

#include <BlynkWidgets.h>
```

`main.cpp` has the initialization somewhere which reads like this:
```cpp
// Modifying lib to avoid linker errors (multiple file projects)
static WiFiClient _blynkWifiClient;
static BlynkArduinoClient _blynkTransport(_blynkWifiClient);
BlynkWifi Blynk(_blynkTransport);
``` 