#include <asm/processor-flags.h>
#include <asm/boot.h>
#include <linux/types.h>

static u8 gettime(void)
{
	struct biosregs ireg,oreg;
	
	initregs(&ireg);
	ireg.ah = 0x02;
	intcall(0x1a,&ireg,&oreg);
	
	return oreg.dh;

}

static int kbd_pending(void)
{
	struct biosregs ireg,oreg;
	
	initregs(&ireg);
	ireg.ah = 0x01;
	intcall(0x16,&ireg,&oreg);
	
	return !(oreg.eflags & X86_EFLAGS_ZF);
}


void kbd_flush(void)
{
	for(;;){
		if(!kbd_pending()){
			break;
		}

		getchar();
	}
}
int getchar_timeout(void)
{
	int cnt = 30;
	int t0,t1;
		
	t0 = gettime();
	
	while(cnt){
		if(kbd_pending())
			return getchar();

		t1 = gettime();
		if(t0 != t1){
			cnt--;
			t0 = t1;
		}
	}	
}
int getchar(void)
{
	struct biosregs ireg,oreg;
	
	initregs(&ireg);	
	intcall(0x16,&ireg,&oreg);
	
	return oreg.al;
}

void __attribute__((section(".inittext"))) my_putchar(int ch)
{
	struct biosregs ireg;
	
	if(ch == '\n')
		my_putchar('\r');

	initregs(&ireg);
	ireg.bx = 0x0007;
	ireg.cx = 0x0001;
	ireg.ah = 0x0e;
	ireg.al = ch;
	intcall(0x10,&ireg,NULL);

}

void __attribute__((section(".inittext"))) puts(const char *str)
{
	while(*str)
		my_putchar(*str++);
}


