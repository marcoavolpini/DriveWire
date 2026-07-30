#include <Arduino.h>

#include <motors.h>

// motor A, left motor
constexpr int LEFT_IN1_PIN = 8;
constexpr int LEFT_IN2_PIN = 3;

// motor B, right motor
constexpr int RIGHT_IN1_PIN = 10;
constexpr int RIGHT_IN2_PIN = 9;

constexpr uint32_t pwmFreq = 17500;

constexpr int L_IN1_CH = 1;
constexpr int L_IN2_CH = 2;
constexpr int R_IN1_CH = 3;
constexpr int R_IN2_CH = 4;

// initialize motors

void initializeMotors() {
    /*
    pinMode(LEFT_IN1_PIN, OUTPUT);
    pinMode(LEFT_IN2_PIN, OUTPUT);

    pinMode(RIGHT_IN1_PIN, OUTPUT); 
    pinMode(RIGHT_IN2_PIN, OUTPUT);

    analogWrite(LEFT_IN1_PIN, 0);
    analogWrite(LEFT_IN2_PIN, 0);

    analogWrite(RIGHT_IN1_PIN, 0);
    analogWrite(RIGHT_IN2_PIN, 0);
    */

    if ((ledcSetup(1, pwmFreq, 8) * ledcSetup(2, pwmFreq, 8) * ledcSetup(3, pwmFreq, 8) * 
ledcSetup(4, pwmFreq, 8))== 0){
        // considering making boolean error values like pwmSetupError but not sure
        return;
    }
    // should i add checks for all of these? is there an easier way to check? 
    ledcAttachPin(LEFT_IN1_PIN, L_IN1_CH);
    ledcAttachPin(LEFT_IN2_PIN, L_IN2_CH);

    ledcAttachPin(RIGHT_IN1_PIN, R_IN1_CH);
    ledcAttachPin(RIGHT_IN2_PIN, R_IN2_CH);

    // set all motors to rest
    ledcWrite(L_IN1_CH, 0);
    ledcWrite(L_IN2_CH, 0);

    ledcWrite(R_IN1_CH, 0);
    ledcWrite(R_IN2_CH, 0);
}

// left motor control

void setLeftMotor(DriveWireState& state, int16_t pwm) {
    
    pwm = constrain(pwm, -255, 255);

    if (pwm > 0) {
        // all these changed from analogWrite(GPIO, pwm) to this
        ledcWrite(L_IN1_CH, pwm); 
        ledcWrite(L_IN2_CH, 0);
    } 
    else if (pwm < 0) {
        ledcWrite(L_IN1_CH, 0);
        ledcWrite(L_IN2_CH, -pwm);
    } else {
        ledcWrite(L_IN1_CH, 0);
        ledcWrite(L_IN2_CH, 0);
    }

    state.leftMotorPwm = pwm;
}

// right motor control 

void setRightMotor(DriveWireState& state, int16_t pwm) {

    pwm = constrain(pwm, -255, 255);

    if (pwm > 0) {
        ledcWrite(R_IN1_CH, pwm);
        ledcWrite(R_IN2_CH, 0);
    } else if (pwm < 0) {
        ledcWrite(R_IN1_CH, 0);
        ledcWrite(R_IN2_CH, -pwm);
    } else {
        ledcWrite(R_IN1_CH, 0);
        ledcWrite(R_IN2_CH, 0);
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

    ledcWrite(L_IN1_CH, 255);
    ledcWrite(L_IN2_CH, 255);

    ledcWrite(R_IN1_CH, 255);
    ledcWrite(R_IN2_CH, 255);

    state.leftMotorPwm = 0;
    state.rightMotorPwm = 0;
}