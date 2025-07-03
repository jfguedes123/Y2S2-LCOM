#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the i8042 Keyboard Controller.
 */

/* IRQ Line */
#define KEYBOARD_IRQ    1
#define MOUSE_IRQ       12

/* KBC I/O Ports */
#define KBC_STATUS_REG  0x64    // Status register (read)
#define KBC_IN_BUF      0x64    // Input buffer (write)
#define KBC_OUT_BUF     0x60    // Output buffer (read)

/* KBC Commands */
#define KBC_READ_CMD    0x20    // Read command byte
#define KBC_WRITE_CMD   0x60    // Write command byte

/* Special Key Codes */
#define TWO_BYTES       0xE0    // Prefix for two-byte scancodes
#define ESC_BREAKCODE   0x81    // Break code for ESC key

/* Status Register Flags */
#define OUT_BUFFER      BIT(0)  // Output buffer full
#define IN_BUFFER       BIT(1)  // Input buffer full
#define MAKE_CODE       BIT(7)  // Distinguish make/break codes

/* Error Flags */
#define TIMEOUT_ERROR   BIT(6)  // Timeout error
#define PARITY_ERROR    BIT(7)  // Parity error
#define ERRORS          (TIMEOUT_ERROR | PARITY_ERROR)  // Combined error mask

/* Mouse Commands */
#define KBC_WRITE_MOUSE  0xD4  // Command to write to mouse
#define MOUSE_ACK        0xFA  // Acknowledgment byte
#define MOUSE_NACK       0xFE  // Negative acknowledgment
#define MOUSE_ERROR      0xFC  // Error
#define MOUSE_ENABLE     0xF4  // Enable data reporting
#define MOUSE_DISABLE    0xF5  // Disable data reporting

/* Mouse Buttons */
#define MOUSE_LB           BIT(0)
#define MOUSE_RB           BIT(1)
#define MOUSE_MB           BIT(2)
#define MOUSE_X_OV         BIT(6)
#define MOUSE_Y_OV         BIT(7)
#define MOUSE_X_SIGN       BIT(4)
#define MOUSE_Y_SIGN       BIT(5)
#define MOUSE_X_SIGN_MASK  0xFF00
#define MOUSE_Y_SIGN_MASK  0xFF00

/**@}*/

#endif /* _LCOM_I8042_H_ */
