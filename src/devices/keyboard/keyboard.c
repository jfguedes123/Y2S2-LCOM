#include "keyboard.h"
#include "../kbc/KBC.h"

int keyboard_hook_id = 1;
uint8_t scancode = 0;

static bool     waiting_for_second = false;
static bool     full_ready         = false;
static uint16_t full_scancode      = 0;

int(kbd_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL)
    return 1;
  *bit_no = BIT(keyboard_hook_id);
  if (sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &keyboard_hook_id))
    return 1;
  return 0;
}

int(kbd_unsubscribe_int)() {
  if (sys_irqrmpolicy(&keyboard_hook_id) != 0)
    return 1;
  return 0;
}

void(kbc_ih)() {
  if (kbc_read_out(KBC_OUT_BUF, &scancode) != 0)
    return;
  if (scancode == TWO_BYTES) {
      waiting_for_second = true;
      return;
  }
  if (waiting_for_second) {
      full_scancode   = (TWO_BYTES << 8) | scancode;
      waiting_for_second = false;
  }
  else {
      full_scancode = scancode;
  }
  full_ready = true;
}

bool scancode_ready(void) {
  return full_ready;
}

uint16_t get_full_scancode(void) {
  full_ready = false;
  return full_scancode;
}
