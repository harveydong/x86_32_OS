#include <linux/init.h>
#include <linux/spinlock.h>

static char[1024];
#define LOG_BUF_LEN (16384)
#define LOG_BUF_MASK (LOG_BUF_LEN - 1)


spinlock_t console_lock = SPIN_LOCK_UNLOCKED;

struct console *console_drivers;
static char log_buf[LOG_BUF_LEN];
static unsigned long log_start;
static unsigned long logged_chars;
unsigned long log_size;



asmlinkage int printk(const char *fmt,...)
{
	va_list args;
	int i;
	char *msg,*p,*buf_end;
	int line_feed;
	static signed char msg_level = -1;
	long flags;
	spin_lock_irqsave(&console_lock,flags);
	va_start(args,fmt);
	i = vsprintf(buf+3,fmt,args);
	buf_end = buf + 3 + i;
	va_end(args);

	for(p = buf + 3; p < buf_end; p++)
	{
		msg = p;
		if(msg_level < 0){
			if(p[0] != '<' || p[1] < '0' || p[1] > '7' || p[2] != '>'){
				p -= 3;
				p[0] = '<';
				p[1] = '0'+4;
				p[2] = '>';
			}else
				msg += 3;
			
			msg_level = p[1] - '0';
	
		}

		line_feed = 0;
		for(;p < buf_end; p++){
			log_buf[(log_start + log_size) & LOG_BUF_MASK] = *p;
			if(log_size < LOG_BUF_LEN)
				log_size++;
			else 
				log_start++;

			logged_chars++;
			if(*p == '\n'){
				line_feed = 1;
				break;
			}
		}

		if(msg_level < console_loglevel && console_drivers){
			struct console *c = console_drivers;
			while(c){
				if((c->flags & CON_ENABLED) && c->write)
					c->write(c,msg,p - msg + line_feed);
				c = c->next;
			}		
		}
		if(line_feed)
			msg_level = -1;
	}	


	spin_unlock_irqrestore(&console_lock,flags);
	
	return i;
}
