#include "game.h"
#include "../xpm_files/rules_menu.xpm"

static xpm_image_t rules_menu_xpm_img;
static uint8_t *rules_menu_pixmap = NULL;

int rules_init(void) {
  rules_menu_pixmap = xpm_load((xpm_map_t) rules_menu_xpm, XPM_INDEXED, &rules_menu_xpm_img);
  if (!rules_menu_pixmap)
    return 1;
  return 0;
}

game_state_t rules_handle_input(uint16_t sc) {
  if (sc == ESC_BREAKCODE)
    return STATE_MENU;
  return STATE_RULES;
}

void rules_draw(void) {
  xpm_draw(0, 0, rules_menu_xpm_img.height, rules_menu_xpm_img.width, rules_menu_pixmap);
}

void rules_handle_mouse(void) {

}

void rules_cleanup(void) {
  if (rules_menu_pixmap) {
    free(rules_menu_pixmap);
    rules_menu_pixmap = NULL;
  }
  vg_rectangle(0, 0, SCREEN_W, SCREEN_H, 0); // Clear screen
}

const state_driver_t rules_driver = {
  .init = rules_init,
  .handle_input = rules_handle_input,
  .handle_mouse = rules_handle_mouse,
  .update = NULL,
  .draw = rules_draw,
  .cleanup = rules_cleanup};
