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

  Wire.begin(SDA_PIN, SCL_PIN);
}

void loop() {

  delay(1000);

  // getting the ESP32 ready to start by sending the INA219 address and a Write bit
  Wire.beginTransmission(0x40);

  Wire.write(0x2); // queues the desired register to send to the INA219 (for bus voltage)

  Wire.endTransmission(false); // false tells ESP32 not to send STOP
  // this sends everything and gets ACK, returns transmission result

  if (Wire.requestFrom(0x40, 2) != 2) { return; }
  // sends 0x40 again, and a read bit. Wants 2 bytes of data from the register.

  uint8_t MSB = Wire.read();
  uint8_t LSB = Wire.read();

  uint16_t MSB_shifted = (MSB << 8);  // bitshift MSB by 8 to make room for the LSB
  uint16_t rawBusVoltage = (MSB_shifted | LSB);

  uint16_t voltageCounts = (rawBusVoltage >> 3); // shave off the status bits

  float busVoltage = (voltageCounts * 0.004); // multiply the voltage count by 4 mV for total


  Serial.println(busVoltage);

  delay(2000);

}