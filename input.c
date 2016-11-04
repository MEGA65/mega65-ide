/*
  Process user input.

  For now, we are using the kernal keyboard routines. However, as we are in C64 mode,
  they will not scan the extra C65 keys. Also, the kernal routines don't allow 3-key
  roll-over (for fast typists).  Thus we should probably adapt the following C128
  3-key rollover routine to run in C64 mode, and scan the extra C65 keys, and possibly
  adapt it to give ASCII codes rather than PETSCII codes, and support multiple
  simultaneous modifier keys:

  http://codebase64.org/doku.php?id=magazines:chacking6#three-key_rollover_for_the_c-128_and_c-64

  But for now, we will just use the kernal routines through cc65's library.

  Key layout we are using:

  // Management of windows
  C= 1 (129) - Window 1 (or close other windows if pressed twice)
  C= 2 (149) - Window 2 (create if not currently enabled)
  C= 3 (150) - Window 3 (create if not currently enabled)
  C= 4 (151) - Window 4 (create if not currently enabled)
  C= 5 (152) - Window 5 (create if not currently enabled)
  C= - (220) - Widen current window
  SHIFT - (221) - Reduce width of current window by 1

  C= Q (171) - Press 3 times to quit without saving

  // Usual cursor navigation stuff
  DOWN (17) - move cursor down
  UP (145) - move cursor up

  ...


*/
#include <stdio.h>
#include <conio.h>
#include "memory.h"
#include "screen.h"

unsigned char key;
unsigned char quit_counter=0;
unsigned char poll_keyboard(void)
{  
  key=cgetc();
  POKE(SCREEN_ADDRESS,key);
  if (key!=171) quit_counter=0;
  else { if ((++quit_counter)>2) return 0xff; }
  return 0;
}
