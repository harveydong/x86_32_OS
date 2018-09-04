
LD=ld -m elf_i386
OBJCOPY=objcopy -O binary -R .note -R .comment -S
LDFLAGS=
LINKFLAGS=-T $(TOPDIR)/arch/x86/vmlinux.lds $(LDFLAGS)
CFLAGS += -m32 -fno-stack-protector -fno-builtin

HEAD:=arch/x86/kernel/head32.o  arch/x86/kernel/init_task.o
SUBDIRS+=arch/x86/kernel arch/x86/mm arch/x86/lib
CORE_FILES:=arch/x86/kernel/kernel.o arch/x86/mm/mm.o $(CORE_FILES)
LIBS:=$(TOPDIR)/arch/x86/lib/lib.a $(LIBS) $(TOPDIR)/arch/x86/lib/lib.a


arch/x86/kernel:dummy
	$(MAKE) linuxsubdirs SUBDIRS=arch/x86/kernel
arch/x86/mm:dummy
	$(MAKE) linuxsubdirs SUBDIRS=arch/x86/mm

MAKEBOOT=$(MAKE) -C arch/x86/boot

vmlinux:arch/x86/vmlinux.lds

FORCE:;

archclean:
	@$(MAKEBOOT) clean

