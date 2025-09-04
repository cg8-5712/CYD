#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <LovyanGFX.hpp>
#define LV_CONF_INCLUDE_SIMPLE
#include <lvgl.h>
#include <SPI.h>

// ==== 显示驱动 ==== //
class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ILI9341 _panel;
  lgfx::Bus_SPI _bus;
  lgfx::Light_PWM _light;
public:
  LGFX(void) {
    { // SPI 配置
      auto cfg = _bus.config();
      cfg.spi_host   = HSPI_HOST;
      cfg.spi_mode   = 0;
      cfg.freq_write = 40000000;   // 稍微稳妥一点
      cfg.freq_read  = 16000000;
      cfg.pin_sclk   = 14;
      cfg.pin_mosi   = 13;
      cfg.pin_miso   = 12;
      cfg.pin_dc     = 2;
      _bus.config(cfg);
      _panel.setBus(&_bus);
    }
    { // Panel 配置
      auto cfg = _panel.config();
      cfg.pin_cs    = 15;
      cfg.pin_rst   = -1;
      cfg.panel_width  = 240;
      cfg.panel_height = 320;
      cfg.offset_rotation = 0;
      _panel.config(cfg);
    }
    { // 背光配置
      auto cfg = _light.config();
      cfg.pin_bl = 21;
      cfg.invert = false;
      _light.config(cfg);
      _panel.setLight(&_light);
    }
    setPanel(&_panel);
  }
};
LGFX tft;

// ==== BLE 扫描 ==== //
BLEScan* pBLEScan;
int scanTime = 5;
lv_obj_t* list;  // 设备列表对象

// ==== LVGL 显示回调 ==== //
static void my_disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    tft.pushImageDMA(area->x1, area->y1, w, h, (lgfx::rgb565_t*)px_map);
    lv_display_flush_ready(disp);
}

// ==== 刷新按钮回调 ==== //
static void btn_event_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_obj_clean(list);  // 清空列表
        // Fix: Store pointer to scan results
        BLEScanResults* results = pBLEScan->start(scanTime, false);

        int count = results->getCount();
        for (int i = 0; i < count; i++) {
            BLEAdvertisedDevice device = results->getDevice(i);
            String name = device.haveName() ? device.getName().c_str() : "(未知)";
            String mac  = device.getAddress().toString().c_str();
            String line = name + " | " + mac;
            lv_list_add_text(list, line.c_str());
        }
        pBLEScan->clearResults();
    }
}


void setup()
{
    Serial.begin(115200);
    tft.begin();

    // ==== 初始化 LVGL ==== //
    lv_init();

    // 创建显示缓冲区
    static lv_color_t buf1[240 * 10]; 
    static lv_color_t buf2[240 * 10];

    // 创建绘制缓冲区
    static lv_draw_buf_t draw_buf1;
    static lv_draw_buf_t draw_buf2;

    // 初始化绘制缓冲区
    lv_draw_buf_init(&draw_buf1,      // draw_buf
                    240,             // 宽
                    10,              // 高
                    LV_COLOR_FORMAT_RGB565,  // 颜色格式
                    240,             // stride（每行像素数）
                    buf1,            // 第一个缓冲区
                    NULL);           // 第二个缓冲区，可选

    lv_draw_buf_init(&draw_buf2,
                    240,
                    10,
                    LV_COLOR_FORMAT_RGB565,
                    240,
                    buf2,
                    NULL);

    // 创建并配置显示设备
    static lv_display_t * disp = lv_display_create(240, 320);
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_draw_buffers(disp, &draw_buf1, &draw_buf2);

    // ==== UI 创建 ==== //
    lv_obj_t * scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_white(), LV_PART_MAIN);

    // 创建列表
    list = lv_list_create(scr);
    lv_obj_set_size(list, 180, 300);
    lv_obj_align(list, LV_ALIGN_LEFT_MID, 0, 0);

    // 创建刷新按钮
    lv_obj_t * btn = lv_btn_create(scr);
    lv_obj_align(btn, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "刷新");
    lv_obj_center(label);

    // ==== 初始化 BLE ==== //
    BLEDevice::init("ESP32_BLE_Scan");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(200);
    pBLEScan->setWindow(200);
}

void loop()
{
    lv_timer_handler();  // LVGL 事件循环
    delay(5);
}