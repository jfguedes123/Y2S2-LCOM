#ifndef RULES_H
#define RULES_H

#include "state.h"

/**
 * @brief Initializes the rules state and loads resources.
 *
 * Loads the rules screen XPM image and prepares it for display.
 *
 * @return 0 on success, 1 on failure (e.g., resource loading error).
 */
int rules_init(void);

/**
 * @brief Handles keyboard input for the rules state.
 *
 * If ESC is pressed, returns to the main menu. Otherwise, stays in the rules state.
 *
 * @param scancode The scancode of the key pressed.
 * @return The next game state based on the input (STATE_MENU or STATE_RULES).
 */
game_state_t rules_handle_input(uint16_t scancode);

/**
 * @brief Handles mouse input for the rules state.
 *
 * (Currently not implemented.) Intended to process mouse events and update the rules state accordingly.
 */
void rules_handle_mouse(void);

/**
 * @brief Draws the rules screen.
 *
 * Renders the rules XPM image to the screen.
 */
void rules_draw(void);

/**
 * @brief Cleans up resources used by the rules state.
 *
 * Frees the loaded XPM pixmap and clears the screen.
 */
void rules_cleanup(void);

/**
 * @brief State driver for the rules state.
 *
 * Provides function pointers for state management (init, input, mouse, draw, cleanup).
 */
extern const state_driver_t rules_driver;

#endif /* RULES_H */
