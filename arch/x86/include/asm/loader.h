#ifndef __LOADER_H_
#define __LOADER_H_

void bios_read(unsigned int lba);

void change_to_real_header(void);
#endif
