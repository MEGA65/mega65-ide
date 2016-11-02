#define FOOTER_COPYRIGHT 0
#define FOOTER_BLANK 1
#define FOOTER_NOMOREBUFFERS 2
#define FOOTER_OUTOFMEM 3
#define FOOTER_BUFFERTOOBIG 4
#define FOOTER_LINETOOLONG 5
#define FOOTER_MAX 5

void setup_screen(void);
void display_footer(unsigned char index);

