#pragma once
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define QOL_IMPLEMENTATION
#include "libs/build.h"

// Hardcoded function used when no CLI argument is provided
static double f(double x) {

    // NOTE: Oscillatory + nonlinear
    // return exp(-x * x) * sin(10.0 * x) + cos(x * x * x) / (1.0 + x * x);

    // NOTE: Fractal-like (finite Weierstrass-style sum)
    double sum = 0.0;
    double denom = 2.0;
    double freq  = 2.0;
    for (int n = 1; n <= 10; n++) {
        sum += sin(freq * x) / denom;
        freq  *= 2.0;
        denom *= 2.0;
    }
    return sum;

    // NOTE: Piecewise function
    // if (x < 0.0) return sin(x);
    // else if (x < 2.0) return sin(x * x);
    // else return log(x - 1.0) + cos(5.0 * x);
}

// Simple expression parser for mathematical expressions
typedef struct {
    const char *expr;
    int pos;
    double x_value;
    bool has_error;
    char error_msg[128];
} ExprParser;

static void skip_whitespace(ExprParser *p) {
    while (p->expr[p->pos] && isspace((unsigned char)p->expr[p->pos])) {
        p->pos++;
    }
}

static double parse_expression(ExprParser *p);
static double parse_term(ExprParser *p);
static double parse_factor(ExprParser *p);
static double parse_power(ExprParser *p);
static double parse_atom(ExprParser *p);

static double parse_atom(ExprParser *p) {
    skip_whitespace(p);
    
    if (!p->expr[p->pos]) {
        p->has_error = true;
        snprintf(p->error_msg, sizeof(p->error_msg), "Unexpected end of expression");
        return 0.0;
    }
    
    if (p->expr[p->pos] == '(') {
        p->pos++;
        double result = parse_expression(p);
        skip_whitespace(p);
        if (p->expr[p->pos] == ')') {
            p->pos++;
            return result;
        }
        p->has_error = true;
        snprintf(p->error_msg, sizeof(p->error_msg), "Mismatched parenthesis at position %d", p->pos);
        return 0.0;
    }
    
    if (p->expr[p->pos] == 'x' || p->expr[p->pos] == 'X') {
        p->pos++;
        return p->x_value;
    }
    
    if (p->expr[p->pos] == '-') {
        p->pos++;
        return -parse_atom(p);
    }
    
    if (p->expr[p->pos] == '+') {
        p->pos++;
        return parse_atom(p);
    }
    
    // Parse number
    if (isdigit((unsigned char)p->expr[p->pos]) || p->expr[p->pos] == '.') {
        char *end;
        double val = strtod(p->expr + p->pos, &end);
        p->pos = (int)(end - p->expr);
        return val;
    }
    
    // Parse functions: sin, cos, exp, log, sqrt, pow
    if (strncmp(p->expr + p->pos, "sin", 3) == 0) {
        p->pos += 3;
        skip_whitespace(p);
        if (p->expr[p->pos] == '(') {
            p->pos++;
            double arg = parse_expression(p);
            skip_whitespace(p);
            if (p->expr[p->pos] == ')') p->pos++;
            return sin(arg);
        }
        return sin(parse_atom(p));
    }
    
    if (strncmp(p->expr + p->pos, "cos", 3) == 0) {
        p->pos += 3;
        skip_whitespace(p);
        if (p->expr[p->pos] == '(') {
            p->pos++;
            double arg = parse_expression(p);
            skip_whitespace(p);
            if (p->expr[p->pos] == ')') p->pos++;
            return cos(arg);
        }
        return cos(parse_atom(p));
    }
    
    if (strncmp(p->expr + p->pos, "exp", 3) == 0) {
        p->pos += 3;
        skip_whitespace(p);
        if (p->expr[p->pos] == '(') {
            p->pos++;
            double arg = parse_expression(p);
            skip_whitespace(p);
            if (p->expr[p->pos] == ')') p->pos++;
            return exp(arg);
        }
        return exp(parse_atom(p));
    }
    
    if (strncmp(p->expr + p->pos, "log", 3) == 0) {
        p->pos += 3;
        skip_whitespace(p);
        if (p->expr[p->pos] == '(') {
            p->pos++;
            double arg = parse_expression(p);
            skip_whitespace(p);
            if (p->expr[p->pos] == ')') p->pos++;
            return log(arg);
        }
        return log(parse_atom(p));
    }
    
    if (strncmp(p->expr + p->pos, "sqrt", 4) == 0) {
        p->pos += 4;
        skip_whitespace(p);
        if (p->expr[p->pos] == '(') {
            p->pos++;
            double arg = parse_expression(p);
            skip_whitespace(p);
            if (p->expr[p->pos] == ')') p->pos++;
            return sqrt(arg);
        }
        return sqrt(parse_atom(p));
    }
    
    if (strncmp(p->expr + p->pos, "pow", 3) == 0) {
        p->pos += 3;
        skip_whitespace(p);
        if (p->expr[p->pos] == '(') {
            p->pos++;
            double base = parse_expression(p);
            skip_whitespace(p);
            if (p->expr[p->pos] == ',') {
                p->pos++;
                skip_whitespace(p);
                double exp = parse_expression(p);
                skip_whitespace(p);
                if (p->expr[p->pos] == ')') {
                    p->pos++;
                    return pow(base, exp);
                }
            }
        }
    }
    
    // Unknown token - this is an error
    p->has_error = true;
    int start_pos = p->pos;
    while (p->expr[p->pos] && !isspace((unsigned char)p->expr[p->pos]) && 
           p->expr[p->pos] != '+' && p->expr[p->pos] != '-' && 
           p->expr[p->pos] != '*' && p->expr[p->pos] != '/' && 
           p->expr[p->pos] != '^' && p->expr[p->pos] != '(' && 
           p->expr[p->pos] != ')' && p->expr[p->pos] != ',') {
        p->pos++;
    }
    int len = p->pos - start_pos;
    if (len > 20) len = 20;
    snprintf(p->error_msg, sizeof(p->error_msg), "Unknown token '%.*s' at position %d", len, p->expr + start_pos, start_pos);
    return 0.0;
}

static double parse_power(ExprParser *p) {
    double left = parse_atom(p);
    skip_whitespace(p);
    
    if (p->expr[p->pos] == '^') {
        p->pos++;
        skip_whitespace(p);
        double right = parse_power(p);
        return pow(left, right);
    }
    
    return left;
}

static double parse_factor(ExprParser *p) {
    double left = parse_power(p);
    skip_whitespace(p);
    
    while (p->expr[p->pos] == '*' || p->expr[p->pos] == '/') {
        char op = p->expr[p->pos];
        p->pos++;
        skip_whitespace(p);
        double right = parse_power(p);
        if (op == '*') {
            left *= right;
        } else {
            if (right == 0.0) return 0.0; // Avoid division by zero
            left /= right;
        }
        skip_whitespace(p);
    }
    
    return left;
}

static double parse_term(ExprParser *p) {
    double left = parse_factor(p);
    skip_whitespace(p);
    
    while (p->expr[p->pos] == '+' || p->expr[p->pos] == '-') {
        char op = p->expr[p->pos];
        p->pos++;
        skip_whitespace(p);
        double right = parse_factor(p);
        if (op == '+') {
            left += right;
        } else {
            left -= right;
        }
        skip_whitespace(p);
    }
    
    return left;
}

static double parse_expression(ExprParser *p) {
    return parse_term(p);
}

static double evaluate_function(const char *expr, double x, bool *has_error, char *error_msg) {
    ExprParser parser = {expr, 0, x, false, ""};
    double result = parse_expression(&parser);
    
    // Check if we parsed the entire expression
    skip_whitespace(&parser);
    if (parser.expr[parser.pos] != '\0' && !parser.has_error) {
        parser.has_error = true;
        snprintf(parser.error_msg, sizeof(parser.error_msg), "Unexpected character '%c' at position %d", parser.expr[parser.pos], parser.pos);
    }
    
    if (has_error) *has_error = parser.has_error;
    if (error_msg && parser.has_error) {
        strncpy(error_msg, parser.error_msg, 127);
        error_msg[127] = '\0';
    }
    
    return result;
}

void plotter_wrapper(const char *function_str) {
    const int SCREEN_W = 1000;
    const int SCREEN_H = 720;
    
    // Plot range (mutable for panning)
    double x_min = -10.0;
    double x_max = 10.0;
    double y_min = -5.0;
    double y_max = 5.0;
    
    // Number of points to sample
    const int NUM_POINTS = 1000;
    
    // Check if we should use the hardcoded function
    bool use_hardcoded = (!function_str || strlen(function_str) == 0);
    const char *display_str = use_hardcoded ? "unknown" : function_str;
    
    // Validate the function string if provided
    if (!use_hardcoded) {
        bool parse_error = false;
        char error_msg[128] = {0};
        (void)evaluate_function(function_str, 0.0, &parse_error, error_msg);
        
        if (parse_error) {
            qol_error("Invalid function expression: %s\n", function_str);
            qol_error("Error: %s\n", error_msg);
            qol_error("Valid examples:\n");
            qol_error("  sin(x) * cos(x)\n");
            qol_error("  x^3 - 2*x + 1\n");
            qol_error("  exp(-x*x/10)\n");
            qol_error("  pow(x, 2)\n");
            return;
        }
    }
    
    InitWindow(SCREEN_W, SCREEN_H, "Function Plotter");
    SetTargetFPS(60);
    
    // Pre-compute function values
    double *x_values = malloc(sizeof(double) * NUM_POINTS);
    double *y_values = malloc(sizeof(double) * NUM_POINTS);
    
    // Initial computation of function values
    for (int i = 0; i < NUM_POINTS; i++) {
        double x = x_min + (x_max - x_min) * i / (NUM_POINTS - 1);
        x_values[i] = x;
        if (use_hardcoded) {
            y_values[i] = f(x);
        } else {
            bool err = false;
            y_values[i] = evaluate_function(function_str, x, &err, NULL);
            if (err) {
                // Should not happen if initial validation passed, but handle gracefully
                y_values[i] = 0.0;
            }
        }
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
                if (use_hardcoded) {
                    y_values[i] = f(x);
                } else {
                    bool err = false;
                    y_values[i] = evaluate_function(function_str, x, &err, NULL);
                    if (err) y_values[i] = 0.0;
                }
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
                if (use_hardcoded) {
                    y_values[i] = f(x);
                } else {
                    bool err = false;
                    y_values[i] = evaluate_function(function_str, x, &err, NULL);
                    if (err) y_values[i] = 0.0;
                }
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
            
            // Display function string (truncate if too long)
            char func_display[64];
            snprintf(func_display, sizeof(func_display), "f(x) = %s", display_str);
            if (strlen(func_display) > 40) {
                func_display[37] = '.';
                func_display[38] = '.';
                func_display[39] = '.';
                func_display[40] = '\0';
            }
            DrawText(func_display, panelX + 10, lineY, 14, GREEN); lineY += 18;
            
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
