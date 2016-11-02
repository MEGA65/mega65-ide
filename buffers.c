#include "memory.h"
#include "buffers.h"
#include "screen.h"

// Here is where buffers live
struct known_buffer *buffers = (struct known_buffer *)0x0400U;

unsigned char buffer_create(unsigned char *name)
{
  unsigned char i,j;
  unsigned char *f;
  
  // Look for a free buffer slot
  for(i=0;i<MAX_BUFFERS;i++) if (!buffers[i].filename[0]) break;
  // Fail if no free slots
  if (i==MAX_BUFFERS) {
    display_footer(FOOTER_NOMOREBUFFERS);
    return 0xff;
  }

  // Set file name in buffer
  f=buffers[i].filename;
  for(j=0;name[j];j++) f[j]=name[j];
    
  return i;
}

void initialise_buffers(void)
{
  unsigned char i;

  // Mark all buffers clean and empty
  lfill(0x00400U,0,1024);

  // Work out total buffer memory available
  total_buffer_memory=0;
  for(i=0;i<buffer_memory_segment_count;i++)
    total_buffer_memory+=buffer_memory_segment_lengths[i];

  // Allocate buffer that we will list all current buffers in
  buffer_create("*buffer-list*");
}
