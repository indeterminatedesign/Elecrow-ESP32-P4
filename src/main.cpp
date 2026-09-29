#include "board_config.h" // board pin define
#include <Arduino.h>      // Arduino core library. Must be placed at the very top to ensure recognition of Arduino APIs

#include <string.h>            // C string lib
#include <esp_log.h>           // ESP-IDF logging library
#include <esp_err.h>           // ESP-IDF error codes
#include <esp_ldo_regulator.h> // ESP32-P4 specific LDO management

#include "esp_panel_drivers_conf.h"
#include "esp_panel_board_custom_conf.h"
#include "ESP_Panel_Library.h"

#include <lvgl.h>
#include "lvgl_v8_port.h"
#include "configuration.h"
#include "CAN_Bus/VehicleData.h"
#include "CAN_Bus/MegasquirtDecoder.h"
#include "CAN_Bus/IOModuleDecoder.h"
#include "CAN_Bus/CanDataField.h"
#include "CAN_Bus/CANBus.h"
#include "DashboardUI.h"

using namespace esp_panel::drivers;
using namespace esp_panel::board;

CANBus canBus;

// 1. Allocate three distinct buffers in memory
VehicleData vehicleDataCANBuffer;    // Buffer A: Modified exclusively by Core 0 decoders
VehicleData vehicleDataShadowBuffer; // Buffer B: The intermediate staging buffer
VehicleData vehicleDataUIBuffer;     // Buffer C: Read exclusively by Core 1 Dashboard

// Pointers to handle triple-buffering
VehicleData *uiBufferPtr = &vehicleDataUIBuffer;

// Decoders look directly at the dedicated CAN input buffer
MegasquirtDecoder msDecoder(vehicleDataCANBuffer);
IOModuleDecoder ioDecoder(vehicleDataCANBuffer);
DashboardUI dashboardUI;

static const char *TAG = "TOUCH_APP"; // Tag for logging messages

// --- Declare the panel pointer globally ---
Board *board = nullptr;

// FreeRTOS Mutex is now ONLY used internally on Core 0 to protect the shadow copy
SemaphoreHandle_t shadowBufferMutex = NULL;
TaskHandle_t renderTaskHandle = NULL;

lv_obj_t *rpmLabel;
lv_obj_t *mapLabel;
lv_obj_t *fuelLabel;

void canRecieveTask(void *param)
{
    while (true)
    {
        twai_message_t msg;
        bool hasNewData = false;

        // Drain the hardware queue rapidly into vehicleDataCANBuffer (Buffer A)
        while (canBus.receiveMessage(msg))
        {
            hasNewData = true;

            // Extended range to 0x5EE to include MAT, BAT V, and AFR frames (0x5E9, 0x5EA, 0x5EB, etc.)
            if (msg.identifier >= 0x5E8 && msg.identifier <= 0x5EE)
            {
                msDecoder.processFrame(msg);
            }
            else if (msg.identifier >= 0x600 && msg.identifier <= 0x603)
            {
                ioDecoder.processFrame(msg);
            }
        }

        // Deep copy from CAN Buffer (A) into Shadow Buffer (B)
        if (hasNewData && shadowBufferMutex != NULL)
        {
            // Using a 0-tick timeout ensures Core 0 NEVER blocks waiting on Core 1's UI.
            // If the lock is held, we just skip this loop's copy and catch it 2ms later.
            if (xSemaphoreTake(shadowBufferMutex, 0) == pdTRUE)
            {
                vehicleDataShadowBuffer = vehicleDataCANBuffer;
                xSemaphoreGive(shadowBufferMutex);
                if (renderTaskHandle != NULL)
                {
                    xTaskNotifyGive(renderTaskHandle);
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(15));
    }
}

void renderDashboardTask(void *param)
{
    while (true)
    {
        // 3. Atomically pull data out of the shadow buffer into the UI buffer.
        // Because we only copy if the mutex is immediately free, this takes less than 1 microsecond.
        if (shadowBufferMutex != NULL)
        {
            if (xSemaphoreTake(shadowBufferMutex, pdMS_TO_TICKS(0)) == pdTRUE)
            {
                // Memory block copy into the buffer your UI is holding a pointer to
                vehicleDataUIBuffer = vehicleDataShadowBuffer;
                xSemaphoreGive(shadowBufferMutex);
            }
        }

        // 4. Render completely lock-free.
        // Your UI looks at vehicleDataUIBuffer pointer safely, unhindered by CAN updates.
        dashboardUI.render();
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
}

void setup()
{
    // put your setup code here, to run once:

    // Initialize the default Serial for debugging (UART0)
    Serial.begin(512000);

    // --- Power Configuration (LDO3 for MIPI D-PHY) ---
    // ESP32-P4's MIPI D-PHY requires specific voltage to function.
    // LDO3 is typically routed to the MIPI power rail on P4 hardware.
    esp_err_t err = ESP_OK;
    esp_ldo_channel_handle_t ldo3_handle = NULL;
    esp_ldo_channel_config_t ldo3_cfg = {
        .chan_id = 3,       // LDO Channel 3
        .voltage_mv = 2500, // Set to 2500mV (2.5V)
    };

    Serial.println("Initializing LDO3 to 2.5V...");
    err = esp_ldo_acquire_channel(&ldo3_cfg, &ldo3_handle);
    if (err != ESP_OK)
    {
        Serial.printf("LDO3 Power Error: %s\n", esp_err_to_name(err));
    }
    else
    {
        Serial.println("LDO3 Power enabled successfully.");
    }

    // --- Power Configuration (LDO4 for I2C/touch pull up) ---
    esp_ldo_channel_handle_t ldo4_handle = NULL;
    esp_ldo_channel_config_t ldo4_cfg = {
        .chan_id = 4,       // LDO Channel 4
        .voltage_mv = 3300, // Set to 3300mV (3.3V)
    };

    Serial.println("Initializing LDO4 to 3.3V...");
    err = esp_ldo_acquire_channel(&ldo4_cfg, &ldo4_handle);
    if (err != ESP_OK)
    {
        Serial.printf("LDO4 Power Error: %s\n", esp_err_to_name(err));
    }
    else
    {
        Serial.println("LDO4 Power enabled successfully.");
    }

    // --- Initialize Display and Touch Panel ---
    Board *board = new Board();
    // Initialize the bus (MIPI-DSI) and the devices (EK79007 & GT911)
    Serial.println("Initializing Panel (EK79007 + GT911)...");
    assert(board->init());

#if LVGL_PORT_AVOID_TEARING_MODE
    auto lcd = board->getLCD();
    // When avoid tearing function is enabled, the frame buffer number should be set in the board driver
    lcd->configFrameBufferNumber(LVGL_PORT_DISP_BUFFER_NUM);
#endif

    // --- Create the FreeRTOS Mutex for the Shadow Buffer ---
    shadowBufferMutex = xSemaphoreCreateMutex();
    if (shadowBufferMutex == NULL) {
        Serial.println("❌ Failed to create shadow buffer mutex!");
        while (1) delay(10); // Halt if OS cannot provision resource
    }

    assert(board->begin());
    Serial.println("Display and Touch system online.");

    Serial.println("Initializing LVGL");
    //  lvgl_port_init(board->getLCD(), board->getTouch());

    Serial.println("Creating UI");
    dashboardUI.begin(board, uiBufferPtr);

    canBus.begin();

    xTaskCreatePinnedToCore(
        renderDashboardTask, "Render Task", 8192, NULL, 2, &renderTaskHandle, 1);

    xTaskCreatePinnedToCore(
        canRecieveTask, "CAN Task", 4096, NULL, 5, NULL, 0);
}

void loop()
{
}
