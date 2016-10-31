
CC65=	/usr/local/bin/cl65
COPTS=	-t c64 -O -Or -Oi -Os

FILES=		m65ide.prg

M65IDESOURCES=	main.c \
		memory.c

HEADERS=	Makefile \
		memory.h

M65IDE.D81:	$(FILES)
	if [ -a M65IDE.D81 ]; then rm -f M65IDE.D81; fi
	cbmconvert -v2 -D8o M65IDE.D81 $(FILES)

m65ide.prg:	$(M65IDESOURCES) $(HEADERS)
	$(CC65) $(COPT) -o m65ide.prg $(M65IDESOURCES)

clean:
	rm -f M65IDE.D81 $(FILES)

cleangen:
	rm $(VHDLSRCDIR)/kickstart.vhdl $(VHDLSRCDIR)/charrom.vhdl *.M65
