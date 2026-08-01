#pragma once

#include <stdint.h>
#include <drivewire_state.h>

bool initializeSensors(DriveWireState& state);

bool refreshElectricalTelemetry(DriveWireState& state);
bool refreshDistanceTelemetry(DriveWireState& state);

bool refreshAllSensors(DriveWireState& state);