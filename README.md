<p>This is yet a very basic Blynk (legacy) project with not much functionality given at this point.</p>

<p>The scope of this and other projects is to ease deployment and debugging for devices that may not be physically accessible during development phase.</p>

<p>The device contacts an OTA Server once after startup to check for updates and self-flash if one is available.
It bridges debugging output to a terminal via a bridge-server (which in my case is included in the same microservice as the OTA-Server).</p>

<p>Linker file modifications are used to not have to worry about renaming binary output after compilation.
The idea is to simply drop the generated `firmware.bin` onto a filesystem the OTA-Server can access.
The OTA Server then can read the raw data at a predefined location and store device and version data in a db or whatever deemed suitable</p>

<p>More detailed info if you are seriously inclined to already use this can be found [here](include/README.md).</p>

<p>Have fun and let me know what you like/dislike.</p>