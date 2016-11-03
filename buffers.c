#include <stdio.h>
#include "memory.h"
#include "buffers.h"
#include "screen.h"

long buffer_space_free=0;

// Here is where buffers live
struct known_buffer *buffers = (struct known_buffer *)0x0400U;

void buffer_eject_from_memory(unsigned char bid)
{
  /* Eject the indicated buffer from memory.
     This simply consists of zeroing the allocation and recalculating free-space,
     unless the buffer is marked dirty, in which case we should save it to disk
     (perhaps after asking the user?) */
  if (buffers[bid].dirty) {
    buffer_save(bid);
  }
  // Zero buffer entry
  lfill((long)&buffers[bid],0,sizeof (struct known_buffer));
  // Recalculate free space, so that we can't accumulate calculation errors
  buffers_calculate_freespace();
}


void buffer_eject_other(unsigned char but_not_this_one)
{
  /*
    Eject a randomly selected buffer from memory and release its allocation.
    Try to eject non-dirty buffers first.

    XXX - Not at all random. We should make this random so that some buffers don't
    always get ejected. Raster line is probably a fine random variable to use to select
    the starting point in the buffer list.
  */
  unsigned char i,dirty;
  for(dirty=0;dirty<2;dirty++) 
    for(i=0;i<MAX_BUFFERS;i++)
      if (i!=but_not_this_one) {
	if (buffers[i].filename[0])
	  if (buffers[i].dirty==dirty)
	    if (buffers[i].resident_address_low||buffers[i].resident_address_high)
	      {
		// Here is a buffer that is not dirty, and is loaded into memory
		buffer_eject_from_memory(i);
		return;
	      }
      }
  // Nothing we could free, so return anyway.
}

void buffers_calculate_freespace(void)
{
  /* Work out how much free space we have in buffer memory given the current
     allocations.

     XXX - Check for overlapping allocations and complain
  */
  unsigned char bid;
  buffer_space_free=total_buffer_memory;
  for(bid=0;bid<MAX_BUFFERS;bid++) {
    buffer_space_free-=buffers[bid].allocated;
  }
  
}

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
  for(j=0;name[j]&&(j<16);j++) f[j]=name[j];
  for(;j<16;j++) f[j]=0;
    
  return i;
}

unsigned char buffer_allocate(unsigned char buffer_id, unsigned int size)
{
  /*
    Allocate size bytes to the specified buffer.  This may cause other buffers to
    be flushed to make space, and/or other buffers to be moved in buffer memory to
    make space.
  */

  long difference = size - buffers[buffer_id].length;
  if (difference < 0) {
    // Trying to allocate buffer to below its used size.
    return 0xff;
  }
  difference = size - buffers[buffer_id].allocated;
  // Return if the request is moot.
  if (difference == 0) return 0x00;
  if (difference < 0) {
    // Shrink allocation -- trivial

    buffers[buffer_id].allocated = size;
    buffer_space_free += difference;

    return 0x00;
  } else {
    // Grow allocation

    // Make space in RAM if required
    while (difference > buffer_space_free) {
      buffer_eject_other(buffer_id);
    }

    // For all buffers that start above the current one in memory, shift them
    // out of the way if necessary.

    // For all buffers that start below the current one in memory, shift them
    // out of the way if necessary.

    // Shift this buffer down if necessary.

    // Finally update allocation information
    difference = size - buffers[buffer_id].allocated;
    buffers[buffer_id].allocated = size;
    buffer_space_free -= difference;
    return 0x00;
  }
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
  buffer_space_free=total_buffer_memory;

  // Allocate buffer that we will list all current buffers in
  buffer_create("*buffer-list*");

  // For testing, try to load a buffer
  i=buffer_create("memory.h");
  buffer_load(i);
}

FILE *f=NULL;
unsigned char filename[2+16+1];
unsigned char data_buffer[256];
int r=0;
unsigned int file_offset;

unsigned char buffer_load(unsigned char buffer_id)
{
  // Initialise null-terminated filename
  lfill((long)filename,0,sizeof filename);
  lcopy((long)buffers[buffer_id].filename,(long)filename,16);

  file_offset=0;
  
  f=fopen(filename,"r");
  if (!f) return 0xff;
  
  while(!feof(f)) {
    r=fread(data_buffer,1,256,f);
    if (r>0) {
      // Draw progress in footline
      *(unsigned char *)(0xa000+(24*80)+(file_offset>>10))=r;
    }
  }
  fclose(f);
  return 0x00;
}

unsigned char buffer_save(unsigned char buffer_id)
{
}

