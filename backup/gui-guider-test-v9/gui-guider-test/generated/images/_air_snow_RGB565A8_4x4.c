#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMAGE__AIR_SNOW_RGB565A8_4X4
#define LV_ATTRIBUTE_IMAGE__AIR_SNOW_RGB565A8_4X4
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMAGE__AIR_SNOW_RGB565A8_4X4 uint8_t _air_snow_RGB565A8_4x4_map[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 

  0x00, 0x9d, 0x75, 0x00, 
  0xea, 0xff, 0xfe, 0xc2, 
  0xaf, 0xff, 0xff, 0x7e, 
  0x1a, 0x9f, 0x79, 0x12, 
};

const lv_image_dsc_t _air_snow_RGB565A8_4x4 = {
  .header.magic = LV_IMAGE_HEADER_MAGIC,
  .header.cf = LV_COLOR_FORMAT_RGB565A8,
  .header.stride = 8,
  .header.w = 4,
  .header.h = 4,
  .data_size = sizeof(_air_snow_RGB565A8_4x4_map),
  .data = _air_snow_RGB565A8_4x4_map,
};
