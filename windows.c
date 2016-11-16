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

void window_prev_buffer(void)
{
  unsigned char old_bid=windows[current_window].bid;
  unsigned char bid=old_bid-1;

  while(bid!=old_bid) {
    if (bid==255) bid=MAX_BUFFERS-1;
    if (buffers[bid].filename[0]) {
      windows[current_window].bid=bid;
      // XXX - Get last edit point from buffer, instead of jumping to the top?
      windows[current_window].first_line=0;
      draw_window(current_window);
      return;
      }
    bid--;
  }
}

void window_next_buffer(void)
{
  unsigned char old_bid=windows[current_window].bid;
  unsigned char bid=old_bid+1;

  while(bid!=old_bid) {
    if (bid>=MAX_BUFFERS) bid=0;
    if (buffers[bid].filename[0]) {
      windows[current_window].bid=bid;
      // XXX - Get last edit point from buffer, instead of jumping to the top?
      windows[current_window].first_line=0;
      draw_window(current_window);
      return;
      }
    bid++;
  }
}

void window_ensure_cursor_in_window(unsigned char w)
{
  // Make sure cursor is still in window.
  unsigned char bid=windows[w].bid;
  unsigned int buffer_line=buffers[bid].current_line;
  unsigned char buffer_column=buffers[bid].current_line;
  unsigned char buffer_xoffset=buffers[bid].current_xoffset;
  unsigned int new_line = windows[w].first_line;

  // XXX - Assumes windows are fixed height of 23 lines
  
  // Fix line number 
  if (buffer_line<new_line) buffers[bid].current_line=new_line;
  if (buffer_line>(new_line+22)) buffers[bid].current_line=new_line+22;

  // XXX - Update column to ensure it is okay
}

void window_copy_up(unsigned char wid,unsigned char count)
{
  unsigned char i;
  unsigned char win_x=windows[wid].x;
  unsigned char win_width=windows[wid].width;
  
  long colour_address=COLOUR_RAM_ADDRESS+2*80;
  unsigned int screen_address=SCREEN_ADDRESS+2*80;

  colour_address+=win_x;
  screen_address+=win_x;

  // XXX Speed up further by working out correct delta and # of lines
  // to copy.
  c65_io_enable();
  while(count) {
    for(i=2;i<24;i++) {
      lcopy(screen_address,screen_address-80,win_width);
      lcopy(colour_address,colour_address-80,win_width);
      screen_address+=80;
      colour_address+=80;
    }
    --count;
  }
}

void window_copy_down(unsigned char wid,unsigned char count)
{
  unsigned char i;
  unsigned char win_x=windows[wid].x;
  unsigned char win_width=windows[wid].width;
  
  long colour_address=COLOUR_RAM_ADDRESS+22*80;
  unsigned int screen_address=SCREEN_ADDRESS+22*80;
  
  colour_address+=win_x;
  screen_address+=win_x;

  // XXX Speed up further by working out correct delta and # of lines
  // to copy.
  c65_io_enable();
  while(count) {
    for(i=2;i<24;i++) {
      lcopy(screen_address,screen_address+80,win_width);
      lcopy(colour_address,colour_address+80,win_width);
      screen_address-=80;
      colour_address-=80;
    }
    --count;
  }
}



void window_scroll(unsigned int count)
{
  // Scroll count lines down (or -count lines up) in window
  unsigned int buffer_lines=buffers[windows[current_window].bid].line_count;
  unsigned int buffer_line=buffers[windows[current_window].bid].current_line;  
  unsigned int new_line=windows[current_window].first_line+count;
  
  if (new_line<buffer_lines) {
    windows[current_window].first_line=new_line;

    window_ensure_cursor_in_window(current_window);

    if (count==-1) {
      // Speed up scrolling up by copying existing window
      window_copy_down(current_window,1);
      // Draw new line at top
      draw_window_line(current_window,0);
      
      // Cursor may have moved to bottom line, so draw if required
      draw_window_line_cursor(current_window,22);

      // Update title
      draw_window_title(current_window,1);
    } else if (count==1) {
      // Speed up scrolling down by copying existing window
      window_copy_up(current_window,1);
      draw_window_line(current_window,22);

      // Cursor may have moved to top line, so draw if required
      draw_window_line_cursor(current_window,0);
      
      // Update title
      draw_window_title(current_window,1);
    } else draw_window(current_window);
  }
}

void initialise_windows(void)
{
  window_count=0;
}

unsigned char window_default_widths[6][5]={
  {80,0,0,0,0},
  {40,40,0,0,0},
  {27,26,27,0,0},
  {20,20,20,20,0},
  {16,16,16,16,16}};

void window_select(unsigned char win_id)
{
  if (win_id>4) return;
  if (window_count<=win_id) {
    // Insufficient windows open
    unsigned char offset=0;
    unsigned char width;
    for(window_count=0;window_count<=win_id;window_count++) {
      width=window_default_widths[win_id][window_count];
      window_initialise(window_count,windows[window_count].bid,
			offset,width);
      offset+=width;
    }
    current_window=win_id;
    draw_windows();
  } else {
    // Don't redraw whole screen if we aren't changing the number of columns
    if (win_id!=current_window) {
      draw_window_title(win_id,1);
      draw_window_title(current_window,0);
      draw_window_all_cursors();
    }
  }
  current_window=win_id;
}

void set_single_window(unsigned char bid)
{
  current_window=0;
  window_initialise(0,bid,0,80);
  window_count=1;
}

void window_initialise(unsigned char wid,unsigned char bid,
		       unsigned char o,unsigned char w)
{
  windows[wid].bid=bid;
  windows[wid].x=o;
  windows[wid].width=w;
  windows[wid].xoffset=0;
  windows[wid].first_line=0;
  windows[wid].buffer_offset_of_first_line=0;
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

  draw_window_title(w_in,w_in==current_window);

  
  // Draw 23 lines from file
  for(l=0;l<23;l++) draw_window_line(w,l);
}

void draw_window_title(unsigned char w_in, unsigned char activeP)
{
  w=w_in;
  // Draw window title
  
  // Start with a blank line
  lfill((long)window_title_buffer,' ',80);
  
  // Work out length of filename
  for(l=0;(l<16)&&buffers[windows[w].bid].filename[l];l++) continue;
  
  // Put line number  
  screen_decimal((long)window_title_buffer+l+1,windows[w].first_line,NORMAL_VIDEO);
  
  // Put the file name
  lcopy((long)buffers[windows[w].bid].filename,(long)window_title_buffer,l);
  window_title_buffer[l]=' ';
  
  ascii_to_screen_segment(window_title_buffer,80,REVERSE_VIDEO);
  lcopy((long)window_title_buffer,SCREEN_ADDRESS+windows[w].x,
	windows[w].width-1);

  // Change colour of header based on whether we are the active window or not
  if (activeP) {
    // white
    lfill(COLOUR_RAM_ADDRESS+windows[w].x,1,windows[w].width-1);
  } else {
    // medium grey
    lfill(COLOUR_RAM_ADDRESS+windows[w].x,12,windows[w].width-1);
  }
}

void draw_window_all_cursors(void)
{
  unsigned char w_id;
  for(w_id=0;w_id<window_count;w_id++)
    if (w_id!=current_window)
      draw_window_update_cursor(w_id);
}

/* Redraw the cursor in this window.
   If it is the active window, then draw it normal (yellow+blinking).
   If not the active window, then it shouldn't blink, and maybe should
   be a different colour? 
*/
void draw_window_update_cursor(unsigned char w_in)
{
  // Work out if cursor is visible
  struct window *win=&windows[w_in];
  int cursor_line=buffers[win->bid].current_line-win->first_line;
  unsigned char cursor_position=buffers[win->bid].current_column-win->xoffset+win->x;
  long cursor_address=COLOUR_RAM_ADDRESS+(cursor_line+1)*80+cursor_position;
  if (cursor_line<0||cursor_line>22) return;
  if (cursor_position>=win->width) return;
  if (w_in==current_window)
    lpoke(cursor_address,ATTRIB_REVERSE+ATTRIB_BLINK+COLOUR_YELLOW);
  else
    lpoke(cursor_address,ATTRIB_REVERSE+COLOUR_YELLOW);
}

void draw_window_line_cursor(unsigned char w_in, unsigned char l_in)
{
  unsigned int screen_line_address=SCREEN_ADDRESS+80;
  struct window *win=&windows[w_in];
  w=w_in; l=l_in;

  screen_line_address+=80*l;

  if ((l+win->first_line)==buffers[win->bid].current_line)
    {
    int cursor_position=buffers[win->bid].current_column-win->xoffset;
    if ((cursor_position>=0)&&(cursor_position<win->width))
      // Draw cursor using VIC-III enhanced attributes
      lpoke(screen_line_address+COLOUR_RAM_ADDRESS-SCREEN_ADDRESS+win->x
	   +cursor_position,ATTRIB_REVERSE+ATTRIB_BLINK+COLOUR_YELLOW);
    }
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
    screen_colour_line_segment(screen_line_address+win->x,win->width-1,COLOUR_BLACK);
    POKE(screen_line_address+win->x,'.');
    lfill(screen_line_address+win->x+1,' ',win->width-2);
  } else {
    // We have the line, so draw the appropriate segment in the appropriate place
    lcopy((long)line_buffer+win->xoffset,
	  (long)screen_line_address+win->x,win->width-1);
    ascii_to_screen_segment((unsigned char *)(screen_line_address+win->x),
			    win->width,NORMAL_VIDEO);
    screen_colour_line_segment(screen_line_address+win->x,win->width-1,
			       COLOUR_LIGHTBLUE);	
  }
  
  draw_window_line_cursor(w_in,l_in);
  
  // Draw border character (white | )
  // XXX - It would be nice to have a scroll-bar type indication here as well.
  POKE(screen_line_address+win->x+win->width-1,0x5d); // vertical line
  lpoke(screen_line_address+COLOUR_RAM_ADDRESS-SCREEN_ADDRESS+win->x+win->width-1,
	COLOUR_WHITE);
}
