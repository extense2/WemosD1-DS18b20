/* Description:
 * This is a temperature monitoring system using:
 * - two DS18b20 sensors (connected in parallel) 
 * - one Wemos D1 microcontroller 
 * - the PushingBox API, which is configured to add the values to a Google Spreadsheet. 
 * The DS18b20 is NOT wired using the one-wire feature. Instead, all 3 wires of the sensors are used. 
 * 
 * Connections: 
 * - Red:    5v
 * - Black:  GND
 * - Yellow: Digital input, on D9 (despite the fact that ONE_WIRE_BUS is defined as 2 in the code!!)
 * - Pull-up resistor: between 5v and Yellow wire
 * 
 * Compiled: 
 * 23 Jan 2018 on WeMos D1 (Retired) ESP8266. 80MHz, 921600, 4M (3M SPIFFS).
 * 
 * Based on: 
 * https://www.teachmemicro.com/log-data-nodemcu-google-sheets/
 * 
*/

#include <ESP8266WiFi.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#define SERIAL_DEBUG 0

// How often to update Google Spreadsheet with temperature data, in milliseconds
#define delayValue 120000

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2

const char* ssid     = "Your SSID here";
const char* password = "Your Password here";
char servername[] = "api.pushingbox.com";

WiFiClient client;

String result;
String conn1 = "GET /pushingbox";
String devid = "Your deviceID here";
String conn3 = " HTTP/1.1";
String value1, value2;


// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

void sendGET() //client function to send/receive GET request data.
{
  if (client.connect(servername, 80)) {  //starts client connection, checks for connection
    Serial.println("connected");
    client.println(conn1+"?devid="+devid+"&data0="+value1+"&data1="+value2+conn3); 
    client.println("Host: api.pushingbox.com");
    client.println("Connection: close");  //close 1.1 persistent connection  
    client.println(); //end of get request
  } 
  else {
    Serial.println("connection failed"); //error message if no client connect
    Serial.println();
  }
  while(client.connected() && !client.available()) delay(1); //waits for data
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
    result = result+c;
  }
  #ifdef SERIAL_DEBUG
    Serial.println(result); 
  #endif
  client.stop(); //stop client
}

void connectWiFi()
{
  Serial.println();
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("\nWiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  connectWiFi();

  // Start up the Temperature library
  // IC Default 9 bit. If you have troubles consider upping it 12. 
  // Ups the delay giving the IC more time to process the temperature measurement
  sensors.begin(); 
  Serial.print("Device Count: ");
  Serial.println(sensors.getDeviceCount());
}

void loop() {
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  #ifdef SERIAL_DEBUG
    Serial.print("Requesting temp...");
  #endif  
  
  sensors.requestTemperatures(); // Send the command to get temperatures
  
  #ifdef SERIAL_DEBUG
    Serial.println("Ok.");
    Serial.print("Temperature for Device 1 is: ");
    Serial.println(sensors.getTempCByIndex(0)); 
    Serial.print("Temperature for Device 2 is: ");
    Serial.println(sensors.getTempCByIndex(1)); 
  #endif

  value1 = sensors.getTempCByIndex(0);
  value2 = sensors.getTempCByIndex(1);
  
  sendGET();
  delay(delayValue);
}
  


