idf_component_register(
    SRCS
        "main.c"
    INCLUDE_DIRS
        "."

    REQUIRES
        esp_lvgl_port
        lvgl__lvgl
        "ui_custom"
    PRIV_REQUIRES 
        nvs_flash
)