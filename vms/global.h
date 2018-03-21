/* FILE NAME "global.h"  by H.Hashimoto  2006.1.10 */

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <machine.h>
#include "iodefine.h"

// write back cache
#define CACHE_WB(type, var)		(*(type*)((unsigned long)&(var) & 0x1FFFFFFF))
// write through cache
#define CACHE_WT(type, var)		(*(type*)((unsigned long)&(var) & 0x1FFFFFFF | 0x80000000))
// no cache
#define CACHE_OFF(type, var)	(*(type*)((unsigned long)&(var) & 0x1FFFFFFF | 0xA0000000))

// cache control
#define CacheON() (P_CCR.BIT.CE=1)
#define CacheOFF() (P_CCR.BIT.CE=0)
#define CacheFLASH() (P_CCR.BIT.CF=1)


#define NR_SLAVES	7
#define NR_CORES	(NR_SLAVES + 1)
//#define NR_THREADS	10
//#define NR_TSKS		1

#define MASTER_ID(i)	(0)
#define SLAVE_ID(i)		(i)

// control BL, MD bit
#define BLoff() set_cr((get_cr()&0xefffffff))
#define BLon() set_cr((get_cr()|0x10000000))
#define USRmode() set_cr((get_cr()|0x40000000))

#endif /* _GLOBAL_H_ */
