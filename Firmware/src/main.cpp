#include <Arduino.h> 

#include <motors.h>
#include <sensors.h>
#include <serial_interface.h>
#include <drivewire_state.h>

// including FreeRTOS libraries
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

DriveWireState state;

// initializing a queue handle for the sensor task that holds a snapshot of sensor telemetry
// it is a pointer to a queue?
QueueHandle_t sensorQueue = nullptr;

// declaring sensorTask
void sensorTask(void* parameter);


void setup(){
  Serial.begin(115200);
  delay(1000);

  Serial.println("DriveWire I2C Firmware Starting...");

  initializeMotors();
  coastMotors(state);

  if (initializeSensors(state)) {Serial.println("Sensors Initialized...");}
  else { Serial.println(state.ina219Online); Serial.println(state.tofOnline);}



  // gonna create the queue and assign it to the handle initialized globally
  sensorQueue = xQueueCreate(1, sizeof(DriveWireState));

  if (sensorQueue == nullptr) {
    Serial.println("Failed to create sensor queue");
  }

  // creating the sensor task
  BaseType_t taskResult = xTaskCreate(
    sensorTask, 
    "sensorTask", 
    4096, 
    &state, 
    1, 
    nullptr
  );

  // checking for successful sensor task creation
  if (taskResult != pdPASS) {
    Serial.println("Failed to create sensor task.");
  }


  // actually initializeMotors is a void function so we can't currently check if successful

  Serial.println("Program starting...");
}

void loop() {

  static unsigned long lastSensorRefresh = 0;

  if ((millis() - lastSensorRefresh) > 1500) {
    refreshAllSensors(state);
    
    lastSensorRefresh = millis();

    Serial.print("Battery voltage: ");
    Serial.print(state.batteryVoltageV);
    Serial.println(" V");

    Serial.print("Battery current: ");
    Serial.print(state.currentMa);
    Serial.println(" mA");

    Serial.print("Distance in front: ");
    Serial.print(state.distanceMm);
    Serial.println(" mm");


  }
  

}

void sensorTask(void* parameter) {

  DriveWireState* sensorState = static_cast<DriveWireState*>(parameter);

  for ( ; ; ) {
    refreshAllSensors(*sensorState);

    //we have to pass a void* back into this FreeRTOS function, but it contains my sensorState data
    xQueueOverwrite(sensorQueue, parameter);

    // making a TickType_t period
    TickType_t sensorPeriod = 1500;

    TickType_t lastWakeTime = xTaskGetTickCount();

    //set the task period
    xTaskDelayUntil(&lastWakeTime, sensorPeriod);
  }


}