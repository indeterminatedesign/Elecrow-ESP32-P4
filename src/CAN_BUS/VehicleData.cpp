#include "VehicleData.h"
// Id, offset, scale, update tolerance
VehicleData::VehicleData()
    : rpm(0x5E8, 2, 1.00f, 0.0f, 5.0f,NullThreshold,6800, CANDataType::U16),    // Offset 2, scale 1.0
      map(0x5E8, 0, 0.1f, 0.0f, 0.1f, NullThreshold, 130, CANDataType::S16),     // Offset 0, scale 0.1
      tps(0x5E8, 6, 0.1f, 5.0f, 1.0f, NullThreshold, NullThreshold, CANDataType::S16),     // Offset 6, scale 0.1
      coolant(0x5E8, 4, 0.1f, 1.0f, 0.0f, 160, 210, CANDataType::S16), // Offset 4, scale 0.1
      pw1(0x5E9, 0, 0.001f, 0.1f, 0.0f, NullThreshold, NullThreshold, CANDataType::U16),   // Changed to 0x5E9, Offset 0, scale 0.001
      iat(0x5E9, 4, 0.1f, 1.0f, 0.0f, NullThreshold, 150, CANDataType::S16),     // Corrected scale parameters
      afr1(0x5EA, 1, 0.1f, 0.0f, 0.0f, NullThreshold, 16, CANDataType::U8),
      battery(0x5EB, 0, 0.1f, 0.1f, 0.1f, 10, 15, CANDataType::U16), // Offset 0, scale 0.1
      fuelLevel(0x600, 0, 1.0f, 5.0f, 0.0f, 10, NullThreshold, CANDataType::U16),
      leftTurn(0x601, 0, 1.0f, 1.0f, 0.0f, NullThreshold, NullThreshold, CANDataType::BOOL),
      rightTurn(0x601, 1, 1.0f, 1.0f, 0.0f, NullThreshold, NullThreshold, CANDataType::BOOL),
      highBeam(0x601, 2, 1.0f, 1.0f, 0.0f, NullThreshold, NullThreshold, CANDataType::BOOL),
      headlights(0x601, 3, 1.0f, 1.0f, 0.0f, NullThreshold, NullThreshold, CANDataType::BOOL),
      speed(0x602, 0, 1.0f, 1.0f, 0.0f, NullThreshold, NullThreshold, CANDataType::U16),
      button0(0x603, 0, 1.0f, 1.0f, 0.0f, NullThreshold, NullThreshold, CANDataType::BOOL),
      button1(0x603, 1, 1.0f, 1.0f, 0.0f, NullThreshold, NullThreshold, CANDataType::BOOL)

{
}

void VehicleData::updateValuesFrom(const VehicleData &source)
{
    rpm.value = source.rpm.value;
    rpm.lastUpdate = source.rpm.lastUpdate;
    map.value = source.map.value;
    map.lastUpdate = source.map.lastUpdate;
    tps.value = source.tps.value;
    tps.lastUpdate = source.tps.lastUpdate;
    coolant.value = source.coolant.value;
    coolant.lastUpdate = source.coolant.lastUpdate;
    iat.value = source.iat.value;
    iat.lastUpdate = source.iat.lastUpdate;
    afr1.value = source.afr1.value;
    afr1.lastUpdate = source.afr1.lastUpdate;
    pw1.value = source.pw1.value;
    pw1.lastUpdate = source.pw1.lastUpdate;
    battery.value = source.battery.value;
    battery.lastUpdate = source.battery.lastUpdate;
    highBeam.value = source.highBeam.value;
    highBeam.lastUpdate = source.highBeam.lastUpdate;
    headlights.value = source.headlights.value;
    headlights.lastUpdate = source.headlights.lastUpdate;
    fuelLevel.value = source.fuelLevel.value;
    fuelLevel.lastUpdate = source.fuelLevel.lastUpdate;
    leftTurn.value = source.leftTurn.value;
    leftTurn.lastUpdate = source.leftTurn.lastUpdate;
    rightTurn.value = source.rightTurn.value;
    rightTurn.lastUpdate = source.rightTurn.lastUpdate;
    speed.value = source.speed.value;
    speed.lastUpdate = source.speed.lastUpdate;
    button0.value = source.button0.value;
    button0.lastUpdate = source.button0.lastUpdate;
    button1.value = source.button1.value;
    button1.lastUpdate = source.button1.lastUpdate;
}