# RPM_LED
Arduino reads RPM data from CAN bus to control interior LED hue.
LEDs appear blue-white at idle, red at redline. LEDs flash when near redline.
Paramaters are set to 2011-2014 Dodge Charger with 5.7L V8.
Should be compatible with any vehicle with a CAN bus, just change redline value
to your engine. Idle is not currently used. Can use OBDII port, I will be
connecting directly to the CAN-C STAR connector for an integrated installation.

**In development:**
- Change LED color to Amber if Malfunction Indicator Light is illuminated
- Read voltage from interior light switch to determine appropriate brightness

**Modules used:** 
- //TODO

**Changelog:**
- v1.0 : LED hue working, brightness defaults to 65%
