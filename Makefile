TOPDIR:=$(shell pwd)
HPATH:=$(TOPDIR)/include
FINDHPATH=$(TOPDIR)/linux

AS=as
LD=ld
CC=gcc
CPP=gcc -E
OBJCOPY=objcopy
OBJDUMP=objdump
CFLAGS_KERNEL=
AR=ar
NM=nm
CP=cp
export CP TOPDIR HPATH AR AS nm  LD CC CPP OBJCOPY OBJDUMP MAKE

CPPFLAGS:=-D__KERNEL__ -I$(HPATH) -I$(TOPDIR)/arch/x86/include 
CFLAGS:=$(CPPFLAGS) -Wall  -m32 
AFLAGS:=-D__ASSEMBLY__ $(CPPFLAGS) -I../include -m32

CORE_FILES=kernel/kernel.o mm/mm.o fs/fs.o 
LIBS=$(TOPDIR)/lib/lib.a
SUBDIRS=kernel mm fs lib

CLEAN_FILES=submenu* vmlinux System.map .tmp* 
CLEAN_DIRS=modules

export CPPFLAGS CFLAGS AFLAGS
export HEAD LDFLAGS LINKFLAGS MAKEBOOT ASFLAGS


include arch/x86/Makefile

all: boot vmlinux

.S.s:
	$(CPP) $(AFLAGS) -o $*.s $<
.S.o:
	$(CC) $(AFLAGS) -c -o $*.o $<

boot: vmlinux
	@$(MAKE) CFLAGS="$(CFLAGS) $(CFLAGS_KERNEL)" -C arch/x86/boot
	$(CP) arch/x86/boot/start arch/x86/boot/loader.bin  .


vmlinux: init/main.o init/version.o linuxsubdirs
	$(LD) $(LINKFLAGS) $(HEAD) init/main.o init/version.o $(CORE_FILES) $(LIBS) -o vmlinux
	$(NM) vmlinux |sort > System.map


linuxsubdirs: $(patsubst %,_dir_%,$(SUBDIRS))

$(patsubst %,_dir_%,$(SUBDIRS)): dummy 
	$(MAKE) CFLAGS="$(CFLAGS) $(CFLAGS_KERNEL)" -C $(patsubst _dir_%,%,$@)


init/version.o: init/version.c
	$(CC) $(CFLAGS) $(CFLAGS_KERNEL) -c -o init/version.o init/version.c
init/main.o: init/main.c
	$(CC) $(CFLAGS) $(CFLAGS_KERNEL) $(PROFILING) -c -o $*.o $<

fs lib kernel mm:dummy
	$(MAKE) CFLAGS="$(CFLAGS $(CFLAGS_KERNEL))" $(subst $@,_dir_$@,$@)


include rules.make

clean: archclean
	rm -rf start loader.bin vmlinux System.map
	rm -f $(CLEAN_FILES)
	find . \( -name '*.[oas]' -o -name core -o -name '.*.flags' \) -type f -print  |xargs rm -f
	#rm -rf $(CLEAN_DIRS)
		
install:
	dd if=start of=c.img bs=512 conv=notrunc
	dd if=loader.bin of=c.img bs=512 seek=1 conv=notrunc
	dd if=vmlinux of=c.img bs=512 seek=80 conv=notrunc
