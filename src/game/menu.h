#ifndef MENU_H
#define MENU_H

#include "game.h"
#include <lcom/lcf.h>

extern char *menu_xpm[];

/**
 * @brief Initializes the menu state and loads all menu resources.
 *
 * Loads the main menu background, option labels, and prepares the menu for display.
 * Resets the current and previous selection indices.
 *
 * @return 0 on success, 1 on failure (e.g., resource loading error).
 */
int menu_init(void);

/**
 * @brief Draws the menu on the screen.
 *
 * Renders the menu background, all menu options, and the selection indicator (ball)
 * next to the currently selected option.
 */
void menu_draw(void);

/**
 * @brief Handles keyboard input for the menu.
 *
 * Processes navigation keys (up/down) to change the selected option,
 * draws and clears the selection indicator as needed, and handles selection
 * (ENTER) or exit (ESC).
 *
 * @param scancode The scancode of the key pressed.
 * @return The next game state based on the input (e.g., STATE_PLAY, STATE_RULES, STATE_EXIT, or STATE_MENU).
 */
game_state_t menu_handle_input(uint16_t scancode);

/**
 * @brief Handles mouse input for the menu.
 *
 * (Currently not implemented.) Intended to process mouse events and update the menu state accordingly.
 */
void menu_handle_mouse(void);

/**
 * @brief Cleans up resources used by the menu.
 *
 * Frees loaded XPM pixmaps and clears the screen.
 */
void menu_cleanup(void);

#endif /* MENU_H */
