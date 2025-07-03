#ifndef __KBC_H
#define __KBC_H

#include "i8042.h"
#include <stdbool.h>
#include <stdint.h>
#include <lcom/lcf.h>

/** @defgroup KBC KBC
 * @{
 *
 * Functions for KBC
 */

int(kbc_read_out)(uint8_t port, uint8_t *output);
int(kbc_write_cmd)(uint8_t port, uint8_t commandByte);
int(restore_kbd)();

/**@}*/

#endif /* __KBC_H */
