#include "xpm_files/menu.xpm"
#include "game.h"
#include "xpm_files/exit.xpm"
#include "xpm_files/play_text.xpm"
#include "xpm_files/rules.xpm"

#define N_OPTS 3
#define BASE_Y 465
#define SPACING 50
#define BALL_X 417

static xpm_image_t menu_xpm_img;
static uint8_t *menu_pixmap = NULL;

static xpm_image_t play_text_xpm_img;
static uint8_t *play_text_pixmap = NULL;

static xpm_image_t rules_xpm_img;
static uint8_t *rules_pixmap = NULL;

static xpm_image_t exit_xpm_img;
static uint8_t *exit_pixmap = NULL;

static const game_state_t opt_states[] = {
  STATE_PLAY,
  STATE_RULES,
  STATE_EXIT};

// Currently selected option
static int cur = 0;

// Previously selected option (To be cleared once a new option is selected)
static int prev_cur = 0;

static void menu_draw_ball(void) {
  int y_new = BASE_Y + cur * SPACING;
  // Draws ball next to current selection
  vg_rectangle(BALL_X, y_new, 20, 20, 63);
}

void menu_draw(void) {
  xpm_draw(0, 0, menu_xpm_img.height, menu_xpm_img.width, menu_pixmap);
  xpm_draw(452, 450, play_text_xpm_img.height, play_text_xpm_img.width, play_text_pixmap);
  xpm_draw(452, 500, rules_xpm_img.height, rules_xpm_img.width, rules_pixmap);
  xpm_draw(452, 550, exit_xpm_img.height, exit_xpm_img.width, exit_pixmap);
  menu_draw_ball();
}

static void menu_clear_ball(void) {
  int y_old = BASE_Y + prev_cur * SPACING;
  // Draws black rectangle next to previous selection
  // in order to delete the previous ball
  vg_rectangle(BALL_X, y_old, 20, 20, 0);
}

int menu_init(void) {
  cur = 0;
  prev_cur = 0;
  menu_pixmap = xpm_load((xpm_map_t) menu_xpm, XPM_INDEXED, &menu_xpm_img);
  if (!menu_pixmap)
    return 1;
  play_text_pixmap = xpm_load((xpm_map_t) play_text_xpm, XPM_INDEXED, &play_text_xpm_img);
  if (!play_text_pixmap)
    return 1;
  rules_pixmap = xpm_load((xpm_map_t) rules_xpm, XPM_INDEXED, &rules_xpm_img);
  if (!rules_pixmap)
    return 1;
  exit_pixmap = xpm_load((xpm_map_t) exit_xpm, XPM_INDEXED, &exit_xpm_img);
  if (!exit_pixmap)
    return 1;
  return 0;
}

game_state_t menu_handle_input(uint16_t sc) {
  switch (sc) {
    case UP_MAKECODE:
      menu_clear_ball();
      prev_cur = cur;
      cur = (cur + N_OPTS - 1) % N_OPTS;
      menu_draw_ball();
      prev_cur = cur;
      break;
    case DOWN_MAKECODE:
      menu_clear_ball();
      prev_cur = cur;
      cur = (cur + 1) % N_OPTS;
      menu_draw_ball();
      prev_cur = cur;
      break;
    case ENTER_MAKECODE:
      // returns a game state (play, rules or exit)
      return opt_states[cur];
    case ESC_BREAKCODE:
      return STATE_EXIT;
    default:
      break;
  }
  //printf("%d", cur);
  return STATE_MENU;
}

void menu_cleanup(void) {
  if (menu_pixmap) {
    free(menu_pixmap);
    menu_pixmap = NULL;
  }
  if (play_text_pixmap) {
    free(play_text_pixmap);
    play_text_pixmap = NULL;
  }
  if (rules_pixmap) {
    free(rules_pixmap);
    rules_pixmap = NULL;
  }
  vg_rectangle(0, 0, SCREEN_W, SCREEN_H, 0); // Clear screen
}

void menu_handle_mouse(void) {

}

const state_driver_t menu_driver = {
  .init = menu_init,
  .handle_input = menu_handle_input,
  .handle_mouse = menu_handle_mouse,
  .update = NULL,
  .draw = menu_draw,
  .cleanup = menu_cleanup};
