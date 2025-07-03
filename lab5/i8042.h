#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/** @defgroup i8042 i8042
 * @{
 *
 */


#define KEYBOARD_IRQ 1

#define KBC_ST_IBF 0x02 
#define KBC_IN_BUF 0X60
#define KBC_WRITE_CMD   0x60
#define KBC_OUT_BUF 0x60
#define KBC_STATUS_REG 0x64
#define KBC_CMD_REG 0x64
#define ESC_BREAK_CODE 0x81
#define TWO_BYTES     0xE0
 
#define OUT_BUFFER BIT(0) 
#define IN_BUFFER  BIT(1) 
#define MAKE_CODE  BIT(7)
#define ERRORS     (BIT(6) | BIT(7)) 

/**@}*/

#endif /* _LCOM_I8042_H */
