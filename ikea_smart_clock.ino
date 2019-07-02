/*********
    IKEA Smart Clock Hack

    This hack is using
    
    Based on
    WifiManger by Rui Santos - project details at http://randomnerdtutorials.com 
    and NeoPixel Ring simple sketch (c) 2013 Shae Erisson

    Released under the GPLv3 license

*********/

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN 0

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 48

// Setting up the NeoPixel library
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 50 // Time (in milliseconds) to pause between pixels

// Set web server port number to 80
WiFiServer server(80);
// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String displaySecondsState = "on";
int seconds = 1;
int z = 0;
int offset = 0;

String displayRainState = "off";

void setup() {
  Serial.begin(115200);

  pixels.begin();
  pixels.clear();
  // Yellow setup indicator light
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 80, 0));
    pixels.show();   // Send the updated pixel colors to the hardware.
    delay(DELAYVAL); // Pause before next pass through loop
  }

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  WiFi.hostname("ikea-smart-clock");
  
  // Uncomment and run it once, if you want to erase all the stored information
  //wifiManager.resetSettings();
  
  // set custom ip for portal
  //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "IKEA Smart Clock"
  // and goes into a blocking loop awaiting configuration
  
  wifiManager.autoConnect("IKEA Smart Clock");
  // or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();
  
  // if you get here you have connected to the WiFi
  Serial.println("Connected.");
  pixels.clear();
  // Green connected indicator light
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 150, 0));
    pixels.show();   // Send the updated pixel colors to the hardware.
    delay(DELAYVAL); // Pause before next pass through loop
  }
  pixels.clear();
  pixels.show();
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the modes on and off
            if (header.indexOf("GET /seconds/on") >= 0) {
              Serial.println("Display seconds on");
              displaySecondsState = "on";
              displayRainState = "off";
              pixels.clear();
              seconds = 1;
              z=0;
              
            } else if (header.indexOf("GET /seconds/off") >= 0) {
              Serial.println("Display seconds off");
              displaySecondsState = "off";
              pixels.clear();

            } else if (header.indexOf("GET /rain/on") >= 0) {
              Serial.println("Display rain on");
              displayRainState = "on";
              displaySecondsState = "off";
              pixels.clear();

            } else if (header.indexOf("GET /rain/off") >= 0) {
              Serial.println("Display rain off");
              displayRainState = "off";
              pixels.clear();
            }
            else {
               pixels.clear();
               pixels.show();
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { border: 1px solid #407ab1; background: #ffffff; color: #407ab1; padding: 12px 40px;font-size: 80%;");
            client.println("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}");
            client.println(".button:hover { border: 1px solid #407ab1; background: #407ab1; color: #ffffff; padding: 16px 40px;</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>IKEA Smart Clock</h1>");
             
            client.println("<p>Display Seconds</p>");    
            if (displaySecondsState=="off") {
              client.println("<p><a href=\"/seconds/on\"><button class=\"button\">Turn ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/seconds/off\"><button class=\"button button2\">Turn OFF</button></a></p>");
            }

            client.println("<p>Display Rain Probability</p>");    
            if (displayRainState=="off") {
              client.println("<p><a href=\"/rain/on\"><button class=\"button\">Turn ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/rain/off\"><button class=\"button button2\">Turn OFF</button></a></p>");
            } 

            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("Seconds: " + displaySecondsState);
    Serial.println("Rain: " + displayRainState);
    Serial.println("");
  }

  // Display seconds
  if (displaySecondsState=="on") {
    Serial.println(seconds);
    if(seconds>=NUMPIXELS){seconds=0;}


    //int realseconds = seconds;
    //int realz = z;
    // If your seconds run backwards and you mounted the strip in the wrong direction, use this
    int realseconds = NUMPIXELS+1-seconds;
    int realz = NUMPIXELS+1-z;
    
    
    pixels.setPixelColor(realseconds, pixels.Color(255, 100, 0));
    pixels.setPixelColor(realz, pixels.Color(0, 0, 0));
    pixels.show();
    seconds=seconds+1;
    z=seconds-1;
    delay(1350);

  // Display rain
  } else if (displayRainState=="on") {
     String colorstring = "|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000|000,000,010|000,000,100|000,000,050|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000|000,000,100|000,000,180|000,000,055|000,000,100|000,000,255|000,000,255|000,000,220|000,000,080|000,000,050|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000|000,000,000";
     // Loop through the NeoPixels
     for(int i=0; i<=NUMPIXELS; i++) {
        // Position of the data for the first Pixel
        int offset = i*12;
        String pixelcolors = colorstring.substring(offset,offset+12);
        String red = pixelcolors.substring(1,4);
        String green = pixelcolors.substring(5,8);
        String blue = pixelcolors.substring(9,12);
        pixels.setPixelColor(i, pixels.Color(red.toInt(), green.toInt(), blue.toInt()));
        pixels.show();
      }
  }

}
