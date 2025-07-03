#include "digits.h"
#include "game.h"
#include "xpm_files/player1win.xpm"
#include "xpm_files/player2win.xpm"

#define SCREEN_W 1024
#define SCREEN_H 768
#define COLOR_PADDLE1 1
#define COLOR_PADDLE2 4
#define COLOR_BG 0
#define COLOR_BALL 63
#define PADDLE_SPEED 5
#define MAX_DY 5
#define MAX_SCORE 10

static xpm_image_t digit_imgs[10];
static uint8_t *digit_pix[10];

static xpm_image_t win1_img, win2_img;
static uint8_t *win1_pix = NULL, *win2_pix = NULL;

// Game end variable (True if one player reaches 10 points)
static bool game_over = false;

// Winning player (0 if game is not over)
static int winner = 0;

// Previous ball and paddles position, used for a specific collision check
static int prev_ball_x, prev_ball_y;
static int prev_p1_y, prev_p2_y;

// Game objects (Paddles and Ball)
typedef struct {
  int x, y, w, h, dy;
} Paddle;
typedef struct {
  int x, y, dx, dy, size;
} Ball;

static Paddle p1, p2;
static Ball ball;

// Point counters (Game ends once one of them is equal to 10)
static int score1, score2;

// Ball respawn timer (Giving players time before the next play)
static int respawn_ticks = 0;

static void reset_ball(void) {
  // Place ball at the center of the screen
  ball.x = (SCREEN_W - ball.size) / 2;
  ball.y = (SCREEN_H - ball.size) / 2;
  // Ball moves to the side of whoever lost the play and moves down
  ball.dx = (ball.dx < 0 ? -2 : 2);
  ball.dy = 2;
}

// Methods to draw game objects
static void draw_paddle(const Paddle *p, uint8_t color) {
  vg_rectangle(p->x, p->y, p->w, p->h, color);
}
static void clear_paddle(const Paddle *p, int prev_y) {
  vg_rectangle(p->x, prev_y, p->w, p->h, COLOR_BG);
}

static void draw_ball(const Ball *b) {
  vg_rectangle(b->x, b->y, b->size, b->size, COLOR_BALL);
}
static void clear_ball(const Ball *b, int prev_x, int prev_y) {
  vg_rectangle(prev_x, prev_y, b->size, b->size, COLOR_BG);
}

static void init_objects(void) {
  p1 = (Paddle) {.x = 20, .y = SCREEN_H / 2 - 50, .w = 20, .h = 100, .dy = 0};
  p2 = (Paddle) {.x = SCREEN_W - 40, .y = SCREEN_H / 2 - 50, .w = 20, .h = 100, .dy = 0};
  ball = (Ball) {.x = (SCREEN_W - 20) / 2, .y = (SCREEN_H - 20) / 2, .dx = -2, .dy = 2, .size = 20};
  score1 = score2 = 0;
}

static void handle_keyboard(uint16_t sc, Paddle *p) {
  switch (sc) {
    case UP_MAKECODE:
      p->dy = -PADDLE_SPEED;
      break;
    case DOWN_MAKECODE:
      p->dy = PADDLE_SPEED;
      break;
    case UP_BREAKCODE:
    case DOWN_BREAKCODE:
      p->dy = 0;
      break;
    default:
      return;
  }
}

// Load xpm files used for tracking score
int load_digits(void) {
  for (int d = 0; d < 10; d++) {
    digit_pix[d] = xpm_load(number_xpms[d], XPM_INDEXED, &digit_imgs[d]);
    if (!digit_pix[d])
      return 1;
  }
  return 0;
}

int play_init(void) {
  init_objects();
  // Ball will wait 2 seconds before moving
  respawn_ticks = 2 * sys_hz();
  if (load_digits() != 0)
    return 1;
  win1_pix = xpm_load((xpm_map_t) player1_win_xpm, XPM_INDEXED, &win1_img);
  win2_pix = xpm_load((xpm_map_t) player2_win_xpm, XPM_INDEXED, &win2_img);
  if (!win1_pix || !win2_pix)
    return 1;
  return 0;
}

game_state_t play_handle_input(uint16_t sc) {
  if (sc == ESC_BREAKCODE)
    return STATE_MENU; // Return to main menu
  handle_keyboard(sc, &p1);
  return STATE_PLAY;
}

void p1_update() {
  //clear_paddle(&p1);
  p1.y += p1.dy;
  // Limits player movement ensuring that they
  // can't push the ball off screen causing a crash
  if (p1.y < ball.size)
    p1.y = ball.size;
  if (p1.y + p1.h > SCREEN_H - ball.size)
    p1.y = SCREEN_H - p1.h - ball.size;
  //draw_paddle(&p1, COLOR_PADDLE1);
}

void p2_update() {
  //clear_paddle(&p2);
  p2.y += p2.dy;
  // Limits player movement ensuring that they
  // can't push the ball off screen causing a crash
  if (p2.y < ball.size)
    p2.y = ball.size;
  if (p2.y + p2.h > SCREEN_H - ball.size)
    p2.y = SCREEN_H - p2.h - ball.size;
  //draw_paddle(&p2, COLOR_PADDLE2);
}

void play_handle_mouse() {
  if (mouse_packet.lb) p2.dy = -PADDLE_SPEED;
  else if (mouse_packet.rb) p2.dy = PADDLE_SPEED;
  else p2.dy = 0;
}

void move_ball() {
  ball.x += ball.dx;
  ball.y += ball.dy;
}

void ball_edge_collision_check() {
  if (ball.y < 0) { // Hit top of the screen
    ball.y = 0;
    ball.dy = -ball.dy;
  }
  else if (ball.y + ball.size > SCREEN_H) { // Hit bottom of the screen
    ball.y = SCREEN_H - ball.size;
    ball.dy = -ball.dy;
  }
}

void ball_p1_collision_check() {
  if (ball.x <= p1.x + p1.w &&
      ball.y + ball.size > p1.y &&
      ball.y < p1.y + p1.h) {
      // Ball's x position is such that is past the player's x position or equal to it and 
      // ball's y position is such that is both lower than the player's highest pixel's y position 
      // and higher than the player's lowest pixel's y position
    if (prev_ball_x > p1.x + p1.w) { // Direct collision (Hit right side of the paddle)
      ball.x = p1.x + p1.w;
      ball.dx = -ball.dx;
      ball.dy += p1.dy / 2; // Add some of the player's velocity to the ball (spin effect)
      if (ball.dy > MAX_DY) ball.dy = MAX_DY;
      if (ball.dy < -MAX_DY) ball.dy = -MAX_DY;
      if (ball.dy == 0) ball.dy = 1; // Stop perfectly horizontal shots from happening
      ball.dx = ball.dx + 1; // Increase ball's x velocity ensuring the game gets harder as it progresses
    }
    else if (prev_ball_y + ball.size <= prev_p1_y) { // Top collision (Hit top edge of the paddle, let ball pass)
      ball.y = p1.y - ball.size;
      ball.dy = -ball.dy;
    }
    else if (prev_ball_y >= prev_p1_y + p1.h) {  // Bottom collision (Hit bottom edge of the paddle, let ball pass)
      ball.y = p1.y + p1.h;
      ball.dy = -ball.dy;
    }
  }
}

void ball_p2_collision_check() {
  if (ball.x + ball.size >= p2.x &&
      ball.y + ball.size > p2.y &&
      ball.y < p2.y + p2.h) { 
      // Ball's x position is such that is past the player's x position or equal to it and 
      // ball's y position is such that is both lower than the player's highest pixel's y position 
      // and higher than the player's lowest pixel's y position
    if (prev_ball_x + ball.size < p2.x) { // Direct collision (Hit left side of the paddle)
      ball.x = p2.x - ball.size;
      ball.dx = -ball.dx;
      ball.dy += p2.dy / 2; // Add some of the player's velocity to the ball (spin effect)
      if (ball.dy > MAX_DY) ball.dy = MAX_DY;
      if (ball.dy < -MAX_DY) ball.dy = -MAX_DY;
      if (ball.dy == 0) ball.dy = 1;  // Stop perfectly horizontal shots from happening
      ball.dx = ball.dx-1; // Increase ball's x velocity ensuring the game gets harder as it progresses
    }
    else if (prev_ball_y + ball.size <= prev_p2_y) { // Top collision (Hit top edge of the paddle, let ball pass)
      ball.y = p2.y - ball.size;
      ball.dy = -ball.dy;
    }
    else if (prev_ball_y >= prev_p2_y + p2.h) { // Bottom collision (Hit bottom edge of the paddle, let ball pass)
      ball.y = p2.y + p2.h;
      ball.dy = -ball.dy;
    }
  }
}

void p1_point_check() {
  if (ball.x + ball.size > SCREEN_W) { // Ball got past player 2
    score1++;
    if (score1 >= MAX_SCORE) { // Check if that was the 10th point
      game_over = true;
      winner = 1;
    }
    else { // If it wasn't prepare next play
      respawn_ticks = 2 * sys_hz();
      reset_ball();
    }
  }
}

void p2_point_check() {
  if (ball.x < 0) { // Ball got past player 1
    score2++;
    if (score2 >= MAX_SCORE) { // Check if that was the 10th point
      game_over = true;
      winner = 2;
    }
    else { // If it wasn't prepare next play
      respawn_ticks = 2 * sys_hz();
      reset_ball();
    }
  }
}

// Update previous positions
void prev_ball_position() {
  prev_ball_x = ball.x;
  prev_ball_y = ball.y;
}

void prev_player_position() {
  prev_p1_y = p1.y;
  prev_p2_y = p2.y;
}

void play_update(void) {
  if (game_over) return;

  prev_player_position();
  prev_ball_position();
  p1_update();
  p2_update();
  //clear_ball(&ball);

  if (respawn_ticks > 0) {
    // A player scored less than 2 seconds ago, wait
    respawn_ticks--;
    return;
  }

  move_ball();
  ball_edge_collision_check();
  ball_p1_collision_check();
  ball_p2_collision_check();
  p1_point_check();
  p2_point_check();

  //draw_ball(&ball);
}

void draw_score(int score, int sx, int sy) {
  int tens = (score / 10) % 10;
  int units = score % 10;

  xpm_draw(sx, sy,
           digit_imgs[tens].height,
           digit_imgs[tens].width,
           digit_pix[tens]);

  xpm_draw(sx + digit_imgs[0].width + 4,
           sy,
           digit_imgs[units].height,
           digit_imgs[units].width,
           digit_pix[units]);
}

static void draw_vertical_dotted_line(void) {
  int x = SCREEN_W / 2;
  for (int y = 0; y < SCREEN_H; y += 10) {
    vg_rectangle(x, y, 2, 5, COLOR_BALL);
  }
}

void play_draw(void) {

  clear_paddle(&p1,prev_p1_y);
  clear_paddle(&p2,prev_p2_y);
  clear_ball(&ball,prev_ball_x, prev_ball_y);

  int score_offset = 30; 
  int digit_width = digit_imgs[0].width;
  int digit_gap = 4;
  int score_y = 20;

  draw_score(score1, (SCREEN_W / 2) - score_offset - 2 * (digit_width + digit_gap), score_y);
  draw_score(score2, (SCREEN_W / 2) + score_offset, score_y);

  if (game_over) {
    vg_rectangle(500,0,24,768,0);
    if (winner == 1) {
      xpm_draw(234, 334, win1_img.height, win1_img.width, win1_pix);
    }
    else {
      xpm_draw(234, 334, win2_img.height, win2_img.width, win2_pix);
    }
    return;
  }

  draw_vertical_dotted_line();

  draw_paddle(&p1, COLOR_PADDLE1);
  draw_paddle(&p2, COLOR_PADDLE2);
  draw_ball(&ball);
}

void play_cleanup(void) {
  // Ensuring player can transition to menu 
  // and back to the game into a new game
  game_over = false;
  winner = 0;
  vg_rectangle(0, 0, SCREEN_W, SCREEN_H, 0);
}

const state_driver_t play_driver = {
  .init = play_init,
  .handle_input = play_handle_input,
  .handle_mouse = play_handle_mouse,
  .update = play_update,
  .draw = play_draw,
  .cleanup = play_cleanup};
