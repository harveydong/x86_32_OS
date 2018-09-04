#ifndef __VIDEO_H_
#define __VIDEO_H_
#include "types.h"
#include "boot.h"

#define ADAPTER_CGA	0	/* CGA/MDA/HGC */
#define ADAPTER_EGA	1
#define ADAPTER_VGA	2

 
#define VIDEO_FIRST_MENU 0x0000

/* Standard BIOS video modes (BIOS number + 0x0100) */
#define VIDEO_FIRST_BIOS 0x0100

/* VESA BIOS video modes (VESA number + 0x0200) */
#define VIDEO_FIRST_VESA 0x0200

/* Video7 special modes (BIOS number + 0x0900) */
#define VIDEO_FIRST_V7 0x0900


#define VIDEO_FIRST_VESA 0x0200
#define VIDEO_RECALC 0x8000
#define VIDEO_FIRST_SPECIAL 0x0f00
#define VIDEO_80x25 0x0f00
#define VIDEO_8POINT 0x0f01
#define VIDEO_80x43 0x0f02
#define VIDEO_80x28 0x0f03
#define VIDEO_CURRENT_MODE 0x0f04
#define VIDEO_80x30 0x0f05
#define VIDEO_80x34 0x0f06
#define VIDEO_80x60 0x0f07
#define VIDEO_GFX_HACK 0x0f08
#define VIDEO_LAST_SPECIAL 0x0f09

int adapter;			/* 0=CGA/MDA/HGC, 1=EGA, 2=VGA+ */
u16 video_segment;
int force_x, force_y;	/* Don't query the BIOS for cols/rows */

int do_restore;	
int graphic_mode;

struct mode_info {
	u16 mode;		/* Mode number (vga= style) */
	u16 x, y;		/* Width, height */
	u16 depth;		/* Bits per pixel, 0 for text mode */
};

struct card_info {
	const char *card_name;
	int (*set_mode)(struct mode_info *mode);
	int (*probe)(void);
	struct mode_info *modes;
	int nmodes;		/* Number of probed modes so far */
	int unsafe;		/* Probing is unsafe, only do after "scan" */
	u16 xmode_first;	/* Unprobed modes to try to call anyway */
	u16 xmode_n;		/* Size of unprobed mode range */
};

static inline u8 in_idx(u16 port, u8 index)
{
	outb(index, port);
	return inb(port+1);
}

static inline void out_idx(u8 v, u16 port, u8 index)
{
	outw(index+(v << 8), port);
}
int mode_defined(u16 mode);

#define __videocard struct card_info __attribute__((section(".videocards")))
extern struct card_info video_cards[], video_cards_end[];
#endif
