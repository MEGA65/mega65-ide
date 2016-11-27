/*
  MEGA65 Memory Access routines that allow access to the full RAM of the MEGA65,
  even though the program is stuck living in the first 64KB of RAM, because CC65
  doesn't (yet) understand how to make multi-bank MEGA65 programs.

*/

#include "memory.h"

unsigned char buffer_memory_segment_count=2;
long buffer_memory_segments[2]={0x0b000,0x12000};
long buffer_memory_segment_lengths[2]={(0xc000-0xb000),
				       (0x1f800-0x12000)};  
long total_buffer_memory;


// Convert an offset in the buffer space to its physical address
unsigned char i;
long buffer_address_to_real(long buffer_address)
{
  for(i=0;i<buffer_memory_segment_count;i++) {
    if (buffer_address < buffer_memory_segment_lengths[i])
      return buffer_memory_segments[i]+buffer_address;
    else buffer_address -= buffer_memory_segment_lengths[i];
  }
  // Invalid address
  return 0;
}

// How many contiguous bytes are there in buffer space at this location?
unsigned int buffer_address_contiguous_bytes(long buffer_address)
{
  for(i=0;i<buffer_memory_segment_count;i++) {
    if (buffer_address < buffer_memory_segment_lengths[i])
      return buffer_memory_segment_lengths[i]-buffer_address;
    else buffer_address -= buffer_memory_segment_lengths[i];
  }
  // Invalid address
  return 0;
}


struct dmagic_dmalist {
  unsigned char command;
  unsigned int count;
  unsigned int source_addr;
  unsigned char source_bank;
  unsigned int dest_addr;
  unsigned char dest_bank;
  unsigned int modulo;
};

struct dmagic_dmalist dmalist;
unsigned char dma_byte;

void do_dma(void)
{
  c65_io_enable();

#ifdef MEGA65
  // Now run DMA job (to and from low 1MB, and list is in low 1MB)
  POKE(0xd702U,0);
  POKE(0xd704U,0);
  POKE(0xd705U,0);
  POKE(0xd706U,0);
#endif
  POKE(0xd701U,((unsigned int)&dmalist)>>8);
  POKE(0xd700U,((unsigned int)&dmalist)&0xff); // triggers DMA
}

unsigned char lpeek(long address)
{
  // Read the byte at <address> in 28-bit address space
  // XXX - Optimise out repeated setup etc
  // (separate DMA lists for peek, poke and copy should
  // save space, since most fields can stay initialised).
  dmalist.command=0x00; // copy
  dmalist.count=1;
  dmalist.source_addr=address&0xffff;
  dmalist.source_bank=(address>>16)&0x7f;
  dmalist.dest_addr=(unsigned int)&dma_byte;
  dmalist.dest_bank=0;

  do_dma();
   
  return dma_byte;
}

void lpoke(long address, unsigned char value)
{  
  dma_byte=value;
  dmalist.command=0x00; // copy
  dmalist.count=1;
  dmalist.source_addr=(unsigned int)&dma_byte;
  dmalist.source_bank=0;
  dmalist.dest_addr=address&0xffff;
  dmalist.dest_bank=(address>>16)&0x7f;

  do_dma(); 
  return;
}

void lcopy(long source_address, long destination_address,
	  unsigned int count)
{
  dmalist.command=0x00; // copy
  dmalist.count=count;
  dmalist.source_addr=source_address&0xffff;
  dmalist.source_bank=(source_address>>16)&0x7f;
  dmalist.dest_addr=destination_address&0xffff;
  dmalist.dest_bank=(destination_address>>16)&0x7f;

  do_dma();
  return;
}

void lfill(long destination_address, unsigned char value,
	  unsigned int count)
{
  dmalist.command=0x03; // fill
  dmalist.count=count;
  dmalist.source_addr=value;
  dmalist.dest_addr=destination_address&0xffff;
  dmalist.dest_bank=(destination_address>>16)&0x7f;

  do_dma();
  return;
}

void c65_io_enable(void)
{
  // Gate C65 IO enable
  POKE(0xd02fU,0xA5);
  POKE(0xd02fU,0x96);
  // Force back to 3.5MHz
  POKE(0xD031,PEEK(0xD031)|0x40);
}
