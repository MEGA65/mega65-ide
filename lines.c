/*
  Text line manipulation routines.

  This basically boils down to being able to read, modify, create and delete lines
  in text buffers.

  Of course this means that we need to work out which line termination character(s)
  we will use.  Ideally we should support them all, and use whatever a buffer already
  uses.  For simplicity for now, we will accept either CR or FL, and insert only CR
  for ease of cross-development and interoperability with UNIX-like systems.

*/

/*
  Find a given line in a buffer, returning its offset in the buffer
  Because memory is the main limiting factor, buffers are stored as linear slabs,
  necessitating a linear search.  We will in time cache the last found line, so that
  the search can continue in either direction from that point, instead of having to
  start from scratch.

  We could also automatically pre-calculate line addresses near to where the cursor is
  currently in a buffer, so that when a user does move around we already know where to,
  and the latency of the line search can be effectively hidden.

*/

#include "memory.h"
#include "buffers.h"
#include "screen.h"

unsigned char line_search_buffer[255];
unsigned char line_search_buffer_bytes;
unsigned char line_search_buffer_offset;
unsigned int line_offset_in_buffer,space_remaining;
unsigned char c;

unsigned char line_find(unsigned char buffer_id, unsigned int line_number)
{
  // Start from the beginning of the buffer, and search forward.
  line_offset_in_buffer=0;
  line_search_buffer_offset=255;
  line_search_buffer_bytes=0;
  
  while(line_number) {
    // Return failure if we have reached the end of the buffer
    if (line_offset_in_buffer>=buffers[buffer_id].length) return 0xffff;

    // Make sure we have some bytes to work with
    if (line_search_buffer_offset>=line_search_buffer_bytes) {
      // We need to read some more bytes from the buffer to search
      space_remaining=buffers[buffer_id].length-line_offset_in_buffer;
      if (space_remaining<255) c=space_remaining; else c=255;
      buffer_get_bytes(buffer_id,line_offset_in_buffer,c,line_search_buffer);
    }
    if (line_search_buffer_offset>=line_search_buffer_bytes) {
      display_footer(FOOTER_FATAL);
      for(;;) continue;
    }

    c=line_search_buffer[line_search_buffer_offset];
    line_search_buffer_offset++;
    line_offset_in_buffer++;
    if (c=='\n'||c=='\r') {
      // Found an end of line marker.
      line_number--;
    }
  }
  if (!line_number) return line_offset_in_buffer;
}
