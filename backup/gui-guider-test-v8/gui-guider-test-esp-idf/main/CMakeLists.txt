idf_component_register(
    SRCS
        "main.c"
    INCLUDE_DIRS
        "."

    REQUIRES
        esp_lvgl_port
        lvgl__lvgl
        "ui_custom"
        espressif__esp_lcd_ili9341
        espressif__esp_lcd_touch
    PRIV_REQUIRES 
        nvs_flash
)