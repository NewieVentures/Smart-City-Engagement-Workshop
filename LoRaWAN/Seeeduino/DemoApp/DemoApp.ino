//IoT Workshop, Seeeduino LoRaWAN kit, DemoApp
//By Chris Evans

#include <LoRaWan.h>
#include "TinyGPS++.h"

#define debugSerial SerialUSB
#define gpsSerial Serial
//#define loraSerial Serial1
//#define groveSerial Serial

TinyGPSPlus gps;

//unsigned char data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA,};
char buffer[256];

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

//--------------
//  Batttery
//--------------
const int pin_battery_status  = A5;
const int pin_battery_voltage = A4;


//---------------
//     OTAA
//---------------
uint8_t DevEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t AppEUI[8] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF };
uint8_t AppKey[16] = { 0xFE, 0xED, 0xFA, 0xCE, 0xFE, 0xED, 0xFA, 0xCE, 0xFE, 0xED, 0xFA, 0xCE, 0xFE, 0xED, 0xFA, 0xCE };


//---------------
//     setup
//---------------
void setup(void)
{
    debugSerial.begin(115200);
    //while(!debugSerial);

    //=======
    //Battery
    //=======
    pinMode(pin_battery_status, INPUT);

    //======
    // GPS
    //======
    char c;
    bool locked;
    gpsSerial.begin(9600);     // open the GPS

    // For S&G, let's get the GPS fix now, before we start running arbitary
    // delays for the LoRa section

    while (!gps.location.isValid())
    {
      while (gpsSerial.available() > 0)
      {
        if (gps.encode(c=Serial.read()))
        {
          gpsLoop();
          if (gps.location.isValid())
          {
          //locked = true;
            break;
          }
        }
      //debugSerial.print(c);
      }

      //if (locked)
        //break;

      if (millis() > 15000 && gps.charsProcessed() < 10)
      {
        debugSerial.println(F("No GPS detected: check wiring."));
        debugSerial.println(gps.charsProcessed());
        while(true);
      } 
      else if (millis() > 20000)
      {
        debugSerial.println(F("Not able to get a fix in alloted time."));  
        break;
      }
    }
    
    setupLoRa();
}

void loop(void)
{
    //=======
    //Battery
    //=======
    int a = analogRead(pin_battery_voltage);
    loRaPacket.data1 = a/1023.0*3.3*11.0;        // there's an 1M and 100k resistor divider
    loRaPacket.data2 = digitalRead(pin_battery_status);

    //======
    // GPS
    //======
    gpsLoop();

    //========
    //LoRaWAN
    //========
    debugSerial.println("Sending to LoRaWAN...");
    sendPacket((uint8_t*)(&loRaPacket), sizeof(loRaPacket));
  
    delay(10000);
}

//--------------
// helper funcs
//--------------

void setupLoRa()
{
    lora.init();
    
    memset(buffer, 0, 256);
    lora.getVersion(buffer, 256, 1);
    debugSerial.print(buffer); 
    
    memset(buffer, 0, 256);
    lora.getId(buffer, 256, 1);
    debugSerial.print(buffer);
    
    // void setId(char *DevAddr, char *DevEUI, char *AppEUI);
    //lora.setId(NULL, NULL, NULL);
    // void setKey(char *NwkSKey, char *AppSKey, char *AppKey);
    //lora.setKey(NULL, NULL, NULL);
    
    lora.setDeciveMode(LWOTAA);
    lora.setDataRate(DR0, AS923);
    
    lora.setDutyCycle(false);
    lora.setJoinDutyCycle(false);
    
    lora.setPower(14);
    
    while(!lora.setOTAAJoin(JOIN));
}

void sendPacket(uint8_t *payload, uint8_t payloadSize)
{
  if(!lora.transferPacket(payload, payloadSize, 10)) debugSerial.println("LoRaWAN Transmission FAILED!");
}

void gpsLoop()
{
    debugSerial.print(F("Location: ")); 
    if (gps.location.isValid())
    {
      loRaPacket.numSatellites  = gps.satellites.value();
      loRaPacket.latitude       = gps.location.lat();
      loRaPacket.longitude      = gps.location.lng();
      loRaPacket.altitude       = gps.altitude.meters();
    }
    else
    {
      debugSerial.print(F("INVALID"));
    }

    debugSerial.print(F("  Date/Time: "));
    if (gps.date.isValid())
    {
      debugSerial.print(gps.date.month());
      debugSerial.print(F("/"));
      debugSerial.print(gps.date.day());
      debugSerial.print(F("/"));
      debugSerial.print(gps.date.year());
    }
    else
    {
      debugSerial.print(F("INVALID"));
    }

    debugSerial.print(F(" "));
    if (gps.time.isValid())
    {
      if (gps.time.hour() < 10) debugSerial.print(F("0"));
      debugSerial.print(gps.time.hour());
      debugSerial.print(F(":"));
      if (gps.time.minute() < 10) debugSerial.print(F("0"));
      debugSerial.print(gps.time.minute());
      debugSerial.print(F(":"));
      if (gps.time.second() < 10) debugSerial.print(F("0"));
      debugSerial.print(gps.time.second());
      debugSerial.print(F("."));
      if (gps.time.centisecond() < 10) debugSerial.print(F("0"));
      debugSerial.print(gps.time.centisecond());
    }
    else
    {
      debugSerial.print(F("INVALID"));
    }

    debugSerial.println();
}

