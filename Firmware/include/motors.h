#pragma once

#include <stdint.h>
#include <drivewire_state.h>

void initializeMotors();

void setLeftMotor(DriveWireState& state, int16_t pwm);
void setRightMotor(DriveWireState& state, int16_t pwm);

void drive(DriveWireState& state, int16_t leftPwm, int16_t rightPwm);

void coastMotors(DriveWireState& state); 

void brakeMotors(DriveWireState& state); 