#include <asm/boot.h>
#include <asm/disk.h>

struct disk_addr_packet {
        unsigned char packet_size;
        unsigned char reserved;
        unsigned short count;
        unsigned short buffer_offset;
        unsigned short buffer_seg;
        unsigned long start_block;
}__attribute__((packed));


static void __memcpy__(void *dst,void *src,uint32 len)
{
        char *tmp_dst = (char *)dst;
        char *tmp_src = (char *)src;
        while(len--){
                (*tmp_dst) = (*tmp_src);
                tmp_dst++;
                tmp_src++;
        }
}

int fill_disk_drive(unsigned driveno, struct disk_drive *drive)
{
        struct biosregs iregs, oregs;

        initregs(&iregs);
        iregs.ah = 8;
        iregs.dl = driveno;
        intcall(0x13, &iregs, &oregs);

        drive->driveno = driveno;
 

        return 0;
}



int read_block(const struct disk_drive *drive, unsigned block, void *memory)
{
        struct biosregs iregs, oregs;
        unsigned char tmp[512] = {0};

        struct disk_addr_packet buffer;


        //struct chs_addr chs;

        //lba_to_chs(drive, block, &chs);

        initregs(&iregs);
 
	buffer.packet_size = 0x10;
        buffer.reserved = 0;
        buffer.count = 1;
        buffer.buffer_offset =((unsigned short)tmp);
        buffer.buffer_seg = 0;
        buffer.start_block = block;

        iregs.ah = 0x42;
        iregs.dl = drive->driveno;
        iregs.si = (unsigned long) &buffer;
 
//	printf("read block here\n");
        intcall(0x13, &iregs, &oregs);
//	printf("read block done\n");

//        printf("return from read block,block:0x%x t1:%x,t2:%x\n",block,tmp[0],tmp[1]);
	__memcpy__(memory,tmp,512);//buffer, memory, 512);
  //      printf("return from read block,block:0x%x t1:%x,t2:%x\n",block,tmp[0],tmp[1]);
	return 0;
}

