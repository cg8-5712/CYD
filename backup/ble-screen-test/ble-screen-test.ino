#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <LovyanGFX.hpp>
#include <U8g2lib.h>

// 显示屏初始化
class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ILI9341 _panel_instance;
  lgfx::Bus_SPI _bus_instance;
  lgfx::Light_PWM _light_instance;
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
    pCfg.offset_rotation = 7;
    _panel_instance.config(pCfg);

    // 背光配置
    auto lCfg = _light_instance.config();
    lCfg.pin_bl = 21;
    lCfg.invert = false;
    lCfg.freq = 44100;
    lCfg.pwm_channel = 7;
    _light_instance.config(lCfg);
    _panel_instance.setLight(&_light_instance);

    setPanel(&_panel_instance);
  }
};

LGFX tft;

BLEScan* pBLEScan;
int scanTime = 5;
int repeatTimes = 12;

int lineY = 0;  // 当前输出行
const int lineHeight = 20;

// 使用 u8g2 的中文字体
static lgfx::U8g2font cn_font(u8g2_font_unifont_t_chinese2); 

void printLine(const char* text) {
  if (lineY > tft.height() - lineHeight) {
    tft.fillRect(0, 0, tft.width(), tft.height(), TFT_BLACK);
    lineY = 0;
  }
  tft.setCursor(0, lineY);
  tft.print(text);
  lineY += lineHeight;
}

void setup() {
  Serial.begin(115200);
  delay(500);  // 稍微延时，确保 ESP32 完全启动

  // 初始化显示
  tft.begin();
  delay(500);  // 增加额外延时，确保显示屏初始化稳定
  tft.fillScreen(TFT_BLACK);  // 清屏
  delay(100);  // 再增加一些延迟

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setFont(&cn_font);

  String deviceName = "ESP32_BLE_Scan";
  BLEDevice::init(deviceName.c_str());

  printLine("===== ESP32 BLE 扫描 =====");
  printLine(("设备名称: " + deviceName).c_str());
  printLine(("MAC地址: " + BLEDevice::getAddress().toString()).c_str());

  pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(150);
  pBLEScan->setWindow(150);
}

void loop() {
  printLine("===== 开始扫描 =====");

  for (int r = 0; r < repeatTimes; r++) {
    printLine(("扫描轮次: " + String(r + 1) + "/" + String(repeatTimes)).c_str());

    BLEScanResults* pResults = pBLEScan->start(scanTime, false);

    if (!pResults) {
      printLine("扫描失败，返回空指针");
      delay(1000);
      continue;
    }

    int count = pResults->getCount();
    for (int i = 0; i < count; i++) {
      BLEAdvertisedDevice device = pResults->getDevice(i);
      String name = device.haveName() ? device.getName().c_str() : "(未知)";
      printLine(("名称: " + name).c_str());
      printLine(("MAC : " + device.getAddress().toString()).c_str());
      printLine(("RSSI: " + String(device.getRSSI()) + " dBm").c_str());
      printLine("--------------------");
    }

    pBLEScan->clearResults();
    delay(500);
  }

  printLine("===== 扫描完成 =====");
  delay(5000);
  tft.fillScreen(TFT_BLACK);  // 每次循环结束后清屏
}
