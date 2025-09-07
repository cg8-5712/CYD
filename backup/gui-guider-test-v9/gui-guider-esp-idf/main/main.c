#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "gui_guider.h"
#include "custom.h"
#include "esp_lvgl_port.h"
#include "esp_log.h"
#include <math.h>
#include "freertos/timers.h"

static const char *TAG = "GUI-GUIDER-TEST";

/**
 * @brief 应用控制器主任务
 *
 * 初始化 UI、动画和事件处理循环
 */
void app_controller_task(void *param)
{
    EventBits_t events;

    ESP_LOGI(TAG, "Initializing UI...");

    // ----------------------------
    // UI 初始化
    // ----------------------------
    lvgl_port_lock(0);            // 获取 LVGL 锁
    setup_ui(&guider_ui);         // 初始化 GUI-Guider UI
    lvgl_port_unlock();            // 释放锁

    // ----------------------------
    // 初始化随机数
    // ----------------------------
    srand(xTaskGetTickCount());

    ESP_LOGI(TAG, "App controller task started");

    // ----------------------------
    // 主事件循环
    // ----------------------------
    // 主循环
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10));
        lv_task_handler();
    }
}
