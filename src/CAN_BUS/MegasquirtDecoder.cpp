#include "MegasquirtDecoder.h"

MegasquirtDecoder::MegasquirtDecoder(VehicleData &signalsRef)
    : _vehicleData(signalsRef)
{
}
void MegasquirtDecoder::processFrame(const twai_message_t &msg)
{
    if (msg.extd)
        return;
    if (msg.data_length_code != 8)
        return;

    // Based on the CAN ID decode the message
    switch (msg.identifier)
    {
    case 0x5E8:
        _vehicleData.map.decode(msg.data);
        _vehicleData.rpm.decode(msg.data);
        _vehicleData.coolant.decode(msg.data);
        _vehicleData.tps.decode(msg.data);
        break;
    case 0x5E9:
        _vehicleData.iat.decode(msg.data);
        _vehicleData.pw1.decode(msg.data);
        break;
    case 0x5EA:
        _vehicleData.afr1.decode(msg.data);
        break;
    case 0x5EB:
        // Battery Voltage is at Offset 0 (Bytes 0 and 1)
        _vehicleData.battery.decode(msg.data);
        break;
    }
}
