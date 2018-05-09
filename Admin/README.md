Workshop Laboratory Setup
=========================

Some tips on setting up the lab to conduct a Smart City Engagement Workshop.


Parameters
----------

- There are 18 individual kits. At least 3 (and preferably more) should be dedicated as spares.
- There are 6 each of 3 fundamentally different types of kit: WiFi, Sigfox and LoRaWAN.
- Lots of non-essential components are shared between kits.
- The boxes contain 3 kits each, one of each type, plus 2 boxes of supporting gear, so a total of 8 boxes.
- Teams of 3 work well, so the collection supports up to 45 people.
- A workshop takes at least 1.5 hours and up to 3 hours.
- Each team needs a workstation, described below.


Workstation
-----------

- PC or laptop with a web browser and Arduino (latest version).
  - If using Windows, ensure the CH341SER drivers can be installed for the Xkit. Other drivers install automatically.
- USB port.
- For the WiFi kit, the ability to connect to a WiFi access point is useful. Otherwise participants will need to use a smartphone or other device.


Preparation
-----------

- Ensure enough workstations are available.
- Ensure Arduino is installed, web access is available and a USB port is accessible.
- Ensure the CH341SER drivers can be installed with user permissions, otherwise install on each workstation.
- Ensure there is Sigfox coverage (use the Sens'it).
- Ensure there is cellular coverage for the cellular modem.


Gotchas
-------

- The "port" name is unpredictable in Windows. In Arduino, usually you need to select the highest number COM port in Tools->Port.
- The XKit is picky about whether the Sigfox board is plugged in or not. If the Sketch is not uploading, try removing the Sigfox board first.
- The Sigfox backend is at http://backend.sigfox.com
- The WiFi UNOs are unreliable. Frequently drop out and can be frustrating to get going.



Original Development Notes
==========================

Xkit
----

To go untethered:
- 9V
- jumper

- Registered device on Thinxtra backend https://backend.sigfox.com
- Registered account on Ubidots
- Found Thinxtra/Xkit-Sample on Github
- Sent event URL to Ubidots for free credits. Get 5000 to start, but costs 500 / device and 100 / dashboard, so going to run out quick.
- Got code for 50 lots of 2000 credits: HUNTER2017


SODAQ
-----

Soldered pins.

Conduit came with Australian plug! Onya Glyn.
Conduit did not have default 192.168.2.1 address! Was a DHCP client instead.
Still got new set up wizard on first login.
Set to DHCP client, LAN side, no HTTPS access, no WAN access.
Default credentials.
MAC address: 00:08:00:4A:18:43
Configured LoRa Network Server:
- Enabled, AU915, sub-band 4, public.
- Mode: NETWORK SERVER (Ubidots also supports packet forwarder, but opted to use Node-RED instead to present friendly interface to Ubidots)
- EUI: DEADBEEFDEADBEEF
- Key: FEEDFACEFEEDFACEFEEDFACEFEEDFACE
- Log destination: FILE
- Path: /var/log/lora-network-server2.log
 
Connected SODAQ One. Selected SODAQ ONE with ONEBase SPI board and cu.usbmodem1441 (SODAQ ExpLoRa) port.

Turns out it has a Serial Passthrough pre-loaded. Prompts with words to that effect. Setting line ending to “Both NL & CR” gets it communicating with the RN2903AU.

Re-programming the SerialPassthrough sketch is also successful.

Downloaded `LoRaWAN_ABP_OTAA_SAMPLE``, renamed it DemoApp and started modifying:

- Changed `DEFAULT_FSB` from 2 to 4
- Changed `DEFAULT_SF_915` from 7 to 9


WiFi
----

Plugged in board, connected to WiFi: Arduino-Uno-WiFi-822ed1.
Browsed to http://192.168.240.1/
Renamed uno822ed1, connected to WiFi, reconnects, shows new IP.

Disaster, ciao doesn’t support POST bodies: http://forum.arduino.cc/index.php?topic=461471.0

Spent hours, no resolution. Almost gave up but then… what about MQTT?

MQTT works! Have to get user to set it up. Need to write ID on bottom of each board.
