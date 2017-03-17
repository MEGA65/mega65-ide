#include <stdio.h>
#include "memory.h"
#include "buffers.h"
#include "screen.h"
#include "windows.h"
#include "input.h"

void main(void)
{  
  // Set keyboard IRQ faster, so that cursor movement etc is faster
  // (approximately 2x normal speed)
  POKE(0xdc05U,0x20);

  setup_screen();
  initialise_windows();
  initialise_buffers();

  while(!poll_keyboard());

  // Exit nicely

  // 1MHz, 40 column
  c65_io_enable();
  POKE(0xd031,0);
  // VIC-II IO mode
  POKE(0xd02f,0);
  // Reset machine
  __asm__ ( "jmp $fce2" );

}
