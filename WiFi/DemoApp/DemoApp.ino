//IoT Workshop, UNO Wifi kit, DemoApp
//By Heath Raftery

#include <Wire.h>
#include <UnoWiFiDevEd.h>

//Change xxxxxx to your 6 character identifier written on the bottom of the UNO WiFi board.
#define ID xxxxxx   


#if ID == xxxxxx
#error You must provide an ID!
#endif

#define CONCAT_(a, b)   a #b
#define CONCAT(a, b)    CONCAT_(a,b)

#define TOPIC           CONCAT("/v1.6/devices/uno_", ID)
#define CONNECTOR       "mqtt"

void setup()
{
  pinMode(4, INPUT);
  
  Serial.begin(9600);
  Ciao.begin();

  //Serial.println(TOPIC);
}

void loop()
{
  char payload[200];
  uint16_t data1;
  uint16_t data2;
  uint16_t data3;
  int16_t  data4;
  int16_t  data5;
  int16_t  data6;

  data1 = analogRead(A0);
  data2 = analogRead(A1);
  data3 = analogRead(A2);
  data4 = analogRead(A3);
  data5 = analogRead(A4);
  data6 = analogRead(A5);
  
  snprintf(payload, sizeof(payload),
           "{ \"data1\": %u, \"data2\": %u, \"data3\": %u, \"data4\": %d, \"data5\": %d, \"data6\": %d }",
             data1, data2, data3, data4, data5, data6);

  Serial.println("Sending data via WiFi:");
  Serial.println(payload);
  
  Ciao.write(CONNECTOR, TOPIC, payload);
  
  //Wait 10min or until button press.
  for(int i=0; i<10*60*5; i++)
  {
    delay(200);
    if(digitalRead(4) == HIGH)
      break;
  }
}

