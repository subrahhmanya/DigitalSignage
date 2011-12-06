SAMPLE = infodisplay

CC ?= g++
COPTS ?= -g

prefix=/usr
exec_prefix=${prefix}
libdir=/usr/lib64
includedir=${prefix}/include

LIBS = -L/usr/X11R6/lib -lGL -lSDL -lSDL_ttf -lXext -lX11 -lm -L${libdir}

all : $(SAMPLE)

clean : 
	rm -f $(SAMPLE)

run : $(SAMPLE)
	./$(SAMPLE)

$(SAMPLE) : $(SAMPLE).cpp
	$(CC) -o $@ $(COPTS) $< $(LIBS)
