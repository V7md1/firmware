#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include "soc/soc_caps.h"
#include <stdint.h>

// =====================================================
// UART
// =====================================================

static const uint8_t TX = 1;
static const uint8_t RX = 2;

#define SERIAL_RX 2
#define SERIAL_TX 1

#define BAD_RX SERIAL_RX
#define BAD_TX SERIAL_TX

#define GPS_SERIAL_TX SERIAL_TX
#define GPS_SERIAL_RX SERIAL_RX

#define USB_as_HID 1

// =====================================================
// I2C
// =====================================================

static const uint8_t SDA = 47;
static const uint8_t SCL = 48;

#define GROVE_SDA 47
#define GROVE_SCL 48

#define SYS_I2C_SDA 47
#define SYS_I2C_SCL 48

// =====================================================
// MAIN SPI - TFT + NRF24
// =====================================================

static const uint8_t SS = 46;
static const uint8_t MOSI = 17;
static const uint8_t MISO = 8;
static const uint8_t SCK = 18;

#define SPI_SCK_PIN 18
#define SPI_MOSI_PIN 17
#define SPI_MISO_PIN 8
#define SPI_SS_PIN 46

// =====================================================
// BUTTONS
// =====================================================

#define BTN_ALIAS "\"OK\""
#define HAS_5_BUTTONS

#define SEL_BTN 0
#define UP_BTN 41
#define DW_BTN 40
#define R_BTN 38
#define L_BTN 39

#define BTN_ACT LOW
#define BUTTONS_PULLUP 1

// =====================================================
// ROTARY ENCODER
// =====================================================

#define ENCODER_INA 9
#define ENCODER_INB 10
#define ENCODER_KEY 3

#define ENCODER_PULLUP 1

// =====================================================
// IR
// =====================================================

#define IR_RX_PIN 4
#define IR_TX_PIN 5

// =====================================================
// NRF24
// =====================================================

#define USE_NRF24_VIA_SPI

#define NRF24_CE_PIN 21
#define NRF24_SS_PIN 14

#define NRF24_MOSI_PIN 17
#define NRF24_MISO_PIN 8
#define NRF24_SCK_PIN 18

// =====================================================
// DISPLAY - ST7789 1.47"
// =====================================================

#define HAS_SCREEN 1

#define ROTATION 3

#define MINBRIGHT (uint8_t)1

#define USER_SETUP_LOADED 1
#define ST7789_DRIVER 1
#define CGRAM_OFFSET 1

#define TFT_RGB_ORDER TFT_BGR

#define TFT_WIDTH 172
#define TFT_HEIGHT 320

#define TFT_INVERSION_ON 1
#define TFT_BACKLIGHT_ON 1

#define TFT_BL 6
#define TFT_RST 16
#define TFT_DC 15
#define TFT_CS 7

#define TFT_MISO -1
#define TFT_MOSI 17
#define TFT_SCLK 18

#define TOUCH_CS -1
#define SMOOTH_FONT 1

// Screen SPI frequency: 20 MHz
#define SPI_FREQUENCY 20000000

#define SPI_READ_FREQUENCY 10000000
#define SPI_TOUCH_FREQUENCY 2500000

#define SUPPORT_TRANSACTIONS 1

// =====================================================
// SD CARD - SEPARATE SPI
// =====================================================

#define SDCARD_CS 42

#define SDCARD_SCK 12
#define SDCARD_MISO 13
#define SDCARD_MOSI 11

// =====================================================
// STATUS / RGB LED
// =====================================================

#define HAS_RGB_LED 1

#define RGB_LED 45

#define LED_TYPE WS2812B
#define LED_ORDER GRB

#define LED_TYPE_IS_RGBW 0
#define LED_COUNT 16
#define LED_COLOR_STEP 15

// =====================================================
// POWER
// =====================================================

#define FP_PIN 19
#define FM_PIN 20
#define FG_PIN 35

#define XPOWERS_CHIP_BQ25896

#define USE_BOOST

// =====================================================
// I2S
// =====================================================

#define PIN_CLK 36
#define PIN_DATA 37
#define PIN_WS 35

// =====================================================
// IO EXPANDER
// =====================================================

#define USE_IO_EXPANDER
#define IO_EXPANDER_AW9523

#define IO_EXP_GPS 1
#define IO_EXP_MIC 4
#define IO_EXP_VIBRO 2
#define IO_EXP_CC_RX 7
#define IO_EXP_CC_TX 12

#endif
