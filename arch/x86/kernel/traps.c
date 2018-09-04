#include <asm/desc.h>

struct desc_struct idt_table[256] __attribute__((__section__(".data.idt"))) = {{0,0},};
