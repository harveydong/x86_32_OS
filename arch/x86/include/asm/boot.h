#ifndef __BOOT_H__
#define __BOOT_H__
#include <asm/screen_info.h>
#include <linux/types.h>
#include <asm/e820.h>

#define STACK_SIZE 512
#define NORMAL_VGA      0xffff          /* 80x25 mode */
#define EXTENDED_VGA    0xfffe          /* 80x50 mode */
#define ASK_VGA         0xfffd


#define E820_MAX_MAP_SIZE 128
#define X86_CF_FLAG (1 << 0)
 
extern char *HEAP;
extern char *heap_end;
extern char _boot_end[];

#define RESET_HEAP() ((void*)(HEAP = _boot_end))

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

struct boot_header {
	unsigned long magic;
	unsigned long image_size;
	unsigned long image_addr;
	unsigned long boot_drive;
	unsigned long header_start_sector;
	unsigned long kernel_load_addr;
	unsigned long kernel_start_sector;
} __attribute__((packed));

#if 0
struct e820entry {
	unsigned long addrl, addrh;
	unsigned long sizel, sizeh;
	unsigned long type;
	unsigned long unused;
} __attribute__((packed));
#endif

struct setup_header {
	__u8	setup_sects;
	__u16	root_flags;
	__u32	syssize;
	__u16	ram_size;
#define RAMDISK_IMAGE_START_MASK	0x07FF
#define RAMDISK_PROMPT_FLAG		0x8000
#define RAMDISK_LOAD_FLAG		0x4000
	__u16	vid_mode;
	__u16	root_dev;
	__u16	boot_flag;
	__u16	jump;
	__u32	header;
	__u16	version;
	__u32	realmode_swtch;
	__u16	start_sys;
	__u16	kernel_version;
	__u8	type_of_loader;
	__u8	loadflags;
#define LOADED_HIGH	(1<<0)
#define QUIET_FLAG	(1<<5)
#define KEEP_SEGMENTS	(1<<6)
#define CAN_USE_HEAP	(1<<7)
	__u16	setup_move_size;
	__u32	code32_start;
	__u32	ramdisk_image;
	__u32	ramdisk_size;
	__u32	bootsect_kludge;
	__u16	heap_end_ptr;
	__u8	ext_loader_ver;
	__u8	ext_loader_type;
	__u32	cmd_line_ptr;
	__u32	initrd_addr_max;
	__u32	kernel_alignment;
	__u8	relocatable_kernel;
	__u8	_pad2[3];
	__u32	cmdline_size;
	__u32	hardware_subarch;
	__u64	hardware_subarch_data;
	__u32	payload_offset;
	__u32	payload_length;
	__u64	setup_data;
} __attribute__((packed));


struct boot_params {
	struct boot_header boot_header;
	unsigned char e820_entries;

	struct e820entry e820[E820_MAX_MAP_SIZE];
		//struct e820map e820_map;
	unsigned char _padding[3];
	struct setup_header hdr;
	struct screen_info screen_info;

} __attribute__((packed));

struct biosregs {
	union {
		struct {
			unsigned long edi;
			unsigned long esi;
			unsigned long ebp;
			unsigned long _esp; /* unused */
			unsigned long ebx;
			unsigned long edx;
			unsigned long ecx;
			unsigned long eax;
			unsigned long _fsgs; /* unused */
			unsigned long _dses; /* unsued */
			unsigned long eflags;
		};
		struct {
			unsigned short di, _hdi;
			unsigned short si, _hsi;
			unsigned short bp, _hbp;
			unsigned short _sp, _hsp;
			unsigned short bx, _hbx;
			unsigned short dx, _hdx;
			unsigned short cx, _hcx;
			unsigned short ax, _hax;
			unsigned short gs, fs;
			unsigned short es, ds;
			unsigned short flags, hflags;
		};
		struct {
			unsigned char dil, dih, _edi2, _edi3;
			unsigned char sil, sih, _esi2, _esi3;
			unsigned char bpl, bph, _ebp2, _ebp3;
			unsigned char _spl, _sph, _esp2, _esp3;
			unsigned char bl, bh, _ebx2, _ebx3;
			unsigned char dl, dh, _edx2, _edx3;
			unsigned char cl, ch, _ecx2, _ecx3;
			unsigned char al, ah, _eax2, _eax3;
			unsigned char _gsl, _gsh, _fsl, _fsh;
			unsigned char _esl, _esh, _dsl, _dsh;
			unsigned char _fl, _fh, _ef2, _ef3;
		};
	};
};

#ifdef __ASSEMBLY__

static inline __attribute__((noreturn)) void die(void)
{
	while (1)
		asm("hlt");
}


static inline void cli(void)
{
	asm volatile("cli");
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
static inline void sti(void)
{
	asm volatile("sti");
}

static inline void io_delay(void)
{
	/* it is safe to use 0x80 port */
	asm volatile("outb %al, $0x80");
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

static inline void outl(unsigned long v, unsigned short port)
{
	asm volatile("movl %0, %%eax; outl %%eax, %1"
			:
			: "i"(v), "i"(port)
			: "eax");
}

static inline unsigned long inl(unsigned short port)
{
	unsigned long v;

	asm volatile("inl %1, %%eax; movl %%eax, %0"
			: "=rm"(v)
			: "i"(port)
			: "eax");
	return v;
}

static inline unsigned short get_ds(void)
{
	unsigned short seg;

	asm volatile("movw %%ds,%0" : "=rm"(seg));
	return seg;
}

static inline unsigned short get_es(void)
{
	unsigned short seg;

	asm volatile("movw %%es,%0" : "=rm"(seg));
	return seg;
}

static inline void set_fs(unsigned short seg)
{
	asm volatile("movw %0,%%fs" : : "rm"(seg));
}

static inline void set_gs(unsigned short seg)
{
	asm volatile("movw %0,%%gs" : : "rm"(seg));
}

static inline unsigned long readl_gs(unsigned short off)
{
	unsigned long v;

	asm volatile("movl %%gs:%1,%0" : "=q"(v) : "m"(off));
	return v;
}

static inline void writel_gs(unsigned short off, unsigned long v)
{
	asm volatile("movl %0,%%gs:%1" : : "qi"(v), "m"(off));
}

static inline unsigned long readl_fs(unsigned short off)
{
	unsigned long v;

	asm volatile("movl %%fs:%1,%0" : "=q"(v) : "m"(off));
	return v;
}

static inline void writel_fs(unsigned short off, unsigned long v)
{
	asm volatile("movl %0,%%fs:%1" : : "qi"(v), "m"(off));
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

static inline u32 rdfs32(addr_t addr)
{
        u32 v;
        asm volatile("movl %%fs:%1,%0":"=r"(v):"m"(*(u32*)addr));

        return v;
}
static inline u32 rdgs32(addr_t addr)
{
        u32 v;
        asm volatile("movl %%gs:%1,%0":"=r"(v):"m"(*(u32*)addr));
        return v;
}

static inline void wrfs32(u32 v,addr_t addr)
{
        asm volatile("movl %1,%%fs:%0":"+m"(*(u32 *)addr):"ri"(v));

}




int getchar(void);
void my_puts(const char *);
void my_putchar(int);
int sprintf(char *buf, const char *fmt, ...);
int printf(const char *fmt, ...);
void puts(const char *str);


void kbd_flush(void);
int getchar_timeout(void);


u16 vga_crtc(void);

void *copy_from_fs(void *dst, addr_t src, size_t len);
void copy_to_fs(addr_t dst, void *src, size_t len);

static inline void * memcpy(void * dest,const void *src,size_t count)
{
	char *tmp = (char *) dest, *s = (char *) src;

	while (count--)
		*tmp++ = *s++;

	return dest;
}
static inline  void *memset(void *dst,int c,size_t len)
{
	char *xs = (char *) dst;

	while (len--)
		*xs++ = c;

	return dst;
	
}
#endif
void console_init(void);

void initregs(struct biosregs *ireg);
void intcall(int intno, const struct biosregs *ireg, struct biosregs *oreg);

int e820_detect(struct boot_params *params);
void e820_show(const struct boot_params *params);

int enable_a20(void);

void set_video(void);
size_t strnlen(const char *s, size_t maxlen);

void go_to_protected_mode(void);
void __attribute__((noreturn)) protected_mode_jump(u32 entrypoint,u32 bootparams);
#endif /*__BOOT_H__*/
