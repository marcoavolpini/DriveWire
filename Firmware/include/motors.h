#pragma once

#include <stdint.h>
#include <drivewire_state.h>

// adding struct for motor task requested action
struct MotorCommand {
    int16_t leftPwm = 0;
    int16_t rightPwm = 0;
    
    bool brake = false;
    bool boost = false;
};


void initializeMotors();

void setLeftMotor(DriveWireState& state, int16_t pwm);
void setRightMotor(DriveWireState& state, int16_t pwm);

void drive(DriveWireState& state, int16_t leftPwm, int16_t rightPwm);

void coastMotors(DriveWireState& state); 

void brakeMotors(DriveWireState& state); 

void boost(DriveWireState& state, DriveWireState& faultState, bool cmd);