/*
 *  This sketch demonstrates how to set up a simple HTTP-like server.
 *  The server will set a GPIO pin depending on the request
 *    http://server_ip/gpio/0 will set the GPIO2 low,
 *    http://server_ip/gpio/1 will set the GPIO2 high
 *  server_ip is the IP address of the ESP8266 module, will be 
 *  printed to Serial when the module is connected.
 */

#include <ESP8266WiFi.h>

const int redPin = 4;
const int greenPin = 14;
const int bluePin = 12;

const char* ssid = "Henry's Living Room 2.4GHz";
const char* password = "13913954971";


// RGB
int red = 255;
int green = 255;
int blue = 255;

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
    Serial.begin(9600);
    delay(10);

    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);

    analogWrite(redPin, 0);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 0);
  
    // Connect to WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
  
    WiFi.begin(ssid, password);
  
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
  
    // Start the server
    server.begin();
    Serial.println("Server started");

    // Print the IP address
    Serial.println(WiFi.localIP());
}

void loop() {
    // Check if a client has connected
    WiFiClient client = server.available();
    if (!client)
    {
        return;
    }
  
    // Wait until the client sends some data
    while(!client.available())
    {
        delay(1);
    }
  
    // Read the first line of the request
    String req = client.readStringUntil('\r');
    client.flush();
  
    // Match the request
    bool on;
    if (req.indexOf("/rgb/") != -1)
    {
        on = true;
        red = hexToInt(req.substring(9, 11));
        green = hexToInt(req.substring(11, 13));
        blue = hexToInt(req.substring(13, 15));
    }
    else if (req.indexOf("/on") != -1)
    {
        on = true;
        Serial.println("Light on");
    }
    else if (req.indexOf("/off") != -1)
    {
        on = false;
        Serial.println("Light off");
    }
    else
    {
        client.stop();
        return;
    }

    // Set GPIO4 according to the request
    if (on)
    {
        analogWrite(redPin, red);
        analogWrite(greenPin, green);
        analogWrite(bluePin, blue);
    }
    else
    {
        analogWrite(redPin, 0);
        analogWrite(greenPin, 0);
        analogWrite(bluePin, 0);
    }
    
    client.flush();

    // Send the response to the client
    client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\nOK");
    delay(1);

    // The client will actually be disconnected 
    // when the function returns and 'client' object is detroyed
}


char* string2char(String command)
{
    if (command.length() != 0)
    {
        char *p = const_cast<char*>(command.c_str());
        return p;
    }
}

int hexToInt(String hex)
{
    return (int)(strtol(string2char(hex), 0, 16));
}
