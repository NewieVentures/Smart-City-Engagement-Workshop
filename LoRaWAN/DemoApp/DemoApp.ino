//IoT Workshop, Sodaq ONE kit, DemoApp
//By Heath Raftery

#include "Arduino.h"
#include "Sodaq_RN2483.h"
#include "Sodaq_UBlox_GPS.h"

#include <Wire.h>
#include <LSM303.h>

#define debugSerial SERIAL_PORT_MONITOR
#define loraSerial Serial1
#define groveSerial Serial

LSM303 compass;


//---------------
//     OTAA
//---------------
uint8_t DevEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t AppEUI[8] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF };
uint8_t AppKey[16] = { 0xFE, 0xED, 0xFA, 0xCE, 0xFE, 0xED, 0xFA, 0xCE, 0xFE, 0xED, 0xFA, 0xCE, 0xFE, 0xED, 0xFA, 0xCE };


//---------------
//     setup
//---------------
void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  Wire.begin();
  compass.init();
  compass.enableDefault();
  sodaq_gps.init(GPS_ENABLE);
  //sodaq_gps.setDiag(debugSerial);


  debugSerial.println("Powering up...");
  delay(3000); //Wait for power up

  while((!SerialUSB) && (millis() < 10000))
    ; // Wait 10 seconds for the Serial Monitor
  
  //Set baud rate
  debugSerial.begin(57600);
  loraSerial.begin(LoRaBee.getDefaultBaudRate());

  // Debug output from LoRaBee
  //LoRaBee.setDiag(debugSerial); // optional

  //Configure LoRa radio and connect to the LoRa Network
  debugSerial.println("Initialising LoRaWAN...");
  setupLoRa();
}

//---------------
//     loop
//---------------
void loop() {
  static int led = 0;
  char report[80];
  
  static struct {
    uint16_t data1;
    uint16_t data2;
    uint16_t data3;
    int16_t  data4;
    int16_t  data5;
    int16_t  data6;
        
    uint16_t numSatellites;
    float    latitude;
    float    longitude;
    float    altitude;
  } loRaPacket = {0};

  debugSerial.println("");

  //=======
  //RGB LED
  //=======
  led = (led+1)%8;
  digitalWrite(LED_RED,   (led & 0x01) ? HIGH : LOW);
  digitalWrite(LED_GREEN, (led & 0x02) ? HIGH : LOW);
  digitalWrite(LED_BLUE,  (led & 0x04) ? HIGH : LOW);

  //=======
  //Battery
  //=======
  uint16_t voltage = (uint16_t)((3.3 / 1.023) * (2.0 + 2.0) / 2.0 * (float)analogRead(BAT_VOLT));
  loRaPacket.data1 = voltage;
  debugSerial.print("Battery: "); debugSerial.print(voltage); debugSerial.println("mV"); 
  
  //=======
  //Compass
  //=======
  compass.read();
  float heading = compass.heading();

  loRaPacket.data2 = (uint16_t)heading;
  loRaPacket.data4 = compass.a.x;
  loRaPacket.data5 = compass.a.y;
  loRaPacket.data6 = compass.a.z;
  snprintf(report, sizeof(report), "Compass: A(%6d, %6d, %6d)  M(%6d, %6d, %6d) H(%f)",
          compass.a.x, compass.a.y, compass.a.z,
          compass.m.x, compass.m.y, compass.m.z, heading);
  debugSerial.println(report);

  //=======
  //GPS
  //=======
  if (sodaq_gps.scan(true, 1000))
  {
    loRaPacket.numSatellites  = sodaq_gps.getNumberOfSatellites();
    loRaPacket.latitude       = sodaq_gps.getLat();
    loRaPacket.longitude      = sodaq_gps.getLon();
    loRaPacket.altitude       = sodaq_gps.getAlt();
    
    debugSerial.print("Got fix: ("); debugSerial.print(sodaq_gps.getLat(), 13);
    debugSerial.print(", ");         debugSerial.print(sodaq_gps.getLon(), 13);
    debugSerial.print(") alt: ");    debugSerial.print(sodaq_gps.getAlt(), 1);
    debugSerial.print("m sats: ");   debugSerial.println(sodaq_gps.getNumberOfSatellites());
  }
  else
    debugSerial.println("No GPS fix.");


  //========
  //LoRaWAN
  //========
  debugSerial.println("Sending to LoRaWAN...");
  sendPacket((uint8_t*)(&loRaPacket), sizeof(loRaPacket));

  //Wait 10min or until button press.
  for(int i=0; i<10*60*5; i++)
  {
    delay(200);
    if(digitalRead(BUTTON) == LOW)
      break;
  }
  
}


//--------------
// helper funcs
//--------------

void setupLoRa()
{
  if(LoRaBee.initOTA(loraSerial, DevEUI, AppEUI, AppKey, false))
    debugSerial.println("OTAA connection successful!");
  else
    debugSerial.println("OTAA connection failed!");
}

void sendPacket(String packet)
{
  sendPacket((uint8_t*)packet.c_str(), packet.length());
}

void sendPacket(uint8_t *payload, uint8_t payloadSize)
{
  switch (LoRaBee.send(1, payload, payloadSize))
  {
    case NoError:
      debugSerial.println("Successful transmission.");
      break;
    case NoResponse:
      debugSerial.println("There was no response from the device.");
      setupLoRa();
      break;
    case Timeout:
      debugSerial.println("Connection timed-out. Check your serial connection to the device! Sleeping for 20sec.");
      delay(20000);
      break;
    case PayloadSizeError:
      debugSerial.println("The size of the payload is greater than allowed. Transmission failed!");
      break;
    case InternalError:
      debugSerial.println("Oh No! This shouldn't happen. Something is really wrong! Try restarting the device!\r\nThe network connection will reset.");
      setupLoRa();
      break;
    case Busy:
      debugSerial.println("The device is busy. Sleeping for 10 extra seconds.");
      delay(10000);
      break;
    case NetworkFatalError:
      debugSerial.println("There is a non-recoverable error with the network connection. You should re-connect.\r\nThe network connection will reset.");
      setupLoRa();
      break;
    case NotConnected:
      debugSerial.println("The device is not connected to the network. Please connect to the network before attempting to send data.\r\nThe network connection will reset.");
      setupLoRa();
      break;
    case NoAcknowledgment:
      debugSerial.println("There was no acknowledgment sent back!");
      // When you get this message you are probaly out of range of the network.
      break;
    default:
      break;
    }
}

