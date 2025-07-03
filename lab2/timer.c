#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int counter = 0;
int hook_id = 0;

int(timer_set_frequency)(uint8_t timer, uint32_t freq) {
  if (timer < 0 || timer > 2)
    return 1;
  if (freq > TIMER_FREQ || freq < 19)
    return 1;

  uint32_t div = TIMER_FREQ / freq;
  uint8_t lsb, msb;

  uint8_t st;
  if (timer_get_conf(timer, &st) != 0)
    return 1;
  if (util_get_MSB(div, &msb) != 0)
    return 1;
  if (util_get_LSB(div, &lsb) != 0)
    return 1;

  uint8_t select_timer;
  switch (timer) {
    case 0:
      st = (st & 0x0F) | TIMER_LSB_MSB | TIMER_SEL0;
      select_timer = TIMER_0;
      break;
    case 1:
      st = (st & 0x0F) | TIMER_LSB_MSB | TIMER_SEL1;
      select_timer = TIMER_1;
      break;
    case 2:
      st = (st & 0x0F) | TIMER_LSB_MSB | TIMER_SEL2;
      select_timer = TIMER_2;
      break;
    default:
      return 1;
  }

  if (sys_outb(TIMER_CTRL, st) != 0)
    return 1;
  if (sys_outb(select_timer, msb) != 0)
    return 1;
  if (sys_outb(select_timer, lsb) != 0)
    return 1;

  return 0;
}

int(timer_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL)
    return 1;
  *bit_no = BIT(hook_id);
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != 0)
    return 1;

  return 0;
}

int(timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id) != 0)
    return 1;
  return 0;
}

void(timer_int_handler)() {
  counter++;
}

int(timer_get_conf)(uint8_t timer, uint8_t *st) {
  if (st == NULL)
    return 1;
  if (timer < 0 || timer > 2)
    return 1;

  uint8_t rb_command = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);

  if (sys_outb(TIMER_CTRL, rb_command) != 0)
    return 1;
  if (util_sys_inb(TIMER_0 + timer, st))
    return 1;

  return 0;
}

int(timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  if (timer < 0 || timer > 2)
    return 1;
    
  union timer_status_field_val status;

  switch (field) {
    case tsf_all:
      status.byte = st;
      break;

    case tsf_initial:
      st = (st >> 4) & 0x03;
      if (st == 1)
        status.in_mode = LSB_only;
      else if (st == 2)
        status.in_mode = MSB_only;
      else if (st == 3)
        status.in_mode = MSB_after_LSB;
      else
        status.in_mode = INVAL_val;
      break;

    case tsf_mode:
      st = (st >> 1) & 0x07;
      if (st == 6)
        st = 2;
      else if (st == 7)
        st = 3;
      status.count_mode = st;
      break;

    case tsf_base:
      status.bcd = st & TIMER_BCD;
      break;
      
    default:
      break;
  }

  if (timer_print_config(timer, field, status) != 0)
    return 1;

  return 0;
}
