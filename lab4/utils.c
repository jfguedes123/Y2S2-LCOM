#include <lcom/lcf.h>

#include <stdint.h>

uint32_t counter_KBC = 0;


int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (lsb == NULL) 
    return 1;

  *lsb = (uint8_t) val & 0xFF;

  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if (msb == NULL) 
    return 1;

  *msb = (uint8_t) val >> 8;

  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  #ifdef LAB3
    counter_KBC++;
  #endif
  if (value == NULL) 
    return 1;
  
  uint32_t val;

  int ret = sys_inb(port, &val);
  if (ret != OK) 
    return ret;

  *value = (uint8_t) (0xFF & val);

  return 0;
}
