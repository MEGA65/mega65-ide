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

// Buffer for currently retrieved line
unsigned char line_buffer_bid=0xff;
unsigned int line_buffer_line_number=0xffff;
unsigned char line_buffer[255];
unsigned char line_buffer_length=0;
unsigned char line_buffer_original_length=0;

unsigned char line_search_buffer[255];
unsigned char line_search_buffer_bytes;
unsigned char line_search_buffer_offset;
unsigned int line_offset_in_buffer,space_remaining;
unsigned char c;

unsigned int line_find_offset(unsigned char buffer_id, unsigned int line_number)
{
  if (!buffers[buffer_id].loaded)
    if (buffer_load(buffer_id)) {
      display_footer(FOOTER_DISKERROR);
      return 0xffff;
    }

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

unsigned char line_fetch(unsigned char buffer_id, unsigned int line_number)  
{
  // Find the line in the buffer
  // XXX - Speed up with line offset cache of some sort
  line_offset_in_buffer=line_find_offset(buffer_id,line_number);
  if (line_offset_in_buffer==0xffff) return 0xff;

  // Read it into the buffer
  buffer_get_bytes(buffer_id,line_offset_in_buffer,255,line_buffer);

  for(line_buffer_length=0;
      (line_buffer_length<0xff)
	&&(line_buffer[line_buffer_length]!='\r')
	&&(line_buffer[line_buffer_length]!='\n');line_buffer_length++)
    continue;
  if (line_buffer_length==0xff) {
    display_footer(FOOTER_LINETOOLONG);
    return 0xff;
  }  
  
  // Remember original length of line, so that it can be used for easier writing-back
  // of modified lines.
  line_buffer_original_length=line_buffer_length;
  return 0x00;
}
