#include <lcom/lcf.h>
#include <stdint.h>
#include "video.h"

static void *video_mem;

struct minix_mem_range mr;
static unsigned int vram_base; 
static unsigned int vram_size;
static unsigned h_res;
static unsigned v_res;
static unsigned bits_per_pixel;
static uint8_t memory_model;
static uint8_t red_mask_size;
static uint8_t green_mask_size;
static uint8_t blue_mask_size;
static uint8_t red_field_position;
static uint8_t green_field_position;
static uint8_t blue_field_position;

int set_video_mode(uint16_t mode) {
  reg86_t r86;
  memset(&r86, 0, sizeof(r86));

  r86.intno = 0x10;
  r86.ah = 0x4F;    
  r86.al = 0x02;
  r86.bx = mode | BIT(14);

  if (sys_int86(&r86) != OK) {
    printf("\tset_video_mode(): sys_int86() failed \n");
    return 1;
  }

  if ((r86.ah != 0x00) || (r86.al != 0x4F)) {
    printf("VBE set mode failed!\n");
    return 1;
  }

  return 0;
}

int get_mode_info(uint16_t mode, vbe_mode_info_t *vmi) {
  if (vbe_get_mode_info(mode, vmi)) {
    return 1;
  }
  return 0;
}

void* map_video_memory(uint32_t vram_base, uint32_t vram_size) {
  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  int r;
  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
  }

  video_mem = (void *) vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
  if (video_mem == MAP_FAILED) {
    return NULL;
  }

  return video_mem;
}

void *(vg_init)(uint16_t mode) {
  // Set the video mode using VBE
  int result = set_video_mode(mode);
  if (result != 0) {
    return NULL;
  }

  // Retrieve mode information
  vbe_mode_info_t vmi;
  result = get_mode_info(mode, &vmi);
  if (result != 0) {
    return NULL;
  }

  // Set video parameters
  vram_base = vmi.PhysBasePtr;
  h_res = vmi.XResolution;
  v_res = vmi.YResolution;
  bits_per_pixel = vmi.BitsPerPixel;
  vram_size = h_res * v_res * (bits_per_pixel / 8);

  // Map the video memory
  video_mem = map_video_memory(vram_base, vram_size);
  if (video_mem == NULL) {
    return NULL;
  }

  return video_mem;
}

void *(vg_init_rectangle)(uint16_t mode) {
  vbe_mode_info_t vmi;
  int result = get_mode_info(mode, &vmi);
  if (result != 0) {
    return NULL;
  }

  vram_base = vmi.PhysBasePtr;
  h_res = vmi.XResolution;
  v_res = vmi.YResolution;
  bits_per_pixel = vmi.BitsPerPixel;
  memory_model = vmi.MemoryModel;
  red_mask_size = vmi.RedMaskSize;
  green_mask_size =vmi.GreenMaskSize;
  blue_mask_size = vmi.BlueMaskSize;
  red_field_position = vmi.RedFieldPosition;
  green_field_position = vmi.GreenFieldPosition;
  blue_field_position = vmi.BlueFieldPosition;

  vram_size = h_res * v_res * ((bits_per_pixel + 7) / 8);

  video_mem = map_video_memory(vram_base, vram_size);
  if (video_mem == NULL) {
    return NULL;
  }

  result = set_video_mode(mode);
  if (result != 0) {
    return NULL;
  }

  return video_mem;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  //if (x >= h_res || y >= v_res || len == 0 || x + len > h_res) return 1;
  uint8_t bytes_per_pixel = (bits_per_pixel + 7) / 8; // handles rounding up
  uint8_t *vram_ptr = (uint8_t *)video_mem;
  uint32_t start_index = (y * h_res + x) * bytes_per_pixel;

  for (uint16_t i = 0; i < len; i++) {
    uint32_t pixel_index = start_index + i * bytes_per_pixel;

    for (uint8_t b = 0; b < bytes_per_pixel; b++) {
      vram_ptr[pixel_index + b] = (color >> (b * 8)) & 0xFF;
    }
  }

  return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  //if (x + width > h_res || y + height > v_res) return 1;
  for (uint16_t row = 0; row < height; row++) {
    if (vg_draw_hline(x, y + row, width, color)) return 1;
  }
  return 0;
}

int (vg_draw_pattern)(uint8_t no_rectangles, uint32_t first, uint8_t step, uint16_t mode) {
  uint16_t rect_width = h_res / no_rectangles;
  uint16_t rect_height = v_res / no_rectangles;
  for (uint16_t row = 0; row < no_rectangles; row++) {
    for (uint16_t col = 0; col < no_rectangles; col++) {
      if (memory_model == INDEXED_COLOR) {
      vg_draw_rectangle(rect_width*col, rect_height*row,rect_width,rect_height,((first + (row * no_rectangles + col) * step) % (1 << bits_per_pixel)));
      }
      else if (memory_model == DIRECT_COLOR) {
        uint32_t color = color_calc(col,row,first,step);
        vg_draw_rectangle(rect_width*col, rect_height*row,rect_width,rect_height,color);
      }
    }
  }
  return 0;
}

uint32_t color_calc(uint16_t col, uint16_t row, uint32_t first, uint8_t step) {
  uint32_t red_first   = (first >> red_field_position)   & ((1 << red_mask_size) - 1);
        uint32_t green_first = (first >> green_field_position) & ((1 << green_mask_size) - 1);
        uint32_t blue_first  = (first >> blue_field_position)  & ((1 << blue_mask_size) - 1);

        uint32_t red = (red_first + col * step) % (1 << red_mask_size);
        uint32_t green = (green_first + row * step) % (1 << green_mask_size);
        uint32_t blue = (blue_first + (row + col) * step) % (1 << blue_mask_size);

        uint32_t color = (red << red_field_position) |
                 (green << green_field_position) |
                 (blue << blue_field_position);
  return color;
}

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint8_t color) {
  uint8_t bytes_per_pixel = (bits_per_pixel + 7) / 8; // handles rounding up
  uint32_t start_index = (y * h_res + x) * bytes_per_pixel;
  uint8_t *vram_ptr = (uint8_t *)video_mem + start_index;
  for (uint8_t b = 0; b < bytes_per_pixel; b++) {
    vram_ptr[b] = (color >> (b * 8)) & 0xFF;
  }
  return 0;
}
