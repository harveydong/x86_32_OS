#include <asm/desc.h>
#include <linux/init.h>
#include <linux/linkage.h>
#include <asm/segment.h>
#include <asm/siginfo.h>
#include <asm/ptrace.h>
#include <asm/hw_irq.h>
#include <linux/sched.h>
#include <asm/desc.h>

struct desc_struct idt_table[256] __attribute__((__section__(".data.idt"))) = {{0,0},};

struct desc_struct default_ldt[] = {{0,0},{0,0},{0,0},{0,0},{0,0}};

static void inline do_trap(int trapnr,int signr,char*str,int vm86,struct pt_regs *regs,long error_code,siginfo_t *info)
{

}


asmlinkage void do_nmi(struct pt_regs *regs,long error_code)
{

}


asmlinkage void do_debug(struct pt_regs *regs,long error_code)
{

}

#define DO_VM86_ERROR(trapnr,signr,str,name)\
asmlinkage void do_##name(struct pt_regs *regs,long error_code)\
{\
	do_trap(trapnr,signr,str,1,regs,error_code,NULL);\
}

#define DO_VM86_ERROR_INFO(trapnr,signr,str,name,sicode,siaddr)\
asmlinkage void do_##name(struct pt_regs *regs,long error_code)\
{\
	siginfo_t info;\
	info.si_signo = signr;\
	info.si_errno = 0;\
	info.si_code = sicode;\
	info.si_addr = (void*)siaddr;\
	do_trap(trapnr,signr,str,1,regs,error_code,&info);\
}
DO_VM86_ERROR_INFO(0,SIGFPE,"divide error",divide_error,FPE_INTDIV,regs->eip)
DO_VM86_ERROR(3,SIGTRAP,"int3",int3);


asmlinkage int system_call(void);


asmlinkage void divide_error(void);
asmlinkage void debug(void);
asmlinkage void nmi(void);
asmlinkage void int3(void);
asmlinkage void page_fault(void);



#define _set_gate(gate_addr,type,dpl,addr) \
do{\
	int __d0,__d1;\
	__asm__ __volatile__("movw %%dx,%%ax\n\t"\
		"movw %4,%%dx\n\t"\
		"movl %%eax,%0\n\t"\
		"movl %%edx,%1"\
		:"=m"(*((long *)(gate_addr))),\
		 "=m"(*(1+(long*)(gate_addr))),"=&a"(__d0),"=&d"(__d1)\
		:"i"((short)(0x8000+(dpl << 13) + (type<<8))),\
		"3"((char*)(addr)),"2"(__KERNEL_CS<<16));\
}while(0)


#define _set_tssldt_desc(n,addr,limit,type)\
__asm__ __volatile__("movw %w3,0(%2)\n\t"\	
	"movw %%ax,2(%2)\n\t"\
	"rorl $16,%%eax\n\t"\
	"movb %%al,4(%2)\n\t"\
	"movb %4,5(%2)\n\t"\
	"movb $0,6(%2)\n\t"\
	"movb %%ah,7(%2)\n\t"\
	"rorl $16,%%eax"\
	:"=m"(*(n)):"a"(addr),"r"(n),"ir"(limit),"i"(type))


void set_tss_desc(unsigned int n,void *addr)
{
	_set_tssldt_desc(gdt_table+__TSS(n),(int)addr,235,0x89);
}


void set_ldt_desc(unsigned int n,void*addr,unsigned int size)
{
	_set_tssldt_desc(gdt_table+__LDT(n),(int)addr,((size << 3)-1),0x82);
}
static void __init set_trap_gate(unsigned int n,void *addr)
{
	_set_gate(idt_table+n,15,0,addr);

}

void set_intr_gate(unsigned int n,void *addr)
{
	_set_gate(idt_table+n,14,0,addr);
}
static void __init set_system_gate(unsigned int n,void *addr)
{
	_set_gate(idt_table+n,15,3,addr);
}

void __init trap_init(void)
{
	set_trap_gate(0,&divide_error);
	set_trap_gate(1,&debug);
	set_intr_gate(2,&nmi);		

	set_system_gate(3,&int3);
	set_trap_gate(14,&page_fault);
	
	set_system_gate(SYSCALL_VECTOR,&system_call);
	
	cpu_init();
	
}
