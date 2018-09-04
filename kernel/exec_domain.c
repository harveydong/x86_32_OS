#include <linux/personality.h>
#include <linux/types.h>

struct exec_domain default_exec_domain = {
	.name = "Linux",
	.handler = NULL,
	.pers_low = 0,
	.pers_high = 0xff,
	.signal_map = NULL,
	.signal_invmap = NULL,
	

};
