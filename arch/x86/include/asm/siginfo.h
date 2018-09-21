#ifndef __SIGINFO_H_
#define __SIGINFO_H_

typedef struct siginfo{
	int si_signo;
	int si_errno;
	int si_code;
	
	union{
		struct {
			void *_addr;
		}_sigfault;

		struct {
			int _band;
			int _fd;
		}_sigpoll;
	}_sifields;

}siginfo_t;

#define si_addr _sifields._sigfault._addr


#define __SI_FAULT (3<<16)

#define SIGTRAP 5
#define SIGFPE 8


#define FPE_INTDIV (__SI_FAULT|1)
#endif
