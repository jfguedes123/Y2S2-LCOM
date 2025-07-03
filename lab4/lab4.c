// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "i8042.h"
#include "i8254.h"
#include "mouse.h"

uint8_t status = 0;
extern int counter;
extern struct packet mouse_packet;

typedef enum {
  START,
  UP,
  VERTEX,
  DOWN,
  END
} SystemState;

SystemState state = START;
uint16_t x_len_total = 0;
uint8_t x_len_;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  // lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(mouse_test_packet)(uint32_t cnt) {
  int ipc_status, r;
  message msg;
  uint8_t irq_set;

  if (mouse_enable_data_reporting() != 0) {
    return 1;
  }

  if (mouse_subscribe_int(&irq_set) != 0) {
    return 1;
  }

  while (cnt != 0) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0) {
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            if (util_sys_inb(KBC_STATUS_REG, &status) != 0) {
              printf("Could not read status");
              return 1;
            }
            mouse_ih();
            r = mouse_packet_builder();
            if (r == 0)
              break;
            else if (r == 1)
              return 1;
            else if (r == 2)
              mouse_print_packet(&mouse_packet);
            cnt--;
          }
      }
    }
  }

  if (mouse_disable_data_reporting() != 0) {
    return 1;
  }

  if (mouse_unsubscribe_int() != 0) {
    return 1;
  }
  return 0;
}

int(mouse_test_async)(uint8_t idle_time) {
  int ipc_status, r;
  message msg;
  uint8_t seconds = 0;
  uint8_t mouse_irq_set = 0, timer_irq_set = 0;
  uint16_t timer_freq = sys_hz();

  if (mouse_enable_data_reporting() != 0)
    return 1;
  if (mouse_subscribe_int(&mouse_irq_set) != 0)
    return 1;
  if (timer_subscribe_int(&timer_irq_set) != 0)
    return 1;

  while (seconds < idle_time) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0) {
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & timer_irq_set) {
            timer_int_handler();
            if (counter % timer_freq == 0) {
              seconds++;
            }
          }
          if (msg.m_notify.interrupts & mouse_irq_set) {
            mouse_ih();
            r = mouse_packet_builder();

            if (r == 0)
              break;
            else if (r == 1)
              return 1;
            else if (r == 2)
              mouse_print_packet(&mouse_packet);

            seconds = 0;
            counter = 0;
          }
      }
    }
  }

  if (mouse_disable_data_reporting() != 0)
    return 1;
  if (timer_unsubscribe_int() != 0)
    return 1;
  if (mouse_unsubscribe_int() != 0)
    return 1;

  return 0;
}

void(update_state_machine)(uint8_t tolerance) {

  switch (state) {

    case START:
      if (mouse_packet.lb && !mouse_packet.rb && !mouse_packet.mb) {
        state = UP;
      }
      break;

    case UP:
      if (mouse_packet.lb && !mouse_packet.rb && !mouse_packet.mb) {
        if ((mouse_packet.delta_y < (0 - tolerance)) || (mouse_packet.delta_x < (0 - tolerance))) {
          state = START;
          x_len_total = 0;
        }
      }
      else if (!mouse_packet.lb && !mouse_packet.rb && !mouse_packet.mb)
        state = VERTEX;
      else
        state = START;
      break;

    case VERTEX:
      if (mouse_packet.rb && !mouse_packet.lb && !mouse_packet.mb)
        state = DOWN;
      else
        state = START;
      break;

    case DOWN:
      if (mouse_packet.rb && !mouse_packet.lb && !mouse_packet.mb) {
        if ((mouse_packet.delta_y > (tolerance)) || (mouse_packet.delta_x < (0 - tolerance))) {
          state = START;
          x_len_total = 0;
        }
      }
      else if (!mouse_packet.rb && !mouse_packet.lb && !mouse_packet.mb && x_len_total >= x_len_)
        state = END;
      else
        state = START;
      break;

    case END:
      break;
  }

  if ((x_len_total + mouse_packet.delta_x > 0) && (x_len_total + mouse_packet.delta_x <= x_len_))
    x_len_total += mouse_packet.delta_x;
  else
    x_len_total = 0;
}

int(mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  int ipc_status, r;
  message msg;
  uint8_t irq_set;

  if (mouse_subscribe_int(&irq_set) != 0)
    return 1;

  if (mouse_enable_data_reporting() != 0)
    return 1;

  x_len_ = x_len;

  while (state != END) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0) {
      printf("Error receiving\n");
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            mouse_ih();
            r = mouse_packet_builder();

            if (r == 0)
              break;
            else if (r == 1)
              return 1;
            else if (r == 2) {
              mouse_print_packet(&mouse_packet);
              update_state_machine(tolerance);
            }
          }
          break;
      }
    }
  }

  if (mouse_disable_data_reporting() != 0)
    return 1;
  if (mouse_unsubscribe_int() != 0)
    return 1;

  return 0;
}

int(mouse_test_remote)(uint16_t period, uint8_t cnt) {
  /* This year you need not implement this. */
  printf("%s(%u, %u): under construction\n", __func__, period, cnt);
  return 1;
}
