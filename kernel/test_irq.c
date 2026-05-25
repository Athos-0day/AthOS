/**
 * @file test_irq.c
 * @author Arthur
 * @brief Test de la gestion des interruptions.
 * @version 0.1
 * @date 2026-04-10
 *
 * @copyright Copyright (c) 2026
 *
 */

#include <n7OS/console.h>
#include <n7OS/cpu.h>
#include <n7OS/irq.h>
#include <n7OS/keyboard.h>
#include <n7OS/process.h>
#include <n7OS/time.h>
#include <stdio.h>

extern void handler_IT();
extern void handler_IT50();
extern void handler_IT32();
extern void handler_IT33();

extern char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
extern uint8_t keyboard_buffer_index_start;
extern uint8_t keyboard_buffer_index_end;
extern uint8_t shift_state;

/**
 * @brief Initialise les handlers d'interruption.
 *
 */
void init_irq() {
  init_irq_entry(50, (uint32_t)handler_IT50);
  init_irq_entry(32, (uint32_t)handler_IT32);
  init_irq_entry(33, (uint32_t)handler_IT33);
}

void handler_en_C() {}

/**
 * @brief Handler de l'IT 50, qui affiche un message de confirmation de la
 * réception de l'IT.
 *
 */
void handler_it50() { printf("J'ai bien recu l'interruption 50\n"); }

/**
 * @brief Handler de l'IT 32, qui gère la réception de l'IT.
 *
 */
void handler_it32() {
  // Désactivation de l'IT
  outb(inb(PICDATA) | 1, PICDATA);

  // Ack de l'IT au PIC
  outb(0x20, 0x20);

  // Incrémentation du compteur système
  incremente_timer();

  // MaJ de l'Affichage
  display_time();

  // Activation de l'IT
  outb(inb(PICDATA) & 0xFE, PICDATA);

  // Ordonnancement
  if (get_timer() % 100u == 0u) {
    scheduler();
  }
}

/**
 * @brief Handler de l'IT 33, qui gère l'interruption clavier.
 *
 */
void handler_it33() {

  // Lecture du code
  char c = '\0';
  uint8_t code_key = inb(KEYB_ENC_CMD_REG);

  // On gère les touches de shift
  if (code_key == SHIFT_PRESSED || code_key == SHIFT_RELEASED) {
    shift_state = code_key == SHIFT_PRESSED ? 1 : 0;
  } else if (code_key < 0x80) {
    // On convertit le scancode en caractère
    c = shift_state ? scancode_map_shift[code_key] : scancode_map[code_key];

    // On calcule le prochain index du buffer
    uint8_t next_index = (keyboard_buffer_index_end + 1) % KEYBOARD_BUFFER_SIZE;

    // On vérifie que le buffer n'est pas plein avant d'écrire
    if (next_index != keyboard_buffer_index_start) {
      keyboard_buffer[keyboard_buffer_index_end] = c;
      keyboard_buffer_index_end = next_index;
    }
  }

  // test pour debug
  if ((c != "\0") && (code_key < 0x80)) {
    console_putchar(c);
  }
  // Acquittement de l'IT au PIC
  outb(0x20, 0x20);
}
