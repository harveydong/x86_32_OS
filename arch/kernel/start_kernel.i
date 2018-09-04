# 1 "start_kernel.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "start_kernel.c"
# 1 "init.h" 1
# 2 "start_kernel.c" 2
# 1 "types.h" 1



typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;
typedef signed char __s8;
typedef unsigned char __u8;

typedef unsigned long long __u64;
typedef unsigned short __u16;
typedef unsigned int __u32;

typedef short __s16;
typedef int __s32;
typedef long long __s64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;

typedef __u64 Elf64_Addr;
typedef __u16 Elf64_Half;
typedef __s16 Elf64_SHalf;
typedef __u64 Elf64_Off;
typedef __s32 Elf64_Sword;
typedef __u32 Elf64_Word;
typedef __u64 Elf64_Xword;
typedef __s64 Elf64_Sxword;

typedef unsigned int size_t;


typedef unsigned int addr_t;
# 3 "start_kernel.c" 2
# 1 "stackprotector.h" 1




# 1 "msr_fun.h" 1







static inline unsigned long long __native_read_tsc(void)
{
 unsigned low,high;
 asm volatile("rdtsc":"=a"(low),"=d"(high));

 return ((low) | ((u64)(high) << 32));
}
# 6 "stackprotector.h" 2
# 1 "percpu.h" 1


# 1 "page_64_types.h" 1
# 4 "percpu.h" 2
# 1 "const.h" 1
# 5 "percpu.h" 2
# 7 "stackprotector.h" 2
# 1 "current.h" 1


# 1 "percpu_defs.h" 1
# 4 "current.h" 2
# 1 "percpu_fun.h" 1
# 10 "percpu_fun.h"
extern void __bad_percpu_size(void);
# 5 "current.h" 2

# 1 "sched.h" 1



# 1 "thread_info.h" 1



# 1 "uaccess.h" 1


# 1 "processor.h" 1





typedef struct {
 unsigned long seg;
}mm_segment_t;



union irq_stack_union{
 char irq_stack[((1 << 12) << 2)];
 struct {
  char gs_base[40];
  unsigned long stack_canary;
 };
};
# 4 "uaccess.h" 2
# 5 "thread_info.h" 2







struct task_struct;
struct thread_info {
 struct task_struct *task;

 unsigned long flags;
 __u32 status;

 __u32 cpu;
 int preempt_count;
 mm_segment_t addr_limit;

 void *sysenter_return;
 int uaccess_err;

};
# 5 "sched.h" 2
# 1 "list.h" 1



struct list_head{
 struct list_head *next,*prev;
};
# 6 "sched.h" 2
# 16 "sched.h"
union thread_union {
 struct thread_info thread_info;
 unsigned long stack[((1 << 12) << 1)/sizeof(long)];
};

struct task_struct{
 volatile long state;
 void *stack;
 unsigned int flags;
 int prio,static_prio;
 unsigned int policy;
 struct mm_struct *mm,*active_mm;
 struct list_head tasks;
 unsigned long stack_canary;

};
extern union thread_union init_thread_union;
extern struct task_struct init_task;
extern struct mm_struct init_mm;
# 7 "current.h" 2

struct task_struct;

extern __attribute((section(".data..percpu" ""))) __typeof__(struct task_struct *) current_task;

static inline struct task_struct *get_current(void)
{
 return ({ typeof(current_task) pfo_ret__; switch(sizeof(current_task)){ case 1: asm("mov""b ""%%""gs"":""%""P1"",%0" :"=q"(pfo_ret__) :"p"(&(current_task))); break; case 2: asm("mov""w ""%%""gs"":""%""P1"",%0" :"=r"(pfo_ret__) :"p"(&(current_task))); break; case 4: asm("mov""l ""%%""gs"":""%""P1"",%0" :"=r"(pfo_ret__) :"p"(&(current_task))); break; case 8: asm("mov""q ""%%""gs"":""%""P1"",%0" :"=r"(pfo_ret__) :"p"(&(current_task))); break; default: __bad_percpu_size(); } pfo_ret__; });
}
# 8 "stackprotector.h" 2


extern union irq_stack_union irq_stack_union;

static inline void boot_init_stack_canary(void)
{
 u64 canary;
 u64 tsc;


 tsc = __native_read_tsc();

 canary += tsc + (tsc << 32UL);
 get_current()->stack_canary = canary;

 do{ typedef typeof(irq_stack_union.stack_canary) pto_T__; if(0){ pto_T__ pto_tmp__; } switch(sizeof(irq_stack_union.stack_canary)){ case 1: asm("mov""b %1,""%%""gs"":""%""0" :"+m"(irq_stack_union.stack_canary) :"qi"((pto_T__)(canary))); break; case 2: asm("mov""w %1,""%%""gs"":""%""0" :"+m"(irq_stack_union.stack_canary) :"ri"((pto_T__)(canary))); break; case 4: asm("mov""l %1,""%%""gs"":""%""0" :"+m"(irq_stack_union.stack_canary) :"ri"((pto_T__)(canary))); break; case 8: asm("mov""q %1,""%%""gs"":""%""0" :"+m"(irq_stack_union.stack_canary) :"re"((pto_T__)(canary))); break; default: __bad_percpu_size(); } }while(0);
}
# 4 "start_kernel.c" 2
# 1 "printk.h" 1




 void early_printk(const char *fmt, ...);
# 5 "start_kernel.c" 2


 void __attribute__((__section__(".init.text"))) __attribute__((no_instrument_function)) start_kernel(void)
{
 boot_init_stack_canary();
 early_printk("ok I am now start kernel ....\n\n\n");
 while(1);
}
