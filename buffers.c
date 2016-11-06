#include <stdio.h>
#include "memory.h"
#include "buffers.h"
#include "screen.h"
#include "windows.h"

long buffer_space_free=0;
long buffer_first_free_byte=0;

// Here is where buffers live
struct known_buffer *buffers = (struct known_buffer *)BUFFER_LIST_BASE;

void buffer_eject_from_memory(unsigned char bid)
{
  /* Eject the indicated buffer from memory.
     This simply consists of zeroing the allocation and recalculating free-space,
     unless the buffer is marked dirty, in which case we should save it to disk
     (perhaps after asking the user?) */
  if (buffers[bid].dirty) {
    if (buffer_save(bid)) {
      display_footer(FOOTER_DISKERROR);
      return;
    }
  }
  buffers[bid].loaded=0;

  // Recalculate free space, so that we can't accumulate calculation errors
  buffers_calculate_freespace();
}

// Destroy buffer without first trying to save to disk!
void buffer_destroy(unsigned char bid)
{  
  // Zero buffer entry
  lfill((long)&buffers[bid],0,sizeof (struct known_buffer));
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


/*
  Relocate a buffer from its current location to a new one. 
  Typically this is called when compacting memory to upate buffer_first_free_byte,
  or when loading a buffer requires the ejection of other buffers.

  Main trick here is making sure we don't corrupt things as we go.
*/
void buffer_relocate(unsigned char bid,long new_resident_address)
{
  long current_resident_address=(((long)buffers[bid].resident_address_high)<<16)
	  +buffers[bid].resident_address_low;
  long difference=new_resident_address-current_resident_address;
  if (!difference) return;
  if (difference<0) {
    // Move upwards in memory
  } else {
    // Move downwards in memory
  }
}

void buffers_calculate_freespace(void)
{
  /* Work out how much free space we have in buffer memory given the current
     allocations.

     XXX - Check for overlapping allocations and complain.

     XXX - This routine is also required to calculate buffer_first_free_byte, and
     thus to compact all existing buffers down.

  */
  unsigned char bid,best_bid;
  long best_buffer_address;
  buffer_space_free=total_buffer_memory;
  for(bid=0;bid<MAX_BUFFERS;bid++) {
    buffer_space_free-=buffers[bid].allocated;
  }

  /*
    We want to start at the beginning of buffer space, and find the 
    buffer which starts closest to this point, but not before it.
    If the buffer starts after the point, it should get shuffled down.
    After this potentially shuffling, we should advance buffer_first_free_byte
    to the end of that allocation. We repeat this until there is no more buffers.
    At this point, buffer_first_free_byte will be valid, and all buffers will have
    been compacted down.
  */

  buffer_first_free_byte=0;
  bid=1;
  while(bid!=MAX_BUFFERS) {
    best_bid=0xff;
    best_buffer_address=0x7fffffff;
    for(bid=0;bid<MAX_BUFFERS;bid++) {
      if (buffers[bid].loaded) {
	long buffer_address=(((long)buffers[bid].resident_address_high)<<16)
	  +buffers[bid].resident_address_low;
	if (buffer_address>=buffer_first_free_byte)
	  if (buffer_address<best_buffer_address) {
	    best_bid=bid;
	    best_buffer_address=buffer_address;
	  }
      }      
    }
    if (best_bid==0xff)
      // all done
      break;
    if (best_buffer_address>buffer_first_free_byte) {
      // Shuffle down
      buffer_relocate(bid,best_buffer_address);
    }
    // Advance search beyond this buffer
    buffer_first_free_byte+=buffers[bid].allocated;
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
    // (.length must be reduced first)
    return 0xff;
  }

  if (!buffers[buffer_id].allocated) {
    // No prior allocation exists - find first free space

    // First pack all buffers down
    buffers_calculate_freespace();

    // Then we can just use first free address
    buffers[buffer_id].resident_address_low=buffer_first_free_byte&0xffffU;
    buffers[buffer_id].resident_address_high=buffer_first_free_byte>>16;
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
  lfill(BUFFER_LIST_BASE,0,BUFFER_LIST_TOP-BUFFER_LIST_BASE+1);

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
  i=buffer_create("memory.c");
  buffer_load(i);
  i=buffer_create("buffers.c");
  buffer_load(i);

  set_single_window(i);
  draw_windows();
  
  i=buffer_create("screen.c");
  buffer_load(i);

}

unsigned int count_this_segment;
long buffer_memory_offset;
long real_memory_offset;
void buffer_getset_bytes(unsigned char buffer_id,unsigned int offset, unsigned int count,
			 unsigned char *data, unsigned char getP)
{  
  while(count) {
    
    // Work out where in the buffer memory space we need to put the next byte
    buffer_memory_offset = buffers[buffer_id].resident_address_low;
    buffer_memory_offset |= ((long)buffers[buffer_id].resident_address_high)<<16;
    buffer_memory_offset += offset;

    // Now work out where that is in real memory
    real_memory_offset = buffer_address_to_real(buffer_memory_offset);

    if (!real_memory_offset) {
      // Something bad has happened.
      FATAL_ERROR;
    }
    
    // And how many bytes we can contigously write
    count_this_segment = buffer_address_contiguous_bytes(buffer_memory_offset);
    if (count_this_segment > count) count_this_segment = count;

    // Copy the bytes
    if (getP)
      lcopy(real_memory_offset,(long)data,count_this_segment);
    else
      lcopy((long)data,real_memory_offset,count_this_segment);

    // See what is left, and update pointers to data, so that data can be copied
    // over buffer segment boundaries.
    offset+= count_this_segment;
    count -= count_this_segment;
    data += count_this_segment;
  }  
}

FILE *f=NULL;
FILE *savef=NULL;
unsigned char string_loading[7]="Reading";
unsigned char string_saving[8]="Flushing";
unsigned char filename[2+16+1];
unsigned char data_buffer[256];
int r=0;
unsigned int file_offset;
unsigned int new_offset;

unsigned char buffer_load(unsigned char buffer_id)
{
  buffers[buffer_id].loaded=0;

  if (!buffers[buffer_id].filename[0]) return 0xff;
  
  if (buffers[buffer_id].filename[0]=='*') {
    // Pseudo buffer.  If thown away, it just becomes empty
    buffers[buffer_id].length=0;
    buffer_allocate(buffer_id,0);
    return 0x00;
  }
  
  // Initialise null-terminated filename
  lfill((long)filename,0,sizeof filename);
  lcopy((long)buffers[buffer_id].filename,(long)filename,16);

  file_offset=0;

  display_footer(FOOTER_BLANK);
  lcopy((long)string_loading,FOOTER_ADDRESS,7);
  for(r=0;filename[r];r++) *(unsigned char *)(FOOTER_ADDRESS+7+1+r)=filename[r];
    
  mungedascii_to_screen_80((unsigned char *)FOOTER_ADDRESS,REVERSE_VIDEO);
  
  f=fopen(filename,"r");
  if (!f) return 0xff;
  
  while(!feof(f)) {
    r=fread(data_buffer,1,256,f);
    if (r>0) {
      new_offset=file_offset+r;
      if ((new_offset<file_offset)||(new_offset==0xffff)) {
	/* File offset is >65534, which is not allowed.
	   (65535 is reserved for returning an error from functions
	   that return the offset within a buffer) */       
	fclose(f);
	buffer_allocate(buffer_id,0);
	display_footer(FOOTER_BUFFERTOOBIG);
	
	return 0xff;
      }

      // Show hex file offset progress while loading
      screen_decimal(FOOTER_ADDRESS+75,file_offset,REVERSE_VIDEO);

      // Allocate more space for buffer if required.
      if (new_offset>buffers[buffer_id].allocated) {
	// Preserve loading progress footer
	// (buffer_allocate may flush another dirty buffer from memory)
	footer_save();

	// Make the allocation
	if (buffer_allocate(buffer_id,new_offset)) {
	  // Allocation failed.
	  fclose(f);
	  display_footer(FOOTER_OUTOFMEM);
	  return 0xff;
	}
	footer_restore();
      }

      // Write bytes to buffer
      buffer_set_bytes(buffer_id,file_offset,r,data_buffer);
      
      file_offset+=r;

      // Draw progress in footline (one > for every 2KB read)
      *(unsigned char *)(FOOTER_ADDRESS+7+1+16+1+(file_offset>>11))=('>'|0x80);
    }
  }
  fclose(f);
  buffers[buffer_id].length=file_offset;
  buffers[buffer_id].loaded=1;
  c65_io_enable();

  return 0x00;
}

/*
  Saving must use a separate file handle to loading, because loading a large buffer
  may cause dirty buffers to be saved.
*/
unsigned char buffer_save(unsigned char buffer_id)
{
  // XXX - actually save the buffer!
  
  buffers[buffer_id].dirty=0;
  return 0xff;
}

