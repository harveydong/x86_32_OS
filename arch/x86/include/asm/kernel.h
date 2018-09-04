#ifndef __KERNEL_H_
#define __KERNEL_H_

#define SEG_KCODE 1
#define SEG_KDATA 2


#define CR0_PE 0x00000001

#define KERNEL_ELF_LBA 80
#define SECT_SIZE 512

#define TMP_ADDR 0x11000

#define HEADER_ENTRY 0x8700

#endif
