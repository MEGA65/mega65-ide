#include <stdio.h>
#include "memory.h"
#include "buffers.h"
#include "screen.h"
#include "windows.h"

int main()
{  
  setup_screen();
  initialise_windows();
  initialise_buffers();

  return 0;
}
