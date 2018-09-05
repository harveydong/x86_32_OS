#include <linux/init.h>
#include <linux/linkage.h>
#include <linux/console.h>
#include <asm/io.h>
#include <linux/types.h>
#include <asm/page.h>
#include <linux/compiler.h>
#include <linux/stdarg.h>

//extern struct boot_params boot_params;

#define __ISA_IO_base ((char __iomem*)(__PAGE_OFFSET))
#define VGABASE (__ISA_IO_base + 0xb8000)

static int max_ypos = 25,max_xpos = 80;
static int current_ypos = 25,current_xpos ;

static void early_vga_write(struct console *con,const char *str,unsigned n)
{
	char c;
	int i,k,j;

	while((c = *str++)!='\0' && n-- > 0){
		if(current_ypos >= max_ypos){
			for(k = 1, j = 0; k < max_ypos; k++,j++){

				for(i = 0; i < max_xpos; i++){
					
					writew(readw(VGABASE + 2*(max_xpos*k+i)),VGABASE+2*(max_xpos*j+i));

				}
			}
			for(i = 0; i < max_xpos;i++){
				writew(0x720,VGABASE+2*(max_xpos*j+i));
			}
			current_ypos = max_ypos - 1;
		}

		if(c == '\n'){
			current_xpos =0;
			current_ypos++;
		}else if(c != '\r'){
			writew(((0x7 << 8)|(unsigned short)c),VGABASE+2*(max_xpos*current_ypos+current_xpos++));
			if(current_xpos >= max_xpos){
				current_xpos = 0;
				current_ypos++;
			}
	
		}	
	}
}

static struct console early_vga_console = {
	.name = "earlyvga",
	.write = early_vga_write,
	.flags = 1,
	.index = -1,
};

static struct console *early_console = &early_vga_console;



#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SMALL	32		/* Must be 32 == 0x20 */
#define SPECIAL	64		/* 0x */

#define do_div(n,base) ({ \
int __res; \
__res = ((unsigned long) n) % (unsigned) base; \
n = ((unsigned long) n) / (unsigned) base; \
__res; })


static inline int isdigit(int ch)
{
	return (ch >= '0') && (ch <= '9');
}


static int skip_atoi(const char **s)
{
	int i = 0;

	while (isdigit(**s))
		i = i * 10 + *((*s)++) - '0';
	return i;
}


static char *number(char *str, long num, int base, int size, int precision,
		    int type)
{
	/* we are called with base 8, 10 or 16, only, thus don't need "G..."  */
	static const char digits[16] = "0123456789ABCDEF"; /* "GHIJKLMNOPQRSTUVWXYZ"; */

	char tmp[66];
	char c, sign, locase;
	int i;

	/* locase = 0 or 0x20. ORing digits or letters with 'locase'
	 * produces same digits or (maybe lowercased) letters */
	locase = (type & SMALL);
	if (type & LEFT)
		type &= ~ZEROPAD;
	if (base < 2 || base > 36)
		return NULL;
	c = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;
	if (type & SIGN) {
		if (num < 0) {
			sign = '-';
			num = -num;
			size--;
		} else if (type & PLUS) {
			sign = '+';
			size--;
		} else if (type & SPACE) {
			sign = ' ';
			size--;
		}
	}
	if (type & SPECIAL) {
		if (base == 16)
			size -= 2;
		else if (base == 8)
			size--;
	}
	i = 0;
	if (num == 0)
		tmp[i++] = '0';
	else
		while (num != 0)
			tmp[i++] = (digits[do_div(num, base)] | locase);
	if (i > precision)
		precision = i;
	size -= precision;
	if (!(type & (ZEROPAD + LEFT)))
		while (size-- > 0)
			*str++ = ' ';
	if (sign)
		*str++ = sign;
	if (type & SPECIAL) {
		if (base == 8)
			*str++ = '0';
		else if (base == 16) {
			*str++ = '0';
			*str++ = ('X' | locase);
		}
	}
	if (!(type & LEFT))
		while (size-- > 0)
			*str++ = c;
	while (i < precision--)
		*str++ = '0';
	while (i-- > 0)
		*str++ = tmp[i];
	while (size-- > 0)
		*str++ = ' ';
	return str;
}
static size_t strnlen(const char *s, size_t maxlen)
{
	const char *es = s;
	while (*es && maxlen) {
		es++;
		maxlen--;
	}

	return (es - s);
}

int vsprintf(char *buf, const char *fmt, va_list args)
{
	int len;
	unsigned long num;
	int i, base;
	char *str;
	const char *s;

	int flags;		/* flags to number() */

	int field_width;	/* width of output field */
	int precision;		/* min. # of digits for integers; max
				   number of chars for from string */
	int qualifier;
	
	for (str = buf; *fmt; ++fmt) {
		if (*fmt != '%') {
			*str++ = *fmt;
			continue;
		}
		
		flags = 0;
	      repeat:
		++fmt;		/* this also skips first '%' */
		switch (*fmt) {
		case '-':
			flags |= LEFT;
			goto repeat;
		case '+':
			flags |= PLUS;
			goto repeat;
		case ' ':
			flags |= SPACE;
			goto repeat;
		case '#':
			flags |= SPECIAL;
			goto repeat;
		case '0':
			flags |= ZEROPAD;
			goto repeat;
		}
		
		field_width = -1;
		if (isdigit(*fmt))
			field_width = skip_atoi(&fmt);
		else if (*fmt == '*') {
			++fmt;
			/* it's the next argument */
			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}
		
		precision = -1;
		if (*fmt == '.') {
			++fmt;
			if (isdigit(*fmt))
				precision = skip_atoi(&fmt);
			else if (*fmt == '*') {
				++fmt;
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}
		
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
			qualifier = *fmt;
			++fmt;
		}

		/* default base */
		base = 10;
		switch (*fmt) {
		case 'c':
			if (!(flags & LEFT))
				while (--field_width > 0)
					*str++ = ' ';
			*str++ = (unsigned char)va_arg(args, int);
			while (--field_width > 0)
				*str++ = ' ';
			continue;

		case 's':
			s = va_arg(args, char *);
			len = strnlen(s, precision);

			if (!(flags & LEFT))
				while (len < field_width--)
					*str++ = ' ';
			for (i = 0; i < len; ++i)
				*str++ = *s++;
			while (len < field_width--)
				*str++ = ' ';
			continue;

		case 'p':
			if (field_width == -1) {
				field_width = 2 * sizeof(void *);
				flags |= ZEROPAD;
			}
			str = number(str,
				     (unsigned long)va_arg(args, void *), 16,
				     field_width, precision, flags);
			continue;

		case 'n':
			if (qualifier == 'l') {
				long *ip = va_arg(args, long *);
				*ip = (str - buf);
			} else {
				int *ip = va_arg(args, int *);
				*ip = (str - buf);
			}
			continue;

		case '%':
			*str++ = '%';
			continue;

			/* integer number formats - set up the flags and "break" */
		case 'o':
			base = 8;
			break;

		case 'x':
			flags |= SMALL;
		case 'X':
			base = 16;
			break;

		case 'd':
		case 'i':
			flags |= SIGN;
		case 'u':
			break;

		default:
			*str++ = '%';
			if (*fmt)
				*str++ = *fmt;
			else
				--fmt;
			continue;
		}
		if (qualifier == 'l')
			num = va_arg(args, unsigned long);
		else if (qualifier == 'h') {
			num = (unsigned short)va_arg(args, int);
			if (flags & SIGN)
				num = (short)num;
		} else if (flags & SIGN)
			num = va_arg(args, int);
		else
			num = va_arg(args, unsigned int);
		str = number(str, num, base, field_width, precision, flags);
	}
	*str = '\0';
	return str - buf;
}

static int vscnprintf(char *buf,size_t size,const char *fmt,va_list args)
{
	int i;
	
	i = vsprintf(buf,fmt,args);
	
	return (i >= size)?(size -1 ):i;
}



asmlinkage void printk(const char *fmt,...)
{
	char buf[512];
	int n;
	va_list ap;
	
	va_start(ap,fmt);
	n = vscnprintf(buf,sizeof(buf),fmt,ap);
	early_console->write(early_console,buf,n);
	va_end(ap);
	

}

int __init setup_early_printk()
{
	max_xpos = 80; //boot_params.screen_info.orig_video_cols;
	max_ypos = 25;//boot_params.screen_info.orig_video_lines;
	current_ypos = 1;//boot_params.screen_info.orig_y;

	return 0;		
}
