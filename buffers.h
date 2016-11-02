/*
  We allow a certain number of open file buffers at a time.
  Note that they may not all be held in RAM at any point in time, however.
  For each file, we need to know its name (so that it can be loaded if it is ejected
  from memory for any reason), and whether it is loaded in memory, how long it is, and
  where in memory it is sitting.

  It is nice to allow for a fairly long list of open buffers, so that relatively
  complex projects can be edited in the IDE.

  Current design Limitations etc:

  1. All buffer context must fit in 1KB of RAM ($0400-$07FF). This limits us to about
     45 open buffers at this point in time.
  2. Each buffer is limited to 64KB in size. Total size of all loaded buffers is also
     limited by available memory.  We currently expect to have about 70KB total for
     loaded buffers (or less if I get around to adding undo support).
  3. Filenames can be only upto 16 characters (only an issue for M65 native file system
     when it is available).
  4. Copy-paste will probably be implemented using a designated *copybuffer* buffer.

*/

// Structure that describes a known buffer
struct known_buffer {
  unsigned char filename[16]; // null if slot unused
  unsigned int length; // used bytes
  unsigned int allocated; // allocated bytes

  // Note that resident addresses are relative to "buffer memory", and are
  // NOT absolute addresses.
  unsigned int resident_address_low ; // or 0 if not currently in memory
  unsigned char resident_address_high; // bits 16-23 of resident address  
  unsigned int dirty : 1 ; // indicates if buffer requires saving to disk
};
#define MAX_BUFFERS (1024/(sizeof (struct known_buffer)))

void initialise_buffers(void);
unsigned char buffer_create(unsigned char *name);
unsigned char buffer_allocate(unsigned char buffer_id, unsigned int size);
unsigned char buffer_load(unsigned char buffer_id);

