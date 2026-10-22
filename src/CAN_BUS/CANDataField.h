#pragma once
#include <Arduino.h>

/**
 * @brief Supported data types for CAN message extraction.
 */
enum class CANDataType
{
    BOOL, ///< Boolean value (0 or 1)
    U16,  ///< Unsigned 16-bit integer
    S16,  ///< Signed 16-bit integer
    U8    // Unsigned byte
};

enum class CANDataFieldThreshold
{
    NOMINAL,  ///Value is within normal operating range
    BELOW_THRESHOLD, ///Value is below the low threshold
    ABOVE_THRESHOLD ///Value is above the high threshold
};

constexpr uint8_t NullThreshold = -1234; // Fixed value to indicate a null for high and low thresholds. This is used to differentiate between a valid threshold and an uninitialized state.

/**
 * @brief Represents a single data field within a CAN message.
 * * This class handles the metadata, storage, and conversion logic for
 * translating raw CAN bus bytes into meaningful physical values.
 */
class CANDataField
{
public:
    CANDataType canDatatype; ///< The underlying data format in the CAN frame
    float_t value = 0;       ///< The processed physical value (scaled)
    uint32_t lastUpdate = 0; ///< Timestamp (ms) of the last successful decode

    // CAN mapping
    uint32_t canId;            ///< The specific CAN Arbitration ID for this field
    uint8_t offset;            ///< Byte index in the 8-byte CAN payload where data starts
    float scale;               ///< Multiplier to convert raw bits to physical units (e.g., 0.1 for 100mV -> 10V)
    float sensorBias;          ///< Offset to apply after scaling (e.g., for calibration)
    float uiUpdateTolerance;   ///< Minimum change required in 'value' to trigger a UI refresh
    float_t previousValue = 0; ///< Last value sent to the UI, used for change detection
    float_t highThreshold = 0; ///< Optional upper limit for color-coding or alerts
    float_t lowThreshold = 0;  ///< Optional lower limit for color-coding or alerts

    /**
     * @brief Construct a new CANDataField object.
     * * @param id The CAN Message ID to listen for.
     * @param off The starting byte position (0-7) within the CAN frame.
     * @param scl The scaling factor applied to the raw value.
     * @param tolerance The delta threshold for UI updates.
     * @param bias The sensor bias.
     * @param lowThreshold The lower threshold for color-coding or alerts.
     * @param highThreshold The upper threshold for color-coding or alerts.
       * @param t The data type of the field (BOOL, U16, S16
     */
    CANDataField(uint32_t id = 0, uint8_t off = 0, float scl = 1.0f, float tolerance = 1.0f, float bias = 0.0f, float_t lowThreshold = 0, float_t highThreshold = 0, CANDataType t = CANDataType::U16)
        : canId(id),
          offset(off),
          scale(scl),
          uiUpdateTolerance(tolerance),
          sensorBias(bias),
          lowThreshold(lowThreshold),
          highThreshold(highThreshold),
          canDatatype(t) {
          };

    void decode(const uint8_t *data);
    void encode(uint8_t *data);
    bool hasChanged(); ///< Flag indicating if the value has changed since the last UI update
    CANDataFieldThreshold thresholdState();
};