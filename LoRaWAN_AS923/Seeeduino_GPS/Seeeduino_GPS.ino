//TTN Mapper for Seeeduino LoRaWAN
//By NewieVentures

#include <LoRaWan.h>
#include "TinyGPS++.h"

#define debugSerial SerialUSB
#define gpsSerial   Serial
#define LED_PIN     13

TinyGPSPlus gps;

char buffer[256];

//---------------
//     OTAA
//---------------
const char *appEui = "DEADBEEFDEADBEEF";
const char *appKey = "FEEDFACEFEEDFACEFEEDFACEFEEDFACE";
//const char *devEUI = "472AC86800390048";

//---------------
//     setup
//---------------
void setup(void)
{
  pinMode(LED_PIN, OUTPUT);
  
  debugSerial.begin(115200);

  //=========
  //LoRA Join
  //=========
  
  //GPS will take a while to lock, so lets make sure we can join first.
  setupLoRa();

  //The LoRaWAN library is retarded in general, but particularly setOTAAJoin. Difficult to do our own because while
  //the RHF76-052 docs say you can check join status with AT+JOIN=? and do auto-joins with AT+JOIN=10, 20, 4 or AT+JOIN=0
  //these all return "ERROR(-1)", so I guess not. Doing joins manually is problematic too because we have to monitor the
  //complicated responses as they come in. Main problem with LoRaWAN library is that if we try to call it again too early
  //we get "+JOIN: No band in 29378ms". To workaround that, lets force join request duty cycle limitation off:
  SerialLoRa.print("AT+LW=JDC, OFF\r\n");
  delay(500);
  
  for(int i=1; true; i++)
  {
    debugSerial.print("OTAA Join Attempt ");
    debugSerial.println(i);

    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    
    //For AS923 and AU915 and all others I've seen, JOIN_ACCEPT_DELAY1 is 5s and JOIN_ACCEPT_DELAY2 is 6s,
    //so the default timeout of 5s will not do. Up it to 7.
    if(lora.setOTAAJoin(JOIN, 7))
      break;
  }

  unsigned long epoch = millis();
  debugSerial.print("Join Successful. Took ");
  debugSerial.print(epoch/1000);
  debugSerial.println(" seconds.");
    
  //=========
  // GPS Lock
  //=========
  char c;
  bool locked = false;
  gpsSerial.begin(9600);     // open the GPS

  debugSerial.println("Waiting for GPS lock.");
  while(!gps.location.isValid())
  {
    while(gpsSerial.available() > 0)
    {
      gps.encode(gpsSerial.read());
      if(gps.location.isValid())
        break;
    }

    /*
    else if (millis() > 30000)
    {
      debugSerial.println(F("Not able to get a fix in alloted time."));
      break;
    }
    */
    unsigned long now = millis();
    if(now - epoch >= 4000)
    {
      digitalWrite(LED_PIN, LOW);
      if(gps.charsProcessed() < 10)
      {
        debugSerial.println(F("No GPS detected: check wiring."));
        debugSerial.println(gps.charsProcessed());
        while (true);
      }
      
      epoch = millis();
      printGPS();
    }
    else if(now - epoch >= 3000)
      digitalWrite(LED_PIN, HIGH);
    else if(now - epoch >= 2000)
      digitalWrite(LED_PIN, LOW);
    else if(now - epoch >= 1000)
      digitalWrite(LED_PIN, HIGH);
  }
  
  debugSerial.println("GPS lock obtained.");
}

void loop(void)
{
  digitalWrite(LED_PIN, HIGH);
  
  //======
  // GPS
  //======
  printGPS();

  //========
  //LoRaWAN
  //========
  bool txResult = false;

  uint8_t txBuffer[9];
  uint32_t latitudeBinary, longitudeBinary;
  uint16_t altitudeInt;
  uint8_t hdopInt;

  static double lastLat=0, lastLon=0;
  
  double thisLat = gps.location.lat();
  double thisLon = gps.location.lng();
  double thisHdop = gps.hdop.hdop();
  double distanceMetres = TinyGPSPlus::distanceBetween(thisLat, thisLon, lastLat, lastLon);
  
  debugSerial.print("Distance: ");
  debugSerial.print(distanceMetres);
  debugSerial.print(". HDOP: ");
  debugSerial.print(thisHdop);
  debugSerial.println(".");
  
  if(distanceMetres > 10 && thisHdop > 0) //then it's worth sending to TTN (we get a garbage hdop==0 on first fix)
  {
    if(thisHdop  < 5) //then it's worth using next time
    {
      lastLat = thisLat;
      lastLon = thisLon;
    }
    
    latitudeBinary = ((thisLat + 90) / 180) * 16777215;
    longitudeBinary = ((thisLon + 180) / 360) * 16777215;
  
    txBuffer[0] = ( latitudeBinary >> 16 ) & 0xFF;
    txBuffer[1] = ( latitudeBinary >> 8 ) & 0xFF;
    txBuffer[2] = latitudeBinary & 0xFF;
  
    txBuffer[3] = ( longitudeBinary >> 16 ) & 0xFF;
    txBuffer[4] = ( longitudeBinary >> 8 ) & 0xFF;
    txBuffer[5] = longitudeBinary & 0xFF;
  
    altitudeInt = gps.altitude.meters();
    txBuffer[6] = ( altitudeInt >> 8 ) & 0xFF;
    txBuffer[7] = altitudeInt & 0xFF;
  
    hdopInt = thisHdop *10;
    txBuffer[8] = hdopInt & 0xFF;
  
    debugSerial.println("Sending to TTN...");
    if(!lora.transferPacket(txBuffer, sizeof(txBuffer), 4)) //needs to block for 4 seconds to send!
      debugSerial.println("LoRaWAN Transmission FAILED!");
  }
  else //if we don't send, only block for 1 second to make for a shorter LED flash
    delay(1000);

  digitalWrite(LED_PIN, LOW);
  
  unsigned long now, epoch = millis();
  do
  {
    while(gpsSerial.available() > 0)
      gps.encode(gpsSerial.read());
    
    now = millis();
  } while(now - epoch < 20000); //wait for 20s
}

//--------------
// helper funcs
//--------------

void setupLoRa()
{
  lora.init();
  lora.setDeviceReset();
  lora.setDeviceDefault();

  memset(buffer, 0, sizeof(buffer));
  lora.getVersion(buffer, sizeof(buffer));
  debugSerial.print(buffer);

  memset(buffer, 0, sizeof(buffer));
  lora.getId(buffer, sizeof(buffer));
  debugSerial.print(buffer);

  lora.setKey(NULL/*NwkSKey*/, NULL/*AppSKey*/, (char*)appKey); //note if _DEBUG_SERIAL_ is set, the output up until here will be printed
  lora.setId(NULL/*DevAddr*/, NULL/*DevEUI*/, (char*)appEui);
  
  lora.setDeciveMode(LWOTAA);
  // a higher spreading factor allows for longer range but lower throughput
  // In AS923, DR0 to DR6 are available, DR7 is FSK. But, DR2 "should" be used for JoinReq.
  lora.setDataRate(DR2, AS923);

  //These are undocumented, but probably disable the radio's duty cycle limit throttling.
  //lora.setDutyCycle(false);
  //lora.setJoinDutyCycle(false);

  //Tx power limit in AU915 is 30dBm (1W) or 26dBm on 500kHz channels. Limit in AS923 not specified but
  //default shall be 14dBm. Some references wrongly (confused with US?) say 19dBm is max.
  //We use max power because even with 3dBi antenna we're safe, and ADR (if it's working) will lower us as
  //necessary. From mDOT developer guide, "For the ADR to judge the SNR correctly, set Tx power to maximum".
  //Unfortunately 1, despite the docs it turns out POWER command only accepts 16, 14, 12, 10, 8, 6, 4 or 2 (see AT+POWER=TABLE).
  //Unfortunately 2, the RHF76-052 is 14dBm max @ 868MHz/915MHz (20dBm max @ 434MHz/470MHz) so 14 is max anyway.
  lora.setPower(20); //20dBm, 14dBm, 11dBm, 8dBm, 5dBm or 2dBm. 14dBm is default.
}

void printGPS()
{
  //** Lat/lon **
  debugSerial.print(F("Location: "));
  if (gps.location.isValid())
  {
    debugSerial.print(gps.location.lat());
    debugSerial.print(F("/"));
    debugSerial.print(gps.location.lng());
    debugSerial.print(F("/"));
    debugSerial.print(gps.altitude.meters());
    debugSerial.print(F(" ("));
    debugSerial.print(gps.satellites.value());
    debugSerial.print(F(")"));
  }
  else
    debugSerial.print(F("INVALID"));

  //** Date **
  debugSerial.print(F("  Date/Time: "));
  //date.isValid just means we got a date string. Accuracy is reported in GPZDG, which is not supported, so we'll just print regardless.
  if (gps.date.isValid())
  {
    debugSerial.print(gps.date.day());
    debugSerial.print(F("/"));
    debugSerial.print(gps.date.month());
    debugSerial.print(F("/"));
    debugSerial.print(gps.date.year());
  }
  else
  {
    debugSerial.print(F("INVALID"));
  }

  //** Time **
  debugSerial.print(F(" "));
  //time.isValid just means we got a time string. Accuracy is reported in GPZDG, which is not supported, so we'll just print regardless.
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

  //** Satellites **
  debugSerial.print(F(" Sats: "));
  //TinyGPS++ doesn't support GPGSA or GPGVA, so we don't know satellite details,
  //just the GGA number (confusion abounds about whether this is "in use" or "in view").
  debugSerial.print(gps.satellites.value());

  debugSerial.println();
}

