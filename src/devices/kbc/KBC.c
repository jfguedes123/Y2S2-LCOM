#include "KBC.h"

int(kbc_read_out)(uint8_t port, uint8_t *output) {
  uint8_t status;
  uint8_t attempts = 10;

  while (attempts) {
    if (util_sys_inb(KBC_STATUS_REG, &status) != 0)
      return 1;

    if ((status & OUT_BUFFER) != 0) {
      if (util_sys_inb(port, output) != 0)
        return 1;
      if ((status & TIMEOUT_ERROR) != 0)
        return 1;
      if ((status & PARITY_ERROR) != 0)
        return 1;
      return 0;
    }

    tickdelay(micros_to_ticks(20000));
    attempts--;
  }
  return 1;
}

int(kbc_write_cmd)(uint8_t port, uint8_t commandByte) {
  uint8_t status;
  uint8_t attempts = 10;

  while (attempts) {
    if (util_sys_inb(KBC_STATUS_REG, &status) != 0)
      return 1;

    if ((status & IN_BUFFER) == 0) {
      if (sys_outb(port, commandByte) != 0)
        return 1;
      return 0;
    }
    tickdelay(micros_to_ticks(20000));
    attempts--;
  }

  return 1;
}

int(restore_kbd)() {
  uint8_t commandByte;

  if (kbc_write_cmd(KBC_IN_BUF, KBC_READ_CMD) != 0)
    return 1;
  if (kbc_read_out(KBC_OUT_BUF, &commandByte) != 0)
    return 1;

  commandByte |= BIT(0);

  if (kbc_write_cmd(KBC_IN_BUF, KBC_WRITE_CMD) != 0)
    return 1;
  if (kbc_write_cmd(KBC_WRITE_CMD, commandByte) != 0)
    return 1;

  return 0;
}
