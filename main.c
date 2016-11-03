#include <stdio.h>
#include "memory.h"
#include "buffers.h"
#include "screen.h"
#include "windows.h"

int main()
{  
  setup_screen();
  initialise_buffers();
  initialise_windows();
 
  return 0;
}
