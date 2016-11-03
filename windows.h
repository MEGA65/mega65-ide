
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

void initialise_windows(void);
void set_single_window(unsigned char bid);
