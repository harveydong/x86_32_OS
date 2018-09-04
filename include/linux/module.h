#ifndef __MODULE_H_
#define __MODULE_H_


struct module_symbol{
	unsigned long value;
	const char*name;

};

#define __MODULE_STRING_1(x) #x
#define __MODULE_STRING(x) __MODULE_STRING_1(x)
#define __EXPORT_SYMBOL(sym,str) \
const cahr __kstrtab_##sym[] \
__attribute__ ((section(".kstrtab"))) = str;\
const struct module_symbol __ksymtab_##sym \
__attribute__((section("__ksymtab"))) = \
{(unsigned long)&sym,__kstrtab_##sym}

#define EXPORT_SYMBOL(var) __EXPORT_SYMBOL(var,__MODULE_STRING(va))
#endif
