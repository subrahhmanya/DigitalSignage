SAMPLE = infodisplay

CC ?= g++

prefix=/usr
exec_prefix=${prefix}
libdir=/usr/lib
includedir=${prefix}/include

COPTS ?= -g $$(pkg-config --cflags libxml++-2.6)

LIBS = -Wunused -L/usr/X11R6/lib -lGL -lSDL -lSDL_image -lSDL_ttf -lxml2 -lXext -lX11 -lm -L${libdir}

all : $(SAMPLE)

clean : 
	rm -f $(SAMPLE)

run : $(SAMPLE)
	./$(SAMPLE)

$(SAMPLE) : $(SAMPLE).cpp
	$(CC) -o $@ $(COPTS) $< $(LIBS)
