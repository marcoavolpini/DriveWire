#include <web.h>

#include <string.h>


const char* ssid = "Blair WiFi";
const char* password = "RogersBlair";

WebServer server(80);
// beginning with a function to setup the WiFi communication
// first version will connect to building wifi
bool setup_wifi(DriveWireState* state) {

    

}

void handleRoot() {
    server.send(200, "text/plain", "hello from esp32!");
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