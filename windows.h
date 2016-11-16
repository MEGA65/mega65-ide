
struct window {
  // Buffer being displayed
  unsigned char bid;
  // location and width of the current window
  unsigned char x;
  unsigned char width;
  // Offset from left of file being displayed
  unsigned char xoffset;
  // Which line is the first displayed on the screen
  unsigned int first_line;
  // For faster finding of where we are in the buffer
  unsigned int buffer_offset_of_first_line;
};
#define MAX_WINDOWS 5

extern unsigned char current_window;

void initialise_windows(void);
void window_initialise(unsigned char wid,unsigned char bid,
		       unsigned char o,unsigned char w);
void set_single_window(unsigned char bid);
void window_select(unsigned char win_id);
void window_prev_buffer(void);
void window_next_buffer(void);
void window_scroll(unsigned int count);

void draw_windows(void);
void draw_window(unsigned char w);
void draw_window_title(unsigned char w_in, unsigned char activeP);
void draw_window_line(unsigned char w_in, unsigned char l_in);
void draw_window_line_cursor(unsigned char w_in, unsigned char l_in);
void draw_window_all_cursors(void);
void draw_window_update_cursor(unsigned char w_in);


