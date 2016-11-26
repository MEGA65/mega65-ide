#define UI_DISABLE_CURSOR 0x80

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
void window_cursor_up(void);
void window_cursor_down(void);
void window_cursor_left(void);
void window_cursor_right(void);
void window_cursor_start_of_line(void);
void window_cursor_end_of_line(void);
unsigned char window_ensure_cursor_in_window(unsigned char w);

void draw_windows(void);
void draw_window(unsigned char w);
void draw_window_title(unsigned char w_in, unsigned char activeP);
void draw_window_line(unsigned char w_in, unsigned char l_in);
void draw_window_line_attributes(unsigned char w_in, unsigned char l_in);
void draw_window_all_cursors(void);
void draw_window_update_cursor(unsigned char w_in);
void redraw_current_window_line(void);

unsigned char ui_notbusy_with_result(unsigned char value);
void ui_busy(void);


