// state.h
#ifndef STATE_H
#define STATE_H

#include <lcom/lcf.h>

/**
 * @brief Enumeration of possible game states.
 */
typedef enum {
  STATE_MENU,   /**< Main menu state */
  STATE_RULES,  /**< Rules screen state */
  STATE_PLAY,   /**< Gameplay state */
  STATE_EXIT    /**< Exit state */
} game_state_t;

/**
 * @brief Structure defining the interface for a game state driver.
 */
typedef struct {
  int (*init)(void);                        /**< Initializes the state. */
  game_state_t (*handle_input)(uint16_t sc);/**< Handles input and returns the next state. */
  void (*handle_mouse)(void);               /**< Handles mouse input */
  void (*update)(void);                     /**< Updates the state logic. */
  void (*draw)(void);                       /**< Draws the state visuals. */
  void (*cleanup)(void);                    /**< Cleans up resources used by the state. */
} state_driver_t;

/**
 * @brief Gets the driver for a given game state.
 * 
 * @param state The game state.
 * @return Pointer to the corresponding state driver, or NULL if not available.
 */
const state_driver_t *get_state_driver(game_state_t state);

#endif /* STATE_H */
