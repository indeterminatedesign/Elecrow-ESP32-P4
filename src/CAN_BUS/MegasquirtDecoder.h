#pragma once
#include "VehicleData.h"
#include "driver/twai.h"
#include "CANDataField.h"

class MegasquirtDecoder
{
public:
    MegasquirtDecoder(VehicleData &signalsRef);

    void processFrame(const twai_message_t &msg);

private:
    VehicleData& _vehicleData;
};
