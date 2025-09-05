#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"

static const char *TAG = "BLE_SCAN";

static int scanTime = 5;      // 单次扫描时间 (秒)
static int repeatTimes = 12;  // 总扫描次数
#define MAX_DEVICES 100

// 用于去重的 MAC 地址列表
static esp_bd_addr_t found_devices[MAX_DEVICES];
static int found_count = 0;

// 判断设备是否已经扫描过
static bool is_device_new(esp_bd_addr_t bda) {
    for (int i = 0; i < found_count; i++) {
        if (memcmp(bda, found_devices[i], sizeof(esp_bd_addr_t)) == 0) {
            return false;
        }
    }
    return true;
}

// GAP 事件回调函数
static void gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch (event) {
        case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
            ESP_LOGI(TAG, "扫描参数设置完成，开始扫描");
            esp_ble_gap_start_scanning(scanTime);
            break;

        case ESP_GAP_BLE_SCAN_RESULT_EVT: {
            esp_ble_gap_cb_param_t *scan_result = param;
            switch (scan_result->scan_rst.search_evt) {
                case ESP_GAP_SEARCH_INQ_RES_EVT: {
                    if (!is_device_new(scan_result->scan_rst.bda)) {
                        break; // 重复设备，跳过
                    }

                    // 保存 MAC
                    if (found_count < MAX_DEVICES) {
                        memcpy(found_devices[found_count], scan_result->scan_rst.bda, sizeof(esp_bd_addr_t));
                        found_count++;
                    }

                    char name[32] = "(未知)";
                    uint8_t *adv_data = NULL;
                    uint8_t adv_len = 0;

                    // 完整名称
                    adv_data = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv,
                                                    ESP_BLE_AD_TYPE_NAME_CMPL,
                                                    &adv_len);
                    if (adv_data == NULL) {
                        // 短名称
                        adv_data = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv,
                                                        ESP_BLE_AD_TYPE_NAME_SHORT,
                                                        &adv_len);
                    }

                    // 确保字符串以 '\0' 结尾
                    if (adv_data != NULL && adv_len > 0 && adv_len < sizeof(name)) {
                        memcpy(name, adv_data, adv_len);
                        name[adv_len] = '\0';
                    } else {
                        strcpy(name, "(未知)");
                    }

                    ESP_LOGI(TAG, "设备: %02x:%02x:%02x:%02x:%02x:%02x, 名称: %s, RSSI: %d",
                             scan_result->scan_rst.bda[0],
                             scan_result->scan_rst.bda[1],
                             scan_result->scan_rst.bda[2],
                             scan_result->scan_rst.bda[3],
                             scan_result->scan_rst.bda[4],
                             scan_result->scan_rst.bda[5],
                             name,
                             scan_result->scan_rst.rssi);
                    break;
                }

                case ESP_GAP_SEARCH_INQ_CMPL_EVT:
                    ESP_LOGI(TAG, "本轮扫描完成，共发现 %d 个新设备", found_count);
                    break;

                default:
                    break;
            }
            break;
        }

        default:
            break;
    }
}

void app_main(void) {
    esp_err_t ret;

    // 初始化 NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 初始化蓝牙控制器
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    // 注册 GAP 回调
    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_cb));

    // 配置扫描参数
    esp_ble_scan_params_t ble_scan_params = {
        .scan_type              = BLE_SCAN_TYPE_ACTIVE,
        .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
        .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
        .scan_interval          = 0x50,
        .scan_window            = 0x30,
        .scan_duplicate         = BLE_SCAN_DUPLICATE_DISABLE
    };
    ESP_ERROR_CHECK(esp_ble_gap_set_scan_params(&ble_scan_params));

    // 循环扫描
    while (1) {
        for (int r = 0; r < repeatTimes; r++) {
            ESP_LOGI(TAG, "===== 扫描轮次 %d/%d =====", r + 1, repeatTimes);

            // 每轮扫描前清空已发现设备，保证每轮去重独立
            found_count = 0;

            esp_ble_gap_start_scanning(scanTime);
            vTaskDelay((scanTime + 1) * 1000 / portTICK_PERIOD_MS);  // 等待扫描完成
        }
        ESP_LOGI(TAG, "===== 本轮连续扫描完成 =====");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
