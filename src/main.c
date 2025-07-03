#include "game/game.h"
#include <lcom/lcf.h>

extern uint8_t scancode;
extern uint32_t counter_KBC;

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");
  lcf_log_output("/home/lcom/labs/proj/output.txt");
  if (lcf_start(argc, argv))
    return 1;
  lcf_cleanup();
  return 0;
}

int(proj_main_loop)(int argc, char *argv[]) {
  int ipc_status, r;
  message msg;
  bool pong = true;

  uint8_t t_irq_set, k_irq_set, m_irq_set;

  if (vg_init(0x105) == NULL) {
    vg_exit();
    return 1;
  }
  if (timer_subscribe_int(&t_irq_set)) {
    vg_exit();
    return 1;
  }
  if (kbd_subscribe_int(&k_irq_set)) {
    vg_exit();
    return 1;
  }
  if (mouse_subscribe_int(&m_irq_set)) {
    vg_exit();
    return 1;
  }
  if (mouse_enable_data_reporting_alt()) {
    vg_exit();
    return 1;
  }

  game_state_t state = STATE_MENU;
  const state_driver_t *drv = get_state_driver(state);
  if (drv->init() != 0) {
    kbd_unsubscribe_int();
    mouse_unsubscribe_int();
    vg_exit();
    return 1;
  }
  drv->draw();

  while (pong) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:

          if (msg.m_notify.interrupts & t_irq_set) {
            timer_int_handler();
            if (state == STATE_PLAY) {
              drv->update();
              drv->draw();
            }
          }

          if (msg.m_notify.interrupts & k_irq_set) {
            kbc_ih();
            if (!scancode_ready())
              break;
            uint16_t code = get_full_scancode();
            game_state_t next = drv->handle_input(code);

            //printf("Scancode: 0x%04X\n", code);
            if (next != state) {
              drv->cleanup();
              if (next == STATE_EXIT) {
                pong = false;
              }
              else {
                state = next;
                drv = get_state_driver(state);
                drv->init();
                drv->draw();
              }
            }
          }

          if (msg.m_notify.interrupts & m_irq_set) {
            mouse_ih();
            mouse_packet_builder();
            if (state == STATE_PLAY) {
              drv->handle_mouse();
            }
          }

          break;

        default:
          break;
      }
    }
  }

  mouse_disable_data_reporting();
  mouse_unsubscribe_int();
  if (timer_unsubscribe_int() != 0)
    return 1;
  if (kbd_unsubscribe_int() != 0)
    return 1;
  drv->cleanup();
  vg_exit();
  return 0;
}
