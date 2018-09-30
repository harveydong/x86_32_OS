
.S.o:
	$(CC) $(AFLAGS) -c $< -o $*.o

all: kernel.o head32.o init_task.o
O_TARGET:=kernel.o

export-objs:=cpuid.o
obj-y:=entry.o setup.o init.o apic.o smp.o mpparse.o smpboot.o io_apic.o apic.o semaphore.o fault.o i8259.o traps.o irq.o time.o

include $(TOPDIR)/rules.make




