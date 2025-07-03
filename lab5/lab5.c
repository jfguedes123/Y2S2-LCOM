// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

#include "video.h"
#include "i8042.h"
#include "keyboard.h"
//#include "timer.c"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  if (vg_init(mode) == NULL) {
    vg_exit();
    return 1;
  }
  tickdelay(delay * sys_hz());
  if (vg_exit() != 0) {
    return 1;
  }
  return 0;
}

extern uint8_t scancode;

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  int ipc_status;
  message msg;
  uint8_t irq_set;

  if (vg_init_rectangle(mode) == NULL) {
    return 1;
  }

  if (vg_draw_rectangle(x, y, width, height, color)) {
    vg_exit();
    return 1;
  }

  if (kbd_subscribe_int(&irq_set)) {
    vg_exit();
    return 1;
  }

  while (scancode != ESC_BREAK_CODE) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0)
      continue;
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & BIT(irq_set)) {
            kbc_ih();
          }
          break;
        default:
          break;
      }
    }
  }

  kbd_unsubscribe_int();
  vg_exit();
  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  int ipc_status;
  message msg;
  uint8_t irq_set;

  if (vg_init_rectangle(mode) == NULL) {
    return 1;
  }

  if (vg_draw_pattern(no_rectangles, first, step, mode)) {
    vg_exit();
    return 1;
  }

  if (kbd_subscribe_int(&irq_set)) {
    vg_exit();
    return 1;
  }

  while (scancode != ESC_BREAK_CODE) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0)
      continue;
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & BIT(irq_set)) {
            kbc_ih();
          }
          break;
        default:
          break;
      }
    }
  }

  kbd_unsubscribe_int();
  vg_exit();
  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  int ipc_status;
  message msg;
  uint8_t irq_set;

  if (vg_init_rectangle(0x105) == NULL) {
    vg_exit();
    return 1;
  }

  xpm_image_t img;
  uint8_t *pixmap = xpm_load(xpm, XPM_INDEXED, &img);
  if (pixmap == NULL) return 1;

  // Draw the pixmap on screen
  for (uint16_t row = 0; row < img.height; row++) {
    for (uint16_t col = 0; col < img.width; col++) {
      uint8_t color = pixmap[row * img.width + col];
      vg_draw_pixel(x + col, y + row, color);
      //vg_draw_rectangle(x+col,y+row,1,1,color);
    }
  }   

  if (kbd_subscribe_int(&irq_set)) {
    vg_exit();
    return 1;
  }

  while (scancode != ESC_BREAK_CODE) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0)
      continue;
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & BIT(irq_set)) {
            kbc_ih();
          }
          break;
        default:
          break;
      }
    }
  }

  kbd_unsubscribe_int();
  vg_exit();
  return 0;
}

extern unsigned int timer_counter;

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {

  if ((xi != xf) && (yi != yf)) {
    return 1;
  }

  int x_dir = (xf > xi) ? 1 : ((xf < xi) ? -1 : 0);
  int y_dir = (yf > yi) ? 1 : ((yf < yi) ? -1 : 0);

  int ipc_status;
  message msg;
  uint8_t k_irq_set;
  uint8_t t_irq_set;

  if (vg_init_rectangle(0x105) == NULL) {
    vg_exit();
    return 1;
  }

  xpm_image_t img;
  uint8_t *pixmap = xpm_load(xpm, XPM_INDEXED, &img);
  if (pixmap == NULL) return 1;

  // Draw the pixmap on screen
  for (uint16_t row = 0; row < img.height; row++) {
    for (uint16_t col = 0; col < img.width; col++) {
      uint8_t color = pixmap[row * img.width + col];
      vg_draw_pixel(xi + col, yi + row, color);
      //vg_draw_rectangle(x+col,y+row,1,1,color);
    }
  }   

  if (kbd_subscribe_int(&k_irq_set)) {
    vg_exit();
    return 1;
  }

  if (timer_subscribe_int(&t_irq_set)) {
    vg_exit();
    return 1;
  }

  bool reached = false;
  uint16_t curr_x = xi, curr_y = yi;
  uint32_t frame_ticks = 0;
  uint32_t hz = sys_hz();
  uint32_t ticks_per_frame = hz / fr_rate;
  uint32_t frames_per_pixel = (speed < 0) ? -speed : 1;
  uint32_t frame_count = 0;

  while (scancode != ESC_BREAK_CODE) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0)
      continue;
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & BIT(k_irq_set)) {
            kbc_ih();
          }
          if (msg.m_notify.interrupts & BIT(t_irq_set)) {
            timer_int_handler();
            if (++frame_ticks >= ticks_per_frame) {
              frame_ticks = 0;

              int16_t delta;
              if (speed > 0) {
                delta = speed;
              }
              else {
                if (++frame_count >= frames_per_pixel) {
                  frame_count = 0;
                  delta = 1;
                }
                else {
                  delta = 0;
                }
              }
    
              if (!reached && delta > 0) {
                vg_draw_rectangle(curr_x, curr_y, img.width, img.height, 0);

                if (x_dir) {
                  int nx = curr_x + x_dir*delta;
                  curr_x = (x_dir>0 ? (nx>xf?xf:nx)
                                    : (nx<xf?xf:nx));
                }
                if (y_dir) {
                  int ny = curr_y + y_dir*delta;
                  curr_y = (y_dir>0 ? (ny>yf?yf:ny)
                                    : (ny<yf?yf:ny));
                }
    
                if (curr_x==xf && curr_y==yf)
                  reached = true;
                

                for (uint16_t r2 = 0; r2 < img.height; r2++)
                  for (uint16_t c2 = 0; c2 < img.width; c2++) {
                    uint8_t color = pixmap[r2 * img.width + c2];
                    vg_draw_pixel(curr_x + c2,
                                      curr_y + r2,
                                      color);
                  }
              }
            }
          }
          break;
        default:
          break;
      }
    }
  }

  kbd_unsubscribe_int();
  timer_unsubscribe_int();
  vg_exit();
  return 0;    
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
