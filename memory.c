/*
  MEGA65 Memory Access routines that allow access to the full RAM of the MEGA65,
  even though the program is stuck living in the first 64KB of RAM, because CC65
  doesn't (yet) understand how to make multi-bank MEGA65 programs.

*/

unsigned char lpeek(long address)
{
  // Read the byte at <address> in 28-bit address space
  return 0xff;
}

void lpoke(long address, unsigned char value)
{
  return;
}

void lcopy(long source_address, long destination_address,
	  unsigned int count)
{
  return;
}
