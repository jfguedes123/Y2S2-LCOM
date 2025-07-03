#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>
#include "controllers/i8042.h"

/** @defgroup keyboard keyboard
 * @{
 *
 * Functions for using the i8042 keyboard
 */

/**
 * @brief Subscribes keyboard interrupts.
 *
 * @param bit_no Address of memory to be initialized with the bit number to be set in the mask returned upon an interrupt.
 * @return 0 on success, 1 on failure.
 */
int (kbd_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes keyboard interrupts.
 *
 * @return 0 on success, 1 on failure.
 */
int (kbd_unsubscribe_int)();

/**
 * @brief Keyboard interrupt handler. Reads scancode from the output buffer.
 */
void (kbc_ih)();

/**
 * @brief Checks if a full scancode is ready to be processed.
 *
 * @return true if a full scancode is ready, false otherwise.
 */
bool scancode_ready(void);

/**
 * @brief Gets the last full scancode read from the keyboard.
 *
 * @return The last full scancode (1 or 2 bytes).
 */
uint16_t get_full_scancode(void);

/**@}*/

#endif /* __KEYBOARD_H */
