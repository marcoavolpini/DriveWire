#pragma once

#include <stdint.h>

struct DriveWireState {

    // motor command state
    int16_t leftMotorPwm = 0;
    int16_t rightMotorPwm = 0;

    // sensor measurements
    float batteryVoltageV = 0.0f;
    float currentMa = 0.0f;
    float powerMw = 0.0f;

    uint16_t distanceMm = 0;

    // hardware status
    bool ina219Online = false;
    bool tofOnline = false;

    // safety flags
    bool obstacleDetected = false;
    bool faultActive = false;
};