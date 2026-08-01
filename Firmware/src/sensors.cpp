#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_VL53L0X.h>

#include <sensors.h>

Adafruit_INA219 ina219;
Adafruit_VL53L0X tof;

constexpr int SDA_PIN = 7;
constexpr int SCL_PIN = 15;

bool initializeSensors(DriveWireState& state) {

    // initalize I2C bus with custom pins
    if (!Wire.begin(SDA_PIN, SCL_PIN)) { 
        
        state.ina219Online = false;
        state.tofOnline = false;

        return false;
    }

    // start both sensors

    bool ina219Status = ina219.begin();

    bool tofStatus = tof.begin();

    // update online status'

    state.ina219Online = ina219Status;
    state.tofOnline = tofStatus;


    return (ina219Status && tofStatus);

}

// updates the current, voltage and power telemetry
bool refreshElectricalTelemetry(DriveWireState& state) {

    // important values
    float busVoltage = 0;
    float currentmA = 0;
    float shuntVoltage_mV = 0;
    float batteryVoltage = 0;
    float systemPower = 0;


    // reading status'
    bool busVoltageSuccess = false;
    bool shuntVoltageSuccess = false;
    bool currentSuccess = false;

    bool refreshSuccess = false;

    // refresh the system voltage
    busVoltage = ina219.getBusVoltage_V();
    busVoltageSuccess = ina219.success();

    if (!busVoltageSuccess) {
        state.ina219Online = false;
        return false;
    } // report error instead of checking the others

    shuntVoltage_mV = ina219.getShuntVoltage_mV();
    shuntVoltageSuccess = ina219.success();

    if (!shuntVoltageSuccess) {
        state.ina219Online = false;
        return false;
    }

    // important to test which side the shunt voltage is actually on to know sign in this equation
    batteryVoltage = busVoltage + (shuntVoltage_mV / 1000);


    // refresh the system current
    currentmA = ina219.getCurrent_mA();
    currentSuccess = ina219.success();

    if (!currentSuccess) {
        state.ina219Online = false;
        return false;
    }


    // refresh system power, doesnt really need to be in an if statement anymore

    if (busVoltageSuccess && shuntVoltageSuccess && currentSuccess) {
        systemPower = batteryVoltage * currentmA;

        refreshSuccess = true;
    }

    // final function success output
    if (refreshSuccess) {

        state.batteryVoltageV = batteryVoltage;
        state.currentMa = currentmA;
        state.powerMw = systemPower;

        state.ina219Online = true;
        return true;
    } else {
        state.ina219Online = false;
        return false;
    }

}