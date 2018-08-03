# Domotica
Domotics or OpenHAB + RFLink (on a ESP32) + all kind of home made interfaces
just started

First problem is that the latest version of RFLink is R48.
We couldn't only find sources of version R29 and R35.
Both sources didn't work correctly, R29 was the best.

I'm trying to contact "the stuntteam" which has the orginal sources.

In the meanwhile I've started with version R29, got it working, and improving it now.

Some ideas that came up until now:
- optimizing the order of the plugins
- introducing a learning mode and a production mode
- use the SI4432 as an transmitter for all frequencies
- use WIFI connection to Domoticz
