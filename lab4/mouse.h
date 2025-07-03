#ifndef __MOUSE_H
#define __MOUSE_H

#include <stdbool.h>
#include <stdint.h>

/** @defgroup mouse mouse
 * @{
 *
 * Functions for using the mouse
 */

int (mouse_subscribe_int)(uint8_t *bit_no);
int (mouse_unsubscribe_int)();
void (mouse_ih)();
int (mouse_packet_builder)();
int (mouse_write_cmd)(uint8_t cmd);
int (mouse_disable_data_reporting)();

/**@}*/

#endif /* __MOUSE_H */
