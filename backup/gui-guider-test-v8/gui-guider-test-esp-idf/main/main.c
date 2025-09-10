#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "gui_guider.h"
#include "custom.h"
#include "esp_lvgl_port.h"
#include "esp_log.h"
#include "driver/gpio.h"

static const char *TAG = "GUI-GUIDER-TEST";

// 如果屏幕有背光引脚，定义它（根据你的硬件修改）
#define LCD_BL_PIN 21

/**
 * @brief GUI 控制任务
 *
 * 初始化 UI 并循环调度 LVGL
 */
void app_controller_task(void *param)
{
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvgl_cfg);   // 初始化 LVGL
    ESP_LOGI(TAG, "LVGL port initialized");

    // 等待屏幕初始化
    // while(lv_scr_act() == NULL){
    //     ESP_LOGI(TAG, "Waiting for lv_scr_act() to be ready...");
    //     vTaskDelay(pdMS_TO_TICKS(10));
    // }

    ESP_LOGI(TAG, "lv_scr_act() is ready: %p", lv_scr_act());

    lvgl_port_lock(0);
    setup_ui(&guider_ui);        // 安全创建 ui->Home
    lvgl_port_unlock();

    while(1){
        lvgl_port_lock(0);
        lv_timer_handler();
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/**
 * @brief ESP-IDF 程序入口
 */
void app_main(void)
{
    ESP_LOGI(TAG, "Starting app_main...");

    // 1️⃣ 初始化 LVGL 端口（显示 & 触摸）
    // const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    // lvgl_port_init(&lvgl_cfg);

    

    // 2️⃣ 打开背光
    gpio_reset_pin(LCD_BL_PIN);
    gpio_set_direction(LCD_BL_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(LCD_BL_PIN, 0);

    // 3️⃣ 创建 GUI 控制任务
    xTaskCreate(
        app_controller_task,
        "app_controller",
        8192,
        NULL,
        5,
        NULL
    );
}

