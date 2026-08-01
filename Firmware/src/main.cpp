#include <Arduino.h> 

#include <motors.h>
#include <sensors.h>
#include <serial_interface.h>
#include <drivewire_state.h>

DriveWireState state;

void setup(){
  Serial.begin(115200);
  delay(1000);

  Serial.println("DriveWire Motor and Sensor Firmware Starting...");

  initializeMotors();
  initializeSensors(state);

}

void loop() {

  delay(1000);

  // first sampled reading

  if(refreshElectricalTelemetry(state)) {
    
    Serial.print("Battery voltage at rest in V: ");
    Serial.println(state.batteryVoltageV);

    Serial.print("Battery current at rest in mA: ");
    Serial.println(state.currentMa);

    Serial.print("System power at rest in mW: ");
    Serial.println(state.powerMw);

  } else{
    if (!state.ina219Online) { Serial.println("INA219 Offline");}
    if (!state.tofOnline) { Serial.println("VL53L0X Offline");}

    Serial.println("Failed to update electrical telemetry");
  }

  delay(4000);

  if(refreshElectricalTelemetry(state)) { // unplug sensor before this to see error
    
    Serial.print("Battery voltage at rest in V: ");
    Serial.println(state.batteryVoltageV);

    Serial.print("Battery current at rest in mA: ");
    Serial.println(state.currentMa);

    Serial.print("System power at rest in mW: ");
    Serial.println(state.powerMw);

  } else{
    if (!state.ina219Online) { Serial.println("INA219 Offline");}
    if (!state.tofOnline) { Serial.println("VL53L0X Offline");}

    Serial.println("Failed to update electrical telemetry");
  }

  delay(3000);

  // to confirm the values did not change after an errored refresh
  Serial.print("Battery voltage unchanged after sensor error in V: ");
  Serial.println(state.batteryVoltageV);

  Serial.print("Battery current unchanged after sensor error in mA: ");
  Serial.println(state.currentMa);

  Serial.print("System power unchanged after sensor error in mW: ");
  Serial.println(state.powerMw);

}