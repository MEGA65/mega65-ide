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
#include "buffers.h"

unsigned char window_count=0;
unsigned char current_window=0;
struct window windows[MAX_WINDOWS];

void initialise_windows(void)
{
  window_count=0;
}

void set_single_window(unsigned char bid)
{
  current_window=0;
  window_count=1;
  windows[0].bid=bid;
  windows[0].x=0;
  windows[0].width=80;
  windows[0].xoffset=0;
  windows[0].first_line=0;
  windows[0].buffer_offset_of_first_line=0;
}

unsigned char w,l;
unsigned char window_title_buffer[80];
void draw_windows(void)
{
  c65_io_enable();
  
  // Clear screen RAM for window region
  lfill(SCREEN_ADDRESS,0x20,24*80);
  lfill(0x1f800,0x01,24*80);
  
  for(w=0;w<window_count;w++) draw_window(w);
}
    
void draw_window(unsigned char w_in)
{
  // Write buffer name in row 0
  struct window *win=&windows[w_in];

  w=w_in;
  
  // Draw window title
  
  // Start with a blank line
  lfill((long)window_title_buffer,' ',80);
  
  // Work out length of filename
  for(l=0;(l<16)&&buffers[windows[w].bid].filename[l];l++) continue;
  
  // Put 4-digit line number
  // (we are using 6-digit hex routine for now, so position it left a bit,
  // and let the filename overwrite it).
  // XXX Line number won't show for buffers with names <1 chars long
  if (l) screen_hex((long)window_title_buffer+l-1,windows[w].first_line);
  
  // Put the file name
  lcopy((long)buffers[windows[w].bid].filename,(long)window_title_buffer,l);
  window_title_buffer[l]=' ';
  
  ascii_to_screen_segment(window_title_buffer,80,REVERSE_VIDEO);
  lcopy((long)window_title_buffer,SCREEN_ADDRESS+windows[w].x,
	windows[w].width-1);    
  
  // Draw 23 lines from file
  for(l=0;l<23;l++) draw_window_line(w,l);
}

void draw_window_line(unsigned char w_in, unsigned char l_in)
{
  // Skip the top line which shows the name of the file
  unsigned int screen_line_address=SCREEN_ADDRESS+80;
  struct window *win=&windows[w_in];
  w=w_in; l=l_in;

  screen_line_address+=80*l;
  
  if (line_fetch(win->bid,win->first_line+l)) {
    // Error fetching line -- draw as black line with blue full-stop in left column
    screen_colour_line_segment(screen_line_address+win->x,win->width-1,0);
    POKE(screen_line_address+win->x,'.');
    lfill(screen_line_address+win->x+1,' ',win->width-2);
  } else {
    // We have the line, so draw the appropriate segment in the appropriate place
    lcopy((long)line_buffer+win->xoffset,
	  (long)screen_line_address+win->x,win->width-1);
    ascii_to_screen_segment((unsigned char *)(screen_line_address+win->x),
			    win->width,NORMAL_VIDEO);
    screen_colour_line_segment(screen_line_address+win->x,win->width-1,14);	
  }
  // Draw border character (reverse 
  POKE(screen_line_address+win->x+win->width-1,0x5d); // vertical line
  lpoke(screen_line_address+COLOUR_RAM_ADDRESS-SCREEN_ADDRESS+win->x+win->width-1,1);
}
