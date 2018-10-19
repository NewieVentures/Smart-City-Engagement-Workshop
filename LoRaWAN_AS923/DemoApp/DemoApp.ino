//Smart City Workshop, Seeeduino LoRaWAN kit, DemoApp
//By NewieVentures


//************
//* Includes *
//************

#include <LoRaWan.h>


//***********
//* Defines *
//***********

#define SERIAL_DEBUG  SerialUSB
#define SERIAL_LORA   Serial1
#define SERIAL_GROVE  Serial

const int PIN_CHARGE_STATUS     = A5;
const int PIN_BATTERY_VOLTAGE   = A4;

//  OTAA
const char AppEUI[]    = "70B3D57ED001385C";
const char AppKey[]    = "4739207A62960378574F5003F2936774";


//**********
//* Global *
//**********

char gBuf[256]; //General scratch buffer

static struct
{
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
} gLoRaPayload = {0}; //packet payload to send via LoRa

unsigned char *gpLoRaPayload = (unsigned char*)&gLoRaPayload;


//---------------
//     setup
//---------------
void setup(void)
{
  SERIAL_DEBUG.begin(115200);
  //while(!SERIAL_DEBUG);
  
  pinMode(PIN_CHARGE_STATUS, INPUT);
  
  randomSeed(analogRead(0));
  
  setupLoRa();

  //The LoRaWAN library is garbage in general, but particularly setOTAAJoin. Difficult to do our own because while the
  //RHF76-052 docs say you can check join status with AT+JOIN=? and do auto-joins with AT+JOIN=10, 20, 4 or AT+JOIN=0
  //these all return "ERROR(-1)", so I guess not. Doing joins manually is problematic too because we have to monitor
  //the complicated responses as they come in.
  //Main problem with LoRaWAN library is that if we try to call it again too early we get "+JOIN: No band in 29378ms".
  //To workaround that, lets force join request duty cycle limitation off do our own delays:
  SerialLoRa.print("AT+LW=JDC, OFF\r\n");
  delay(500);
  
  for(int i=1; true; i++)
  {
    SERIAL_DEBUG.print("OTAA Join Attempt ");
    SERIAL_DEBUG.println(i);

    digitalWrite(PIN_LED, HIGH);
    delay(500);
    digitalWrite(PIN_LED, LOW);
    
    //For AS923 and AU915 and all others I've seen, JOIN_ACCEPT_DELAY1 is 5s and JOIN_ACCEPT_DELAY2 is 6s,
    //so the default timeout of 5s will not do. Up it to 7.
    if(lora.setOTAAJoin(JOIN, 7))
      break;

    delay(random(2000)); //insert a random delay before the next attempt
  }

  unsigned long epoch = millis();
  SERIAL_DEBUG.print("Join Successful. Took ");
  SERIAL_DEBUG.print(epoch/1000);
  SERIAL_DEBUG.println(" seconds.");
}

void loop(void)
{
  return; //DELETE THIS LINE
  
  digitalWrite(PIN_LED, HIGH);
  
  //Battery
  gLoRaPayload.data1 = analogRead(PIN_BATTERY_VOLTAGE); // note there's a 1M/100k resistor divider
  gLoRaPayload.data2 = digitalRead(PIN_CHARGE_STATUS); //"0 while charging, return 1 while charge done or no battery insert"

  //LoRaWAN
  SERIAL_DEBUG.println("Sending via LoRaWAN...");
  if(!lora.transferPacket(gpLoRaPayload, sizeof(gLoRaPayload), 4)) //Needs to block for 4 seconds to send!
    SERIAL_DEBUG.println("LoRaWAN Transmission FAILED!");
  
  digitalWrite(PIN_LED, LOW);
  
  delay(120000); //wait 2 minutes
}

//--------------
// helper funcs
//--------------

void setupLoRa()
{
  lora.init();
  lora.setDeviceReset();
  lora.setDeviceDefault();

  memset(gBuf, 0, sizeof(gBuf));
  lora.getVersion(gBuf, sizeof(gBuf));
  SERIAL_DEBUG.print(gBuf);

  memset(gBuf, 0, sizeof(gBuf));
  lora.getId(gBuf, sizeof(gBuf));
  SERIAL_DEBUG.print(gBuf);

  //Note the LoRaWAN library has _DEBUG_SERIAL_ set by default, which means this call will spit out
  //the lora module output up to now. So we call it early to avoid too much junk.
  lora.setKey(NULL/*NwkSKey*/, NULL/*AppSKey*/, (char*)AppKey);
  lora.setId(NULL/*DevAddr*/, NULL/*DevEUI*/, (char*)AppEUI);
  
  lora.setDeciveMode(LWOTAA); //Watch the creative spelling by the library.
  // A higher spreading factor allows for longer range but lower throughput
  // In AS923, DR0 to DR6 are available, DR7 is FSK. But, DR2 "should" be used for JoinReq.
  lora.setDataRate(DR2, AS923);

  //These are undocumented, but probably disable the radio's duty cycle limit throttling.
  //lora.setDutyCycle(false);
  //lora.setJoinDutyCycle(false);

  //Tx power limit in AU915 is 30dBm (1W) or 26dBm on 500kHz channels. Limit in AS923 not specified but
  //default shall be 14dBm. Some references wrongly (confused with US?) say 19dBm is max.
  //We use max power because even with 3dBi antenna we're safe, and ADR (if it's working) will lower us as
  //necessary. See mDOT developer guide: "For the ADR to judge the SNR correctly, set Tx power to maximum".
  //Unfortunately 1, despite the docs it turns out POWER command only accepts 16, 14, 12, 10, 8, 6, 4 or 2 (see AT+POWER=TABLE).
  //Unfortunately 2, the RHF76-052 is 14dBm max @ 868MHz/915MHz (20dBm max @ 434MHz/470MHz) so 14 is max anyway.
  lora.setPower(20); //20dBm, 14dBm, 11dBm, 8dBm, 5dBm or 2dBm. 14dBm is default.
}

