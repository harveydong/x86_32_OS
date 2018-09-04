OUTPUT_FORMAT("elf64-x86-64","elf64-x86-64","elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(phys_startup_64)
PHDRS{
	text PT_LOAD FLAGS(5);
	data PT_LOAD FLAGS(7);
	user PT_LOAD FLAGS(5);
	percpu PT_LOAD FLAGS(6);
	init PT_LOAD FLAGS(7);
	note PT_NOTE FLAGS(0);
}

SECTIONS{
	. = (0xffffffff80000000 + ((0x1000000 + (0x1000000 - 1)) & ~(0x1000000 - 1))); 
	phys_startup_64 = startup_64 - 0xffffffff80000000;

	.text : AT(ADDR(.text) - 0xffffffff80000000){
	
		_text = .;
		*(.head.text)
		. = ALIGN(8);
		
		_stext = .;
		. = ALIGN(8);
		*(.text.hot)
		*(.text)
		
		. = ALIGN(8);
		__sched_text_start = .;
		*(.sched.text)
		__sched_text_end = .;
		
		. = ALIGN(8);
		__lock_text_start = .;
		*(.spinlock.text)
		__lock_text_end = .;

		. = ALIGN(8);
		__kprobes_text_start = .;
		*(.kprobes.text)
		__kprobes_text_end = .;
		
		. = ALIGN(8);
		__irqentry_text_start = .;
		*(.irqentry.text)
		__irqentry_text_end = .;
		
		*(.fixup)
		_etext = .; 
	}:text = 0x9090


	. = ALIGN((1<<12));
	.rodata : AT(ADDR(.rodata) - 0xffffffff80000000) {
		__start_rodata = .;
		*(.rodata)
		*(.rodata.*)
	}	
	.data : AT(ADDR(.data) - 0xffffffff80000000) {

		_sdata = .;
		. = ALIGN(((1 << 12) << 1));
		*(.data..init_task)
		. = ALIGN((1<<12));
		*(.data..page_aligned)
		*(.data)
		*(.ref.data)
		_edata = .;
	}:data
	. = ALIGN(4096);


	. = ALIGN(1 << 12);
	.bss : AT(ADDR(.bss) - 0xffffffff80000000) {
		__bss_start = .;
		*(.bss..page_aligned)
		*(.bss)
		. = ALIGN(4);
		__bss_stop = .;
	}
	
	. = ALIGN(1<<12);	
	__per_cpu_load = .;
	.data..percpu  : AT(__per_cpu_load - 0xffffffff80000000){
		__per_cpu_start = .;
		*(.data..percpu..first)
		*(.data..percpu..page_aligned)
		*(.data..percpu)
		*(.data..percpu..shared_aligned)
		__per_cpu_end = .;

	} :percpu . = __per_cpu_load + SIZEOF(.data..percpu);

	. = __per_cpu_end;
	. = ALIGN((1<<12));
	.init.text : AT(ADDR(.init.text) - 0xffffffff80000000){
		_sinittext = .;
		*(.init.text) 
		_einittext = .;
	}:init



	_end = .;

	/DISCARD/ : {
		*(.exit.text)
		
	}

	/DISCARD/ : {

		*(.eh_frame)
	}

	
}

init_per_cpu__gdt_page = gdt_page + __per_cpu_load;
init_per_cpu__irq_stack_union = irq_stack_union + __per_cpu_load;

