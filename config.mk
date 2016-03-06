EXECUTABLE := lightstatus

CC := clang

SOURCES = lightstatus.c bar.c battery.c bspwm.c datetime.c packages.c setup_loop.c sound.c wifi.c

#INCLUDES = $(SOURCES:.c=.h) config.h icons.h -I/usr/include/alsa
INCLUDES = -I./ -I/usr/include/alsa

OBJECT_DIR = objects

STD := gnu99

WARNINGS := -Wall -Wextra \
			-pedantic -Wpedantic -pedantic-errors \
			-Wshadow -Wpointer-arith \
			-Wcast-align -Wwrite-strings -Wmissing-prototypes\
			-Wmissing-declarations -Wredundant-decls -Wnested-externs \
			-Winline -Wno-long-long -Wuninitialized \
			-Wconversion -Wstrict-prototypes
DISABLED_WARNINGS := -Wno-strict-aliasing

CFLAGS = -std=$(STD) $(WARNINGS) $(DISABLED_WARNINGS) -g -O2

LDFLAGS = -liw -lev -lasound

INSTALL_DIR = /usr/local/bin
