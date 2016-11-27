
CC65=	/usr/local/bin/cc65
CL65=	/usr/local/bin/cl65
COPTS=	-t c64 -O -Or -Oi -Os --cpu 65c02

FILES=		m65ide.prg \
		autoboot.c65

M65IDESOURCES=	main.c \
		memory.c \
		screen.c \
		buffers.c \
		lines.c \
		windows.c \
		input.c

ASSFILES=	main.s \
		memory.s \
		screen.s \
		buffers.s \
		lines.s \
		windows.s \
		input.s \
		charset.s

HEADERS=	Makefile \
		memory.h \
		screen.h \
		buffers.h \
		lines.h \
		windows.h \
		input.h

M65IDE.D81:	$(FILES)
	if [ -a M65IDE.D81 ]; then rm -f M65IDE.D81; fi
	cbmconvert -v2 -D8o M65IDE.D81 $(FILES) $(M65IDESOURCES) $(HEADERS)

%.s:	%.c $(HEADERS)
	$(CC65) $(COPTS) -o $@ $<

m65ide.prg:	$(ASSFILES)
	$(CL65) $(COPTS) -vm -m m65ide.map -o m65ide.prg $(ASSFILES)

clean:
	rm -f M65IDE.D81 $(FILES)

cleangen:
	rm $(VHDLSRCDIR)/kickstart.vhdl $(VHDLSRCDIR)/charrom.vhdl *.M65
