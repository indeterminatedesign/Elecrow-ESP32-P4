#include "CANBus.h"

// Set to 1 to enable Serial logs during troubleshooting. 
// Set to 0 for production to make the code run at maximum speed.
#define DEBUG_CAN 0 

void CANBus::begin() {
    // --- CAN configuration ---
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
        (gpio_num_t)CAN_TX, 
        (gpio_num_t)CAN_RX, 
        TWAI_MODE_NORMAL
    );
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    // Install driver
    if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK) {
        Serial.println("❌ CAN driver install failed");
        while (1);
    }

    // Start CAN
    if (twai_start() != ESP_OK) {
        Serial.println("❌ CAN start failed");
        while (1);
    }
    
    #if DEBUG_CAN
    Serial.println("🚀 CAN Driver successfully initialized.");
    #endif
}

void CANBus::sendMessage(uint32_t id, uint8_t *data, uint8_t len) {
    twai_message_t tx_msg = {};
    tx_msg.identifier = id;
    tx_msg.data_length_code = len;
    
    // Explicitly flag extended vs standard IDs to avoid driver confusion
    tx_msg.flags = (id > 0x7FF) ? TWAI_MSG_FLAG_EXTD : TWAI_MSG_FLAG_NONE;
    memcpy(tx_msg.data, data, len);

    if (twai_transmit(&tx_msg, pdMS_TO_TICKS(10)) == ESP_OK) {
        #if DEBUG_CAN
        Serial.printf("TX: ID=0x%03X\n", id);
        #endif
    } else {
        #if DEBUG_CAN
        Serial.println("❌ TX failed");
        #endif
    }
}

bool CANBus::receiveMessage(twai_message_t &rx_msg) {
    if (twai_receive(&rx_msg, pdMS_TO_TICKS(0)) == ESP_OK) {
        #if DEBUG_CAN
        Serial.printf("RX: ID=0x%03X DLC=%d DATA:", rx_msg.identifier, rx_msg.data_length_code);
        for (int i = 0; i < rx_msg.data_length_code; i++) {
            Serial.printf(" %02X", rx_msg.data[i]);
        }
        Serial.println();
        #endif
        return true;
    } else {
        // No message received within timeout. 
        // 🛑 This previously printed every microsecond, completely crashing performance.
        #if DEBUG_CAN
        Serial.println("No Message Recieved");
        #endif
        return false;
    }
}
