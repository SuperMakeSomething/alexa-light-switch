/* NodeMCU IoT Light Switch Code (Alexa Enabled Light Switch - Super Make Something Episode 18) - https://youtu.be/SCEoIMU8xQ8
 * by: Alex - Super Make Something
 * date: February 18th, 2019
 * license: Creative Commons - Attribution - Non-Commercial.  More information available at: http://creativecommons.org/licenses/by-nc/3.0/
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "fauxmoESP.h"
#include <Servo.h>

#define SERIAL_BAUDRATE     115200 //Serial baudrate

fauxmoESP fauxmo; //Declare fauxmoESP object
Servo lightServo; //Declare servo object

const int pwmPin = 12;  // D6 pin. More info: https://github.com/esp8266/Arduino/blob/master/variants/d1_mini/pins_arduino.h#L49-L61
const int transistorPin = 5; //D1 pin. 

// Name virtual devices
const char* deviceZero="Hallway Light";
//const char* deviceOne="LED";

int pauseTime=1000; //[ms]
int onPos=0;
int offPos=170;
int neutralPos=80;

#define WIFI_SSID "SuperMakeSomethingWifi" //Wifi SSID
#define WIFI_PASS "hunter2" //Wifi Password

void setup()
{

    // Init serial port
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    Serial.println();

    lightServo.attach(pwmPin); // Attach servo to pwmPin
    pinMode(transistorPin, OUTPUT); // Declare transistorPin as output
    //pinMode(ledPin, OUTPUT); // Declare LED pin as output pin
    
    Serial.println("Setting up wifi!");
    wifiSetup(); //Set up wifi

    //Setup fauxmo
    fauxmo.createServer(true); // not needed, this is the default value
    fauxmo.setPort(80); // This is required for gen3 devices
    fauxmo.enable(true);

    // Add virtual devices
    fauxmo.addDevice(deviceZero); //Add deviceZero
    //fauxmo.addDevice(deviceOne); //Add deviceOne -- Code template to add more devices
    
    fauxmo.onSetState(callback); //What function to execute when message is received
}

// Main Loop
void loop()
{
    // fauxmoESP uses an async TCP server but a sync UDP server
    // Therefore, we have to manually poll for UDP packets
    fauxmo.handle();
    delay(500);
    
    /* //Check for memory leaks
    static unsigned long last = millis();
    if (millis() - last > 5000) {
        last = millis();
        Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
    }
    */
}

// Device Callback
void callback(unsigned char device_id, const char * device_name, bool state, unsigned char value)
{   
    Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);
    Serial.println(device_name);
    if (strcmp(device_name, deviceZero)==0)
    {
      if (state==1) //Turn switch on
      {
        digitalWrite(transistorPin,HIGH);
        Serial.println("TransistorPin is HIGH");
        lightServo.write(onPos); // Move servo to on position
        pause(pauseTime); // Pause to wait for servo to stop moving
        lightServo.write(neutralPos); // Move servo to neutral position
        pause(pauseTime); //Pause to wait for servo to stop moving
        digitalWrite(transistorPin,LOW);
        Serial.println("Transistor Pin is LOW");
      }
      else //Turn switch off
      {
        digitalWrite(transistorPin,HIGH);
        Serial.println("TransistorPin is HIGH");
        lightServo.write(offPos); // Move servo to on position
        pause(pauseTime); // Pause to wait for servo to stop moving
        lightServo.write(neutralPos); // Move servo to neutral position
        pause(pauseTime); // Pause to wait for servo to stop moving
        digitalWrite(transistorPin,LOW);
        Serial.println("Transistor Pin is LOW");
      }
    }
    /*
	// Code template to add more devices 
     if (strcmp(device_name, deviceOne)==0)
    {
      if (state==1) //Turn switch on
      {
        digitalWrite(ledPin,HIGH); // Turn on LED 
      }
      else //Turn switch off
      {
        digitalWrite(ledPin,LOW); // Turn off LED 
      }
    }
    */
}

//Pause Function
void pause(int pauseTime)
{
  Serial.println("Entering pause function!");
  unsigned long currentMillis = millis();
  long previousMillis = currentMillis;
  while ((currentMillis-previousMillis)<pauseTime/2)
  {
    //Serial.println("In pause loop!");
    currentMillis=millis();
  }
  ESP.wdtFeed();
  currentMillis = millis();
  previousMillis = currentMillis;
  while ((currentMillis-previousMillis)<pauseTime/2)
  {
    //Serial.println("In pause loop!");
    currentMillis=millis();
  }
  Serial.println("Exiting pause function!");
}

//Wifi Setup
void wifiSetup() 
{
    WiFi.mode(WIFI_STA); 
    wifi_set_sleep_type(LIGHT_SLEEP_T);

    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(100);
    }
    Serial.print("Connected!");
    Serial.println();

    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
    Serial.println();

}
