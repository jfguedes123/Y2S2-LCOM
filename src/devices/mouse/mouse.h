#ifndef __MOUSE_H
#define __MOUSE_H

#include <stdbool.h>
#include <stdint.h>

extern struct packet mouse_packet;

/** @defgroup mouse mouse
 * @{
 *
 * Functions for using the mouse
 */

/**
 * @brief Subscribes mouse interrupts.
 *
 * @param bit_no Address of memory to be initialized with the bit number to be set in the mask returned upon an interrupt.
 * @return 0 on success, 1 on failure.
 */
int (mouse_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes mouse interrupts.
 *
 * @return 0 on success, 1 on failure.
 */
int (mouse_unsubscribe_int)();

/**
 * @brief Mouse interrupt handler. Reads a byte from the mouse output buffer.
 */
void (mouse_ih)();

/**
 * @brief Builds a mouse packet from the bytes read.
 *
 * @return 2 when a full packet is ready, 1 if the first byte is invalid, 0 otherwise.
 */
int (mouse_packet_builder)();

/**
 * @brief Writes a command to the mouse.
 *
 * @param cmd Command byte to send to the mouse.
 * @return 0 on success, 1 on failure.
 */
int (mouse_write_cmd)(uint8_t cmd);

/**
 * @brief Disables mouse data reporting.
 *
 * @return 0 on success, 1 on failure.
 */
int (mouse_disable_data_reporting)();

/**
 * @brief Enables mouse data reporting (alternative method).
 *
 * @return 0 on success, 1 on failure.
 */
int (mouse_enable_data_reporting_alt)();

/**@}*/

#endif /* __MOUSE_H */
