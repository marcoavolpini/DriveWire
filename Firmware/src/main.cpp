#include <Arduino.h> 

#include <motors.h>
#include <sensors.h>
#include <serial_interface.h>
#include <drivewire_state.h>

#include <Wire.h>

DriveWireState state;

constexpr int SDA_PIN = 7;
constexpr int SCL_PIN = 15;

void setup(){
  Serial.begin(115200);
  delay(1000);

  Serial.println("DriveWire I2C Firmware Starting...");

  initializeMotors();
  coastMotors(state);

  initializeSensors(state);

  // actually initializeMotors is a void function so we can't currently check if successful

  Serial.println("Up and running.");
}

void loop() {

  static unsigned long lastSensorRefresh = 0;

  if ((millis() - lastSensorRefresh) > 1500) {
    refreshAllSensors(state);
    
    lastSensorRefresh = millis();
  }
  

}