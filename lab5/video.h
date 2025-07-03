#ifndef VIDEO_H
#define VIDEO_H

#define INDEXED_COLOR 4
#define DIRECT_COLOR 6

#include <lcom/lcf.h>
#include <stdint.h>

int set_video_mode(uint16_t mode);
int get_mode_info(uint16_t mode, vbe_mode_info_t *vmi);
void* map_video_memory(uint32_t vram_base, uint32_t vram_size);
void* (vg_init)(uint16_t mode);
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
void *(vg_init_rectangle)(uint16_t mode);
int (vg_draw_pattern)(uint8_t no_rectangles, uint32_t first, uint8_t step, uint16_t mode);
uint32_t color_calc(uint16_t col, uint16_t row, uint32_t first, uint8_t step);
int (vg_draw_pixel)(uint16_t x, uint16_t y, uint8_t color);

#endif // VIDEO_H
