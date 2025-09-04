/*
 * LovyanGFX Hello World Example for ESP32
 *
 * This example demonstrates:
 * - LovyanGFX initialization
 * - Display driver setup
 * - Basic drawing functions (text, shapes, colors)
 * - Simple UI elements
 *
 * Hardware Requirements:
 * - ESP32 development board
 * - TFT display (e.g., ILI9341, ST7789)
 *
 * Required Libraries (add to platformio.ini):
 * - lovyan03/LovyanGFX@^1.1.0
 */

#include "HardwareSerial.h"
#include <Arduino.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

// Include ESP32 SPI definitions
#include <driver/spi_common.h>

// Create LGFX object with configuration matching main.cpp
class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ILI9341 _panel_instance;
  lgfx::Bus_SPI _bus_instance;
  lgfx::Light_PWM _light_instance;
  lgfx::Touch_XPT2046 _touch_instance;

public:
  LGFX() {
    // SPI总线配置
    auto cfg = _bus_instance.config();
    cfg.spi_host = HSPI_HOST;
    cfg.spi_mode = 0;
    cfg.freq_write = 55000000;
    cfg.freq_read = 20000000;
    cfg.spi_3wire = false;
    cfg.use_lock = true;
    cfg.dma_channel = 1;
    cfg.pin_sclk = 14;
    cfg.pin_mosi = 13;
    cfg.pin_miso = 12;
    cfg.pin_dc   = 2;
    _bus_instance.config(cfg);
    _panel_instance.setBus(&_bus_instance);

    // 面板配置
    auto pCfg = _panel_instance.config();
    pCfg.pin_cs  = 15;
    pCfg.pin_rst = -1;
    pCfg.panel_width  = 240;
    pCfg.panel_height = 320;
    pCfg.offset_rotation = 0;  // 改为0，正常方向显示
    _panel_instance.config(pCfg);

    // 背光配置
    auto lCfg = _light_instance.config();
    lCfg.pin_bl = 21;
    lCfg.invert = false;
    lCfg.freq = 44100;
    lCfg.pwm_channel = 7;
    _light_instance.config(lCfg);
    _panel_instance.setLight(&_light_instance);

    // 触摸配置 (XPT2046) - 使用软件SPI
    auto tCfg = _touch_instance.config();
    tCfg.pin_cs = 33;
    tCfg.pin_sclk = 25;
    tCfg.pin_miso = 39;
    tCfg.pin_mosi = 32;
    tCfg.pin_int = 36;
    tCfg.bus_shared = false;  // 不共享总线
    tCfg.offset_rotation = 0;
    tCfg.spi_host = -1;  // 使用软件SPI
    _touch_instance.config(tCfg);
    _panel_instance.setTouch(&_touch_instance);

    // 设置面板
    setPanel(&_panel_instance);
  }
};

// Create display instance
LGFX tft;

// Button state variables
bool buttonPressed = false;
unsigned long lastButtonCheck = 0;
const int BUTTON_CHECK_INTERVAL = 100; // Check button every 100ms

// Simple button drawing function
void drawButton(int x, int y, int w, int h, const char* text, bool pressed) {
    // Draw button background
    if (pressed) {
        tft.fillRect(x, y, w, h, 0x6666); // Dark gray when pressed
    } else {
        tft.fillRect(x, y, w, h, 0xCCCC); // Light gray
    }

    // Draw button border
    tft.drawRect(x, y, w, h, 0x0000); // Black border

    // Draw button text
    tft.setTextColor(0x0000); // Black text
    tft.setTextSize(2);
    int textWidth = strlen(text) * 12; // Approximate text width
    int textX = x + (w - textWidth) / 2;
    int textY = y + (h - 16) / 2; // 16 is approximate text height
    tft.setCursor(textX, textY);
    tft.print(text);
}

// Simple text drawing function
void drawText(int x, int y, const char* text, int textSize, uint16_t color) {
    tft.setTextColor(color);
    tft.setTextSize(textSize);
    tft.setCursor(x, y);
    tft.print(text);
}

void setup() {
    Serial.begin(115200);
    delay(100); // 等待串口初始化
    Serial.println("LovyanGFX Hello World Demo Starting...");

    // Initialize display
    tft.init();
    tft.setRotation(0); // Portrait orientation (matching main.cpp)
    tft.fillScreen(0x0000); // Black background

    // Draw title
    tft.setTextColor(0xFFFF); // White text
    tft.setTextSize(3);
    tft.setCursor(20, 80);
    tft.print("Hello World!");

    // Draw subtitle
    tft.setTextColor(0xFFE0); // Yellow text
    tft.setTextSize(2);
    tft.setCursor(30, 120);
    tft.print("LovyanGFX on ESP32");

    // Draw initial button
    drawButton(60, 160, 120, 50, "Click me!", false);

    Serial.println("LovyanGFX setup complete!");
    Serial.println("Touch configuration:");
    Serial.printf("  CS: %d, SCLK: %d, MOSI: %d, MISO: %d, INT: %d\n", 33, 25, 32, 39, 36);
    Serial.println("You should now see the UI on your display.");
    Serial.println("Try touching the button area (60,160) to (180,210)");

    // uint16_t touchParams[8] = {
    //     0, 240,    // 左上角触摸坐标
    //     240, 300,  // 右上角触摸坐标
    //     240, 0,    // 右下角触摸坐标
    //     0, 0       // 左下角触摸坐标
    // };
  }

void loop() {
    unsigned long currentTime = millis();

    // Check touch input
    if (currentTime - lastButtonCheck >= BUTTON_CHECK_INTERVAL) {
        lastButtonCheck = currentTime;

        // Get touch point
        int32_t touchX, touchY;
        bool touched = tft.getTouch(&touchX, &touchY);

        // Debug output for touch coordinates
        if (touched) {
            Serial.printf("Touch detected: X=%d, Y=%d\n", touchX, touchY);
        }

        // Check if touch is within button area (x=60, y=160, w=120, h=50)
        bool touchInButton = (touched && touchX >= 60 && touchX <= 180 && touchY >= 90 && touchY <= 140);

        // Additional debug info
        if (touched) {
            if (touchInButton) {
                Serial.println("Touch is within button area");
            } else {
                Serial.println("Touch is outside button area");
            }
        }

        if (touchInButton != buttonPressed) {
            buttonPressed = touchInButton;

            // Redraw button with new state
            drawButton(60, 160, 120, 50, "Click me!", buttonPressed);

            if (buttonPressed) {
                Serial.println("Button pressed!");
                // Add your button press logic here
            } else {
                Serial.println("Button released!");
                // Add your button release logic here
            }
        }
    }

    delay(10); // Small delay to prevent overwhelming the CPU
}
