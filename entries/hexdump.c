#include "hexdump.h"
#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../libs/build.h"

void hexdump(const char *input) {
    unsigned char *data = NULL;
    size_t data_size = 0;
    char source_name[256] = {0};

    // Determine input source
    if (!input || strlen(input) == 0) {
        // Read from stdin
        strcpy(source_name, "<stdin>");
        
        // Read from stdin
        size_t buffer_size = 4096;
        data = malloc(buffer_size);
        if (!data) {
            qol_error("hexdump: memory allocation failed\n");
            return;
        }

        size_t total_read = 0;
        size_t bytes_read;
        while ((bytes_read = fread(data + total_read, 1, buffer_size - total_read, stdin)) > 0) {
            total_read += bytes_read;
            if (total_read >= buffer_size) {
                buffer_size *= 2;
                unsigned char *new_data = realloc(data, buffer_size);
                if (!new_data) {
                    qol_error("hexdump: memory reallocation failed\n");
                    free(data);
                    return;
                }
                data = new_data;
            }
        }
        data_size = total_read;
    } else {
        // Try to open as file first
        FILE *fp = fopen(input, "rb");
        if (fp) {
            // It's a file
            strncpy(source_name, input, sizeof(source_name) - 1);
            
            fseek(fp, 0, SEEK_END);
            long file_size = ftell(fp);
            fseek(fp, 0, SEEK_SET);

            if (file_size < 0) {
                qol_error("hexdump: cannot determine file size\n");
                fclose(fp);
                return;
            }

            data_size = (size_t)file_size;
            data = malloc(data_size);
            if (!data) {
                qol_error("hexdump: memory allocation failed\n");
                fclose(fp);
                return;
            }

            size_t bytes_read = fread(data, 1, data_size, fp);
            fclose(fp);

            if (bytes_read != data_size) {
                qol_error("hexdump: read error (expected %zu bytes, got %zu)\n", data_size, bytes_read);
                free(data);
                return;
            }
        } else {
            // Treat as direct string input
            strncpy(source_name, "\"", sizeof(source_name) - 1);
            strncat(source_name, input, sizeof(source_name) - strlen(source_name) - 1);
            strncat(source_name, "\"", sizeof(source_name) - strlen(source_name) - 1);
            
            data_size = strlen(input);
            data = malloc(data_size);
            if (!data) {
                qol_error("hexdump: memory allocation failed\n");
                return;
            }
            memcpy(data, input, data_size);
        }
    }

    if (data_size == 0) {
        qol_error("hexdump: no data to display\n");
        if (data) free(data);
        return;
    }

    const int SCREEN_W = 1400;
    const int SCREEN_H = 900;
    const int FONT_SIZE = 18;
    const int LINE_HEIGHT = 24;
    const int MARGIN_X = 40;
    const int MARGIN_Y = 60;
    const int BYTES_PER_LINE = 16;
    const int OFFSET_WIDTH = 80;
    const int HEX_WIDTH = 500;
    const int ASCII_X = MARGIN_X + OFFSET_WIDTH + HEX_WIDTH + 40;

    // Calculate how many lines we can display
    int visible_lines = (SCREEN_H - 2 * MARGIN_Y - 40) / LINE_HEIGHT;
    int total_lines = (int)((data_size + BYTES_PER_LINE - 1) / BYTES_PER_LINE);
    int scroll_offset = 0;
    int max_scroll = (total_lines > visible_lines) ? (total_lines - visible_lines) : 0;

    InitWindow(SCREEN_W, SCREEN_H, TextFormat("Hexdump - %s", source_name));
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Handle scrolling (removed W/S)
        if (IsKeyPressed(KEY_UP)) {
            if (scroll_offset > 0) scroll_offset--;
        }
        if (IsKeyPressed(KEY_DOWN)) {
            if (scroll_offset < max_scroll) scroll_offset++;
        }
        if (IsKeyPressed(KEY_PAGE_UP)) {
            scroll_offset = (scroll_offset > visible_lines) ? scroll_offset - visible_lines : 0;
        }
        if (IsKeyPressed(KEY_PAGE_DOWN)) {
            scroll_offset = (scroll_offset + visible_lines < max_scroll) ? scroll_offset + visible_lines : max_scroll;
        }
        if (IsKeyPressed(KEY_HOME)) {
            scroll_offset = 0;
        }
        if (IsKeyPressed(KEY_END)) {
            scroll_offset = max_scroll;
        }

        BeginDrawing();
            ClearBackground((Color){20, 20, 25, 255}); // Dark background

            // Draw header with better formatting
            int header_y = MARGIN_Y - 40;
            DrawText("HEXDUMP", MARGIN_X, header_y, 24, YELLOW);
            
            char header[512];
            snprintf(header, sizeof(header), "Source: %s  |  Size: %zu bytes  |  Lines: %d", 
                     source_name, data_size, total_lines);
            DrawText(header, MARGIN_X, header_y + 30, FONT_SIZE - 2, LIGHTGRAY);

            // Draw column headers (with more spacing from source text)
            int col_header_y = MARGIN_Y + 15;
            DrawText("Offset", MARGIN_X, col_header_y, FONT_SIZE - 2, GRAY);
            DrawText("Hex", MARGIN_X + OFFSET_WIDTH + 10, col_header_y, FONT_SIZE - 2, GRAY);
            DrawText("ASCII", ASCII_X, col_header_y, FONT_SIZE - 2, GRAY);
            
            // Draw separator line
            DrawLine(MARGIN_X, col_header_y + 18, SCREEN_W - MARGIN_X, col_header_y + 18, DARKGRAY);

            // Draw hexdump lines with better formatting
            int y = col_header_y + 25;
            for (int line = 0; line < visible_lines && (line + scroll_offset) < total_lines; line++) {
                int actual_line = line + scroll_offset;
                size_t offset = (size_t)actual_line * BYTES_PER_LINE;
                
                // Format offset (8 hex digits)
                char offset_str[16];
                snprintf(offset_str, sizeof(offset_str), "%08lx", (unsigned long)offset);
                DrawText(offset_str, MARGIN_X, y, FONT_SIZE, (Color){150, 150, 255, 255}); // Light blue for offset

                // Build hex bytes part (16 bytes, space-separated, extra space after 8th byte)
                char hex_part[80] = {0};
                for (int i = 0; i < BYTES_PER_LINE; i++) {
                    size_t byte_pos = offset + i;
                    if (byte_pos < data_size) {
                        char hex_byte[4];
                        snprintf(hex_byte, sizeof(hex_byte), "%02x", data[byte_pos]);
                        strcat(hex_part, hex_byte);
                    } else {
                        strcat(hex_part, "  "); // Padding for incomplete lines
                    }
                    // Add space separator (extra space after 8th byte like hexdump -C)
                    if (i == 7) {
                        strcat(hex_part, " "); // Extra space after 8th byte
                    }
                    if (i < BYTES_PER_LINE - 1) {
                        strcat(hex_part, " "); // Normal space separator
                    }
                }

                // Draw hex bytes (single string for perfect alignment)
                int hex_x = MARGIN_X + OFFSET_WIDTH + 10;
                DrawText(hex_part, hex_x, y, FONT_SIZE, RAYWHITE);

                // Build and draw ASCII part with color coding
                for (int i = 0; i < BYTES_PER_LINE; i++) {
                    size_t byte_pos = offset + i;
                    unsigned char c = ' ';
                    Color ascii_color = DARKGRAY;
                    
                    if (byte_pos < data_size) {
                        c = data[byte_pos];
                        if (isprint(c)) {
                            ascii_color = (c == 0) ? DARKGRAY : RAYWHITE;
                            if (c >= '0' && c <= '9') {
                                ascii_color = (Color){150, 255, 150, 255}; // Digits
                            } else if (c >= 'A' && c <= 'Z') {
                                ascii_color = (Color){150, 200, 255, 255}; // Uppercase
                            } else if (c >= 'a' && c <= 'z') {
                                ascii_color = (Color){200, 200, 255, 255}; // Lowercase
                            }
                        } else {
                            c = '.';
                            ascii_color = (Color){255, 150, 150, 255}; // Non-printable
                        }
                    }
                    
                    char ascii_char[2] = {c, '\0'};
                    DrawText(ascii_char, ASCII_X + i * 14, y, FONT_SIZE, ascii_color);
                }

                y += LINE_HEIGHT;
            }

            // Draw scrollbar if needed
            if (max_scroll > 0) {
                int scrollbar_x = SCREEN_W - 25;
                int scrollbar_h = SCREEN_H - 2 * MARGIN_Y - 40;
                int scrollbar_thumb_h = (scrollbar_h * visible_lines) / total_lines;
                if (scrollbar_thumb_h < 20) scrollbar_thumb_h = 20; // Minimum thumb size
                int scrollbar_thumb_y = MARGIN_Y + (scrollbar_h - scrollbar_thumb_h) * scroll_offset / max_scroll;
                
                DrawRectangle(scrollbar_x, MARGIN_Y, 6, scrollbar_h, (Color){40, 40, 45, 255});
                DrawRectangle(scrollbar_x, scrollbar_thumb_y, 6, scrollbar_thumb_h, (Color){100, 100, 110, 255});
            }

            // Draw instructions (updated, removed W/S)
            char instructions[256];
            snprintf(instructions, sizeof(instructions), 
                     "Up/Down: Scroll | Page Up/Down: Jump | Home/End: Top/Bottom | Esc: Quit");
            int text_width = MeasureText(instructions, 14);
            DrawText(instructions, SCREEN_W - text_width - MARGIN_X, SCREEN_H - 25, 14, GRAY);

        EndDrawing();
    }

    CloseWindow();
    free(data);
}
