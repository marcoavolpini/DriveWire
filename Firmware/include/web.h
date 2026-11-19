#pragma once

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#include <drivewire_state.h>

void setup_wifi(DriveWireState* state);