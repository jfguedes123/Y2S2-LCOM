#ifndef VIDEO_H
#define VIDEO_H

#define INDEXED_COLOR 4
#define DIRECT_COLOR 6

#include <lcom/lcf.h>
#include <stdint.h>

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

/**
 * @brief Draws a filled rectangle on the screen.
 *
 * Draws a rectangle of width @p w and height @p h, with the top-left corner at (@p x, @p y),
 * filled with the color @p c. The color is interpreted according to the current video mode.
 *
 * @param x The x-coordinate of the top-left corner of the rectangle.
 * @param y The y-coordinate of the top-left corner of the rectangle.
 * @param w The width of the rectangle in pixels.
 * @param h The height of the rectangle in pixels.
 * @param c The color to fill the rectangle with.
 */
void vg_rectangle(int x, int y, int w, int h, uint8_t c);

/**
 * @brief Draws an XPM image on the screen.
 *
 * Renders a pixmap (XPM image) at position (@p xi, @p yi) with the specified width and height.
 * The image data is provided as a linear array of color indices in @p pixmap.
 *
 * @param xi The x-coordinate of the top-left corner where the image will be drawn.
 * @param yi The y-coordinate of the top-left corner where the image will be drawn.
 * @param img_height The height of the image in pixels.
 * @param img_width The width of the image in pixels.
 * @param pixmap Pointer to the image pixel data (color indices).
 * @return 0 on success, non-zero on failure.
 */
int xpm_draw(uint16_t xi, uint16_t yi, uint16_t img_height, uint16_t img_width, uint8_t *pixmap);
#endif // VIDEO_H
