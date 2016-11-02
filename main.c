#include <stdio.h>
#include "memory.h"
#include "buffers.h"
#include "screen.h"

int main()
{
  // Enable C65 features
  POKE(0xd02fU,0xA5);
  POKE(0xd02fU,0x96);
  
  setup_screen();
  initialise_buffers();
  
  return 0;
}
