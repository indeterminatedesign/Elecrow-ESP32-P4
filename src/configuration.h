
#define CAN_TX 3   // <-- adjust to your  board
#define CAN_RX 2    // <-- adjust to your  board

#define RECEIVE_RATE_MS 50 //How often to check for incoming CAN messages

// ==================================
#define EXAMPLE_I2C_ADDR    (ESP_IO_EXPANDER_I2C_CH422G_ADDRESS)
#define EXAMPLE_I2C_SDA_PIN 8         // I2C data line pins
#define EXAMPLE_I2C_SCL_PIN 9         // I2C clock line pin
// Extend IO Pin define
#define TP_RST 1
#define LCD_BL 2
#define LCD_RST 3
#define SD_CS 4
#define USB_SEL 5

// Interval:
#define TRANSMIT_RATE_MS 1000
#define POLLING_RATE_MS 1000

/* —————————————————————————————————————————————————————————————————————— 
                                DEFINITIONS 
————————————————————————————————————————————————————————————————————————— */
#define PRINTF_ORIGINAL(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__);
#define PRINTF_PRINT(fmt, ...)    Serial.print(fmt);
#define PRINTF_LN(fmt, ...)       Serial.println(fmt);

#define PRINTF_ERROR(fmt, ...)      do { \
                                        Serial.print("ERROR"); \
                                        Serial.printf(fmt, ##__VA_ARGS__); \
                                        Serial.print("\r\n"); \
                                    } while(0)  
#define PRINTF_WARN(fmt, ...)       do { \
                                        Serial.print("WARN: "); \
                                        Serial.printf(fmt, ##__VA_ARGS__); \
                                        Serial.print("\r\n"); \
                                    } while(0)  
#define PRINTF_INFO(fmt, ...)       do { \
                                        Serial.print("INFO: "); \
                                        Serial.printf(fmt, ##__VA_ARGS__); \
                                        Serial.print("\r\n"); \
                                    } while(0)  
#define PRINTF_DEBUG(fmt, ...)      do { \
                                        Serial.print("DEBUG: "); \
                                        Serial.printf(fmt, ##__VA_ARGS__); \
                                        Serial.print("\r\n"); \
                                    } while(0)

#define MAIN_INFO(fmt, ...)         PRINTF_ERROR(fmt, ##__VA_ARGS__)  // Info level log macro
#define MAIN_ERROR(fmt, ...)        PRINTF_ERROR(fmt, ##__VA_ARGS__)  // Error level log macro

#define LV_COLOR_RED        lv_color_make(0xFF, 0x00, 0x00) // LVGL Red
#define LV_COLOR_GREEN      lv_color_make(0x00, 0xFF, 0x00) // LVGL Green
#define LV_COLOR_BLUE       lv_color_make(0x00, 0x00, 0xFF) // LVGL Blue
#define LV_COLOR_WHITE      lv_color_make(0xFF, 0xFF, 0xFF) // LVGL White
#define LV_COLOR_BLACK      lv_color_make(0x00, 0x00, 0x00) // LVGL Black
#define LV_COLOR_GRAY       lv_color_make(0x80, 0x80, 0x80) // LVGL gray
#define LV_COLOR_YELLOW     lv_color_make(0xFF, 0xFF, 0x00) // LVGL yellow