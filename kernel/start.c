#include <../include/debug.h>
#include <inttypes.h>
#include <n7OS/console.h>
#include <n7OS/cpu.h>
#include <n7OS/irq.h>
#include <n7OS/keyboard.h>
#include <n7OS/kheap.h>
#include <n7OS/mem.h>
#include <n7OS/paging.h>
#include <n7OS/process.h>
#include <n7OS/processor_structs.h>
#include <n7OS/sys.h>
#include <n7OS/time.h>

extern void init_irq();

void kernel_start(void) {
  // initialisation de la pile du kernel
  init_kheap();
  // initialisation de la pagination
  initialise_paging();
  // initialisation de la console
  init_console();
  // initialisation des appels systemes
  init_syscall();
  // initialisation des interruptions
  init_irq();
  // initialisation de timer
  init_timer();
  // initialisation du clavier
  init_keyboard();
  // lancement des interruptions
  sti();

  /**
  // test de la pagination
  alloc_page_entry(0xA0000000,1,0);
  uint32_t *ptr = (uint32_t *) 0xA0000000;
  (*ptr)++;
  */

  // test de l'interruption 50
  //__asm__ __volatile__("int $50" ::);

  init_processes();
}
