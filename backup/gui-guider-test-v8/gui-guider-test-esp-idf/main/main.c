#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "lvgl.h"
#include "gui_guider.h"
#include "esp_lcd_ili9341.h"
#include "esp_lcd_touch.h"

static const char *TAG = "LVGL_GUI";

// 屏幕和触控的 SPI 引脚，根据你的硬件修改
#define LCD_MISO   -1
#define LCD_MOSI   23
#define LCD_SCLK   18
#define LCD_CS     5
#define LCD_DC     16
#define LCD_RST    17
#define LCD_BL     21

#define TOUCH_MISO 19
#define TOUCH_MOSI 23
#define TOUCH_SCLK 18
#define TOUCH_CS   4
#define TOUCH_IRQ  2

// LVGL 缓冲区
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[320*10]; // 10 行缓存

// ILI9341 SPI handle
static spi_device_handle_t spi_lcd;

// ILI9341 刷屏回调
void ili9341_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    // 设置显示区域
    ili9341_set_window(area->x1, area->y1, area->x2, area->y2);

    // 写入像素（16bit）
    ili9341_write_pixels((uint16_t*)color_p, lv_area_get_size(area));

    lv_disp_flush_ready(disp_drv);
}

// XPT2046 触控回调
bool xpt2046_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    uint16_t x = 0, y = 0;
    bool pressed = xpt2046_read_cb(&x, &y); // 用户自行实现 SPI 读
    data->point.x = x;
    data->point.y = y;
    data->state = pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    return false;
}

// LVGL 循环任务
void lvgl_task(void *arg)
{
    while(1){
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Starting GUI...");

    // 1️⃣ 初始化 SPI（LCD & Touch）
    spi_bus_config_t buscfg = {
        .miso_io_num = LCD_MISO,
        .mosi_io_num = LCD_MOSI,
        .sclk_io_num = LCD_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1
    };
    spi_bus_initialize(VSPI_HOST, &buscfg, SPI_DMA_CH_AUTO);

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 40*1000*1000,
        .mode = 0,
        .spics_io_num = LCD_CS,
        .queue_size = 1,
    };
    spi_bus_add_device(VSPI_HOST, &devcfg, &spi_lcd);

    // 2️⃣ 初始化 LVGL
    lv_init();

    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, 320*10);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 320;
    disp_drv.ver_res = 240;
    disp_drv.flush_cb = ili9341_flush_cb;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // 3️⃣ 初始化触控
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = xpt2046_read_cb;
    lv_indev_drv_register(&indev_drv);

    // 4️⃣ 打开背光
    gpio_reset_pin(LCD_BL);
    gpio_set_direction(LCD_BL, GPIO_MODE_OUTPUT);
    gpio_set_level(LCD_BL, 1);

    // 5️⃣ 创建 GUI-Guider UI
    setup_ui(&guider_ui);
    lv_scr_load(guider_ui.Home);

    // 6️⃣ 创建 LVGL 循环任务
    xTaskCreate(lvgl_task, "lvgl_task", 4096, NULL, 5, NULL);
}
