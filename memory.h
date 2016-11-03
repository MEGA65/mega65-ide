/*
  Overall Memory layout:

  $00000-$003FF - C64 ZP, stack etc
  $00400-$007FF - Meta-data for current open buffers
  $00800-$09FFF - M65IDE program
  $0A000-$0A7FF - 2KB screen RAM
  $0A800-$0FFFF -                               22KB unused (some under IO/ROM)
  $10000-$11FFF - C65 DOS
  $12000-$1F7FF - 54KB RAM for buffers etc
  $1F800-$1FFFF - C65 2KB colour RAM for screen

  Screen is the main thing to sort out, because we need to have the C64/C65 character
  set in scope for the VIC-III (we are not using MEGA65-only VIC-IV arbitrary screen
  buffer registers).  This requires 2KB somewhere.  Ideally we don't want to mess up
  the contiguous 54KB of RAM in the 2nd 64KB bank, that we will use for buffers for
  files being edited.

*/

extern unsigned char buffer_memory_segment_count;
extern long buffer_memory_segments[2];
extern long buffer_memory_segment_lengths[2];
extern long total_buffer_memory;

long buffer_address_to_real(long buffer_address);
unsigned int buffer_address_contiguous_bytes(long buffer_address);

void c65_io_enable(void);
unsigned char lpeek(long address);
void lpoke(long address, unsigned char value);
void lcopy(long source_address, long destination_address,
	   unsigned int count);
void lfill(long destination_address, unsigned char value,
	   unsigned int count);
#define POKE(X,Y) (*(unsigned char*)(X))=Y
#define PEEK(X) (*(unsigned char*)(X))
