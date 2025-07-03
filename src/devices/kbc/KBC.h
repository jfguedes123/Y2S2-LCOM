#ifndef __KBC_H
#define __KBC_H

#include "controllers/i8042.h"
#include <stdbool.h>
#include <stdint.h>
#include <lcom/lcf.h>

/** @defgroup KBC KBC
 * @{
 *
 * Functions for KBC
 */


/**
 * @brief Reads a byte from the KBC output buffer.
 *
 * Tries to read a byte from the specified port (usually KBC_OUT_BUF).
 * Checks for parity and timeout errors. Retries up to 10 times if the output buffer is not ready.
 *
 * @param port Port to read from (e.g., KBC_OUT_BUF).
 * @param output Pointer to variable where the read byte will be stored.
 * @return 0 on success, 1 on failure (e.g., error or timeout).
 */
int(kbc_read_out)(uint8_t port, uint8_t *output);
/**
 * @brief Writes a command byte to the specified KBC port.
 *
 * Tries to write a command byte to the given port (usually KBC_IN_BUF).
 * Retries up to 10 times if the input buffer is not ready.
 *
 * @param port Port to write to (e.g., KBC_IN_BUF).
 * @param commandByte Command byte to write.
 * @return 0 on success, 1 on failure (e.g., error or timeout).
 */
int(kbc_write_cmd)(uint8_t port, uint8_t commandByte);

/**
 * @brief Restores the keyboard to its default state.
 *
 * Reads the current command byte, sets the interrupt enable bit, and writes it back.
 *
 * @return 0 on success, 1 on failure.
 */
int(restore_kbd)();

/**@}*/

#endif /* __KBC_H */
