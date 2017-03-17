/*
  We allow a certain number of open file buffers at a time.
  Note that they may not all be held in RAM at any point in time, however.
  For each file, we need to know its name (so that it can be loaded if it is ejected
  from memory for any reason), and whether it is loaded in memory, how long it is, and
  where in memory it is sitting.

  It is nice to allow for a fairly long list of open buffers, so that relatively
  complex projects can be edited in the IDE.

Comments regarding current design:

  1. All buffer context must fit in 1KB of RAM ($0400-$07FF). This limits us to about
     45 open buffers at this point in time.
  2. Each buffer is limited to 65534 bytes in length. Total size of all loaded buffers
     is also limited by available memory.  We currently expect to have about 70KB 
     total for loaded buffers (or less if I get around to adding undo support).
     (But note that buffers are automatically unloaded to make room for others, so
      you can be editing more than 70KB of buffers in total at any point in time).
  3. Lines are limited to 254 bytes in length. Files with longer lines will cause
     "line too long" errors.
  4. Filenames can be only upto 16 characters (only an issue for M65 native file system
     when it is available).
  5. Copy-paste will probably be implemented using a designated *copybuffer* buffer.

*/

// Structure that describes a known buffer
struct known_buffer {
  unsigned char filename[16]; // null if slot unused
  unsigned int length; // used bytes
  unsigned int line_count; // number of lines in buffer
  unsigned int allocated; // allocated bytes

  // Note that resident addresses are relative to "buffer memory", and are
  // NOT absolute addresses.
  unsigned int resident_address_low ; // or 0 if not currently in memory
  unsigned char resident_address_high; // bits 16-23 of resident address  
  unsigned int dirty : 1 ; // indicates if buffer requires saving to disk
  unsigned int loaded : 1; // indicates if buffer currently in RAM

  // Current edit state info
  unsigned int current_line;
  unsigned char current_column;
};
#define BUFFER_LIST_BASE 0x032cU
#define BUFFER_LIST_TOP 0x07ffU
#define BUFFER_LIST_BYTES (BUFFER_LIST_TOP-BUFFER_LIST_BASE+1)
#define MAX_BUFFERS (BUFFER_LIST_BYTES/(sizeof (struct known_buffer)))

extern struct known_buffer *buffers;

extern long buffer_first_free_byte;


void initialise_buffers(void);
unsigned char buffer_create(unsigned char *name);
unsigned char buffer_allocate(unsigned char buffer_id, unsigned int size);
unsigned char buffer_load(unsigned char buffer_id);
void buffer_relocate(unsigned char bid,long new_resident_address);
void buffer_move_mem(long from,long to, unsigned int length);
void buffers_calculate_freespace(void);
void buffer_eject_from_memory(unsigned char bid);
void buffer_eject_other(unsigned char but_not_this_one);
unsigned char buffer_save(unsigned char bid);
unsigned char buffer_load(unsigned char bid);
void buffer_getset_bytes(unsigned char buffer_id,unsigned int offset, unsigned int count,
			 unsigned char *data, unsigned char getP);
#define buffer_set_bytes(A,B,C,D) buffer_getset_bytes(A,B,C,D,0)
#define buffer_get_bytes(A,B,C,D) buffer_getset_bytes(A,B,C,D,1)

