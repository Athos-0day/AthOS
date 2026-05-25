#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <inttypes.h>
#include <n7OS/time.h>

/* Width of the screen in text mode (columns) */
#define VGA_WIDTH 80
/* Height of the screen in text mode (rows) */
#define VGA_HEIGHT 25

/* Base address of VGA text buffer in memory */
#define SCREEN_ADDR 0xB8000

#define PORT_CMD 0x3D4
#define PORT_DATA 0x3D5

#define CMD_HIGH 0xE
#define CMD_LOW 0xF

/* VGA Color Definitions */
#define BLACK 0x0
#define BLUE 0x1
#define GREEN 0x2
#define CYAN 0x3
#define RED 0x4
#define PURPLE 0x5
#define BROWN 0x6
#define GRAY 0x7
#define D_GRAY 0x8
#define L_BLUE 0x9
#define L_GREEN 0xA
#define L_CYAN 0xB
#define L_RED 0xC
#define L_PURPLE 0xD
#define YELLOW 0xE
#define WHITE 0xF

/* Character Attribute Configuration */
#define BLINK (0 << 7)
#define BACK (BLACK << 4)
#define TEXT GREEN
#define CHAR_COLOR (BLINK | BACK | TEXT)

/**
 * @brief Initializes the console subsystem.
 */
void init_console();

/**
 * @brief Outputs a byte to the screen.
 *
 * @param c Character to write
 */
void console_putchar(const char c);

/**
 * @brief Outputs a sequence of bytes to the screen.
 *
 * @param s Pointer to the character buffer.
 * @param len Number of bytes to write.
 */
void console_putbytes(const char *s, int len);

/**
 * @brief Updates the hardware cursor to the current position 'pos'.
 */
void console_update_cursor();

/**
 * @brief Clears the screen and resets the current position to the beginning.
 * * @param start_row The row index from which to start clearing.
 */
void console_clear_screen(uint8_t start_row);

/**
 * @brief Manages and computes the screen position.
 * Prevents row or column overflow and triggers a scroll
 * if the end of the screen is reached.
 */
void console_update_pos();

/**
 * @brief Outputs a tabulation without moving to the next row.
 */
void console_put_tab();

/**
 * @brief Erases the first row and shifts content to create a new row at the
 * bottom.
 */
void console_scroll();

/**
 * @brief Displays the time every second.
 * @param time The time structure to be displayed.
 */
void print_heure(heure_t heure);

/**
 * @brief Displays the console header.
 */
void print_header();

/**
 * @brief Displays process1 special message with a specific color.
 *
 * @param color_index Index of the color to use in the palette.
 */
void print_process1_message(uint8_t color_index);

#endif