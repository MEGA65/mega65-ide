#include <stdio.h>
#include "memory.h"

int main()
{
  // Enable C65 features
  POKE(0xd02fU,0xA5);
  POKE(0xd02fU,0x96);

  //  POKE(0xd031U,0xff);
  
  POKE(0x0426,0);
  
  lpoke(0x0427,0x22);
  
  //  printf("$0400 = $%02x\n",lpeek(0x0400));
  return 0;
}
