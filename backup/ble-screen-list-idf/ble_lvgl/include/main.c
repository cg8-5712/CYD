#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_ble_scan.h"
#include "BLEDevice.h"
#include "LovyanGFX.hpp"
#include "lvgl.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#define LV_CONF_INCLUDE_SIMPLE

static const char *TAG = "BLE_LVGL";

// BLE 配置
BLEScan* pBLEScan;
int scanTime = 5;
lv_obj_t* list;  // 设备列表对象

// 显示驱动
LGFX tft;

// 显示回调
static void my_disp_flush(lv_disp_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    tft.pushImageDMA(area->x1, area->y1, w, h, (lgfx::rgb565_t*)px_map);
    lv_disp_flush_ready(disp);
}

// 刷新按钮回调
static void btn_event_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_obj_clean(list);  // 清空列表

        BLEScanResults results = pBLEScan->start(scanTime, false);

        int count = results.getCount();
        for (int i = 0; i < count; i++) {
            BLEAdvertisedDevice device = results.getDevice(i);
            String name = device.haveName() ? device.getName().c_str() : "(未知)";
            String mac = device.getAddress().toString().c_str();
            String line = name + " | " + mac;
            lv_list_add_text(list, line.c_str());
        }
        pBLEScan->clearResults();
    }
}

void lvgl_task(void *pvParameter)
{
    // LVGL 事件循环
    while (1) {
        lv_timer_handler();  // LVGL 事件循环
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

void ble_scan_task(void *pvParameter)
{
    // 初始化 BLE
    BLEDevice::init("ESP32_BLE_Scan");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(200);
    pBLEScan->setWindow(200);

    // 启动扫描
    while (1) {
        pBLEScan->start(scanTime, false);
        vTaskDelay(scanTime * 1000 / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    // 初始化串口
    ESP_LOGI(TAG, "ESP32 BLE LVGL Demo");

    // 初始化显示驱动
    tft.begin();
    
    // 初始化 LVGL
    lv_init();

    // 创建显示缓冲区
    static lv_color_t buf1[240 * 10]; 
    static lv_color_t buf2[240 * 10];
    static lv_disp_draw_buf_t draw_buf1;
    static lv_disp_draw_buf_t draw_buf2;

    lv_disp_draw_buf_init(&draw_buf1, buf1, NULL, 240 * 10);
    lv_disp_draw_buf_init(&draw_buf2, buf2, NULL, 240 * 10);

    // 创建并配置显示设备
    static lv_disp_t * disp = lv_disp_drv_register();
    lv_disp_set_draw_buffers(disp, &draw_buf1, &draw_buf2);
    lv_disp_set_flush_cb(disp, my_disp_flush);

    // 创建UI界面
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

    // 创建任务
    xTaskCreate(lvgl_task, "lvgl_task", 2048, NULL, 5, NULL);
    xTaskCreate(ble_scan_task, "ble_scan_task", 2048, NULL, 5, NULL);
}
