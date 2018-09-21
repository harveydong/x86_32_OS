#include <linux/linkage.h>
#include <asm/ptrace.h>
#include <linux/printk.h>

asmlinkage void do_page_fault(struct pt_regs *regs,unsigned long error_code)
{
	printk("do page fault here\n");
}
