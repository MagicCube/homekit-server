/*
 *  This sketch demonstrates how to set up a simple HTTP-like server.
 *  The server will set a GPIO pin depending on the request
 *    http://server_ip/gpio/0 will set the GPIO2 low,
 *    http://server_ip/gpio/1 will set the GPIO2 high
 *  server_ip is the IP address of the ESP8266 module, will be 
 *  printed to Serial when the module is connected.
 */

#include <DHT.h>
#include <ESP8266WiFi.h>

const int redPin = 4;
const int greenPin = 14;
const int bluePin = 12;
const int dhtPin = 13;

const char* ssid = "Henry's Living Room 2.4GHz";
const char* password = "13913954971";


// RGB
int red = 255;
int green = 255;
int blue = 255;

// Create an instance of DHT11
DHT dht(dhtPin, DHT11);

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
    Serial.begin(9600);
    setupGPIOs();
    setupWiFi();
}

void loop() {
    WiFiClient client = server.available();
    if (!client)
    {
        return;
    }
  
    while(!client.available())
    {
        delay(1);
    }
  
    String req = client.readStringUntil('\r');
    client.flush();
  
    if (req.indexOf("/led") != -1)
    {
        handleLED(client, req);
    }
    else if (req.indexOf("/dht") != -1)
    {
        handleDHT(client, req);
    }
    else
    {
        client.stop();
        return;
    }
}




void setupGPIOs()
{
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);

    analogWrite(redPin, 0);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 0);
}

void setupWiFi()
{
    // Connect to WiFi network
    Serial.print("Connecting to "); Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
  
    // Start the server
    server.begin();
    Serial.println("Server started");
    Serial.println(WiFi.localIP());
}

void handleLED(WiFiClient client, String req)
{
    bool on;
    if (req.indexOf("/led/rgb/") != -1)
    {
        on = true;
        red = hexToInt(req.substring(13, 15));
        green = hexToInt(req.substring(15, 17));
        blue = hexToInt(req.substring(17, 19));
    }
    else if (req.indexOf("/led/on") != -1)
    {
        on = true;
        Serial.println("Light on");
    }
    else if (req.indexOf("/led/off") != -1)
    {
        on = false;
        Serial.println("Light off");
    }

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
    client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\nOK");
}


void handleDHT(WiFiClient client, String req)
{
    float hum = dht.readHumidity();
    float temp = dht.readTemperature();
    client.flush();
    client.print("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"temperature\":" +
        String(temp) + ",\"humidity\":" +
        String(hum) + "}");
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
