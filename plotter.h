#pragma once
#include <raylib.h>
#include <math.h>
#include <stdlib.h>

#define QOL_IMPLEMENTATION
#include "libs/build.h"

// Define your function here: f(x) = <whatever>
// Also update FUNCTION_STRING below to match your function
// #define FUNCTION_STRING "f(x) = x^3 - 2x + 1"

static double f(double x) {
    // return sin(x);
    // return x * x;                    // parabola -> "f(x) = x^2"
    return sin(x) * cos(x);          // product -> "f(x) = sin(x) * cos(x)"
    // return exp(-x * x / 10);         // gaussian -> "f(x) = e^(-x^2/10)"
    // return x * sin(x);               // x * sin(x) -> "f(x) = x * sin(x)"
    // return pow(x, 3) - 2 * x + 1;    // polynomial -> "f(x) = x^3 - 2x + 1"
}

void plotter(void) {
    const int SCREEN_W = 1000;
    const int SCREEN_H = 720;
    
    // Plot range (mutable for panning)
    double x_min = -10.0;
    double x_max = 10.0;
    double y_min = -5.0;
    double y_max = 5.0;
    
    // Number of points to sample
    const int NUM_POINTS = 1000;
    
    InitWindow(SCREEN_W, SCREEN_H, "Function Plotter");
    SetTargetFPS(60);
    
    // Pre-compute function values
    double *x_values = malloc(sizeof(double) * NUM_POINTS);
    double *y_values = malloc(sizeof(double) * NUM_POINTS);
    
    // Initial computation of function values
    for (int i = 0; i < NUM_POINTS; i++) {
        double x = x_min + (x_max - x_min) * i / (NUM_POINTS - 1);
        x_values[i] = x;
        y_values[i] = f(x);
    }
    
    // Use fixed range or auto-scaled range
    double plot_y_min = y_min;
    double plot_y_max = y_max;
    
    // Mouse drag state
    bool is_dragging = false;
    Vector2 last_mouse_pos = {0, 0};
    
    #define WORLD_TO_SCREEN_X(wx) ((int)((wx - x_min) / (x_max - x_min) * SCREEN_W))
    #define WORLD_TO_SCREEN_Y(wy) ((int)(SCREEN_H - (wy - plot_y_min) / (plot_y_max - plot_y_min) * SCREEN_H))
    
    while (!WindowShouldClose()) {
        // Handle mouse drag for panning
        Vector2 mouse_pos = GetMousePosition();
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            is_dragging = true;
            last_mouse_pos = mouse_pos;
        }
        
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            is_dragging = false;
        }
        
        if (is_dragging && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 delta = {mouse_pos.x - last_mouse_pos.x, mouse_pos.y - last_mouse_pos.y};
            
            // Convert screen delta to world delta
            double world_delta_x = -delta.x * (x_max - x_min) / SCREEN_W;
            double world_delta_y = delta.y * (plot_y_max - plot_y_min) / SCREEN_H;
            
            // Update ranges
            x_min += world_delta_x;
            x_max += world_delta_x;
            y_min += world_delta_y;
            y_max += world_delta_y;
            plot_y_min = y_min;
            plot_y_max = y_max;
            
            // Recompute function values for new range
            for (int i = 0; i < NUM_POINTS; i++) {
                double x = x_min + (x_max - x_min) * i / (NUM_POINTS - 1);
                x_values[i] = x;
                y_values[i] = f(x);
            }
            
            last_mouse_pos = mouse_pos;
        }
        
        // Handle mouse wheel zoom
        float wheel_move = GetMouseWheelMove();
        if (wheel_move != 0.0f) {
            // Get world coordinates under mouse cursor
            double mouse_world_x = x_min + (x_max - x_min) * mouse_pos.x / SCREEN_W;
            double mouse_world_y = plot_y_max - (plot_y_max - plot_y_min) * mouse_pos.y / SCREEN_H;
            
            // Zoom factor (1.1 for smooth zooming)
            double zoom_factor = (wheel_move > 0) ? 1.0 / 1.1 : 1.1;
            
            // Calculate new ranges centered on mouse position
            double x_range = x_max - x_min;
            double y_range = plot_y_max - plot_y_min;
            
            double new_x_range = x_range * zoom_factor;
            double new_y_range = y_range * zoom_factor;
            
            // Adjust ranges to keep mouse position fixed in world coordinates
            double x_center = mouse_world_x;
            double y_center = mouse_world_y;
            
            x_min = x_center - new_x_range * (mouse_pos.x / SCREEN_W);
            x_max = x_min + new_x_range;
            
            y_min = y_center - new_y_range * (1.0 - mouse_pos.y / SCREEN_H);
            y_max = y_min + new_y_range;
            plot_y_min = y_min;
            plot_y_max = y_max;
            
            // Recompute function values for new range
            for (int i = 0; i < NUM_POINTS; i++) {
                double x = x_min + (x_max - x_min) * i / (NUM_POINTS - 1);
                x_values[i] = x;
                y_values[i] = f(x);
            }
        }
        
        BeginDrawing();
            ClearBackground(BLACK);
            
            // Draw grid and axes
            const Color gridColor = Fade(DARKGRAY, 0.3f);
            const Color axisColor = GRAY;
            
            // Vertical grid lines
            for (int i = 0; i <= 10; i++) {
                double x = x_min + (x_max - x_min) * i / 10.0;
                int sx = WORLD_TO_SCREEN_X(x);
                int sy1 = WORLD_TO_SCREEN_Y(plot_y_min);
                int sy2 = WORLD_TO_SCREEN_Y(plot_y_max);
                DrawLine(sx, sy1, sx, sy2, gridColor);
            }
            
            // Horizontal grid lines
            for (int i = 0; i <= 10; i++) {
                double y = plot_y_min + (plot_y_max - plot_y_min) * i / 10.0;
                int sx1 = WORLD_TO_SCREEN_X(x_min);
                int sx2 = WORLD_TO_SCREEN_X(x_max);
                int sy = WORLD_TO_SCREEN_Y(y);
                DrawLine(sx1, sy, sx2, sy, gridColor);
            }
            
            // Draw axes
            int x_axis_y = WORLD_TO_SCREEN_Y(0);
            int y_axis_x = WORLD_TO_SCREEN_X(0);
            int x_axis_start = WORLD_TO_SCREEN_X(x_min);
            int x_axis_end = WORLD_TO_SCREEN_X(x_max);
            int y_axis_start = WORLD_TO_SCREEN_Y(plot_y_min);
            int y_axis_end = WORLD_TO_SCREEN_Y(plot_y_max);
            
            DrawLine(x_axis_start, x_axis_y, x_axis_end, x_axis_y, axisColor);
            DrawLine(y_axis_x, y_axis_start, y_axis_x, y_axis_end, axisColor);
            
            // Draw axis labels
            char buf[32];
            for (int i = 0; i <= 10; i++) {
                double x = x_min + (x_max - x_min) * i / 10.0;
                int sx = WORLD_TO_SCREEN_X(x);
                int sy = WORLD_TO_SCREEN_Y(0);
                snprintf(buf, sizeof(buf), "%.1f", x);
                DrawText(buf, sx - 15, sy + 5, 12, RAYWHITE);
            }
            
            for (int i = 0; i <= 10; i++) {
                double y = plot_y_min + (plot_y_max - plot_y_min) * i / 10.0;
                int sx = WORLD_TO_SCREEN_X(0);
                int sy = WORLD_TO_SCREEN_Y(y);
                snprintf(buf, sizeof(buf), "%.1f", y);
                DrawText(buf, sx + 5, sy - 10, 12, RAYWHITE);
            }
            
            // Draw function curve
            for (int i = 0; i < NUM_POINTS - 1; i++) {
                int sx1 = WORLD_TO_SCREEN_X(x_values[i]);
                int sy1 = WORLD_TO_SCREEN_Y(y_values[i]);
                int sx2 = WORLD_TO_SCREEN_X(x_values[i + 1]);
                int sy2 = WORLD_TO_SCREEN_Y(y_values[i + 1]);
                
                // Only draw if both points are within valid Y range
                if (y_values[i] >= plot_y_min && y_values[i] <= plot_y_max &&
                    y_values[i + 1] >= plot_y_min && y_values[i + 1] <= plot_y_max) {
                    DrawLine(sx1, sy1, sx2, sy2, GREEN);
                }
            }
            
            // Draw function points (optional, for debugging)
            // for (int i = 0; i < NUM_POINTS; i++) {
            //     int sx = WORLD_TO_SCREEN_X(x_values[i]);
            //     int sy = WORLD_TO_SCREEN_Y(y_values[i]);
            //     if (y_values[i] >= plot_y_min && y_values[i] <= plot_y_max) {
            //         DrawCircle(sx, sy, 2, RED);
            //     }
            // }
            
            // HUD
            const int panelW = 320;
            const int panelH = 140;
            const int panelX = 20;
            const int panelY = 20;
            DrawRectangle(panelX, panelY, panelW, panelH, Fade(BLACK, 0.7f));
            DrawRectangleLines(panelX, panelY, panelW, panelH, RAYWHITE);
            
            int lineY = panelY + 12;
            DrawText("Function Plotter", panelX + 10, lineY, 20, RAYWHITE); lineY += 24;
            
            // DrawText(FUNCTION_STRING, panelX + 10, lineY, 16, GREEN); lineY += 20;
            
            snprintf(buf, sizeof(buf), "Range: [%.2f, %.2f]", x_min, x_max);
            DrawText(buf, panelX + 10, lineY, 16, YELLOW); lineY += 20;
            
            snprintf(buf, sizeof(buf), "Y: [%.2f, %.2f]", plot_y_min, plot_y_max);
            DrawText(buf, panelX + 10, lineY, 16, YELLOW); lineY += 20;
            
            DrawText("Drag to pan | Scroll to zoom | Esc to quit", panelX + 10, lineY, 14, GRAY);
            
        EndDrawing();
    }
    
    CloseWindow();
    free(x_values);
    free(y_values);
    
    #undef WORLD_TO_SCREEN_X
    #undef WORLD_TO_SCREEN_Y
}
