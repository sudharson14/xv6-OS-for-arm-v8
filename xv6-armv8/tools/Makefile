include ../makefile.inc

CFLAGS = -Werror -Wall
CFLAGS += -iquote ../

all: mkfs

mkfs: mkfs.c
	$(HOSTCC) $(CFLAGS) -o $@ $^

clean:
	rm -f mkfs	
