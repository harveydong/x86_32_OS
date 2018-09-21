#include <linux/linkage.h>
#include <asm/errno.h>

asmlinkage long sys_ni_syscall(void)
{
	return -ENOSYS;
}
