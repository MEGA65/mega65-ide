/*
  Display window functions.

  We support multiple display windows open at a time.

  For now, windows must be the full-height of the display.  That is, when multiple
  files are visible, they appear side-by-side.  This is to support intuitive display
  of stack traces when debugging a running program.

  Each window consists of the lines of text, together with a border column on the
  right hand side.  Eventually we might show a scroll-bar in this column for added
  value.  If the user wants to know the line number they are editing, they should
  look to the footer line, where we intend to show this information whenever the 
  cursor is moved.

  As we support lines of upto 254 characters in length, windows can be scrolled side-
  ways by moving the cursor left and right (or using various accelerated movements).

  At the top of the screen, each window will have the name of the buffer listed.
  
*/

#include "windows.h"

struct window windows[MAX_WINDOWS];

 
