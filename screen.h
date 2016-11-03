#define SCREEN_ADDRESS (0xa000U)
#define FOOTER_ADDRESS (SCREEN_ADDRESS+24*80)

#define FOOTER_COPYRIGHT     0
#define FOOTER_BLANK         1
#define FOOTER_NOMOREBUFFERS 2
#define FOOTER_OUTOFMEM      3
#define FOOTER_BUFFERTOOBIG  4
#define FOOTER_LINETOOLONG   5
#define FOOTER_FATAL         6
#define FOOTER_DISKERROR     7
#define FOOTER_MAX           7

#define REVERSE_VIDEO 0x80
#define NORMAL_VIDEO 0x00

void setup_screen(void);
void display_footer(unsigned char index);
void ascii_to_screen_80(unsigned char *p, unsigned char bits);
void footer_save(void);
void footer_restore(void);

void screen_colour_line(unsigned char line,unsigned char colour);
