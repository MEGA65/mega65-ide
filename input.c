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
  C= B (191) - Switch to previous possible buffer in this window
  C= N (191) - Switch to next possible buffer in this window
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
#include "windows.h"

unsigned char key;
unsigned char last_key;
unsigned char quit_counter=0;
unsigned char poll_keyboard(void)
{  
  key=cgetc();
  POKE(SCREEN_ADDRESS,key);
  if (key!=171) quit_counter=0;
  else { if ((++quit_counter)>2) return 0xff; }

  switch(key) {
  case 129: // Switch to window 1, or close other windows
    if (last_key==129) { set_single_window(0); draw_windows(); }
    else window_select(0);
    break;
  case 149: // Switch to window 2, creating it if necessary
    window_select(1);
    break;
  case 150: case 151: case 152: // Switch to window 3-5
    window_select(key-150+2);
    break;
  case 191: // Change buffer in current window
    window_prev_buffer();
    break;
  case 170: // Change buffer in current window
    window_next_buffer();
    break;    
  case 17: // cursor down
    window_scroll(1);
    break;
  case 145: // cursor up
    window_cursor_up();
    break;
  case 157: // cursor left
    window_cursor_left();
    break;
  case 29: // cursor right
    window_cursor_right();
    break;
  case 1: // cursor to start of line
    window_cursor_start_of_line();
    break;
  case 5: // cursor to end of line
    window_cursor_end_of_line();
    break;
  }
  last_key=key;
  return 0;
}
