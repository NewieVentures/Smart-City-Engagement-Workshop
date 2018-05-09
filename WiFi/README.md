Getting Started with the WiFi Kit
====================================

Installation
------------

1. Plug the UNO WiFi board into the USB port of the computer.
2. Open the "DemoApp" folder.
3. Double-click the "DemoApp" file to open in Arduino.
4. Select the "Sketch->Include Library->Manage Libraries..." menu item.
5. Enter "uno wifi" in the search field and then select and install the "Arduino Uno WiFi Dev Ed Library" by Arduino.
6. After installation, click close.
7. Select the "Tools->Board->Arduino Uno WiFi" menu item
8. Select the "Tools->Ports->Arduino Uno WiFi" menu item.


Connect to WiFi
---------------

9. Look underneath the UNO to find the 6 character identifier.
10. Connect to the WiFi network named Arduino-Uno-WiFi-xxxxxx where xxxxxx is your UNO's identifier.
11. Browse to http://192.168.240.1
12. Click "CHANGE" next to "Hostname" in the web page that appears.
13. Enter "unoxxxxxx" as the hostname, where xxxxxx is your UNO's identifier.
14. Select a suitable Wifi connection, enter the password and click Connect.
15. Click the "CONNECTIVITY" link in "SETTINGS" on the left of the web page.
16. Enter "things.ubidots.com" into the "Server hostname or IP" field.
17. Enter "zJYipkVm3AXYPdjqIpzKmMSMY64OvL" into the "Username" field. Leave "Password" blank.
18. Check the "Enable MQTT client" box and confirm that the state becomes "connected". If so, then you've successfully connected over WiFi! If not, double check the steps so far, or seek help.


Send via WiFi
-------------

19. In the DemoApp window, find the line that starts with "#define ID", about 8 lines from the top.
20. Add your UNO's identifier. For example, if your UNO identifier is 822ed1, the line should look like:
* #define ID    822ed1
* _The spaces between "ID" and "822ed1" are not important._
21. Save your changes and then click the "tick" button at the top left of the DemoApp window.
* Some red and white text output should appear in the black section at the bottom of the window.
* If *Done compiling* appears in the green section above the black section, move to the next step.
* If *Done compiling* does not appear, double check each of the steps so far or seek some help.
22. Click the "arrow" button next to the "tick" button to program your changes to the UNO WiFi board. After about 15 seconds "Done uploading" should appear in the green section. If not, double check the connections or seek help.

The UNO board will automatically start sending messages every minute.


Verify transmission to Internet
-------------------------------

24. Open the Ubidots website in a browser and use the username and password to login:
* https://app.ubidots.com/accounts/signin/
* Username: _NewcastleIoTPioneers_
* Password: _m(pDTUGVAg48_
25. Click the "Devices" link near the centre top.
26. If a device appears named "uno_XXXXXX" where XXXXXX matches your UNO's ID, then rejoice in having transmitted your first WiFi message! If not, try refreshing the website, or seek some help.
27. Click on your device to see the data that was transmitted. The 6 variables are simply the raw values of the analog inputs on the UNO. For reference, they are:
* data1 - a0
* data2 - a1
* data3 - a2
* data4 - a3
* data5 - a4
* data6 - a5
28. These raw values are a little hard to work with, so let's create some friendlier variables. Click the "Add Variable" box at the end of the variables and select "Derived".
29. Select your device and then select the "data1" variable.
30. In the edit box that appears, type "*100/1023" to multiply data1 by 100 and divide by 1023.
31. Click save and then name the new variable "PercentA0". Note it can take a while for the variable to be saved.


Create a Dashboard
------------------

32. Time to put this WiFi data to use. Click the "Dashboards" link near the centre top of the Ubidots website.
33. Click the Dashboards icon near the top left of the window.
34. Click the small orange "+" icon to create a new Dashboard.
35. Give your new Dashboard a name, click the "tick" button, and then click the Dashboard name to open your new Dashboard.
36. Let's create your first widget. Click the big orange "+" icon to add a widget.
37. Try a "Chart" first. Click "Chart" and then "Line chart".
38. Click the orange "+" to add a variable. Find your device in the Widget creation dialog that appears and click it.
39. Click "PercentA0" and then "Add Variable".
40. Click "Finish" and watch your data come alive.
41. Let's try a Gauge next. Click the big orange "+" icon to add another widget, select "Indicator" and then "Gauge".
42. Select your device and then select "data2".
43. Set the min value to 0 and the max value to 1023 and click "Finish".
45. Check out the "Uno Example" for some other ideas.


Add some sensors
----------------

46. Remove the Grove Shield from under the LCD in the Grove Starter Kit.
47. Carefully align the pins with the sockets on the UNO, and firmly connect the two.
48. Use one of the included cables to connect the push button sensor to the socket marked D4.
49. Now you can skip the 10 minute delay between data updates by pushing the button!
50. Try connecting the "Rotary Angle Sensor" (names are on the bottom of each sensor) to the socket marked A0 so you can control the data1 value.


Get Creative
------------

51. Think about some of the possibilities:
* What could this data be used for?
* Are there any other sensors (eg. moisture, proximity, sound) that might be useful?
* Is there an Internet service (eg. email, SMS, Twitter, IFTTT) that could be triggered via an event?
* What about untethering the board from the computer and running off a battery?
52. Start experimenting. Keep these guidelines in mind:
* All the data in Ubidots is visible to everyone in the workshop. That means you can add data from other people's things to your dashboard!
* That also means you must be careful not to modify dashboards or devices you did not create, or you risk ruining someone else's work.
* To change the data sent by the UNO, go back to Arduino and modify the code.
* Note that regardless of the data you send, it must fit the existing 6 data value pattern. The first three data values are unsigned integers and the last three are signed integers. Seek help if you'd like to send a different format.
* You're not restricted to Ubidots. If there's another service you'd like to send the data to, either use Ubidots events to push it there, or seek help to redirect the WiFi backend.
