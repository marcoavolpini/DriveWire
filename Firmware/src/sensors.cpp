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


// distance ToF telemetry
bool refreshDistanceTelemetry(DriveWireState& state) {

    // a type of measurement struct
    VL53L0X_RangingMeasurementData_t measure;
    uint16_t distance = 0;

    VL53L0X_Error status = tof.rangingTest(&measure, false);

    if (status != VL53L0X_ERROR_NONE) {
        state.tofOnline = false;
        return false;
    }

    if (measure.RangeStatus == 0) { 
        distance = measure.RangeMilliMeter;
        state.distanceMm = distance;

        state.tofOnline = true;
        return true;

    } else {
        state.tofOnline = true;
        return false;
    }

}


// convenience function - refreshes everything at once

bool refreshAllSensors(DriveWireState& state){ 

    // note that currently one could succeed, and then one telemetry point is updated and one isnt
    bool electricalState = refreshElectricalTelemetry(state);
    bool tofState = refreshDistanceTelemetry(state);

    return (electricalState && tofState);

}


// based on my explicit INA219 reading test using only Wire - only for testing

float getBusVoltage() {

    // getting the ESP32 ready to start by sending the INA219 address and a Write bit  
    Wire.beginTransmission(0x40);

    Wire.write(0x2); // queues the desired register to send to the INA219 (for bus voltage)

    Wire.endTransmission(false); // false tells ESP32 not to send STOP
    // this sends everything and gets ACK, returns transmission result

    if (Wire.requestFrom(0x40, 2) != 2) { return NAN; }
    // sends 0x40 again, and a read bit. Wants 2 bytes of data from the register.

    uint8_t MSB = Wire.read();
    uint8_t LSB = Wire.read();

    uint16_t MSB_shifted = (MSB << 8);  // bitshift MSB by 8 to make room for the LSB
    uint16_t rawBusVoltage = (MSB_shifted | LSB);

    uint16_t voltageCounts = (rawBusVoltage >> 3); // shave off the status bits

    float busVoltage = (voltageCounts * 0.004); // multiply the voltage count by 4 mV for total

    return busVoltage;  
}