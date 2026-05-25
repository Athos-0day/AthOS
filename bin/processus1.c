#include <n7OS/console.h>

void processus1() {
  uint32_t loop_count = 0u;
  uint8_t color_index = 0u;
  static const uint8_t palette_size = 15u;

  print_process1_message(color_index);

  while (1) {
    loop_count++;

    if ((loop_count % 10000u) == 0u) {
      color_index = (uint8_t)((color_index + 1u) % palette_size);
      loop_count = 0u;
      print_process1_message(color_index);
    }
  }
}
