#ifndef __MPSPEC_H_
#define __MPSPEC_H_

#define SMP_MAGIC_IDENT (('_' << 24) | ('P' << 16) | ('M' << 8)|'_')

struct intel_mp_floating{

	char mpf_signature[4];/*_MP_*/
	unsigned int mpf_physptr;
	unsigned char mpf_length;
	unsigned char mpf_specification;
	unsigned char mpf_checksum;
	unsigned char mpf_feature1;
	unsigned char mpf_feature2;
	unsigned char mpf_feature3;
	unsigned char mpf_feature4;
	unsigned char mpf_feature5;

};

extern void find_smp_config(void);

#endif
