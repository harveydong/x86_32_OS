#ifndef __PERCPU_FUN_H_
#define __PERCPU_FUN_H_


#define __stringify_1(x...)	#x
#define __stringify(x...)	__stringify_1(x)
#define __percpu_seg	gs
#define __percpu_arg(x)	"%%"__stringify(__percpu_seg)":""%"#x

extern void __bad_percpu_size(void);


#define percpu_to_op(op,var,val)	\
	do{				\
		typedef typeof(var) pto_T__;			\
		if(0){			\
			pto_T__ pto_tmp__;	\
		}			\
		switch(sizeof(var)){	\
		case 1:			\
			asm(op"b %1,"__percpu_arg(0)\
				:"+m"(var)		\
				:"qi"((pto_T__)(val))); \
			break;			\
		case 2:				\
			asm(op"w %1,"__percpu_arg(0) \
				:"+m"(var)	\
				:"ri"((pto_T__)(val)));\
			break;			\
						\
		case 4:				\
			asm(op"l %1,"__percpu_arg(0)	\
				:"+m"(var)		\
				:"ri"((pto_T__)(val))); \
			break;				\
		case 8:					\
			asm(op"q %1,"__percpu_arg(0)	\
				:"+m"(var)		\
				:"re"((pto_T__)(val)));	\
			break;				\
		default:				\
			__bad_percpu_size();		\
		}					\
	}while(0)			

#define percpu_write(var,val)	percpu_to_op("mov",var,val)




#define percpu_from_op(op,var,constraint)	\
({						\
	typeof(var) pfo_ret__;			\
	switch(sizeof(var)){			\
	case 1:					\
		asm(op"b "__percpu_arg(P1)",%0"	\
			:"=q"(pfo_ret__)	\
			:constraint);		\
		break;				\
	case 2:					\
		asm(op"w "__percpu_arg(P1)",%0"	\
			:"=r"(pfo_ret__)	\
			:constraint);		\
		break;				\
	case 4:					\
		asm(op"l "__percpu_arg(P1)",%0"	\
			:"=r"(pfo_ret__)	\
			:constraint);		\
		break;				\
	case 8:					\
		asm(op"q "__percpu_arg(P1)",%0"	\
			:"=r"(pfo_ret__)	\
			:constraint);		\
		break;				\
	default:				\
		__bad_percpu_size();		\
						\
	}					\
	pfo_ret__;				\
})

#define percpu_read_stable(var)	percpu_from_op("mov",var,"p"(&(var)))

#define percpu_read(var)	percpu_from_op("mov",var,"m"(var))

#endif
