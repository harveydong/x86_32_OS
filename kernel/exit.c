#include <linux/kernel.h>
#include <linux/linkage.h>

NORET_TYPE void do_exit(long code)
{

}

asmlinkage long sys_exit(int error_code)
{
	do_exit(error_code);
}
