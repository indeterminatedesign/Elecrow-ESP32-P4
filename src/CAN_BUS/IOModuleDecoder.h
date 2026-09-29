#pragma once
#include <Arduino.h>
#include "VehicleData.h"
#include "driver/twai.h"

class IOModuleDecoder
{
public:
    IOModuleDecoder(VehicleData& _signalsRef);

    void processFrame(const twai_message_t& msg);

private:
    VehicleData& _vehicleData;
};
