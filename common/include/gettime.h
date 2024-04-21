#ifndef _GETTIME_H_
#define _GETTIME_H_

#include <stdint.h>

uint64_t gettime() {
	uint32_t hi, lo;

	asm ("rdtsc; movl %%edx, %0; movl %%eax, %1"
	: "=r" (hi), "=r" (lo)
	:
	: "%edx", "%eax"
	);

	uint64_t res = 0;
	res = hi;
	res = (res << 32) | lo;

	return res;
}

#endif /* _GETTIME_H_ */
