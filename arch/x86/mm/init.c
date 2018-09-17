#include <linux/init.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/io.h>
#include <asm/dma.h>
#include <asm/highmem.h>
#include <linux/mmzone.h>
#include <asm/pgtable-2level.h>
#include <asm/fixmap.h>
#include <linux/mm.h>
#include <linux/bootmem.h>

unsigned long highstart_pfn,highend_pfn;


void __init mem_init(void)
{

}
pte_t *kmap_pte;
pgprot_t kmap_prot;

#define kmap_get_fixmap_pte(vaddr)\
	pte_offset(pmd_offset(pgd_offset_k(vaddr),(vaddr)),(vaddr))

void __init kmap_init(void)
{
	unsigned long kmap_vstart;
	kmap_vstart = __fix_to_virt(FIX_KMAP_BEGIN);
	kmap_pte = kmap_get_fixmap_pte(kmap_vstart);
	kmap_prot = PAGE_KERNEL;
}
static void __init fixrange_init(unsigned long start,unsigned long end,pgd_t *pgd_base)
{
	unsigned long vaddr;
	int i,j;
	pgd_t *pgd;
	pmd_t *pmd;
	pte_t *pte;


	vaddr = start;
	i = __pgd_offset(vaddr);
	j = __pmd_offset(vaddr);
	
	pgd = pgd_base + i;
	
	for(; (i < PTRS_PER_PGD) && (vaddr != end); pgd++,i++){

#if X86_PAE
		if(pgd_none(*pgd)){
			pmd = (pmd_t*)alloc_bootmem_low_pages(PAGE_SIZE);
			set_pgd(pgd,__pgd(__pa(pmd) + 0x1));
			if(pmd != pmd_offset(pgd,0)){
				printk("PAE BUG #02!\n");
			}
		}
		
		pmd = pmd_offset(pgd,vaddr);
#else
		pmd = (pmd_t*)pgd;
#endif		
		for(; (j < PTRS_PER_PMD)&&(vaddr != end); pmd++,j++){
			if(pmd_none(*pmd)){
				pte = (pte_t *)alloc_bootmem_low_pages(PAGE_SIZE);
				set_pmd(pmd,__pmd(_KERNPG_TABLE+__pa(pte)));
				if(pte != pte_offset(pmd,0))
					BUG();
			}

			vaddr += PMD_SIZE;
		}
	}	

}
static void __init pagetable_init(void)
{
	unsigned long vaddr,end;
	pgd_t *pgd,*pgd_base;
	int i,j,k;
		
	pmd_t *pmd;
	pte_t *pte;

	end = (unsigned long)__va(max_low_pfn*PAGE_SIZE);
	pgd_base = swapper_pg_dir;

#if X86_PAE
	for(i = 0; i < PTRS_PER_PGD;i++){
		pgd = pgd_base + i;
		__pgd_clear(pgd);
	}

#endif

	i = __pgd_offset(PAGE_OFFSET);//kernel space 0xc000_0000
	pgd = pgd_base + i;

	printk("i is %x,and pgd:0x%p\n",i,pgd);
	printk("pgd base:0x%x\n",pgd_base);
	printk("boot user pgd ptrs:%d\n",BOOT_USER_PGD_PTRS);
	for(; i < PTRS_PER_PGD;pgd++,i++)
	{
		vaddr = i*PGDIR_SIZE;
		printk("first for i:%d,vaddr:0x%x,end:0x%x,and k:%d\n",i,vaddr,end,k);
		if(end && (vaddr >= end))
			break;
#if X86_PAE		
		pmd = (pmd_t*)alloc_bootmem_low_pages(PAGE_SIZE);
		set_pgd(pgd,__pgd(__pa(pmd)+0x1));
#else
		pmd = (pmd_t*)pgd;
#endif		

		printk("pmd:0x%x,and pmd_offset:0x%x\n",pmd,pmd_offset(pgd,0));
		if(pmd != pmd_offset(pgd,0))
			BUG();

		for(j = 0; j < PTRS_PER_PMD;pmd++,j++){
			vaddr = i*PGDIR_SIZE + j*PMD_SIZE;
			
			if(end && vaddr >= end)
				break;
		
			pte = (pte_t *)alloc_bootmem_low_pages(PAGE_SIZE);
			printk("pte:0x%x,pmd:0x%x\n",pte,pmd);
			set_pmd(pmd,__pmd(_KERNEL_TABLE+__pa(pte)));
			if(pte != pte_offset(pmd,0))
				BUG();

		
			for(k = 0; k < PTRS_PER_PTE;pte++,k++){
				vaddr = i*PGDIR_SIZE+j*PMD_SIZE+k*PAGE_SIZE;
				if(end && vaddr >=end)
					break;
		//		printk("pte:0x%x\n",pte);
				*pte = mk_pte_phys(__pa(vaddr),PAGE_KERNEL);
	
		//		printk("vaddr is 0x%x,pa:0x%x\n",vaddr,__pa(vaddr));	
		//		printk("k:%d,*pte is 0x%x\n",k,*pte);
			}	
		}
	}
	
//	printk("before fix_to_virt\n");
//	while(1);	
	vaddr = __fix_to_virt(__end_of_fixed_address - 1) & PMD_MASK;
	fixrange_init(vaddr,0,pgd_base);

	vaddr = PKMAP_BASE;
	fixrange_init(vaddr,vaddr+PAGE_SIZE*LAST_PKMAP,pgd_base);
	
	pgd = swapper_pg_dir + __pgd_offset(vaddr);
	pmd = pmd_offset(pgd,vaddr);
	pte = pte_offset(pmd,vaddr);
	
	pkmap_page_table = pte;

#if X86_PAE
	pgd_base[0] = pgd_base[USER_PTRS_PER_PGD];
#endif			

}

void __init paging_init(void)
{

	printk("into paging init\n");
	pagetable_init();
	__asm__("movl %%ecx,%%cr3"::"c"(__pa(swapper_pg_dir)));

	
//	set_in_cr4(X86_CR4_PAE);


	__flush_tlb_all();


	kmap_init();

	{
		unsigned long zones_size[MAX_NR_ZONES] = {0,0,0};
		unsigned int max_dma,high,low;
		
		max_dma = virt_to_phys((char*)MAX_DMA_ADDRESS) >> PAGE_SHIFT;
		
		low = max_low_pfn;
		high = highend_pfn;
		
		if(low < max_dma)
			zones_size[ZONE_DMA] = low;
		else{
			zones_size[ZONE_DMA] = max_dma;
			zones_size[ZONE_NORMAL] = low - max_dma;
			zones_size[ZONE_HIGHMEM] = high - low;	
		}

		free_area_init(zones_size);
				
	}


	return;
	
}
