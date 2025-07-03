#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

static int hook_id = 0;
unsigned int timer_counter = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  
  if (timer != 0 && timer != 1 && timer != 2) {
    return 1;
  }

  if (freq < 19 && freq > 1193182) {
    return 1;
  }

  uint16_t div = TIMER_FREQ / freq;
  uint8_t lsb;
  util_get_LSB(div, &lsb);
  uint8_t msb;
  util_get_MSB(div, &msb);

  uint8_t status;
  if (timer_get_conf(timer, &status) != 0) {
    return 1;
  }

  uint8_t control_word = (timer << 6) | TIMER_LSB_MSB | (status & 0x0F);

  if (sys_outb(TIMER_CTRL, control_word)) {
    return 1;
  }

  if (sys_outb(TIMER_0 + timer, lsb) || sys_outb(TIMER_0 + timer, msb)) {
    return 1;
  }

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = hook_id;
  if (sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&hook_id) != 0) {return -1;}
  return 0;
}

int (timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id) != OK) {
    return 1;
  }
  return 0;
}

void (timer_int_handler)() {
  timer_counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  uint8_t cmd;
  if (timer == 0) {cmd = 0xE2;}
  else if (timer == 1) {cmd = 0xE4;}
  else if (timer == 2) {cmd = 0xE8;}
  else {return 1;}
  if (sys_outb(TIMER_CTRL, cmd)) {
    return 1;
  }
  if (util_sys_inb(TIMER_0 + timer, st)) {
    return 1;
  }
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  union timer_status_field_val conf;
  switch (field) {
  case tsf_all:
    conf.byte = st;
    break;
  case tsf_initial:
    switch ((st >> 4) & 0x03) {
      case 0x01:
        conf.in_mode = LSB_only;
        break;
      case 0x02:
        conf.in_mode = MSB_only;
        break;
      case 0x03:
        conf.in_mode = MSB_after_LSB;
        break;
      default:
        conf.in_mode = INVAL_val;
        break;
    }
    break;
  case tsf_mode:
    conf.count_mode = (st >> 1) & 0x07;
    if (conf.count_mode == 6) {
      conf.count_mode = 2;
    }
    if (conf.count_mode == 7) {
      conf.count_mode = 3;
    } 
    break;
  case tsf_base:
    conf.bcd = (st & 0x01) ? 1 : 0;
    break;   
  }

  return timer_print_config(timer, field, conf);
}
