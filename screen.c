#include "ascii.h"
#include "screen.h"
#include "memory.h"

unsigned char *footer_messages[FOOTER_MAX+1]={
  "MEGA65 IDE v00.01 : (C) Copyright 2016 Paul Gardner-Stephen etc.  CTRL-q to exit",
  "                                                                                ",
  "No more buffers.  Close a buffer and try again.                                 ",
  "Out of memory. Closing buffers might help.                                      ",
  "Buffer too large (must be <65535 bytes).                                        ",
  "Line too long. Lines must be <255 characters in length.                         ",
  "Fatal error occurred. Probably a bug. Clue: no clue                             ",
  "Disk error. Could not read/write buffer or other thing to/from disk.            ",
  "Read       lines from                                                           "
};

unsigned char screen_hex_buffer[6];

unsigned char screen_hex_digits[16]={
  '0','1','2','3','4','5',
  '6','7','8','9',1,2,3,4,5,6};
unsigned char to_screen_hex(unsigned char c)
{
  return screen_hex_digits[c&0xf];
}

void screen_hex(unsigned int addr,long value)
{
  POKE(addr+0,to_screen_hex(value>>20));
  POKE(addr+1,to_screen_hex(value>>16));
  POKE(addr+2,to_screen_hex(value>>12));
  POKE(addr+3,to_screen_hex(value>>8));
  POKE(addr+4,to_screen_hex(value>>4));
  POKE(addr+5,to_screen_hex(value>>0));
}

unsigned char screen_decimal_digits[16][5]={
  {0,0,0,0,1},
  {0,0,0,0,2},
  {0,0,0,0,4},
  {0,0,0,0,8},
  {0,0,0,1,6},
  {0,0,0,3,2},
  {0,0,0,6,4},
  {0,0,1,2,8},
  {0,0,2,5,6},
  {0,0,5,1,2},
  {0,1,0,2,4},
  {0,2,0,4,8},
  {0,4,0,9,6},
  {0,8,1,9,2},
  {0,6,3,8,4},
  {3,2,7,6,8}
};

unsigned char ii,j,carry,temp;
unsigned int value;
void screen_decimal(unsigned int addr,unsigned int v,unsigned char bits)
{
  // XXX - We should do this off-screen and copy into place later, to avoid glitching
  // on display.
  
  value=v;
  
  // Start with all zeros
  for(ii=0;ii<5;ii++) screen_hex_buffer[ii]=0;
  
  // Add power of two strings for all non-zero bits in value.
  // XXX - We should use BCD mode to do this more efficiently
  for(ii=0;ii<16;ii++) {
    if (value&1) {
      carry=0;
      for(j=4;j<128;j--) {
	temp=screen_hex_buffer[j]+screen_decimal_digits[ii][j]+carry;
	if (temp>9) {
	  temp-=10;
	  carry=1;
	} else carry=0;
	screen_hex_buffer[j]=temp;
      }
    }
    value=value>>1;
  }

  // Now convert to ascii digits
  for(j=0;j<5;j++) screen_hex_buffer[j]=screen_hex_buffer[j]|'0'|bits;

  // and shift out leading zeros
  for(j=0;j<4;j++) {
    if (screen_hex_buffer[0]!=('0'|bits)) break;
    screen_hex_buffer[0]=screen_hex_buffer[1];
    screen_hex_buffer[1]=screen_hex_buffer[2];
    screen_hex_buffer[2]=screen_hex_buffer[3];
    screen_hex_buffer[3]=screen_hex_buffer[4];
    screen_hex_buffer[4]=' '|bits;
  }
  // Copy to screen
  for(j=0;j<4;j++) POKE(addr+j,screen_hex_buffer[j]);
}

long addr;
void display_footer(unsigned char index)
{  
  addr=(long)footer_messages[index];  
  lcopy(addr,FOOTER_ADDRESS,80);
  set_screen_attributes(FOOTER_ADDRESS,80,ATTRIB_REVERSE);
}

unsigned char saved_footer_buffer[80];
void footer_save(void)
{
  lcopy((long)FOOTER_ADDRESS,(long)saved_footer_buffer,80);
}

void footer_restore(void)
{
  lcopy((long)saved_footer_buffer,(long)FOOTER_ADDRESS,80);
}

void setup_screen(void)
{
  unsigned char v;

  c65_io_enable();
  
  // 80-column mode, fast CPU, extended attributes enable
  *((unsigned char*)0xD031)=0xe0;

  // Put screen memory somewhere (2KB required)
  // We are using $A800-$AFFF for screen
  // Our custom ASCII charset lives at $A000
  *(unsigned char *)0xD018U=
    (((CHARSET_ADDRESS-0x8000U)>>11)<<1)
    +(((SCREEN_ADDRESS-0x8000U)>>10)<<4);
  

  // VIC RAM Bank to $C000-$FFFF
  v=*(unsigned char *)0xDD00U;
  v&=0xfc;
  v|=0x01;
  *(unsigned char *)0xDD00U=v;

  // Screen colours
  POKE(0xD020U,0);
  POKE(0xD021U,6);

  // Clear screen RAM
  lfill(SCREEN_ADDRESS,0x20,2000);

  // Clear colour RAM: white text
  lfill(0x1f800,0x01,2000);

  display_footer(FOOTER_COPYRIGHT);    
}

void screen_colour_line(unsigned char line,unsigned char colour)
{
  // Set colour RAM for this screen line to this colour
  // (use bit-shifting as fast alternative to multiply)
  lfill(0x1f800+(line<<6)+(line<<4),colour,80);
}

unsigned char i;

void fatal_error(unsigned char *filename, unsigned int line_number)
{
  display_footer(FOOTER_FATAL);
  for(i=0;filename[i];i++) POKE(FOOTER_ADDRESS+44+i,filename[i]|REVERSE_VIDEO);
  POKE(FOOTER_ADDRESS+44+i,':'|REVERSE_VIDEO); i++;
  screen_decimal(FOOTER_ADDRESS+44+i,line_number,REVERSE_VIDEO);
  lfill(COLOUR_RAM_ADDRESS-SCREEN_ADDRESS+FOOTER_ADDRESS,2,80);
  for(;;) continue;
}

void set_screen_attributes(long p,unsigned char count,unsigned char attr)
{
  // This involves setting colour RAM values, so we need to either LPOKE, or
  // map the 2KB colour RAM in at $D800 and work with it there.
  // XXX - For now we are LPOKING
  long addr=COLOUR_RAM_ADDRESS-SCREEN_ADDRESS+p;
  for(i=0;i<count;i++) {
    lpoke(addr,lpeek(addr)|attr);
    addr++;
  }
}
