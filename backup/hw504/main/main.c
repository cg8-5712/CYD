#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define TAG "HW504_PWM"

// 遥感引脚
#define ADC_CHANNEL    ADC2_CHANNEL_7   // ADC1_CHANNEL_7 = GPIO35, 对应 Y 轴
#define BUTTON_GPIO    36               // 按压开关接 GPIO22，可修改
                                        // 使用 GPIO21 供电 3.3V

#define PWM_MAX        255
#define ADC_MAX        4095             // 12位ADC

// 模拟PWM值
static int pwm_value = 0;

void app_main(void)
{
    // ADC 初始化
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_11);

    // 按键初始化
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    while (1) {
        int adc_val = adc1_get_raw(ADC_CHANNEL);  // 读取摇杆
        ESP_LOGI(TAG, "ADC Value: %d", adc_val);
        int button_state = gpio_get_level(BUTTON_GPIO); // 读取按键

        // 计算增量：摇杆中心附近不变化，向上增加，向下减少
        int delta = 0;
        int dead_zone = 200;  // 中心死区
        int center = ADC_MAX / 2;

        if (adc_val > center + dead_zone) {
            delta = ((adc_val - center) * 255) / (ADC_MAX / 2); // 向上增加
        } else if (adc_val < center - dead_zone) {
            delta = -((center - adc_val) * 255) / (ADC_MAX / 2); // 向下减少
        }

        pwm_value += delta;
        if (pwm_value > PWM_MAX) pwm_value = PWM_MAX;
        if (pwm_value < 0) pwm_value = 0;

        // 按键逻辑：PWM=0，按下=>满; PWM!=0，按下=>0
        if (button_state == 0) { // 假设低电平表示按下
            if (pwm_value == 0) {
                pwm_value = PWM_MAX;
            } else {
                pwm_value = 0;
            }
        }

        ESP_LOGI(TAG, "PWM Value: %d", pwm_value);

        vTaskDelay(pdMS_TO_TICKS(100));  // 100ms更新一次
    }
}
