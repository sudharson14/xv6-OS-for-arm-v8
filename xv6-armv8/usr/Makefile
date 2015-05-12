include ../makefile.inc

CFLAGS += -iquote ../
ASFLAGS += -I ../
ULIB = ulib.o usys.o printf.o umalloc.o

MKFS = ../tools/mkfs
FS_IMAGE = ../build/fs.img

UPROGS=\
	_cat\
	_echo\
	_grep\
	_info\
	_init\
	_kill\
	_ln\
	_ls\
	_mkdir\
	_rm\
	_sh\
	_stressfs\
	_usertests\
	_wc\
	_zombie\


all: $(FS_IMAGE)

_%: %.o $(ULIB)
	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $@ $^  -L ../ $(LIBGCC)
	$(OBJDUMP) -S $@ > $*.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $*.sym

_forktest: forktest.o $(ULIB)
	# forktest has less library code linked in - needs to be small
	# in order to be able to max out the proc table.
	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o _forktest forktest.o ulib.o usys.o
	$(OBJDUMP) -S _forktest > forktest.asm

$(FS_IMAGE): $(MKFS)  $(UPROGS)
	$(MKFS) $@  $(UPROGS) UNIX
	$(OBJDUMP) -S usys.o > usys.asm

clean: 
	rm -f *.o *.d *.asm *.sym $(FS_IMAGE) \
	.gdbinit \
	$(UPROGS)
