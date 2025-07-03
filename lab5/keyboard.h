#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>
#include "i8042.h"

#define DELAY_US 20000

/** @defgroup keyboard keyboard
 * @{
 *
 */

 int (kbd_subscribe_int)(uint8_t *bit_no);
 int (kbd_unsubscribe_int)();
 void (kbc_ih)();


/**@}*/

#endif /* __KEYBOARD_H */
