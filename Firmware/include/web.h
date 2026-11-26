#pragma once

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#include <drivewire_state.h>

void setup_wifi(DriveWireState* state, QueueHandle_t motorCommandQueue, QueueHandle_t boostQueue);

void handle_web();