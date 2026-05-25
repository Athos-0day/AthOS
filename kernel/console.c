#include <../include/debug.h>
#include <n7OS/console.h>
#include <n7OS/cpu.h>
#include <unistd.h>

/* Variable to stock the screen adress */
uint16_t *scr_tab;
/* Actual row in the screen */
uint16_t row;
/* Actual column in the screen */
uint16_t column;

void init_console() {
  // Set the pointer of the screen
  scr_tab = (uint16_t *)SCREEN_ADDR;
  // Clear the screen
  console_clear_screen(0u);
  // Print the header
  print_header();
  // Init of row and column
  row = 3u;
  column = 0u;
  // Set the cursor position
  console_update_cursor();
}

void console_clear_screen(uint8_t start_row) {
  uint16_t cursor_pos;

  for (uint8_t nrow = start_row; nrow < VGA_HEIGHT; nrow++) {
    for (uint8_t ncolumn = 0u; ncolumn < VGA_WIDTH; ncolumn++) {
      cursor_pos = (nrow * VGA_WIDTH) + ncolumn;
      // Replace the actual value by a space
      scr_tab[cursor_pos] = CHAR_COLOR << 8 | ' ';
    }
  }
}

void console_update_cursor() {
  // Compute the current position
  uint16_t cursor_pos = (row * VGA_WIDTH) + column;
  // Update the hardware cursor
  outb(CMD_LOW, PORT_CMD);
  outb(cursor_pos & 255, PORT_DATA);
  outb(CMD_HIGH, PORT_CMD);
  outb(cursor_pos >> 8, PORT_DATA);
}

void console_scroll() {
  uint16_t screen_pos;

  // Move up all the row and erase the first one
  for (uint8_t nrow = 3u; nrow < VGA_HEIGHT; nrow++) {
    for (uint8_t ncolumn = 0u; ncolumn < VGA_WIDTH; ncolumn++) {
      screen_pos = ((VGA_WIDTH * nrow) + ncolumn);

      // Put space at the last line
      if (nrow == VGA_HEIGHT - 1) {
        scr_tab[screen_pos] = CHAR_COLOR << 8 | ' ';
      }
      // Otherwise we move the character
      else {
        scr_tab[screen_pos] = scr_tab[((VGA_WIDTH * (nrow + 1)) + ncolumn)];
      }
    }
  }
  // New values
  row = VGA_HEIGHT - 1;
  column = 0u;
}

void console_update_pos() {
  // First we check the current column
  if (column == VGA_WIDTH) {
    row++;
    column = 0u;
  }
  // Then we check the current row
  if (row == VGA_HEIGHT) {
    console_scroll();
  }
}

void console_put_tab() {
  uint8_t i = 0u;

  while ((i < 8u) && (column != VGA_WIDTH)) {
    console_putchar(' ');
    i++;
  }
}

void console_putchar(const char c) {
  // Compute the current position in the screen
  uint16_t current_pos = (VGA_WIDTH * row) + column;

  // ASCII Character
  if (((uint32_t)c > 31u) && ((uint32_t)c < 127u)) {
    scr_tab[current_pos] = CHAR_COLOR << 8 | c;
    column++;
  }

  // Line Feed
  else if (c == '\n') {
    row++;
    column = 0u;
  }

  // Carriage Return
  else if (c == '\r') {
    column = 0u;
  }

  // Horizontal Tab
  else if (c == '\t') {
    console_put_tab();
  }

  // Backspace
  else if (c == '\b') {
    // Check if there is something to erase on the current row
    if (column > 0u) {
      if (row > 3 || (row == 3 && column > 0)) {
        scr_tab[current_pos - 1u] = CHAR_COLOR << 8 | ' ';
        column--;
      }
    }
    // Otherwise we move up to the upper row
    else if (row > 0u) {
      row--;
      column = VGA_WIDTH - 1u;
      current_pos = (VGA_WIDTH * row) + column;
      scr_tab[current_pos] = CHAR_COLOR << 8 | ' ';
    }
  }

  // Form Feed
  else if (c == '\f') {
    // Erase the whole screen
    console_clear_screen(3u);
    // Update values
    row = 3u;
    column = 0u;
  }
  // Update the position
  console_update_pos();
  // Update the cursor
  console_update_cursor();
}

void console_putbytes(const char *s, int len) {
  for (int i = 0; i < len; i++) {
    console_putchar(s[i]);
  }
}

void print_header() {
  const char *message = "Your favorite OS is running";
  row = 0u;
  column = 0u;
  for (uint8_t ind = 0u; ind < 27u; ind++) {
    console_putchar(message[ind]);
  }

  heure_t start_time = {0, 0, 0};
  print_heure(start_time);

  row = 2u;
  column = 0u;
  for (uint8_t i = 0u; i < VGA_WIDTH; i++) {
    console_putchar('-');
  }
}

static void put_time(char *time_buffer, heure_t heure) {
  const char *prefix = "uptime: ";
  for (int i = 0; i < 8; i++) {
    time_buffer[i] = prefix[i];
  }

  // Heures
  time_buffer[8] = '0' + ((heure.hour % 100) / 10);
  time_buffer[9] = '0' + (heure.hour % 10);
  time_buffer[10] = ':';
  // Minutes
  time_buffer[11] = '0' + (heure.min / 10);
  time_buffer[12] = '0' + (heure.min % 10);
  time_buffer[13] = ':';
  // Secondes
  time_buffer[14] = '0' + (heure.sec / 10);
  time_buffer[15] = '0' + (heure.sec % 10);

  time_buffer[16] = '\0';
}

void print_heure(heure_t heure) {
  char time_buf[17];
  put_time(time_buf, heure);

  // Calcul de la colonne de départ (80 - 16 = 64)
  uint8_t start_col = VGA_WIDTH - 16;

  for (uint8_t i = 0; i < 16; i++) {
    uint16_t pos = (0 * VGA_WIDTH) + (start_col + i);
    scr_tab[pos] = (uint16_t)(CHAR_COLOR << 8) | time_buf[i];
  }
}

void print_process1_message(uint8_t color_index) {
  const char *message = "Hello, world from P1";
  static const int couleurs[] = {BLUE,   GREEN, CYAN,     RED,    PURPLE,
                                 BROWN,  GRAY,  D_GRAY,   L_BLUE, L_GREEN,
                                 L_CYAN, L_RED, L_PURPLE, YELLOW, WHITE};

  const uint8_t taille = sizeof(couleurs) / sizeof(couleurs[0]);
  const uint16_t row_proc1 = 1u;

  if (color_index >= taille) {
    return;
  }

  for (uint8_t ind = 0u; ind < 20u; ind++) {
    uint16_t pos = (row_proc1 * VGA_WIDTH) + ind;
    scr_tab[pos] =
        (uint16_t)((BLINK | BACK | couleurs[color_index]) << 8) | message[ind];
  }
}