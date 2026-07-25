#include <Arduino.h>

#include <motors.h>

// motor A, left motor
constexpr int LEFT_IN1_PIN = 8;
constexpr int LEFT_IN2_PIN = 3;

// motor B, right motor
constexpr int RIGHT_IN1_PIN = 10;
constexpr int RIGHT_IN2_PIN = 9;

// initialize motors

void initializeMotors() {
    pinMode(LEFT_IN1_PIN, OUTPUT);
    pinMode(LEFT_IN2_PIN, OUTPUT);

    pinMode(RIGHT_IN1_PIN, OUTPUT); 
    pinMode(RIGHT_IN2_PIN, OUTPUT);

    analogWrite(LEFT_IN1_PIN, 0);
    analogWrite(LEFT_IN2_PIN, 0);

    analogWrite(RIGHT_IN1_PIN, 0);
    analogWrite(RIGHT_IN2_PIN, 0);
}

// left motor control

void setLeftMotor(DriveWireState& state, int16_t pwm) {
    
    pwm = constrain(pwm, -255, 255);

    if (pwm > 0) {
        analogWrite(LEFT_IN1_PIN, pwm); 
        analogWrite(LEFT_IN2_PIN, 0);
    } 
    else if (pwm < 0) {
        analogWrite(LEFT_IN1_PIN, 0);
        analogWrite(LEFT_IN2_PIN, -pwm);
    } else {
        analogWrite(LEFT_IN1_PIN, 0);
        analogWrite(LEFT_IN2_PIN, 0);
    }

    state.leftMotorPwm = pwm;
}

// right motor control 

void setRightMotor(DriveWireState& state, int16_t pwm) {

    pwm = constrain(pwm, -255, 255);

    if (pwm > 0) {
        analogWrite(RIGHT_IN1_PIN, pwm);
        analogWrite(RIGHT_IN2_PIN, 0);
    } else if (pwm < 0) {
        analogWrite(RIGHT_IN1_PIN, 0);
        analogWrite(RIGHT_IN2_PIN, -pwm);
    } else {
        analogWrite(RIGHT_IN1_PIN, 0);
        analogWrite(RIGHT_IN2_PIN, 0);
    }

    state.rightMotorPwm = pwm;
}

// drive function using previous two blocks

void drive(DriveWireState& state, int16_t leftPwm, int16_t rightPwm) {

    setLeftMotor(state, leftPwm);
    setRightMotor(state, rightPwm);

}

// coast function using drive building block

void coastMotors( DriveWireState& state) {
    
    drive(state, 0, 0);

}

// brake function: full power on both motor inputs to lock wheels

void brakeMotors(DriveWireState& state) {

    analogWrite(LEFT_IN1_PIN, 255);
    analogWrite(LEFT_IN2_PIN, 255);

    analogWrite(RIGHT_IN1_PIN, 255);
    analogWrite(RIGHT_IN2_PIN, 255);

    state.leftMotorPwm = 0;
    state.rightMotorPwm = 0;
}