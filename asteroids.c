#include <math.h>
#include <riv.h> // MUST BE THE FIRST LINE
#include <stdio.h>

struct SpaceObject {
  int size;
  int x;
  int y;
  float dx;
  float dy;
  float angle;
  bool exists;
};

void draw_ship(struct SpaceObject ship) {
  riv_draw_circle_fill(ship.x, ship.y, ship.size, RIV_COLOR_WHITE);
  riv_draw_point(ship.x + ship.size * sin(ship.angle),
                 ship.y - ship.size * cos(ship.angle), RIV_COLOR_RED);
}

void wrap_around(struct SpaceObject *o) {
  if (o->x < 0 - o->size)
    o->x = 256 + o->size;
  if (o->x > 256 + o->size)
    o->x = 0 - o->size;
  if (o->y < 0 - o->size)
    o->y = 256 + o->size;
  if (o->y > 256 + o->size)
    o->y = 0 - o->size;
}

float distance(int x, int y, int xo, int yo) {
  return sqrt((x - xo) * (x - xo) + (y - yo) * (y - yo));
}
int main() { // entry point
  struct SpaceObject ship = {10, 128, 128, 0, 0, 0, true};
  struct SpaceObject asteroids[100];
  struct SpaceObject bullets[100];
  int n_bullets = 0;

  // struct SpaceObject asteroid ={10, 128, 128, 1, 2, 0, true};
  int n_asteroids = 0;
  for (int i = 0; i < 5; i++) {
    struct SpaceObject asteroid = {10 + riv_rand_uint(30),
                                   riv_rand_uint(255),
                                   riv_rand_uint(255),
                                   riv_rand_int(-2, 2),
                                   riv_rand_int(-2, 2),
                                   riv_rand_float() * 3.14 * 2,
                                   true};
    asteroids[n_asteroids] = asteroid;
    n_asteroids += 1;
  }

  int score = 0;

  do { // main loop
    // handle inputs
    if (riv->keys[RIV_GAMEPAD_LEFT].down)
      ship.angle += 0.1;
    if (riv->keys[RIV_GAMEPAD_RIGHT].down)
      ship.angle -= 0.1;
    float thrust = 0;
    if (riv->keys[RIV_GAMEPAD_UP].down) {
      thrust = 0.3;
      ship.dx += sin(ship.angle) * thrust;
      ship.dy += -cos(ship.angle) * thrust;
    }

    ship.x += ship.dx;
    ship.y += ship.dy;

    ship.dx *= 0.999;
    ship.dy *= 0.999;

    wrap_around(&ship);


    if (riv->keys[RIV_GAMEPAD_A1].press) {
      struct SpaceObject bullet = {
          2,          ship.x, ship.y, 5 * sin(ship.angle), -5 * cos(ship.angle),
          ship.angle, true};
      bullets[n_bullets] = bullet;
      n_bullets += 1;
    }

    // draw screen
    riv_clear(RIV_COLOR_BLACK); // clear screen
    draw_ship(ship);

    for (int i = 0; i < n_asteroids; i++) {
      asteroids[i].x += asteroids[i].dx;
      asteroids[i].y += asteroids[i].dy;
      wrap_around(&asteroids[i]);
      if (distance(asteroids[i].x, asteroids[i].y, ship.x, ship.y) <
          ship.size / 2 + asteroids[i].size / 2) {
        riv_draw_circle_line(asteroids[i].x, asteroids[i].y, asteroids[i].size,
                             RIV_COLOR_RED);
        score = 0;
      } else {
        riv_draw_circle_line(asteroids[i].x, asteroids[i].y, asteroids[i].size,
                             RIV_COLOR_WHITE);
      }
    }
    for (int i = 0; i < n_bullets; i++) {
      bullets[i].x += bullets[i].dx;
      bullets[i].y += bullets[i].dy;
      if (bullets[i].x < 0)
        bullets[i].exists = false;
      if (bullets[i].x > 256)
        bullets[i].exists = false;
      if (bullets[i].y < 0)
        bullets[i].exists = false;
      if (bullets[i].y > 256)
        bullets[i].exists = false;
      riv_draw_point(bullets[i].x, bullets[i].y, RIV_COLOR_WHITE);
    }

    for (int i = 0; i < n_bullets; i++) {
      for (int j = 0; j < n_asteroids; j++) {
        float d = distance(bullets[i].x, bullets[i].y, asteroids[j].x,
                           asteroids[j].y);
        if (d < asteroids[j].size / 2) {
          asteroids[j].exists = false;
          bullets[i].exists = false;
          bullets[i].y = -1000;
          score += 100;
          if (asteroids[j].size > 10) {
            for (int i = 0; i <= 2; i++) {
              struct SpaceObject asteroid = {2 + asteroids[j].size / 2,
                                             asteroids[j].x,
                                             asteroids[j].y,
                                             riv_rand_int(-2, 2),
                                             riv_rand_int(-2, 2),
                                             riv_rand_float() * 3.14 * 2,
                                             true};
              asteroids[n_asteroids] = asteroid;
              n_asteroids += 1;
            }
          }
        }
      }
    }
    char buf[128];
    riv_snprintf(buf, sizeof(buf), "SCORE %d", score);
    riv_draw_text(buf, RIV_SPRITESHEET_FONT_3X5, RIV_BOTTOMLEFT, 1, 10, 1,
                  RIV_COLOR_WHITE);

    for (int i = 0; i < n_asteroids; i++) {
      if (!asteroids[i].exists) {
        int j;
        for (j = i; j < n_asteroids - 1; j++) {
          asteroids[j] = asteroids[j + 1];
          n_asteroids -= 1;
        }
      }
    }
    for (int i = 0; i < n_bullets; i++) {
      if (!bullets[i].exists) {
        int j;
        for (j = i; j < n_bullets - 1; j++) {
          bullets[j] = bullets[j + 1];
          n_bullets -= 1;
        }
      }
    }

    while (n_asteroids < 5) {
      struct SpaceObject asteroid = {10 + riv_rand_uint(30),
                                     riv_rand_uint(255),
                                     riv_rand_uint(255),
                                     riv_rand_int(-2, 2),
                                     riv_rand_int(-2, 2),
                                     riv_rand_float() * 3.14 * 2,
                                     true};
      asteroids[n_asteroids] = asteroid;
      n_asteroids += 1;
    }

  } while (riv_present()); // refresh screen and wait next frame
}
