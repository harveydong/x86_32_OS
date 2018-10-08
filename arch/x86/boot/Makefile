
AS=as --32 -D__ASSEMBLY__

all: start loader.bin

BOOTFLAGS = -D__ASSEMBLY__

INC:=-I../include -I../../../include/
start:start.S
	gcc $(INC) $(BOOTFLAGS) -m16 -c start.S
	ld -m elf_i386 -Ttext 0x7c00 -e boot_start -o start.elf start.o
	objcopy -O binary   start.elf start
setup.o: setup.c
	gcc -m16 $(INC) $(BOOTFLAGS) -mregparm=3 -fno-builtin  -fno-stack-protector  -c setup.c

copy.o:copy.S
	gcc -m32  $(BOOTFLAGS) -fno-builtin -mregparm=3 -c copy.S

bioscall.o:bioscall.S
	gcc -m16 $(BOOTFLAGS) -c -mregparm=3  bioscall.S
main.o: main.c
	gcc -m16 $(BOOTFLAGS) $(INC) -mregparm=3 -fno-builtin -fno-stack-protector -c main.c
tty.o: tty.c
	gcc -m16 $(BOOTFLAGS) $(INC) -fno-builtin -mregparm=3 -fno-stack-protector  -c tty.c
regs.o: regs.c 
	gcc -m16 $(BOOTFLAGS) $(INC) -fno-builtin -mregparm=3 -fno-stack-protector -c regs.c


memory.o: memory.c
	gcc -m16 $(BOOTFLAGS) $(INC) -fno-builtin -mregparm=3 -fno-stack-protector  -c memory.c
video.o: video.c
	gcc -m16 $(BOOTFLAGS) $(INC) -fno-builtin -mregparm=3 -fno-stack-protector  -c video.c
video-vesa.o: video-vesa.c
	gcc -m16 $(BOOTFLAGS) $(INC)  -fno-builtin -mregparm=3 -fno-stack-protector  -c video-vesa.c
video-vga.o: video-vga.c
	gcc -m16 $(BOOTFLAGS) $(INC) -fno-builtin -mregparm=3 -fno-stack-protector  -c video-vga.c
video-bios.o: video-bios.c
	gcc -m16 $(BOOTFLAGS) $(INC) -fno-builtin -mregparm=3 -fno-stack-protector  -c video-bios.c


printf.o: printf.c
	gcc -m16 $(BOOTFLAGS) $(INC) -fno-builtin -mregparm=3 -fno-stack-protector  -c printf.c
string.o: string.c
	gcc -m16 $(BOOTFLAGS) $(INC) -fno-builtin -mregparm=3 -fno-stack-protector  -c string.c


pm.o: pm.c
	gcc -m16 $(BOOTFLAGS) $(INC) -fno-builtin -mregparm=3 -fno-stack-protector  -c pm.c

a20.o:a20.c
	gcc -m16 $(BOOTFLAGS) $(INC) -fno-builtin -mregparm=3 -fno-stack-protector  -c a20.c
pmjump.o:pmjump.S
	gcc  -m32 $(BOOTFLAGS) $(INC)  -fno-builtin -mregparm=3 -c pmjump.S
disk.o:disk.c
	gcc -m16 $(BOOTFLAGS) $(INC) -fno-builtin -mregparm=3 -fno-stack-protector  -c disk.c

console.o:console.c
	gcc -m16 $(BOOTFLAGS) $(INC) -fno-builtin -mregparm=3 -fno-stack-protector  -c console.c



loader.bin: setup.o copy.o tty.o regs.o bioscall.o memory.o video.o video-vesa.o video-vga.o video-bios.o pm.o a20.o pmjump.o  printf.o string.o disk.o console.o
	ld   -m elf_i386 -T head.ld -o loader.elf $^
	objcopy -O binary loader.elf $@


clean:
	rm  *.o start  *.bin *.elf
