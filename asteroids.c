#include <math.h>
#include <riv.h> // MUST BE THE FIRST LINE
#include <stdio.h>


struct Point {
  float x;
  float y;
};


struct Point zero_vector() {
  struct Point p = {0, 0};
  return p;
}

struct Point rand_int_vector(int lb, int ub) {
  struct Point p = {lb + ub*riv_rand_float(), lb + ub*riv_rand_float()};
  return p;
}

struct Point sum_vectors(struct Point p1, struct Point p2) {
  struct Point p = {p1.x + p2.x, p1.y + p2.y};
  return p;
}

struct Point scale_vector(struct Point p, float scale) {
  struct Point sp = {p.x*scale , p.y*scale};
  return sp;
}
struct Point normalize_vector(struct Point p) {
  float mag = sqrt(p.x*p.x + p.y*p.y);
  struct Point np = scale_vector(p, 1/mag);
  return np;
}

struct Point convert_from_polar(int xcenter, int ycenter, int radius,
                                float angle) {
  int x = xcenter + radius * cos(angle);
  int y = ycenter + radius * sin(angle);
  struct Point p = {x, y};
  return p;
}
struct SpaceObject {
  int size;
  struct Point pos;
  struct Point speed;
  float angle;
  bool exists;
};

void draw_ship(struct SpaceObject ship) {
  riv_draw_circle_fill((int)ship.pos.x, (int)ship.pos.y, ship.size, RIV_COLOR_WHITE);
  riv_draw_point(ship.pos.x + ship.size * sin(ship.angle),
                 ship.pos.y - ship.size * cos(ship.angle), RIV_COLOR_RED);
}

void draw_asteroid(struct SpaceObject asteroid, uint32_t color) {
  float angle_increment = 2 * 3.14 / 7;
  struct Point points[7];
  points[0] = convert_from_polar(asteroid.pos.x, asteroid.pos.y, asteroid.size / 2, 0);
  for (int i = 1; i < 7; i++) {
    points[i] = convert_from_polar(asteroid.pos.x, asteroid.pos.y, asteroid.size / 2,
                                   i * angle_increment);
    riv_draw_line(points[i - 1].x, points[i - 1].y, points[i].x, points[i].y,
                  color);
  }
    riv_draw_line(points[7-1].x, points[7-1].y, points[0].x, points[0].y,
                  color);
}

void wrap_around(struct SpaceObject *o) {
  if (o->pos.x < 0 - o->size)
    o->pos.x = 256 + o->size;
  if (o->pos.x > 256 + o->size)
    o->pos.x = 0 - o->size;
  if (o->pos.y < 0 - o->size)
    o->pos.y = 256 + o->size;
  if (o->pos.y > 256 + o->size)
    o->pos.y = 0 - o->size;
}

float distance(struct Point p1, struct Point p2) {
  return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

int main() { // entry point
  struct SpaceObject ship = {10, {128, 128}, zero_vector(), 0, true};
  struct SpaceObject asteroids[100];
  struct SpaceObject bullets[100];
  int n_bullets = 0;

  // struct SpaceObject asteroid ={10, 128, 128, 1, 2, 0, true};
  int n_asteroids = 0;
  for (int i = 0; i < 5; i++) {
    struct SpaceObject asteroid = {10 + riv_rand_uint(30),
                                    rand_int_vector(0, 255),
                                    rand_int_vector(-2, 2),
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
    struct Point thrust = zero_vector();
    if (riv->keys[RIV_GAMEPAD_UP].down) {
      thrust.x = 0.3*sin(ship.angle);
      thrust.y = -0.3*cos(ship.angle);

      ship.speed = sum_vectors(ship.speed, thrust);
    }

    ship.pos = sum_vectors(ship.pos, ship.speed);
    ship.speed = scale_vector(ship.speed, 0.999);

    wrap_around(&ship);

    if (riv->keys[RIV_GAMEPAD_A1].press) {
      struct Point speed = {5*sin(ship.angle), -5*cos(ship.angle)};
      struct SpaceObject bullet = {
          2,  ship.pos, speed,
          ship.angle, true};
      bullets[n_bullets] = bullet;
      n_bullets += 1;
    }

    // draw screen
    riv_clear(RIV_COLOR_BLACK); // clear screen
    draw_ship(ship);

    for (int i = 0; i < n_asteroids; i++) {
      asteroids[i].pos = sum_vectors(asteroids[i].pos, asteroids[i].speed);
      wrap_around(&asteroids[i]);
      if (distance(asteroids[i].pos, ship.pos) <
          ship.size / 2 + asteroids[i].size / 2) {
        riv_draw_circle_line(asteroids[i].pos.x, asteroids[i].pos.y, asteroids[i].size,
                             RIV_COLOR_RED);
        draw_asteroid(asteroids[i], RIV_COLOR_RED);
        score = 0;
      } else {
        draw_asteroid(asteroids[i], RIV_COLOR_WHITE);
      }
    }
    for (int i = 0; i < n_bullets; i++) {
      bullets[i].pos = sum_vectors(bullets[i].pos, bullets[i].speed);
      if (bullets[i].pos.x < 0)
        bullets[i].exists = false;
      if (bullets[i].pos.x > 256)
        bullets[i].exists = false;
      if (bullets[i].pos.y < 0)
        bullets[i].exists = false;
      if (bullets[i].pos.y > 256)
        bullets[i].exists = false;
      riv_draw_point(bullets[i].pos.x, bullets[i].pos.y, RIV_COLOR_WHITE);
    }

    for (int i = 0; i < n_bullets; i++) {
      for (int j = 0; j < n_asteroids; j++) {
        float d = distance(bullets[i].pos, asteroids[j].pos);
        if (d < asteroids[j].size / 2) {
          asteroids[j].exists = false;
          bullets[i].exists = false;
          bullets[i].pos.y = -1000;
          score += 100;
          if (asteroids[j].size > 10) {
            for (int i = 0; i <= 2; i++) {
              struct SpaceObject asteroid = {2 + asteroids[j].size / 2,
                                             asteroids[j].pos,
                                             rand_int_vector(-2, 2),
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
