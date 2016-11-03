#include "screen.h"
#include "memory.h"

unsigned char *footer_messages[FOOTER_MAX+1]={
  "MEGA65 IDE v00.01 : (C) Copyright 2016 Paul Gardner-Stephen etc.  CTRL-q to exit",
  "                                                                                ",
  "No more buffers.  Close a buffer and try again.                                 ",
  "Out of memory. Closing buffers might help.                                      ",
  "Buffer too large (must be <65535 bytes).                                        ",
  "Line too long. Lines must be <255 characters in length.                         ",
  "Fatal error: Something horrible has happened to memory. Probably a bug.         ",
  "Disk error. Could not read/write buffer or other thing to/from disk.            "
};
unsigned char footers_initialised=0;

unsigned char mungedascii_to_screen(unsigned char in)
{
  if ((in>='A')&&(in<='Z')) return (in-0x00);
  if ((in>='a')&&(in<='z')) return (in-0x40);
  return in;
}

void mungedascii_to_screen_80(unsigned char *p,unsigned char bits)
{
  int i;
  for(i=0;i<80;i++) {
    p[i]=mungedascii_to_screen(p[i])|bits;
  }
}

unsigned char ascii_to_screen(unsigned char in)
{
  if ((in>='A')&&(in<='Z')) return (in-0x40);
  if ((in>='a')&&(in<='z')) return (in-0x20);
  return in;
}

void ascii_to_screen_80(unsigned char *p,unsigned char bits)
{
  int i;
  for(i=0;i<80;i++) {
    p[i]=ascii_to_screen(p[i])|bits;
  }
}

void initialise_footers(void)
{
  unsigned char f;
  for(f=0;f<=FOOTER_MAX;f++) {
    unsigned char *p=footer_messages[f];
    mungedascii_to_screen_80(p,REVERSE_VIDEO);
  }
}

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

long addr;
void display_footer(unsigned char index)
{  
  if (!footers_initialised) initialise_footers();

  addr=(long)footer_messages[index];  
  lcopy(addr,FOOTER_ADDRESS,80);  
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
  // We are using $A000-$A7FF for now
  // and use lower-case char set
  *(unsigned char *)0xD018U=0x06+(((SCREEN_ADDRESS-0x8000U)>>10)<<4);
  

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
