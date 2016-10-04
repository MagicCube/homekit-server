#include <DHT.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SSD1306.h>

// GPIO Pins
const int DHT_PIN = D2;
const int RED_PIN = D5;
const int GREEN_PIN = D4;
const int BLUE_PIN = D3;
const int OLED_SDA_PIN = D6;
const int OLED_SCK_PIN = D7;

const char* ssid = "Henry's Living Room 2.4GHz";
const char* password = "13913954971";


// RGB LED
int red = 255;
int green = 255;
int blue = 255;
int brightness = 100;

int temperature = 0;
int humidity = 0;
int dhtUpdateInterval = 5 * 1000;
long dhtLastUpdateTime = 0;
long birghtnessLastDisplayTime = 0;

// DHT11
DHT dht(DHT_PIN, DHT11);

// OLED Display
SSD1306 display(0x3c, OLED_SDA_PIN, OLED_SCK_PIN);

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
    Serial.begin(9600);
    setupGPIOs();
    setupScreen();
    println("Starting Lighthouse...");
    setupWiFi();
}

void loop()
{
    if (dhtLastUpdateTime == 0 ||
        millis() - dhtLastUpdateTime > dhtUpdateInterval)
    {
        updateDHT();
        render();
    }
    else if (birghtnessLastDisplayTime != 0 && birghtnessLastDisplayTime < millis())
    {
        birghtnessLastDisplayTime = 0;
        render();
    }
    WiFiClient client = server.available();
    if (!client)
    {
        return;
    }
    
    while (!client.available())
    {
        delay(1);
    }
    handleClient(client);
}



void updateDHT()
{
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    dhtLastUpdateTime = millis();
}

void render()
{
    bool showBrightness = birghtnessLastDisplayTime > millis();
    
    display.clear();
    display.setFont(ArialMT_Plain_24);
    display.drawString(5, 32, String(temperature));
    display.drawString(45, 32, String(humidity));
    
    display.setFont(ArialMT_Plain_10);
    display.drawString(5, 10, "TEMP");
    display.drawString(45, 10, "HUM");
    
    if (showBrightness)
    {
        display.drawString(85, 10, "LED");
        int frameHeight = 30;
        int rectHeight = brightness * 0.01 * frameHeight;
        display.fillRect(90, 26 + (frameHeight - rectHeight), 10, rectHeight);
        display.drawRect(90, 26, 10, frameHeight);
    }

    display.display();
}



void setupGPIOs()
{
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);

    analogWrite(RED_PIN, 0);
    analogWrite(GREEN_PIN, 0);
    analogWrite(BLUE_PIN, 0);
}

void setupScreen()
{
    display.init();
    display.setLogBuffer(5, 30);
    display.flipScreenVertically();
    display.clear();
    display.setContrast(255);
}

void setupWiFi()
{
    // Connect to WiFi network
    print("Connecting to "); println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        print(".");
    }
    println("\nWiFi connected");

    // Start the server
    server.begin();
    println(WiFi.localIP().toString());
    delay(1000);
}








void print(String text)
{
    Serial.print(text);
    display.clear();
    display.print(text);
    display.drawLogBuffer(0, 0);
    display.display();
}

void println(String text)
{
    print(text + "\n");
}




void handleClient(WiFiClient client)
{
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
    }
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
        brightness = hexToInt(req.substring(19, 21));
        Serial.println("LED color -> #" + req.substring(13, 19));
    }
    else if (req.indexOf("/led/on") != -1)
    {
        on = true;
        Serial.println("LED switched ON");
    }
    else if (req.indexOf("/led/off") != -1)
    {
        on = false;
        Serial.println("LED switched OFF");
    }

    if (on)
    {
        analogWrite(RED_PIN, red);
        analogWrite(GREEN_PIN, green);
        analogWrite(BLUE_PIN, blue);
        birghtnessLastDisplayTime = millis() + 2000;
        render();
    }
    else
    {
        analogWrite(RED_PIN, 0);
        analogWrite(GREEN_PIN, 0);
        analogWrite(BLUE_PIN, 0);
    }

    client.flush();
    client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\nOK");
}


void handleDHT(WiFiClient client, String req)
{
    client.flush();
    client.print("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"temperature\":" +
        String(temperature) + ",\"humidity\":" +
        String(humidity) + "}");
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
