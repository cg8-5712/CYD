#include <lvgl.h>

void setup() {
  lv_init();
  // 只初始化一个基础控件，验证是否有编译问题
  lv_obj_t * label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label, "Hello, LVGL!");
}

void loop() {
  lv_task_handler();
  delay(10);
}
