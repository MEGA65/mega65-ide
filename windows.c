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
#include "memory.h"
#include "screen.h"
#include "lines.h"
#include "windows.h"

unsigned char window_count=0;
struct window windows[MAX_WINDOWS];

void initialise_windows(void)
{
  window_count=0;
}

void set_single_window(unsigned char bid)
{
  window_count=1;
  windows[0].bid=bid;
  windows[0].x=0;
  windows[0].width=80;
  windows[0].xoffset=0;
  windows[0].first_line=0;
  windows[0].buffer_offset_of_first_line=0;
}

unsigned char w,l;
void draw_windows(void)
{
  c65_io_enable();
  
  // Clear screen RAM for window region
  lfill(SCREEN_ADDRESS,0x20,24*80);
  lfill(0x1f800,0x01,24*80);
  
  for(w=0;w<window_count;w++) {
    // Write buffer name in row 0
    struct window *win=&windows[w];

    unsigned int screen_line_address=SCREEN_ADDRESS;

    // Draw 23 lines from file
    for(l=0;l<23;l++) {
      screen_line_address+=80;
      if (line_fetch(win->bid,win->first_line)) {
	// Error fetching line -- draw as black line with blue full-stop in left column
	screen_colour_line(l+1,0);
	POKE(screen_line_address,'.');
	lfill(screen_line_address+1,' ',79);
      } else {
	// We have the line, so draw the appropriate segment in the appropriate place
	lcopy((long)line_buffer+win->xoffset,(long)screen_line_address,80);
	ascii_to_screen_80(screen_line_address,NORMAL_VIDEO);
	screen_colour_line(l+1,14);
      }
    }
  }
}
