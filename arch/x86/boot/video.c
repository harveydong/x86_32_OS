#include <linux/types.h>
#include <asm/boot.h>
#include <asm/screen_info.h>
#include <asm/video.h>

u16 video_segment;
#define H(x)	((x)-'a'+10)
#define SCAN	((H('s')<<12)+(H('c')<<8)+(H('a')<<4)+H('n'))

#define VIDEO_CURRENT_MODE 0x0f04


extern struct boot_params params;

void store_cursor_position(void)
{
	struct biosregs ireg,oreg;
	
	initregs(&ireg);
	ireg.ah = 0x03;
	intcall(0x10,&ireg,&oreg);
	
	params.screen_info.orig_x = oreg.dl;
	params.screen_info.orig_y = oreg.dh;
	
	if (oreg.ch & 0x20)
		params.screen_info.flags |= VIDEO_FLAGS_NOCURSOR;

	if ((oreg.ch & 0x1f) > (oreg.cl & 0x1f))
		params.screen_info.flags |= VIDEO_FLAGS_NOCURSOR;

}

static void store_video_mode(void)
{
	struct biosregs ireg,oreg;
	
	initregs(&ireg);
	ireg.ah = 0x0f;
	intcall(0x10,&ireg,&oreg);
	
	params.screen_info.orig_video_mode = oreg.al & 0x7f;
	params.screen_info.orig_video_page = oreg.al & 0x7f;
	
}


static void store_mode_params(void)
{
	u16 font_size;
	int x,y;
	
	store_cursor_position();
	store_video_mode();
	if (params.screen_info.orig_video_mode == 0x07) {
		/* MDA, HGC, or VGA in monochrome mode */
		video_segment = 0xb000;
	} else {
		/* CGA, EGA, VGA and so forth */
		video_segment = 0xb800;
	}
 
	set_fs(0);
	font_size = rdfs16(0x485);
	
	params.screen_info.orig_video_points = font_size;

	x = rdfs16(0x44a);
	y = (adapter == ADAPTER_CGA) ? 25 : rdfs8(0x484)+1;

	if (force_x)
		x = force_x;
	if (force_y)
		y = force_y;

	params.screen_info.orig_video_cols  = x;
	params.screen_info.orig_video_lines = y;
}
static unsigned int get_entry(void)
{
	char entry_buf[4];
	int i,len = 0;
	int key;
	unsigned int v = 0;
	
	do{
		key = getchar();
		if (key == '\b') {
			if (len > 0) {
				puts("\b \b");
				len--;
			}
		} else if ((key >= '0' && key <= '9') ||
			   (key >= 'A' && key <= 'Z') ||
			   (key >= 'a' && key <= 'z')) {
			if (len < sizeof entry_buf) {
				entry_buf[len++] = key;
				my_putchar(key);
			}
		}
	}while(key != '\r');

	my_putchar('\n');
	
	if (len == 0)
		return VIDEO_CURRENT_MODE; /* Default */

	v = 0;
	for (i = 0; i < len; i++) {
		v <<= 4;
		key = entry_buf[i] | 0x20;
		v += (key > '9') ? key-'a'+10 : key-'0';
	}
	
	return v;
}
void probe_cards(int unsafe)
{
	struct card_info *card;
	static u8 probed[2];

	if (probed[unsafe])
		return;

	probed[unsafe] = 1;

	for (card = video_cards; card < video_cards_end; card++) {
		if (card->unsafe == unsafe) {
			if (card->probe)
				card->nmodes = card->probe();
			else
				card->nmodes = 0;
		}
	}
}

static void display_menu(void)
{
	struct card_info *card;
	struct mode_info *mi;
	char ch;
	int i;
	int nmodes;
	int modes_per_line;
	int col;

	nmodes = 0;
	for (card = video_cards; card < video_cards_end; card++)
		nmodes += card->nmodes;

	modes_per_line = 1;
	if (nmodes >= 20)
		modes_per_line = 3;

	for (col = 0; col < modes_per_line; col++)
		puts("Mode: Resolution:  Type: ");
	my_putchar('\n');

	col = 0;
	ch = '0';
	for (card = video_cards; card < video_cards_end; card++) {
		mi = card->modes;
		for (i = 0; i < card->nmodes; i++, mi++) {
			char resbuf[32];
			int visible = mi->x && mi->y;
			u16 mode_id = mi->mode ? mi->mode :
				(mi->y << 8)+mi->x;

			if (!visible)
				continue; /* Hidden mode */

			if (mi->depth)
				sprintf(resbuf, "%dx%d", mi->y, mi->depth);
			else
				sprintf(resbuf, "%d", mi->y);

			printf("%c %03X %4dx%-7s %-6s",
			       ch, mode_id, mi->x, resbuf, card->card_name);
			col++;
			if (col >= modes_per_line) {
				my_putchar('\n');
				col = 0;
			}

			if (ch == '9')
				ch = 'a';
			else if (ch == 'z' || ch == ' ')
				ch = ' '; /* Out of keys... */
			else
				ch++;
		}
	}
	if (col)
		my_putchar('\n');
}

static unsigned int mode_menu(void)
{
	int key;
	unsigned int sel;
	
	puts("Press <ENTER> to see video mode available, <SPACE> to continue,or wait 30 sec\n");
	
	kbd_flush();
	while(1){
		key = getchar_timeout();
		if(key == ' ' || key == 0)
			return VIDEO_CURRENT_MODE;
		if(key == '\r')
			break;
		my_putchar('\a');
	}

	for(;;){
	 	display_menu();
		puts("Enter a video mode or \"scan\" to scan for additional mode:");
		sel = get_entry();
		
		if(sel != SCAN)
			return sel;
		
		probe_cards(1);
	}
}

static struct saved_screen {
	int x, y;
	int curx, cury;
	u16 *data;
} saved;


static void save_screen(void)
{
	/* Should be called after store_mode_params() */
	saved.x = params.screen_info.orig_video_cols;
	saved.y = params.screen_info.orig_video_lines;
	saved.curx = params.screen_info.orig_x;
	saved.cury = params.screen_info.orig_y;

	if (!heap_free(saved.x*saved.y*sizeof(u16)+512))
		return;		/* Not enough heap to save the screen */

	saved.data = GET_HEAP(u16, saved.x*saved.y);

	set_fs(video_segment);
	copy_from_fs(saved.data, 0, saved.x*saved.y*sizeof(u16));
}



 

int mode_defined(u16 mode)
{
	struct card_info *card;
	struct mode_info *mi;
	int i;

	for (card = video_cards; card < video_cards_end; card++) {
		mi = card->modes;
		for (i = 0; i < card->nmodes; i++, mi++) {
			if (mi->mode == mode)
				return 1;
		}
	}

	return 0;
}


static void vga_recalc_vertical(void)
{
	unsigned int font_size, rows;
	u16 crtc;
	u8 pt, ov;

	set_fs(0);
	font_size = rdfs8(0x485); /* BIOS: font size (pixels) */
	rows = force_y ? force_y : rdfs8(0x484)+1; /* Text rows */

	rows *= font_size;	/* Visible scan lines */
	rows--;			/* ... minus one */

	crtc = vga_crtc();

	pt = in_idx(crtc, 0x11);
	pt &= ~0x80;		/* Unlock CR0-7 */
	out_idx(pt, crtc, 0x11);

	out_idx((u8)rows, crtc, 0x12); /* Lower height register */

	ov = in_idx(crtc, 0x07); /* Overflow register */
	ov &= 0xbd;
	ov |= (rows >> (8-1)) & 0x02;
	ov |= (rows >> (9-6)) & 0x40;
	out_idx(ov, crtc, 0x07);
}

static int raw_set_mode(u16 mode, u16 *real_mode)
{
	int nmode, i;
	struct card_info *card;
	struct mode_info *mi;

	/* Drop the recalc bit if set */
	mode &= ~VIDEO_RECALC;

	/* Scan for mode based on fixed ID, position, or resolution */
	nmode = 0;
	for (card = video_cards; card < video_cards_end; card++) {
		mi = card->modes;
		for (i = 0; i < card->nmodes; i++, mi++) {
			int visible = mi->x || mi->y;

			if ((mode == nmode && visible) ||
			    mode == mi->mode ||
			    mode == (mi->y << 8)+mi->x) {
				*real_mode = mi->mode;
				return card->set_mode(mi);
			}

			if (visible)
				nmode++;
		}
	}

	/* Nothing found?  Is it an "exceptional" (unprobed) mode? */
	for (card = video_cards; card < video_cards_end; card++) {
		if (mode >= card->xmode_first &&
		    mode < card->xmode_first+card->xmode_n) {
			struct mode_info mix;
			*real_mode = mix.mode = mode;
			mix.x = mix.y = 0;
			return card->set_mode(&mix);
		}
	}

	/* Otherwise, failure... */
	return -1;
}


int set_mode(u16 mode)
{
	int rv;
	u16 real_mode;

	/* Very special mode numbers... */
	if (mode == VIDEO_CURRENT_MODE)
		return 0;	/* Nothing to do... */
	else if (mode == NORMAL_VGA)
		mode = VIDEO_80x25;
	else if (mode == EXTENDED_VGA)
		mode = VIDEO_8POINT;

	rv = raw_set_mode(mode, &real_mode);
	if (rv)
		return rv;

	if (mode & VIDEO_RECALC)
		vga_recalc_vertical();

	/* Save the canonical mode number for the kernel, not
	   an alias, size specification or menu position */
 
	return 0;
}
void set_video(void)
{
	u16 mode = params.hdr.vid_mode;
	
	RESET_HEAP();
	store_mode_params();
	save_screen();
	
	probe_cards(0);

	for(;;){
		//if (mode == ASK_VGA)
			mode = mode_menu();
		
		if (!set_mode(mode))
			break;
		printf("Undefined video mode number: %x\n", mode);
		mode = ASK_VGA;

	}
	params.hdr.vid_mode = mode;
	 
	store_mode_params();
}
