#include <ArduinoJson.h>       // needed for JSON encapsulation (send multiple variables with one string)
#include <ESPAsyncWebServer.h> // needed to create a simple webserver (make sure tools -> board is set to ESP32, otherwise you will get a "WebServer.h: No such file or directory" error)
#include <HardwareSerial.h>
#include <SPIFFS.h>
#include <WebSocketsServer.h> // needed for instant communication between client and server through Websockets
#include <WiFi.h>             // needed to connect to WiFi


// -------------------------- START WEB CODE SECTION --------------------------
// Attention: some of the web code section may contain snippets of code from various tutorials and example code from documentation

// Used for ARRAY_LENGTH of the sensor angle and distance values
const int ARRAY_LENGTH = 2;

AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void webSocketEvent(byte num, WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
    case WStype_DISCONNECTED:
        Serial.println("Client " + String(num) + " disconnected");
        break;
    case WStype_CONNECTED: // when a client connects, this branch is taken
        Serial.println("Client " + String(num) + " connected");

        break;
    case WStype_TEXT: // when a client sends data, this branch is taken
        break;
    }
}

// parameters used for connecting to eduroam
#define EAP_ANONYMOUS_IDENTITY "anonymous@rose-hulman.edu"
#define EAP_IDENTITY "_PUT_USER_HERE_@rose-hulman.edu"
#define EAP_PASSWORD "_PUT_PASSWORD_HERE_"

const char *ssid = "eduroam";

void setupServerStuff() {
    if (!SPIFFS.begin()) { // This is used to read files from flash memory
        Serial.println("SPIFFS could not initialize");
    }

    Serial.begin(115200);

    WiFi.mode(WIFI_MODE_STA); // Configure ESP32 as a station (client)

    WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_ANONYMOUS_IDENTITY, EAP_IDENTITY, EAP_PASSWORD); // Connect to eduroam

    while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
        delay(500);
        Serial.println("Connecting to WiFi...");
    }

    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress()); // Print the MAC address (used for adding ESP32 to the network via EIT)

    Serial.print("ESP32 IP on the WiFi network: ");
    Serial.println(WiFi.localIP()); // Print the IP address - used for connecting to the ESP32 from a web browser

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) { // When a client requests the root page, this function is called
        Serial.println("Somone's attempting to load the page.");
        request->send(SPIFFS, "/webpage.html", "text/html"); // Send the webpage to the client
    });

    server.onNotFound([](AsyncWebServerRequest *request) { request->send(404, "text/plain", "File not found"); }); // If the client requests a file that doesn't exist, send a 404 error

    server.serveStatic("/", SPIFFS, "/");

    webSocket.begin();                 // start websocket
    webSocket.onEvent(webSocketEvent); // if there's an incomming websocket message, go to function 'webSocketEvent'

    server.begin(); // Start the TCP server
}

void sendJson(String l_type, String l_value) {
    String jsonString = "";
    StaticJsonDocument<200> doc;
    JsonObject object = doc.to<JsonObject>();
    object["type"] = l_type;
    object["value"] = l_value;
    serializeJson(doc, jsonString);
    webSocket.broadcastTXT(jsonString);
}

void sendJsonArray(String l_type, int l_array_values[]) {
    String jsonString = "";
    const size_t CAPACITY = JSON_ARRAY_SIZE(ARRAY_LENGTH) + 100;
    StaticJsonDocument<CAPACITY> doc;

    JsonObject object = doc.to<JsonObject>();
    object["type"] = l_type;
    JsonArray value = object.createNestedArray("value");
    for (int i = 0; i < ARRAY_LENGTH; i++) {
        value.add(l_array_values[i]);
    }
    serializeJson(doc, jsonString);
    webSocket.broadcastTXT(jsonString);
}

int distInMM = 0;

int minAngle = -30;
int maxAngle = 30;
int targetAngle = minAngle;
int angleStep = 2;

void handleWebLoop() {
    // read out the current "time" ("millis()" gives the time in ms since the Arduino started)
    webSocket.loop();
    int sendRay[] = {targetAngle, distInMM};
    sendJsonArray("angleRange", sendRay);
}

// --------------------- END WEB CODE -------------------

// --------------------- SONAR CODE ---------------------

// can't use 0, 2, 15,16 or 17
#define transmit0 19 // 38 //15
#define transmit1 21 // 35 //2
#define transmit2 23 // 34 //0
#define transmit3 4
#define transmit4 26
#define transmit5 14 // 33 //25
#define transmit6 5
#define transmit7 18

//One instruction cycle is ~4.26ns

#define PULSE_HALF_PERIOD 2831 //positive pulse period in instruction cycles

int TXArray[8] = {transmit0, transmit1, transmit2, transmit3, transmit4, transmit5, transmit6, transmit7};

int TXoutputStatus[8] = {0, 0, 0, 0, 0, 0, 0, 0};

#define chirpPin 22


int pulseDelay = 24000000; //time between chirps in instruction cycles
int lastPulseTime = 0;
int pulseCountTarget = 10; // number of times to toggle each pin in a single chirp
int phaseTickOffset = 0;
int pulseTrainStartTime = 0;
uint32_t currentTime;
int toggleCountArray[8] = {0, 0, 0, 0, 0, 0, 0, 0};
uint32_t lastActiveTime0 = 0;
uint32_t lastActiveTime1 = 0;
uint32_t lastActiveTime2 = 0;
uint32_t lastActiveTime3 = 0;
uint32_t lastActiveTime4 = 0;
uint32_t lastActiveTime5 = 0;
uint32_t lastActiveTime6 = 0;
uint32_t lastActiveTime7 = 0;

HardwareSerial SerialPort(2); // use UART2

void setup() {
    SerialPort.begin(38400, SERIAL_8E1, 16, 17); //Start the UART2 port for MSP communication
    SerialPort.setTimeout(10);
    Serial.begin(115200);
    Serial.setTimeout(20); //VERY IMPORTANT line of code that prevents any Serial communication from locking up the code
    pinMode(transmit0, OUTPUT);
    pinMode(transmit1, OUTPUT);
    pinMode(transmit2, OUTPUT);
    pinMode(transmit3, OUTPUT);
    pinMode(transmit4, OUTPUT);
    pinMode(transmit5, OUTPUT);
    pinMode(transmit6, OUTPUT);
    pinMode(transmit7, OUTPUT);
    pinMode(chirpPin, OUTPUT);
    pinMode(13, OUTPUT);

    setupServerStuff();
}

#define TX_ELEMENT_SPACING 1.14 // distance between elements in wavelengths
int convertAngleToPhaseTickOffset(int angleDeg) {
    float angleRad = angleDeg * 3.14159265359 / 180;
    float phaseTickOffsetFloat = TX_ELEMENT_SPACING * sin(angleRad) * 2 * PULSE_HALF_PERIOD;
    int phaseTickOffsetInt = (int)phaseTickOffsetFloat;
    return phaseTickOffsetInt;
}

void loop() {

    // UART
    if (SerialPort.available()) {
        String distance = SerialPort.readString();
        distInMM = distance.toInt();
        handleWebLoop();
        GPIO.out_w1ts = ((uint32_t)1 << 13);
    } else {
        GPIO.out_w1tc = ((uint32_t)1 << 13);
    }

    if (Serial.available()) {
        String inputAngle = Serial.readString();
        targetAngle = inputAngle.toInt();
        phaseTickOffset = convertAngleToPhaseTickOffset(targetAngle);
        Serial.println("New angle: " + (String)targetAngle + " New phaseTickOffset: " + (String)phaseTickOffset);
    }

    uint32_t currentTime = ESP.getCycleCount();

    if (currentTime - lastPulseTime > pulseDelay) {

        GPIO.out_w1tc = ((uint32_t)1 << chirpPin);
        delayMicroseconds(5);
        GPIO.out_w1ts = ((uint32_t)1 << chirpPin);
        delayMicroseconds(10);
        GPIO.out_w1tc = ((uint32_t)1 << chirpPin);
        delayMicroseconds(452);

        pulseTrainStartTime = currentTime;
        int indexToCheck;

        // turn off all the pins using direct to register access - faster than digitalWrite
        GPIO.out_w1tc = ((uint32_t)1 << transmit0);
        GPIO.out_w1tc = ((uint32_t)1 << transmit1);
        GPIO.out_w1tc = ((uint32_t)1 << transmit2);
        GPIO.out_w1tc = ((uint32_t)1 << transmit3);
        GPIO.out_w1tc = ((uint32_t)1 << transmit4);
        GPIO.out_w1tc = ((uint32_t)1 << transmit5);
        GPIO.out_w1tc = ((uint32_t)1 << transmit6);
        GPIO.out_w1tc = ((uint32_t)1 << transmit7);

        currentTime = ESP.getCycleCount();
        phaseTickOffset = convertAngleToPhaseTickOffset(targetAngle);
        if (phaseTickOffset < 0) {
            indexToCheck = 0;
            int tempPhaseTickOffset = -phaseTickOffset;

            lastActiveTime7 = currentTime;
            lastActiveTime6 = currentTime + tempPhaseTickOffset * 1;
            lastActiveTime5 = currentTime + tempPhaseTickOffset * 2;
            lastActiveTime4 = currentTime + tempPhaseTickOffset * 3;
            lastActiveTime3 = currentTime + tempPhaseTickOffset * 4;
            lastActiveTime2 = currentTime + tempPhaseTickOffset * 5;
            lastActiveTime1 = currentTime + tempPhaseTickOffset * 6;
            lastActiveTime0 = currentTime + tempPhaseTickOffset * 7;
        } else {
            indexToCheck = 7;
            lastActiveTime0 = currentTime;
            lastActiveTime1 = currentTime + phaseTickOffset;
            lastActiveTime2 = currentTime + phaseTickOffset * 2;
            lastActiveTime3 = currentTime + phaseTickOffset * 3;
            lastActiveTime4 = currentTime + phaseTickOffset * 4;
            lastActiveTime5 = currentTime + phaseTickOffset * 5;
            lastActiveTime6 = currentTime + phaseTickOffset * 6;
            lastActiveTime7 = currentTime + phaseTickOffset * 7;
        }

        while (toggleCountArray[indexToCheck] <= pulseCountTarget - 1) {
            int portOutToggle = 0;
            if (toggleCountArray[0] < pulseCountTarget && currentTime > lastActiveTime0 + PULSE_HALF_PERIOD) {
                portOutToggle |= 0b01;
                lastActiveTime0 = currentTime;
                ++toggleCountArray[0];
            }
            if (toggleCountArray[1] < pulseCountTarget && currentTime > lastActiveTime1 + PULSE_HALF_PERIOD) {
                portOutToggle |= 0b10;
                lastActiveTime1 = currentTime;
                ++toggleCountArray[1];
            }
            if (toggleCountArray[2] < pulseCountTarget && currentTime > lastActiveTime2 + PULSE_HALF_PERIOD) {
                portOutToggle |= 0b100;
                lastActiveTime2 = currentTime;
                ++toggleCountArray[2];
            }
            if (toggleCountArray[3] < pulseCountTarget && currentTime > lastActiveTime3 + PULSE_HALF_PERIOD) {
                portOutToggle |= 0b1000;
                lastActiveTime3 = currentTime;
                ++toggleCountArray[3];
            }
            if (toggleCountArray[4] < pulseCountTarget && currentTime > lastActiveTime4 + PULSE_HALF_PERIOD) {
                portOutToggle |= 0b10000;
                lastActiveTime4 = currentTime;
                ++toggleCountArray[4];
            }
            if (toggleCountArray[5] < pulseCountTarget && currentTime > lastActiveTime5 + PULSE_HALF_PERIOD) {
                portOutToggle |= 0b100000;
                lastActiveTime5 = currentTime;
                ++toggleCountArray[5];
            }
            if (toggleCountArray[6] < pulseCountTarget && currentTime > lastActiveTime6 + PULSE_HALF_PERIOD) {
                portOutToggle |= 0b1000000;
                lastActiveTime6 = currentTime;
                ++toggleCountArray[6];
            }
            if (toggleCountArray[7] < pulseCountTarget && currentTime > lastActiveTime7 + PULSE_HALF_PERIOD) {
                portOutToggle |= 0b10000000;
                lastActiveTime7 = currentTime;
                ++toggleCountArray[7];
            }
            writePort(portOutToggle); // FIX HERE
            currentTime = ESP.getCycleCount();
        }
        lastPulseTime = currentTime;
        memset(toggleCountArray, 0, sizeof(toggleCountArray));
        targetAngle += angleStep;
        if (targetAngle > maxAngle) {
            targetAngle = minAngle;
        }
    }
}

// function to toggle the pins based on the bit mask toggleInt
void writePort(int toggleInt) {
    for (int i = 0; i < 8; i++) {
        if (toggleInt & 1 << i) {
            if (TXoutputStatus[i] & 1) {
                GPIO.out_w1tc = ((uint32_t)1 << TXArray[i]);
                TXoutputStatus[i] = 0;
            } else {
                GPIO.out_w1ts = ((uint32_t)1 << TXArray[i]);
                TXoutputStatus[i] = 1;
            }
        }
    }
}
