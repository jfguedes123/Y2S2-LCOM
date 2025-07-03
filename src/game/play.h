#ifndef PLAY_H
#define PLAY_H

#include "state.h"
#include <stdint.h>

/**
 * @brief Initializes the play state, game objects, and resources.
 *
 * Loads digit and win images, resets scores and positions, initializes paddles and ball,
 * and prepares the game for play. Also sets up the ball respawn timer.
 *
 * @return 0 on success, non-zero on failure (e.g., resource loading error).
 */
int play_init(void);

/**
 * @brief Handles keyboard input for the play state.
 *
 * Processes the given scan code, updates the first player's paddle movement,
 * and handles state transitions (e.g., returning to menu on ESC).
 *
 * @param sc Keyboard scan code.
 * @return The next game state (e.g., STATE_PLAY or STATE_MENU).
 */
game_state_t play_handle_input(uint16_t sc);

/**
 * @brief Handles mouse input for the play state.
 *
 * Updates the second player's paddle movement based on mouse button states
 * (left button moves up, right button moves down, none stops movement).
 */
void play_handle_mouse(void);

/**
 * @brief Updates the game logic for the play state.
 *
 * Updates previous positions, moves paddles and ball, checks for collisions,
 * updates scores, handles ball respawn timer, and manages game-over logic.
 * If the game is over, no further updates are performed.
 */
void play_update(void);

/**
 * @brief Draws the current frame for the play state.
 *
 * Clears previous paddle and ball positions, draws the current score,
 * win screens if the game is over, the center dotted line, paddles, and ball.
 */
void play_draw(void);

/**
 * @brief Cleans up the play state.
 *
 * Resets game-over state, winner, and clears the screen.
 * Prepares for a new game or state transition.
 */
void play_cleanup(void);

/**
 * @brief State driver for the play state.
 *
 * Provides function pointers for state management (init, input, mouse, update, draw, cleanup).
 */
extern const state_driver_t play_driver;

#endif /* PLAY_H */
