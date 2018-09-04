#include <asm/boot.h>

static void bios_set_video_mode(unsigned char mode)
{
	struct biosregs iregs;
	initregs(&iregs);
	iregs.ah = 0;
	iregs.al = mode;
	intcall(0x10,&iregs,0);
}
void console_init(void)
{
	bios_set_video_mode(3);
}
