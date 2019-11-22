#include <DallasTemperature.h>
#include <OneWire.h>
#include <SPI.h>
#include <Wire.h>
#include "SSD1306.h"
#define BLYNK_PRINT Serial // Enables Serial Monitor
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>

int receivedBpm;
String BPM;
float temp;
float Fahrenheit;
SoftwareSerial s(D6,D5);

SSD1306  display(0x3C, 4, 5);

String apiKey = "7Z6XXMDI43GNHRHB"; 
const char *server = "api.thingspeak.com";

char auth[] = "299ca62addfd4767b2bf15249fdaa605";
char ssid[] = "A1";
char pass[] = "techniche";

#define ONE_WIRE_BUS 2                          //D1 pin of nodemcu

OneWire oneWire(ONE_WIRE_BUS);
 
DallasTemperature sensors(&oneWire);  

WiFiClient client;

void setup() {
s.begin(115200);
Serial.begin(115200);
sensors.begin();

WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");

Blynk.begin(auth, ssid, pass);

 display.init();
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0,"################################");
  display.drawString(20, 18,"SMART GLOVES");
  display.drawString(25, 30,"BETA VER. 1.0");
  display.drawString(0, 50,"################################");
  display.display();
  delay(10000);

}
 
void loop() {
  sensors.requestTemperatures();
  temp=sensors.getTempCByIndex(0);
  Fahrenheit = sensors.toFahrenheit(temp);
  Serial.println(temp);
  Blynk.run();
  if (s.available()>0)
  {
    receivedBpm=s.read();
    //Serial.println(receivedBpm);
    BPM=String(receivedBpm);
//    Serial.println("BPM:"+BPM);
    Blynk.virtualWrite(V5, BPM);
    Blynk.virtualWrite(V6, temp);
    Blynk.virtualWrite(V7, Fahrenheit);
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(35, 5,"--T E M P--");
  display.drawString(20,15,String(temp)+"°C"+" | "+String(Fahrenheit)+"°F");
  display.drawString(0, 25,"---------------------------------------------------");
  display.drawString(42, 35,"--BPM--");
  display.drawString(53,45,BPM);
  display.display();
    if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                            
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += BPM;
                             postStr +="&field2=";
                             postStr += String(temp);
                             postStr += "\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
 
                             Serial.println("BPM: "+BPM);
//                             Serial.print(" degrees Celcius, Humidity: ");
//                             Serial.print(h);
//                             Serial.println("%. Send to Thingspeak.");
                        }
          client.stop();
 
          Serial.println("Waiting...");
  
  // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
  //delay(10000);
  }
  }
