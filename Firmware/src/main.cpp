#include <Arduino.h> 

#include <motors.h>
#include <sensors.h>
#include <serial_interface.h>
#include <drivewire_state.h>

// including FreeRTOS libraries
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

DriveWireState sensorData;
DriveWireState motorData;

// initializing a queue handle for the sensor task that holds a snapshot of sensor telemetry
QueueHandle_t sensorQueue = nullptr;
// same for motor task
QueueHandle_t motorCommandQueue = nullptr;

// declaring sensorTask
void sensorTask(void* parameter);


void setup(){
  Serial.begin(115200);
  delay(1000);

  Serial.println("DriveWire I2C Firmware Starting...");

  initializeMotors();
  coastMotors(motorData);

  if (initializeSensors(sensorData)) {Serial.println("Sensors Initialized...");}
  else { Serial.println(sensorData.ina219Online); Serial.println(sensorData.tofOnline);}



  // gonna create the queue and assign it to the handle initialized globally
  sensorQueue = xQueueCreate(1, sizeof(DriveWireState));
  // same with the motor queue
  motorCommandQueue = xQueueCreate(1, sizeof(MotorCommand));

  if (sensorQueue == nullptr || motorCommandQueue == nullptr) {
    Serial.println("Failed to create sensor queue or motor queue");

    Serial.println("The program will not run");
    for ( ; ; ) {
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }

  // creating the sensor task
  BaseType_t taskResult = xTaskCreate(
    sensorTask, 
    "sensorTask", 
    4096, 
    &sensorData, 
    1, 
    nullptr
  );

  // checking for successful sensor task creation
  if (taskResult != pdPASS) {
    Serial.println("Failed to create sensor task.");

    Serial.println("The program will not run");
    for ( ; ; ) {
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }


  // actually initializeMotors is a void function so we can't currently check if successful

  Serial.println("Program starting...");
}

void loop() {

  DriveWireState receivedState;

  if (xQueueReceive(sensorQueue, &receivedState, 0) == pdTRUE) {
    Serial.print("Battery voltage: ");
    Serial.print(receivedState.batteryVoltageV);
    Serial.println(" V");  

    Serial.print("Battery current: ");
    Serial.print(receivedState.currentMa);
    Serial.println(" mA");

    Serial.print("Distance in front: ");
    Serial.print(receivedState.distanceMm);
    Serial.println(" mm");    
  }
  


}

void sensorTask(void* parameter) {

  DriveWireState* sensorState = static_cast<DriveWireState*>(parameter);
    
  // making a TickType_t period
  // added pdMS_TO_TICKS to be sure 0.1s period
  const TickType_t sensorPeriod = pdMS_TO_TICKS(100);

  TickType_t lastWakeTime = xTaskGetTickCount();

  for ( ; ; ) {

    refreshAllSensors(*sensorState);

    xQueueOverwrite(sensorQueue, sensorState);

    //set the task period
    xTaskDelayUntil(&lastWakeTime, sensorPeriod);
  }


}