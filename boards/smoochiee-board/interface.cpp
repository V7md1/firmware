#include "core/bus_HAL.h"
#include "core/powerSave.h"

#include <Wire.h>
#include <XPowersLib.h>
#include "driver/gpio.h"

XPowersPPM PPM;

static bool pmuReady = false;

// =====================================================
// ROTARY ENCODER - EC11
// =====================================================

static volatile int32_t encoderTicks = 0;
static volatile uint8_t encoderLastState = 0;

static portMUX_TYPE encoderMux =
    portMUX_INITIALIZER_UNLOCKED;

static const int8_t encoderTable[16] = {
     0, -1,  1,  0,
     1,  0,  0, -1,
    -1,  0,  0,  1,
     0,  1, -1,  0
};

void IRAM_ATTR encoderISR() {

    uint8_t a = gpio_get_level((gpio_num_t)ENCODER_INA);
    uint8_t b = gpio_get_level((gpio_num_t)ENCODER_INB);

    uint8_t currentState = (a << 1) | b;

    uint8_t index =
        (encoderLastState << 2) | currentState;

    int8_t movement = encoderTable[index];

    portENTER_CRITICAL_ISR(&encoderMux);

    encoderTicks += movement;
    encoderLastState = currentState;

    portEXIT_CRITICAL_ISR(&encoderMux);
}

// =====================================================
// GPIO SETUP
// =====================================================

void _setup_gpio() {

    // Navigation buttons
    pinMode(UP_BTN, INPUT_PULLUP);
    pinMode(SEL_BTN, INPUT_PULLUP);
    pinMode(DW_BTN, INPUT_PULLUP);
    pinMode(R_BTN, INPUT_PULLUP);
    pinMode(L_BTN, INPUT_PULLUP);

    // Rotary encoder
    pinMode(ENCODER_INA, INPUT_PULLUP);
    pinMode(ENCODER_INB, INPUT_PULLUP);
    pinMode(ENCODER_KEY, INPUT_PULLUP);

    encoderLastState =
        (digitalRead(ENCODER_INA) << 1) |
         digitalRead(ENCODER_INB);

    attachInterrupt(
        digitalPinToInterrupt(ENCODER_INA),
        encoderISR,
        CHANGE
    );

    attachInterrupt(
        digitalPinToInterrupt(ENCODER_INB),
        encoderISR,
        CHANGE
    );

    // NRF24
    pinMode(NRF24_SS_PIN, OUTPUT);
    digitalWrite(NRF24_SS_PIN, HIGH);

    pinMode(NRF24_CE_PIN, OUTPUT);
    digitalWrite(NRF24_CE_PIN, LOW);

    // IR
    pinMode(IR_RX_PIN, INPUT);
    pinMode(IR_TX_PIN, OUTPUT);

    digitalWrite(IR_TX_PIN, LOW);

    bruceConfigPins.irRx = IR_RX_PIN;
    bruceConfigPins.irTx = IR_TX_PIN;

    // I2C
    setSysI2CBus(&Wire);

    Wire.setPins(SYS_I2C_SDA, SYS_I2C_SCL);

    Wire.begin(SYS_I2C_SDA, SYS_I2C_SCL);

    // PMU
    pmuReady = PPM.init(
        Wire,
        SYS_I2C_SDA,
        SYS_I2C_SCL,
        BQ25896_SLAVE_ADDRESS
    );

    if (pmuReady) {

        PPM.setSysPowerDownVoltage(3300);
        PPM.setInputCurrentLimit(3250);
        PPM.disableCurrentLimitPin();

        PPM.setChargeTargetVoltage(4208);
        PPM.setPrechargeCurr(64);
        PPM.setChargerConstantCurr(832);

        PPM.enableMeasure(PowersBQ25896::CONTINUOUS);

        PPM.disableOTG();
        PPM.enableCharge();
    }
}

// =====================================================
// BATTERY
// =====================================================

bool isCharging() {

    if (!pmuReady) return false;

    return PPM.isCharging();
}

int getBattery() {

    if (!pmuReady) return 0;

    int voltage = PPM.getBattVoltage();

    int percent =
        (voltage - 3300) * 100 / (float)(4150 - 3350);

    if (percent < 1) percent = 1;
    if (percent > 100) percent = 100;

    if (PPM.isCharging() && percent >= 97) {

        PPM.disableBatLoad();
        percent = 95;
    }

    if (PPM.isChargeDone()) {
        percent = 100;
    }

    return percent;
}

// =====================================================
// BRIGHTNESS
// =====================================================

void _setBrightness(uint8_t brightval) {

    if (brightval == 0) {

        analogWrite(TFT_BL, 0);

    } else {

        int bl = MINBRIGHT +
            round(((255 - MINBRIGHT) * brightval / 100));

        analogWrite(TFT_BL, bl);
    }
}

// =====================================================
// INPUT HANDLER
// =====================================================

void InputHandler(void) {

    // Physical buttons
    bool _u = digitalRead(UP_BTN) == BTN_ACT;
    bool _d = digitalRead(DW_BTN) == BTN_ACT;
    bool _l = digitalRead(L_BTN) == BTN_ACT;
    bool _r = digitalRead(R_BTN) == BTN_ACT;
    bool _s = digitalRead(SEL_BTN) == BTN_ACT;

    // Encoder button
    bool encoderPressed =
        digitalRead(ENCODER_KEY) == LOW;

    // Get encoder movement safely
    int32_t ticks;

    portENTER_CRITICAL(&encoderMux);

    ticks = encoderTicks;
    encoderTicks = 0;

    portEXIT_CRITICAL(&encoderMux);

    // Convert encoder ticks into detents
    static int32_t encoderPosition = 0;

    encoderPosition += ticks;

    bool encoderUp = false;
    bool encoderDown = false;

    while (encoderPosition >= 4) {

        encoderDown = true;
        encoderPosition -= 4;
    }

    while (encoderPosition <= -4) {

        encoderUp = true;
        encoderPosition += 4;
    }

    // Any input?
    if (
        _s || _u || _d || _l || _r ||
        encoderPressed || encoderUp || encoderDown
    ) {

        if (wakeUpScreen()) {
            return;
        }

        AnyKeyPress = true;
    }

    // Encoder rotation:
    // RIGHT = DOWN
    // LEFT  = UP

    if (encoderDown) {

        DownPress = true;
        NextPagePress = true;
    }

    if (encoderUp) {

        UpPress = true;
        PrevPagePress = true;
    }

    // Encoder OK
    if (encoderPressed) {
        SelPress = true;
    }

    // Original buttons
    if (_l) PrevPress = true;
    if (_r) NextPress = true;

    if (_u) {

        UpPress = true;
        PrevPagePress = true;
    }

    if (_d) {

        DownPress = true;
        NextPagePress = true;
    }

    if (_s) SelPress = true;

    // ESC combination
    if (_l && _r) {

        EscPress = true;

        NextPress = false;
        PrevPress = false;
    }
}

// =====================================================
// POWER OFF
// =====================================================

void powerOff() {

    esp_sleep_enable_ext0_wakeup(
        (gpio_num_t)SEL_BTN,
        BTN_ACT
    );

    esp_deep_sleep_start();
}

// =====================================================
// REBOOT / POWER BUTTON
// =====================================================

void checkReboot() {

    int countDown = 0;

    if (
        digitalRead(L_BTN) == BTN_ACT &&
        digitalRead(R_BTN) == BTN_ACT
    ) {

        uint32_t time_count = millis();

        while (
            digitalRead(L_BTN) == BTN_ACT &&
            digitalRead(R_BTN) == BTN_ACT
        ) {

            if (millis() - time_count > 500) {

                if (countDown == 0) {

                    int textWidth =
                        tft.textWidth("PWR OFF IN 3/3", 1);

                    tft.fillRect(
                        tftWidth / 2 - textWidth / 2,
                        7,
                        textWidth,
                        18,
                        bruceConfig.bgColor
                    );
                }

                tft.setTextSize(1);

                tft.setTextColor(
                    bruceConfig.priColor,
                    bruceConfig.bgColor
                );

                countDown =
                    (millis() - time_count) / 1000 + 1;

                if (countDown < 4) {

                    tft.drawCentreString(
                        "PWR OFF IN " +
                        String(countDown) + "/3",
                        tftWidth / 2,
                        12,
                        1
                    );

                } else {

                    tft.fillScreen(bruceConfig.bgColor);

                    while (
                        digitalRead(L_BTN) == BTN_ACT ||
                        digitalRead(R_BTN) == BTN_ACT
                    );

                    delay(200);

                    powerOff();
                }

                delay(10);
            }
        }

        delay(30);

        if (millis() - time_count > 500) {

            tft.fillRect(
                60,
                12,
                tftWidth - 60,
                tft.fontHeight(1),
                bruceConfig.bgColor
            );

            drawStatusBar();
        }
    }
}
