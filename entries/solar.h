#pragma once
#include <raylib.h>
#include <math.h>

#include "../libs/build.h"

#define SCREEN_W 1000
#define SCREEN_H 720
#define MAX_PLANETS 7
#define SPEED_INC 0.05

typedef struct {
    float radius;
    float distance;      // Distance from sun
    float angle;         // Current orbital angle
    float speed;         // Orbital speed
    Color base_color;    // Base color of the planet
    const char *name;
} Planet;

// Get planet position in 2D space
static Vector2 get_planet_position(Planet *planet, float time);

// Draw a dotted circle for the orbit
static void draw_dotted_orbit(Vector2 center, float radius, Color color);

// Check if a point is in the shadow of a planet
static bool is_point_in_shadow(Vector2 point, Vector2 planet_center, float planet_radius, Vector2 sun_pos);

// Draw a planet as a simple colored dot
static void draw_planet_dot(Vector2 center, float radius, Color color);

static void draw_sun(Vector2 sun_pos, float sun_radius);
static void draw_orbit(Planet *planets, Vector2 sun_pos, Vector2 earth_pos, Planet *moon);
static void draw_shadows(Planet *planets, Vector2 sun_pos, Vector2 *planet_positions, Vector2 moon_pos, Planet *moon);
static void draw_planets(Planet *planets, Vector2 *planet_positions, Vector2 moon_pos, Planet *moon);

// Main solar system simulation function
void solar(void);
