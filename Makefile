
CC65=	/usr/local/bin/cc65
CL65=	/usr/local/bin/cl65
COPTS=	-t c64 -O -Or -Oi -Os

FILES=		m65ide.prg \
		autoboot.c65

M65IDESOURCES=	main.c \
		memory.c \
		screen.c

ASSFILES=	main.s \
		memory.s \
		screen.s

HEADERS=	Makefile \
		memory.h \
		screen.h

M65IDE.D81:	$(FILES)
	if [ -a M65IDE.D81 ]; then rm -f M65IDE.D81; fi
	cbmconvert -v2 -D8o M65IDE.D81 $(FILES)

opt65:	opt65.c
	gcc -o opt65 opt65.c

%.s:	%.c $(HEADERS) ./opt65
	if [ -a temp.s ]; then rm -f temp.s; fi
	$(CC65) $(COPTS) -o temp.s $<
#	./opt65 temp.s > $@
	cp temp.s $@
	rm temp.s

m65ide.prg:	$(ASSFILES)
	$(CL65) $(COPTS) -o m65ide.prg $(ASSFILES)

clean:
	rm -f M65IDE.D81 $(FILES)

cleangen:
	rm $(VHDLSRCDIR)/kickstart.vhdl $(VHDLSRCDIR)/charrom.vhdl *.M65
