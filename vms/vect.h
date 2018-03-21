/* FILE NAME "vect.h"  by H.Hashimoto  2006.1.10 */

#ifndef _VECT_H_
#define _VECT_H_

#include "global.h"


// RESET CODE definition
#define RESET_PON		0x000		// EXPEVT
#define RESET_MANUAL	0x020		// EXPEVT

// INTERRUPT CODE definition
#define TLB_MISS_READ	0x040		// EXPEVT
#define TLB_MISS_WRITE	0x060		// EXPEVT
#define TLB_1ST_PAGE	0x080		// EXPEVT
#define TLB_PROTECT_READ	0x0a0	// EXPEVT
#define TLB_PROTECT_WRITE	0x0c0	// EXPEVT
#define ADR_ERROR_READ	0x0e0		// EXPEVT
#define ADR_ERROR_WRITE	0x100		// EXPEVT
//#define RESERVED		0x120
//#define RESERVED 		0x140 
#define TRAP			0x160		// (with TRA)
#define ILLEGAL_INST	0x180		// EXPEVT
#define ILLEGAL_SLOT	0x1a0		// EXPEVT
#define NMI				0x1c0		// INTEVT
#define USER_BREAK		0x1e0 		// EXPEVT
#define IRL15			0x200 		// INTEVT
#define IRL14			0x220 		// INTEVT
#define IRL13			0x240 		// INTEVT
#define IRL12			0x260		// INTEVT
#define IRL11			0x280 		// INTEVT
#define IRL10			0x2a0 		// INTEVT
#define IRL9			0x2c0 		// INTEVT
#define IRL8			0x2e0 		// INTEVT
#define IRL7			0x300 		// INTEVT
#define IRL6			0x320 		// INTEVT
#define IRL5			0x340 		// INTEVT
#define IRL4			0x360 		// INTEVT
#define IRL3			0x380 		// INTEVT
#define IRL2			0x3a0 		// INTEVT
#define IRL1			0x3c0 		// INTEVT
//#define RESERVED 		0x3e0
#define TMU0_TUNI0		0x400 		// INTEVT
#define TMU1_TUNI1		0x420 		// INTEVT
#define TMU2_TUNI2		0x440 		// INTEVT
#define TMU2_TICPI2		0x460 		// INTEVT
#define RTC_ATI			0x480 		// INTEVT
#define RTC_PRI			0x4a0 		// INTEVT
#define RTC_CUI			0x4c0 		// INTEVT
#define SCI_ERI			0x4e0 		// INTEVT
#define SCI_RXI			0x500 		// INTEVT
#define SCI_TXI			0x520 		// INTEVT
#define SCI_TEI			0x540 		// INTEVT
#define WDT_ITI			0x560 		// INTEVT
#define REF_RCMI		0x580 		// INTEVT
#define DEF_RPVI		0x5a0 		// INTEVT
#define DMA_ADDERR		0x5c0 		// EXPEVT
#define H_UDI			0x5e0 		// INTEVT2
#define	IRQ0			0x600 		// INTEVT2
#define IRQ1			0x620 		// INTEVT2
#define IRQ2			0x640 		// INTEVT2
#define IRQ3			0x660 		// INTEVT2
#define IRQ4			0x680 		// INTEVT2
#define IRQ5			0x6a0 		// INTEVT2
//#define RESERVED 		0x6c0
//#define RESERVED 		0x6e0
#define PINT0_7 		0x700		// INTEVT2
#define PINT0_15		0x720 		// INTEVT2
//#define RESERVED		0x740
//#define RESERVED		0x760
//#define RESERVED		0x780
//#define RESERVED		0x7A0
//#define RESERVED		0x7C0
//#define RESERVED		0x7E0
#define DMAC_DEI0		0x800
#define DMAC_DEI1		0x820
#define	DMAC_DEI2		0x840
#define DMAC_DEI3		0x860
#define	IrDA_ERI1		0x880
#define	IrDA_RXI1		0x8A0
#define IrDA_BRI1		0x8C0
#define IrDA_TXI1		0x8E0
#define SCIF_ERI2		0x900
#define SCIF_RXI2		0x920
#define SCIF_BRI2		0x940
#define SCIF_TXI2		0x960
#define ADC_ADI			0x980
//#define	RESERVED	0x9A0
//#define	RESERVED	0x9C0
//#define	RESERVED	0x9E0
#define TLBCPU_ADERR	0xA00		// (H'070)
#define	TLBPROTECT_LOOP	0xA20		// (H'0D0)

#define INTCODE_MAX		TLBPROTECT_LOOP


// INTERRUPT PRIORITY definition
#define	IP_TLB		15
#define IP_ADR		15
#define	IP_TRAP		15
#define IP_ILLEGAL	15
#define IP_NMI		15
#define IP_TMU0		0
#define IP_TMU1		0
#define IP_TMU2		0
#define IP_RTC		0
#define IP_SCI		0
#define IP_WDT		0
#define IP_REF		0
#define IP_IRQ0		15
#define IP_IRQ1		0
#define IP_IRQ2		0
#define IP_IRQ3		0
#define IP_IRQ4		0
#define IP_IRQ5		0
#define IP_PINT0_7	0
#define IP_PINT8_15	0
#define IP_DMAC		0
#define IP_IrDA		0
#define IP_SCIF		0
#define IP_ADC		0
#define IP_RESERVED	15

// IPRx definition
#define WORD_IPRA	((IP_TMU0<<12)|(IP_TMU1<<8)|(IP_TMU2<<4)|(IP_RTC))
#define WORD_IPRB	((IP_WDT<<12)|(IP_REF<<8)|(IP_SCI<<4)|(0))
#define WORD_IPRC	((IP_IRQ3<<12)|(IP_IRQ2<<8)|(IP_IRQ1<<4)|(IP_IRQ0))
#define WORD_IPRD	((IP_PINT0_7<<12)|(IP_PINT8_15<<8)|(IP_IRQ5<<4)|(IP_IRQ4))
#define WORD_IPRE	((IP_DMAC<<12)|(IP_IrDA<<8)|(IP_SCIF<<4)|(IP_ADC))

// ICRx definition
#define WORD_ICR
#define WORD_ICR1	0x0002  //irq0 sense select (low level)
#define WORD_ICR2
#define WORD_ICR3


// vector table
extern void (**vecttbl[])(void);

#define SET_VECT(vect_, intcode, handler)	(vect_[(intcode)>>5] = (handler))

// interrupt mask (im) table
extern char* imasktbl[];

#define SET_IM(imask_, intcode, ip)		(imask_[(intcode)>>5] = ((ip)<<4))

// initial stack pointer (isp) table
extern void* const isptbl[];

// stack pointer (sp) table
extern volatile void* sptbl[];


// interrupt handler
extern void EXPHandler(void);

extern void reset_master(void);
extern void reset_slave(void);

extern void control_thread(void);
extern void swap_thread(void);

extern void term_idle(void);
extern void term_sleep(void);
extern void term_notify(void);


// thread size
#define THREAD_SIZE 	(4096) // 4 KB

struct thread_info {
	int id;
	unsigned long sp;
};

struct pt_regs {
	long r14;
	long r13;
	long r12;
	long r11;
	long r10;
	long r9;
	long r8;
	long r7;
	long r6;
	long r5;
   	long r4;
   	long r3;
	long r2;
    long r1;
    long r0;
	long mach;
	long macl;
	long gbr;
    long pr;
    long spc;
	long ssr;
};

union thread_union {
	struct thread_info info;
	long stack[THREAD_SIZE / sizeof(long)];
};

#define GET_INITIAL_SP(pth)		((unsigned long)((unsigned long)(pth) + sizeof(union thread_union) - sizeof(struct pt_regs)))

#define GET_THREAD_INFO(sp)		((struct thread_info*)(~(THREAD_SIZE - 1) & (sp)))
#define GET_PT_REGS(sp)			((struct pt_regs*)(sp))


#endif /* _VECT_H_ */
