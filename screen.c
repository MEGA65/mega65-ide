#include "screen.h"
#include "memory.h"

unsigned char *footer_messages[6]={
  "MEGA65 IDE v00.01 : (C) Copyright 2016 Paul Gardner-Stephen etc.  CTRL-q to exit",
  "                                                                                "
  "No more buffers.  Close a buffer and try again.                                 ",
  "Out of memory. Closing buffers might help.                                      ",
  "Buffer too large (too many lines, or 64KB limit reached)                        ",
  "Line too long. Lines must be <255 characters in length.                         ",
};
unsigned char footers_initialised=0;

unsigned char ascii_to_screen(unsigned char in)
{
  if ((in>='A')&&(in<='Z')) return (in-0x00);
  if ((in>='a')&&(in<='z')) return (in-0x40);
  return in;
}

void initialise_footers(void)
{
  unsigned char i,f;
  for(f=0;f<=FOOTER_MAX;f++) {
    unsigned char *p=footer_messages[f];
    for(i=0;i<80;i++)
      p[i]=ascii_to_screen(p[i])|0x80;
  }
}

void display_footer(unsigned char index)
{  
  unsigned char i;

  if (!footers_initialised) initialise_footers();
  
  lcopy((long)footer_messages[index],(0xb800U+24*80),80);
  for(i=0;i<80;i++)
    POKE((0xb800U+24*80)+i,PEEK((0xb800U+24*80)+i)|0x80);
}

void setup_screen(void)
{
  unsigned char v;
  
  // 80-column mode, fast CPU, extended attributes enable
  *((unsigned char*)0xD031)=0xe0;

  // Put screen memory somewhere (2KB required)
  // We are using $B800-$BFFF for now
  // and use lower-case char set
  *(unsigned char *)0xD018U=0xE6;

  // VIC RAM Bank to $C000-$FFFF
  v=*(unsigned char *)0xDD00U;
  v&=0xfc;
  v|=0x01;
  *(unsigned char *)0xDD00U=v;

  // Screen colours
  POKE(0xD020U,0);
  POKE(0xD021U,6);

  // Clear screen RAM
  lfill(0x0b800,0x20,2000);

  // Clear colour RAM: white text
  lfill(0x1f800,0x01,2000);

  display_footer(FOOTER_COPYRIGHT);
  
}
