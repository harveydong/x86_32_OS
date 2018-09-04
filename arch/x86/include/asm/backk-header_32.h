#ifndef __HEADER_32_H__
#define __HEADER_32_H__
#include <stdarg.h>
#include "bootparam.h"
#include <linux/types.h>

extern struct setup_header hdr;
extern struct boot_params boot_params;
extern char _end[];
extern char *HEAP;
extern char *heap_end;

#define RESET_HEAP() ((void*)(HEAP = _end))

static inline char *__get_heap(size_t s, size_t a, size_t n)
{
	char *tmp;

	HEAP = (char *)(((size_t)HEAP+(a-1)) & ~(a-1));
	tmp = HEAP;
	HEAP += s*n;
	return tmp;
}
#define GET_HEAP(type, n) \
	((type *)__get_heap(sizeof(type),__alignof__(type),(n)))

static inline bool heap_free(size_t n)
{
	return (int)(heap_end-HEAP) >= (int)n;
}

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(*(x)))
struct biosregs {
	union {
		struct {
			u32 edi;
			u32 esi;
			u32 ebp;
			u32 _esp;
			u32 ebx;
			u32 edx;
			u32 ecx;
			u32 eax;
			u32 _fsgs;
			u32 _dses;
			u32 eflags;
		};
		struct {
			u16 di,hdi;
			u16 si,hsi;
			u16 bp,hbp;
			u16 _sp,_hsp;
			u16 bx,hbx;
			u16 dx,hdx;
			u16 cx,hcx;
			u16 ax,hax;
			u16 gs,fs;
			u16 es,ds;
			u16 flags,hflags;
		};
		
		struct {
			u8 dil,dih,edi2,edi3;
			u8 sil,sih,esi2,esi3;
			u8 bpl,bph,ebp2,ebp3;		
			u8 _spl,_sph,_esp2,_esp3;
			u8 bl,bh,ebx2,ebx3;
			u8 dl,dh,edx2,edx3;
			u8 cl,ch,ecx2,ecx3;
			u8 al,ah,eax2,eax3;
		};

	};



};

static inline void io_delay(void)
{
	const u16 DELAY_PORT = 0x80;
	asm volatile("outb %%al,%0"::"dN"(DELAY_PORT));
}
static inline void outb(u8 v, u16 port)
{
	asm volatile("outb %0,%1" : : "a" (v), "dN" (port));
}
static inline u8 inb(u16 port)
{
	u8 v;
	asm volatile("inb %1,%0" : "=a" (v) : "dN" (port));
	return v;
}

static inline void outw(u16 v, u16 port)
{
	asm volatile("outw %0,%1" : : "a" (v), "dN" (port));
}
static inline u16 inw(u16 port)
{
	u16 v;
	asm volatile("inw %1,%0" : "=a" (v) : "dN" (port));
	return v;
}

static inline void set_gs(u16 seg)
{
	asm volatile("movw %0,%%gs"::"rm"(seg));
}
static inline u16 ds(void)
{
	u16 seg;
	asm("movw %%ds,%0":"=rm"(seg));
	return seg;
}

static inline u16 fs(void)
{
	u16 seg;
	asm volatile("movw %%fs,%0":"=rm"(seg));
	return seg;
}

static inline u16 gs(void)
{
	u16 seg;
	asm volatile("movw %%gs,%0":"=rm"(seg));
	return seg;
}

static inline void set_fs(u16 seg){
	asm volatile("movw %0,%%fs"::"rm"(seg));

}

static inline u32 rdgs32(addr_t addr)
{
	u32 v;
	asm volatile("movl %%gs:%1,%0":"=r"(v):"m"(*(u32*)addr));
	return v;
}


static inline u32 rdfs32(addr_t addr)
{
	u32 v;
	asm volatile("movl %%fs:%1,%0":"=r"(v):"m"(*(u32*)addr));

	return v;
}

static inline void wrfs32(u32 v,addr_t addr)
{
	asm volatile("movl %1,%%fs:%0":"+m"(*(u32 *)addr):"ri"(v));

}
static inline u16 rdfs16(addr_t addr)
{
	u16 v;
	asm volatile("movw %%fs:%1,%0":"=r"(v):"m"(*(u16*)addr));
	return v;
}

static inline u8 rdfs8(addr_t addr)
{
	u8 v;
	asm volatile("movb %%fs:%1,%0" : "=q" (v) : "m" (*(u8 *)addr));
	return v;
}
static inline int isdigit(int ch)
{
	return (ch >= '0') && (ch <= '9');
}

#define NORMAL_VGA	0xffff		/* 80x25 mode */
#define EXTENDED_VGA	0xfffe		/* 80x50 mode */
#define ASK_VGA		0xfffd		/* ask for it at bootup */

void *memcpy(void *dst,void*src,size_t len);
void *memset(void *dst,int c,size_t len);
#define memcpy(d,s,l) __builtin_memcpy(d,s,l)
#define memset(d,c,l) __builtin_memset(d,c,l)
void intcall(u8 int_no,const struct biosregs *ireg,struct biosregs *oreg);
void *copy_from_fs(void *dst, addr_t src, size_t len);
void copy_to_fs(addr_t dst, void *src, size_t len);

void initregs(struct biosregs *regs);
void my_puts(const char *);
void my_putchar(int);
int getchar(void);
void __attribute__((noreturn)) go_to_protected_mode(void);
void __attribute__((noreturn)) die(void);
/* printf.c */
int sprintf(char *buf, const char *fmt, ...);
int vsprintf(char *buf, const char *fmt, va_list args);
int printf(const char *fmt, ...);
size_t strnlen(const char *s, size_t maxlen);

u16 vga_crtc(void);


int enable_a20(void);
void __attribute__((noreturn)) protected_mode_jump(u32 entrypoint,u32 bootparams);

void kbd_flush(void);
int getchar_timeout(void);
void set_video(void);
int detect_memory(void);
#endif
