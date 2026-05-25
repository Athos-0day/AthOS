#include <n7OS/cpu.h>
#include <stdio.h>

void processus1() {
  sti();
  printf("Hello, world from P1\n");
  for (;;)
    ;
}
