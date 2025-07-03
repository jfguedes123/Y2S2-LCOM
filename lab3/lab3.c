#include <lcom/lab3.h>
#include <lcom/lcf.h>

#include "i8042.h"
#include "i8254.h"
#include "keyboard.h"
#include "KBC.h"

#include <stdbool.h>
#include <stdint.h>

extern uint8_t scancode;
extern int counter;
extern uint32_t counter_KBC;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  // lcf_trace_calls("/Users/joaofilipeguedes/MINIX-LCOM/shared/grupo_2leic05_7/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  // lcf_log_output("/Users/joaofilipeguedes/MINIX-LCOM/shared/grupo_2leic05_7/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {

  int ipc_status, r;
  message msg;
  uint8_t irq_set;

  if (kbd_subscribe_int(&irq_set))
    return 1;

  while (scancode != ESC_BREAKCODE) { /* You may want to use a different condition */
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
            kbc_ih();
            kbd_print_scancode(!(scancode & MAKE_CODE), scancode == TWO_BYTES ? 2 : 1, &scancode);
          }
          break;
        default:
          break;
      } /* no other notifications expected: do nothing */
    }
    else { /* received a standard message, not a notification */
      /* no standard messages expected: do nothing */
    }
  }

  if (kbd_unsubscribe_int() != 0)
    return 1;
  if (kbd_print_no_sysinb(counter_KBC) != 0)
    return 1;
  return 0;
}

int(kbd_test_poll)() {

  while (scancode != ESC_BREAKCODE) {

    uint8_t status;
    int attempts = 10;

    while (attempts > 0) {
      if (util_sys_inb(KBC_STATUS_REG, &status))
        return 1;

      if ((status & OUT_BUFFER)) {
        if ((status & ERRORS)) {
          return 1;
        }
        else {
          if (util_sys_inb(KBC_WRITE_CMD, &scancode)) {
            return 1;
          }
          else {
            kbd_print_scancode(!(scancode & BIT(7)), scancode == TWO_BYTES ? 2 : 1, &scancode);
          }
        }
      }
      attempts--;
    }
  }

  restore_kbd();
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {

  int ipc_status, r;
  message msg;
  uint8_t irq_set_TIMER, irq_set_KBC;
  int seconds = 0;

  if (timer_subscribe_int(&irq_set_TIMER) != 0)
    return 1;
  if (kbd_subscribe_int(&irq_set_KBC) != 0)
    return 1;

  while (scancode != ESC_BREAKCODE && seconds < n) { /* You may want to use a different condition */
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                 /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set_KBC) { /* subscribed interrupt */
            kbc_ih();
            kbd_print_scancode(!(scancode & BIT(7)), scancode == TWO_BYTES ? 2 : 1, &scancode);
            seconds = 0;
            counter = 0;
          }

          if (msg.m_notify.interrupts & irq_set_TIMER) { /* subscribed interrupt */
            timer_int_handler();
            if (counter % 60 == 0) {
              timer_print_elapsed_time();
              seconds++;
            }
          }

          break;
        default:
          break;
      } /* no other notifications expected: do nothing */
    }
    else { /* received a standard message, not a notification */
      /* no standard messages expected: do nothing */
    }
  }

  if (timer_unsubscribe_int() != 0)
    return 1;
  if (kbd_unsubscribe_int() != 0)
    return 1;
  if (kbd_print_no_sysinb(counter_KBC) != 0)
    return 1;
  return 0;
}
