#include "IOModuleDecoder.h"

IOModuleDecoder::IOModuleDecoder(VehicleData &signalsRef)
    : _vehicleData(signalsRef)
{
}

void IOModuleDecoder::processFrame(const twai_message_t &msg)
{
    switch (msg.identifier)
    {

    // Fuel level
    case 0x600:
    {
        _vehicleData.fuelLevel.decode(msg.data);
    }
    break;

    // Turn signals + lights
    case 0x601:
    {
        _vehicleData.leftTurn.decode(msg.data);
        _vehicleData.rightTurn.decode(msg.data);
        _vehicleData.highBeam.decode(msg.data);
    }
    break;

    // Vehicle speed
    case 0x602:
    {
        _vehicleData.speed.decode(msg.data);
    }
    break;
    case 0x603:
    {
        _vehicleData.button0.decode(msg.data);
        _vehicleData.button1.decode(msg.data);
    }
    break;
    }
}