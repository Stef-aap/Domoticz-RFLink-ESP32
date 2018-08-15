# Domotica
Domoticz or OpenHAB + RFLink (on a ESP32) + all kind of home made interfaces

Problem is that the latest version of RFLink is R48. We couldn't only find sources of version R29 and R35. Both sources didn't work correctly, R29 was the best. We tried to contact "the stuntteam" which owns the orginal sources but no response.

In the meanwhile we've started with version R29, got it working, and improving it now.

Some ideas that Are implemented:
- Compiling for the ESP32 (probably even the ESP8266 is good enough)
- Optimizing the order of the plugins
- Simplify and shorten the program
- Register devices that are allowed to get no false positives
- Automatic generate new Device-ID's, s o the Home Automation software can really get inbetween the controller and the device.
-

Some ideas for the future
- use an SI4432 as the transmitter for all frequencies
- use WIFI connection to Domoticz

