
.S.o:
	$(CC) $(AFLAGS) -c $< -o $*.o

all: kernel.o head32.o init_task.o
O_TARGET:=kernel.o

export-objs:=cpuid.o
obj-y:=entry.o setup.o init.o traps.o  apic.o smp.o mpparse.o smpboot.o io_apic.o
include $(TOPDIR)/rules.make




