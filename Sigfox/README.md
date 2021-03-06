Getting Started with the Sigfox Kit
===================================

Installation
------------

1. Plug the UNO board into the USB port of the computer.
3. Open _libraries_ folder in the _Xkit-Repos_ folder.
4. Copy the contents of the _libraries_ folder into _C:\Users\<yourusername>\Documents\Arduino\libraries_
5. Open the _DemoApp_ folder in the _Xkit-Repos_ folder.
6. Double-click the _DemoApp_ file to open in Arduino.
7. Select the _Tools->Board->Arduino/Genuino Uno_ menu item.
8. Select the _Tools->Programmer->AVRISP mkII_ menu item
9. Click the "tick" button at the top left of the DemoApp window.
Some red and white text output should appear in the black section at the bottom of the window.
If _Done compiling_ appears in the green section above the black section, then installation has been a success!
If _Done compiling_ does not appear, double check each of the steps so far or seek some help.


Programming
-----------

10. Click the "arrow" button next to the "tick" button. This programs the UNO board. After about 15 seconds _Done uploading_ should appear in the green section. If not, double check the connections or seek help.


Send a Sigfox message
---------------------

11. Unplug the USB cable.
12. Plug the Sigfox board into the UNO board, making sure all the pins line up perfectly.
13. Screw the antenna into the gold antenna port on the Sigfox board. You're now ready to send your first Sigfox message!
14. Plug the USB cable back in.
15. Press the button on the Sigfox board and keep a close eye on the board. After about 5 seconds you should see a blue LED flash 3 times. If so, the message has been sent! As well as responding to the button press, messages are automatically sent every 10 minutes, or if a magnet is brought near the board.


Verify transmission to Internet
-------------------------------

16. Open the Ubidots website in a browser and use the username and password to login:
* https://app.ubidots.com/accounts/signin/
* Username: `NewcastleIoTPioneers`
* Password: redacted

17. Click the _Devices_ link near the centre top.
18. Look for the sticker on the underside of the UNO board, and note the _SigfoxID_, eg. _002BEFDB_.
19. If a device appears named _xkit_SigfoxID_ matching the last 6 characters of your SigfoxID (case is not important), then rejoice in having transmitted your first Sigfox message! If not, try pressing the button on the Sigfox board again, or try refreshing the website, or seek some help.
20. Click on your device to see the data that was transmitted. The 7 variables include 6 data variables generated by the Xkit board and 1 more containing all the metadata produced by the Sigfox system. For reference, they are:
* _data1_ - temperature in degrees Celsius times 100.
* _data2_ - atmospheric pressure in Pascals divided by 3.
* _data3_ - light level. Higher is brighter.
* _data4_ - acceleration in the left right board direction. 250 is 1g.
* _data5_ - acceleration in the top bottom board direction. 250 is 1g.
* _data6_ - acceleration in the up down board direction. 250 is 1g - should be near 250 due to gravity.
* _message_ - incrementing packet sequence number. Also contains _Context_ data on the base station and device IDs, server time, approximate latitude and longitude, received signal strength indicator and signal to noise ratio.
21. These raw values are a little hard to work with, so let's create some friendlier variables. Click the _Add Variable_ box at the end of the variables and select _Derived_.
22. Select your device and then select the _data1_ variable.
23. In the edit box that appears, type `/100` to divide _data1_ by 100.
24. Click save and then name the new variable _Temperature_. Note it can take a while for the variable to be saved.
25. Repeat the process and this time select _data2_ and enter `*3/100` to convert to hectopascals. Name the new variable _Pressure_.


Create a Dashboard
------------------

26. Time to put this Sigfox data to use. Click the _Dashboards_ link near the centre top of the Ubidots website.
27. Click the Dashboards icon near the top left of the window.
28. Click the small orange "+" icon to create a new Dashboard.
29. Give your new Dashboard a name, click the "tick" button, and then click the Dashboard name to open your new Dashboard.
30. Let's create your first widget. Click the big orange "+" icon to add a widget.
31. Try a _Chart_ first. Click _Chart_ and then _Double axis_.
32. Click the orange "+" to add a variable. Find your device in the Widget creation dialog that appears and click it.
33. Click _Temperature_ and then _Add Variable_.
34. Repeat the add variable procedure and this time add _Pressure_.
35. Click _Finish_ and watch your data come alive.
36. Let's try a Gauge next. Click the big orange "+" icon to add another widget, select _Indicator_ and then _Gauge_.
37. Select your device and then select _data3_ (light level).
38. Set the min value to `500` and the max value to `2500` and click _Finish_.
39. What about a map? Try adding a map widget. Use the _message_ variable and the lat/lng context data will be automatically picked. Be sure to zoom a long way out - location is very approximate!
40. Check out the _Xkit Example_ for some other ideas.


Get Creative
------------

41. Think about some of the possibilities:
* What could this data be used for?
* Are there any other sensors (eg. moisture, proximity, sound) that might be useful?
* Is there an Internet service (eg. email, SMS, Twitter, IFTTT) that could be triggered via an event?
* What about untethering the board from the computer and running off a battery?

42. Start experimenting. Keep these guidelines in mind:
* All the data in Ubidots is visible to everyone in the workshop. That means you can add data from other people's things to your dashboard!
* That also means you must be careful not to modify dashboards or devices you did not create, or you risk ruining someone else's work.
* To change the data sent by the Xkit, go back to Arduino and modify the code. To upload changes *you must remove the Xkit board from the UNO board!* Alternatively, remove the two jumpers from P9 on the Xkit board.
* Note that regardless of the data you send, it must fit the existing 6 data value pattern. The first three data values are unsigned integers and the last three are signed integers. Seek help if you'd like to use the 12 bytes in some other way.
* You're not restricted to Ubidots. If there's another service you'd like to send the data to, either use Ubidots events to push it there, or seek help to redirect the Sigfox backend.

