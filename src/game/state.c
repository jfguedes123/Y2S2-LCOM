#include "game.h"

extern const state_driver_t menu_driver;
extern const state_driver_t rules_driver;

const state_driver_t *get_state_driver(game_state_t s) {
  switch (s) {
    case STATE_MENU: return &menu_driver;
    case STATE_RULES: return &rules_driver;
    case STATE_PLAY: return &play_driver;
    case STATE_EXIT: return NULL;
    default: return NULL;
  }
}
