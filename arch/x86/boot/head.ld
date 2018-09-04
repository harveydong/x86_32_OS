/*.ld
 *
 * Linker script for the i386 setup code
 */
OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(setup)

SECTIONS
{
	. = 0x7d00;


	.header : { 
		*(.head.text)
	}

	inittext       : { *(.inittext) }
        .initdata       : { *(.initdata) }
        __end_init = .;

	.text : {*(.text)}
	.text32		: { *(.text32) }

	. = ALIGN(16);
	.rodata		: { *(.rodata*) }

	.videocards	: {
		video_cards = .;
		*(.videocards)
		video_cards_end = .;
	}

	. = ALIGN(16);
	.data		: { *(.data*) }

	. = ALIGN(16);
	.bss		:
	{
		__bss_start = .;
		*(.bss)
		__bss_end = .;
	}
	. = ALIGN(16);
	_end = .;


	 /DISCARD/ : { *(.note*) }
}

