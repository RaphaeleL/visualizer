#include "solar.h"
#include <math.h>

#include "../libs/build.h"

static void draw_sun(Vector2 sun_pos, float sun_radius) {
    // Sunlit
    for (int y = 0; y < SCREEN_H; y++) {
        for (int x = 0; x < SCREEN_W; x++) {
            Vector2 pixel_pos = (Vector2){(float)x, (float)y};
            Vector2 to_sun = (Vector2){sun_pos.x - pixel_pos.x, sun_pos.y - pixel_pos.y};
            float dist_to_sun = sqrtf(to_sun.x * to_sun.x + to_sun.y * to_sun.y);

            // Background brightness decreases with distance from sun
            float brightness = 1.0f / (1.0f + dist_to_sun * 0.002f);
            brightness = fminf(1.0f, brightness);

            // Yellow/grey-ish color
            Color bg_color = (Color){ (unsigned char)(180 * brightness), (unsigned char)(160 * brightness), (unsigned char)(100 * brightness), 255 };

            DrawPixel(x, y, bg_color);
        }
    }

    // Sun
    DrawCircleV(sun_pos, sun_radius, (Color){255, 255, 100, 255});
    DrawCircleV(sun_pos, sun_radius - 5, (Color){255, 200, 50, 255});
}

static void draw_orbit(Planet *planets, Vector2 sun_pos, Vector2 earth_pos, Planet *moon) {
    for (int i = 0; i < MAX_PLANETS; i++) {
        if (i == 3) continue; // Skip moon
        Planet *p = &planets[i];
        draw_dotted_orbit(sun_pos, p->distance, (Color){100, 100, 100, 255});
    }

    // Draw dotted orbit circle for Moon (centered around Earth)
    draw_dotted_orbit(earth_pos, moon->distance, (Color){150, 150, 150, 255});
}

static void draw_shadows(Planet *planets, Vector2 sun_pos, Vector2 *planet_positions, Vector2 moon_pos, Planet *moon) {
    for (int y = 0; y < SCREEN_H; y++) {
        for (int x = 0; x < SCREEN_W; x++) {
            Vector2 pixel_pos = (Vector2){(float)x, (float)y};

            // Check if pixel is in shadow of any planet
            bool in_shadow = false;
            for (int i = 0; i < MAX_PLANETS; i++) {
                if (i == 3) continue; // Skip moon, handled separately
                Planet *p = &planets[i];
                if (is_point_in_shadow(pixel_pos, planet_positions[i], p->radius, sun_pos)) {
                    in_shadow = true;
                    break;
                }
            }
            // Check moon shadow
            if (!in_shadow && is_point_in_shadow(pixel_pos, moon_pos, moon->radius, sun_pos)) {
                in_shadow = true;
            }

            // If in shadow, draw black pixel
            if (in_shadow) {
                DrawPixel(x, y, BLACK);
            }
        }
    }
}

static void draw_planets(Planet *planets, Vector2 *planet_positions, Vector2 moon_pos, Planet *moon) {
    // Skip moon (index 3) - drawn separately
    for (int i = 0; i < MAX_PLANETS; i++) {
        if (i == 3) continue; // Skip moon
        Planet *p = &planets[i];
        draw_planet_dot(planet_positions[i], p->radius, p->base_color);

        // Draw planet label (horizontal, moves with planet)
        Vector2 label_pos = (Vector2){ planet_positions[i].x, planet_positions[i].y - p->radius - 15.0f };
        DrawText(p->name, (int)label_pos.x - MeasureText(p->name, 12) / 2, (int)label_pos.y, 12, WHITE);
    }
    // Draw moon separately (orbiting Earth)
    draw_planet_dot(moon_pos, moon->radius, moon->base_color);

    // Draw moon label
    Vector2 moon_label_pos = (Vector2){ moon_pos.x, moon_pos.y - moon->radius - 15.0f };
    DrawText(moon->name, (int)moon_label_pos.x - MeasureText(moon->name, 12) / 2, (int)moon_label_pos.y, 12, WHITE);
}

// Get planet position in 2D space
static Vector2 get_planet_position(Planet *planet, float time) {
    float angle = planet->angle + planet->speed * time;
    float x = planet->distance * cosf(angle);
    float y = planet->distance * sinf(angle);
    return (Vector2){x, y};
}

// Draw a dotted circle for the orbit
static void draw_dotted_orbit(Vector2 center, float radius, Color color) {
    int num_points = (int)(radius * 2.0f * PI / 3.0f); // Adjust spacing based on radius
    if (num_points < 60) num_points = 60;
    if (num_points > 300) num_points = 300;
    
    // Draw dotted circle - draw every other point to create dotted effect
    for (int i = 0; i < num_points; i += 2) {
        float angle = (float)i / num_points * 2.0f * PI;
        float x = center.x + radius * cosf(angle);
        float y = center.y + radius * sinf(angle);
        DrawPixel((int)x, (int)y, color);
        // Draw a small dot (2x2 pixels) for better visibility
        DrawPixel((int)x + 1, (int)y, color);
        DrawPixel((int)x, (int)y + 1, color);
        DrawPixel((int)x + 1, (int)y + 1, color);
    }
}

// Check if a point is in the shadow of a planet
static bool is_point_in_shadow(Vector2 point, Vector2 planet_center, float planet_radius, Vector2 sun_pos) {
    // Vector from sun to planet
    Vector2 sun_to_planet = (Vector2){planet_center.x - sun_pos.x, planet_center.y - sun_pos.y};
    float dist_sun_planet = sqrtf(sun_to_planet.x * sun_to_planet.x + sun_to_planet.y * sun_to_planet.y);
    
    if (dist_sun_planet < 0.001f) return false; // Planet is at sun
    
    // Vector from sun to point
    Vector2 sun_to_point = (Vector2){point.x - sun_pos.x, point.y - sun_pos.y};
    float dist_sun_point = sqrtf(sun_to_point.x * sun_to_point.x + sun_to_point.y * sun_to_point.y);
    
    // If point is closer to sun than planet, it's not in shadow
    if (dist_sun_point < dist_sun_planet) return false;
    
    // Check if point is behind the planet (in the shadow cone)
    // Calculate angle between sun->planet and sun->point
    float dot = (sun_to_planet.x * sun_to_point.x + sun_to_planet.y * sun_to_point.y) / (dist_sun_planet * dist_sun_point);
    
    // Calculate the angle of the shadow cone (based on planet radius)
    float shadow_angle = atan2f(planet_radius, dist_sun_planet);
    float point_angle = acosf(dot);
    
    // If point is within the shadow cone angle, check distance
    if (point_angle <= shadow_angle) {
        // Check if point is within the shadow cone at this distance
        float expected_radius = planet_radius * (dist_sun_point / dist_sun_planet);
        Vector2 point_from_planet = (Vector2){point.x - planet_center.x, point.y - planet_center.y};
        float dist_from_planet = sqrtf(point_from_planet.x * point_from_planet.x + point_from_planet.y * point_from_planet.y);
        
        // Project point onto the shadow axis
        Vector2 shadow_dir = (Vector2){sun_to_planet.x / dist_sun_planet, sun_to_planet.y / dist_sun_planet};
        float proj_dist = point_from_planet.x * shadow_dir.x + point_from_planet.y * shadow_dir.y;
        
        // If projection is positive (point is behind planet along shadow axis)
        if (proj_dist > 0) {
            // Check if point is within the expanding shadow cone
            float perp_dist = sqrtf(dist_from_planet * dist_from_planet - proj_dist * proj_dist);
            float shadow_radius_at_dist = planet_radius + (expected_radius - planet_radius) * (proj_dist / (dist_sun_point - dist_sun_planet));
            
            if (perp_dist <= shadow_radius_at_dist) return true;
        }
    }
    
    return false;
}

// Draw a planet as a simple colored dot
static void draw_planet_dot(Vector2 center, float radius, Color color) {
    DrawCircleV(center, radius, color);
}

void solar(void) {
    InitWindow(SCREEN_W, SCREEN_H, "Solar System");
    SetTargetFPS(60);

    // Sun position (centered on screen)
    Vector2 sun_pos = (Vector2){SCREEN_W / 2.0f, SCREEN_H / 2.0f};
    float sun_radius = 40.0f;
    
    // Planets (distance from center, starting angle, speed, radius, color)
    Planet planets[MAX_PLANETS] = {
        {.radius = 8.0f,  .distance = 120.0f, .angle = 0.0f,       .speed = 0.02f + SPEED_INC,  .base_color = (Color){200, 150, 100, 255}, .name = "Mercury"},
        {.radius = 10.0f, .distance = 160.0f, .angle = PI * 0.5f,  .speed = 0.015f+ SPEED_INC, .base_color = (Color){255, 200, 100, 255}, .name = "Venus"},
        {.radius = 12.0f, .distance = 200.0f, .angle = PI,         .speed = 0.01f + SPEED_INC,  .base_color = (Color){100, 150, 255, 255}, .name = "Earth"},
        {.radius = 4.0f,  .distance = 35.0f,  .angle = 0.0f,       .speed = 365.0f * (0.01f + SPEED_INC),  .base_color = (Color){200, 200, 200, 255}, .name = "Moon"},
        {.radius = 10.0f, .distance = 260.0f, .angle = PI * 1.5f,  .speed = 0.008f+ SPEED_INC, .base_color = (Color){255, 100, 50, 255},  .name = "Mars"},
        {.radius = 25.0f, .distance = 350.0f, .angle = PI * 0.25f, .speed = 0.004f+ SPEED_INC, .base_color = (Color){255, 200, 150, 255}, .name = "Jupiter"},
        {.radius = 22.0f, .distance = 420.0f, .angle = PI * 0.75f, .speed = 0.003f+ SPEED_INC, .base_color = (Color){255, 220, 180, 255}, .name = "Saturn"},
    };
    
    float time = 0.0f;
    float time_speed = 0.5f;

    while (!WindowShouldClose()) {
        // Update time
        time += time_speed * GetFrameTime();

        BeginDrawing();
        
            // Get all planet positions first (orbiting around the sun)
            Vector2 planet_positions[MAX_PLANETS];
            for (int i = 0; i < MAX_PLANETS; i++) {
                if (i == 3) continue; // Skip moon, it orbits Earth
                Planet *p = &planets[i];
                Vector2 pos = get_planet_position(p, time);
                planet_positions[i] = (Vector2){ sun_pos.x + pos.x, sun_pos.y + pos.y };
            }
            
            // Calculate moon position (orbits Earth)
            Planet *earth = &planets[2];
            QOL_UNUSED(earth);
            Vector2 earth_pos = planet_positions[2];
            Planet *moon = &planets[3];
            float moon_angle = moon->angle + moon->speed * time;
            Vector2 moon_pos = (Vector2){
                earth_pos.x + moon->distance * cosf(moon_angle),
                earth_pos.y + moon->distance * sinf(moon_angle)
            };

            planet_positions[3] = moon_pos;

            draw_sun(sun_pos, sun_radius);
            draw_orbit(planets, sun_pos, earth_pos, moon);
            draw_shadows(planets, sun_pos, planet_positions, moon_pos, moon);
            draw_planets(planets, planet_positions, moon_pos, moon);

        EndDrawing();
    }

    CloseWindow();
}
