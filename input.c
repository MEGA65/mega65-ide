/*
  Process user input.

  For now, we are using the kernal keyboard routines. However, as we are in C64 mode,
  they will not scan the extra C65 keys. Also, the kernal routines don't allow 3-key
  roll-over (for fast typists).  Thus we should probably adapt the following C128
  3-key rollover routine to run in C64 mode, and scan the extra C65 keys, and possibly
  adapt it to give ASCII codes rather than PETSCII codes, and support multiple
  simultaneous modifier keys:

  http://codebase64.org/doku.php?id=magazines:chacking6#three-key_rollover_for_the_c-128_and_c-64

  But for now, we will just use the kernal routines through cc65's library.

*/

void poll_keyboard(void)
{
  
}
