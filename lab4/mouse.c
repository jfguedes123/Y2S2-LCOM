#include "mouse.h"
#include "KBC.h"

int mouse_hook_id = 2, packet_index = 0;
uint8_t byte, mouse_data_packet[3];
struct packet mouse_packet;

int(mouse_subscribe_int)(uint8_t *bit_no) {
  *bit_no = BIT(mouse_hook_id);
  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id) != OK)
    return 1;

  return 0;
}

int(mouse_unsubscribe_int)() {
  if (sys_irqrmpolicy(&mouse_hook_id) != OK)
    return 1;
  return 0;
}

void(mouse_ih)() {
  if (kbc_read_out(KBC_OUT_BUF, &byte) != 0)
    return;
}

int(mouse_packet_builder)() {
  if (packet_index == 0 && ((byte & BIT(3)) == 0)) {
    return 1;
  }

  mouse_data_packet[packet_index] = byte;
  packet_index++;

  if (packet_index == 3) {
    for (int i = 0; i < 3; i++) {
      mouse_packet.bytes[i] = mouse_data_packet[i];
    }

    mouse_packet.lb = mouse_data_packet[0] & MOUSE_LB;
    mouse_packet.rb = mouse_data_packet[0] & MOUSE_RB;
    mouse_packet.mb = mouse_data_packet[0] & MOUSE_MB;

    mouse_packet.x_ov = mouse_data_packet[0] & MOUSE_X_OV;
    mouse_packet.y_ov = mouse_data_packet[0] & MOUSE_Y_OV;

    mouse_packet.delta_x = (mouse_data_packet[0] & MOUSE_X_SIGN) ? (MOUSE_X_SIGN_MASK | mouse_data_packet[1]) : mouse_data_packet[1];

    mouse_packet.delta_y = (mouse_data_packet[0] & MOUSE_Y_SIGN) ? (MOUSE_Y_SIGN_MASK | mouse_data_packet[2]) : mouse_data_packet[2];

    packet_index = 0;
    return 2;
  }
  return 0;
}

int(mouse_write_cmd)(uint8_t cmd) {
  uint8_t r;
  uint8_t attempts = 10; 

  while (attempts && r != MOUSE_ACK) {
    if (kbc_write_cmd(KBC_IN_BUF, KBC_WRITE_MOUSE) != 0) {
      tickdelay(micros_to_ticks(20000));
      attempts--;
      continue;
    }

    if (kbc_write_cmd(KBC_OUT_BUF, cmd) != 0) {
      tickdelay(micros_to_ticks(20000));
      attempts--;
      continue;
    }

    if (kbc_read_out(KBC_OUT_BUF, &r) != 0) {
      tickdelay(micros_to_ticks(20000));
      attempts--;
      continue;
    }

    if (r == MOUSE_ACK) {
      return 0;
    }
    else if (r == MOUSE_NACK) {
      tickdelay(micros_to_ticks(20000));
      attempts--;
      continue;
    }
    else {
      return 1;
    }
  }

  return 1;
}

int(mouse_disable_data_reporting)() {
  if (mouse_write_cmd(MOUSE_DISABLE) != 0)
    return 1;
  return 0;
}
