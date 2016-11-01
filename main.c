#include <stdio.h>
#include "memory.h"

void write_d02f(unsigned char v)
{
  POKE(0xd02fU,v);
}

int main()
{
  // Enable C65 features
  write_d02f(0xA5);
  write_d02f(0x96);

  //  POKE(0xd031U,0xff);
  
  POKE(0x0426,0);
  
  lpoke(0x0427,0x22);

  POKE(0x0425,0);

  //  printf("$0400 = $%02x\n",lpeek(0x0400));
  return 0;
}
