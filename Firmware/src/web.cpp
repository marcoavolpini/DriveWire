#include <web.h>

#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <dashboard.h>
#include <motors.h>


const char* ssid = "Blair WiFi";
const char* password = "RogersBlair";


WebServer server(80);
// beginning with a function to setup the WiFi communication
// first version will connect to building wifi


// the handle that will be passed from main
static QueueHandle_t webMotorQueue = nullptr;


void handleRoot() {
    server.send(200, "text/html", dashboardHtml);
}


void handleForward() {
    MotorCommand command;
    command.leftPwm = 255;
    command.rightPwm = 255;
    xQueueOverwrite(webMotorQueue, &command);

    server.send(200, "text/plain", "OK");
}


void handleReverse() {
    MotorCommand command;
    command.leftPwm = -255;
    command.rightPwm = -255;
    xQueueOverwrite(webMotorQueue, &command);

    server.send(200, "text/plain", "OK");
}

void handlePivotRight() {
    MotorCommand command;
    command.leftPwm = 255;
    command.rightPwm = -255;
    xQueueOverwrite(webMotorQueue, &command);

    server.send(200, "text/plain", "OK");
}

void handlePivotLeft() {
    MotorCommand command;
    command.leftPwm = -255;
    command.rightPwm = 255;
    xQueueOverwrite(webMotorQueue, &command);

    server.send(200, "text/plain", "OK");
}

void handleForwardLeft() {
    MotorCommand command;
    command.leftPwm = 190;
    command.rightPwm = 255;
    xQueueOverwrite(webMotorQueue, &command);

    server.send(200, "text/plain", "OK");
}

void handleForwardRight() {
    MotorCommand command;
    command.leftPwm = 255;
    command.rightPwm = 190;
    xQueueOverwrite(webMotorQueue, &command);

    server.send(200, "text/plain", "OK");
}

void handleReverseLeft() {
    MotorCommand command;
    command.leftPwm = -190;
    command.rightPwm = -255;
    xQueueOverwrite(webMotorQueue, &command);

    server.send(200, "text/plain", "OK");
}

void handleReverseRight() {
    MotorCommand command;
    command.leftPwm = -255;
    command.rightPwm = -190;
    xQueueOverwrite(webMotorQueue, &command);

    server.send(200, "text/plain", "OK");
}


void handleStop() {
    MotorCommand command;
    command.brake = true;
    command.leftPwm = 0;
    command.rightPwm = 0;

    xQueueOverwrite(webMotorQueue, &command);

    server.send(200, "text/plain", "OK");
}

void handleNotFound() {
    String message = "File not found\n\n";

    message += "URI: ";
    message += server.uri();

    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";

    message += "\nArguments: ";
    message += server.args();

    message += "\n";

    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }

    server.send(404, "text/plain", message);

}

void setup_wifi(DriveWireState* state, QueueHandle_t motorComQueue) {

    // grabbing the queue handle and saving it globally
    webMotorQueue = motorComQueue;

    // code for the building wifi mode
    /*
    // for the initial test, gonna use wifi station, but long term i have a handoff plan using both modes
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    // waiting for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    */

    // code for hosting it's own network
    WiFi.mode(WIFI_AP);

    if (!WiFi.softAP("DriveWire", "GR86Speed")) {
        Serial.println("Could not start WiFi");
    }

    state->ssid = "DriveWire";
    state->stationMode = false;
    state->IPaddress = WiFi.softAPIP();



    //state->ssid = ssid;
    //state->password = password;

    //state->IPaddress = WiFi.localIP();

    if (MDNS.begin("esp32")) {
        state->MDNSresponded = true;
    }

    // handling initial server load
    server.on("/", handleRoot);

    // if not found
    server.onNotFound(handleNotFound);

    // handling forward
    server.on("/forward", HTTP_POST, handleForward);

    // handling stop
    server.on("/stop", HTTP_POST, handleStop);

    // handling reverse
    server.on("/reverse", HTTP_POST, handleReverse);

    server.on("/pivot-left", HTTP_POST, handlePivotLeft);
    server.on("/pivot-right", HTTP_POST, handlePivotRight);

    server.on("/forward-left", HTTP_POST, handleForwardLeft);
    server.on("/forward-right", HTTP_POST, handleForwardRight);
    server.on("/reverse-left", HTTP_POST, handleReverseLeft);
    server.on("/reverse-right", HTTP_POST, handleReverseRight);

    server.begin();
}


void handle_web() {
    server.handleClient();
}