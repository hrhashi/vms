/* FILE NAME "vect.c"  by H.Hashimoto  2006.1.10 */

#include "vect.h"


#pragma section STK
long stk_area[20][THREAD_SIZE / sizeof(long)];
#pragma section

#pragma section SHARE
void (**vecttbl[NR_CORES])(void);
char* imasktbl[NR_CORES];
volatile void* sptbl[NR_CORES];

void (*vect_master[(INTCODE_MAX>>5) + 1])(void);
void (*vect_slave[(INTCODE_MAX>>5) + 1])(void);

char imask_master[(INTCODE_MAX>>5) + 1];
char imask_slave[(INTCODE_MAX>>5) + 1];
#pragma section

void* const isptbl[NR_CORES] = {
	&stk_area[MASTER_ID(0)][THREAD_SIZE / sizeof(long)],
	&stk_area[SLAVE_ID(1)][THREAD_SIZE / sizeof(long)],
	&stk_area[SLAVE_ID(2)][THREAD_SIZE / sizeof(long)],
	&stk_area[SLAVE_ID(3)][THREAD_SIZE / sizeof(long)],
	&stk_area[SLAVE_ID(4)][THREAD_SIZE / sizeof(long)],
	&stk_area[SLAVE_ID(5)][THREAD_SIZE / sizeof(long)],
	&stk_area[SLAVE_ID(6)][THREAD_SIZE / sizeof(long)],
	&stk_area[SLAVE_ID(7)][THREAD_SIZE / sizeof(long)],
};

static void init_vect(void)
{
	int i;

	for(i = 0; i <= (INTCODE_MAX>>5); i++) {
		vect_master[i] = term_idle;
		vect_slave[i] = term_idle;
		imask_master[i] = 0;
		imask_slave[i] = 0;
	}

	SET_VECT(vect_master, IRQ0, control_thread);
	SET_VECT(vect_slave, IRQ0, swap_thread);

	vecttbl[MASTER_ID(0)] = vect_master;
	vecttbl[SLAVE_ID(1)] = vect_slave;
	vecttbl[SLAVE_ID(2)] = vect_slave;
	vecttbl[SLAVE_ID(3)] = vect_slave;
	vecttbl[SLAVE_ID(4)] = vect_slave;
	vecttbl[SLAVE_ID(5)] = vect_slave;
	vecttbl[SLAVE_ID(6)] = vect_slave;
	vecttbl[SLAVE_ID(7)] = vect_slave;


	SET_IM(imask_master, TLB_MISS_READ, IP_TLB);
	SET_IM(imask_master, TLB_MISS_READ, IP_TLB);
	SET_IM(imask_master, TLB_MISS_WRITE, IP_TLB);
	SET_IM(imask_master, TLB_1ST_PAGE, IP_TLB);
	SET_IM(imask_master, TLB_PROTECT_READ, IP_TLB);
	SET_IM(imask_master, TLB_PROTECT_WRITE, IP_TLB);
	SET_IM(imask_master, ADR_ERROR_READ, IP_ADR);
	SET_IM(imask_master, ADR_ERROR_WRITE, IP_ADR);
	SET_IM(imask_master, TRAP, IP_TRAP);
	SET_IM(imask_master, ILLEGAL_INST, IP_ILLEGAL);
	SET_IM(imask_master, ILLEGAL_SLOT, IP_ILLEGAL);
	SET_IM(imask_master, NMI, IP_NMI);
	SET_IM(imask_master, IRQ0, IP_IRQ0);

	SET_IM(imask_slave, TLB_MISS_READ, IP_TLB);
	SET_IM(imask_slave, TLB_MISS_READ, IP_TLB);
	SET_IM(imask_slave, TLB_MISS_WRITE, IP_TLB);
	SET_IM(imask_slave, TLB_1ST_PAGE, IP_TLB);
	SET_IM(imask_slave, TLB_PROTECT_READ, IP_TLB);
	SET_IM(imask_slave, TLB_PROTECT_WRITE, IP_TLB);
	SET_IM(imask_slave, ADR_ERROR_READ, IP_ADR);
	SET_IM(imask_slave, ADR_ERROR_WRITE, IP_ADR);
	SET_IM(imask_slave, TRAP, IP_TRAP);
	SET_IM(imask_slave, ILLEGAL_INST, IP_ILLEGAL);
	SET_IM(imask_slave, ILLEGAL_SLOT, IP_ILLEGAL);
	SET_IM(imask_slave, NMI, IP_NMI);
	SET_IM(imask_slave, IRQ0, IP_IRQ0);


	imasktbl[MASTER_ID(0)] = imask_master;
	imasktbl[SLAVE_ID(1)] = imask_slave;
	imasktbl[SLAVE_ID(2)] = imask_slave;
	imasktbl[SLAVE_ID(3)] = imask_slave;
	imasktbl[SLAVE_ID(4)] = imask_slave;
	imasktbl[SLAVE_ID(5)] = imask_slave;
	imasktbl[SLAVE_ID(6)] = imask_slave;
	imasktbl[SLAVE_ID(7)] = imask_slave;
}

static void init_intc(void)
{
	//P_INTC.ICR.WORD = WORD_ICR;
	P_INTX.ICR1.WORD = WORD_ICR1;
	//P_INTX.ICR2.WORD = WORD_ICR2;
	//P_INTX.ICR3.WORD = WORD_ICR3;

	P_INTC.IPRA.WORD = WORD_IPRA;
	P_INTC.IPRB.WORD = WORD_IPRB;
	P_INTX.IPRC.WORD = WORD_IPRC;
	P_INTX.IPRD.WORD = WORD_IPRD;
	P_INTX.IPRE.WORD = WORD_IPRE;
}

static void init_vbr(void)
{
	set_vbr((void*)((unsigned long)EXPHandler - 0x100));
}


static void reset_master(void)
{
	CacheOFF();
	CacheFLASH();
	CacheON();

	init_vect();
	init_vbr();
	init_intc();
	set_imask(0); // interrput priority 0
	nop();
	
	_INITSCT(); // section initialize
		
	BLoff(); // block bit off
}

void reset_slave(void)
{
	CacheOFF();
	CacheFLASH();
	CacheON();

	init_vbr();
	init_intc();
	set_imask(0); // interrput priority 0
	nop();

	BLoff();
}

void aho(void)
{
	int k;
	k++;
}

void control_thread(void)
{
	struct thread_info* pth;
	struct pt_regs* regs;

	pth = (struct thread_info*)stk_area[8];
	pth->sp = GET_INITIAL_SP(pth);
	pth->id = 427;
	regs = GET_PT_REGS(pth->sp);

	
	CACHE_WT(long, regs->spc) = (long)aho;
	CACHE_WT(long, regs->ssr) = (long)get_cr();
	CACHE_WT(long, regs->pr) = (long)term_sleep;

	
	sptbl[SLAVE_ID(1)] = (void*)pth->sp;
	//vmu_send_signal_master();


}

void term_notify(void)
{
	//vmu_send_signal_slave();
	while(1);
	nop();
	nop();
}

void term_idle(void)
{
	while(1);
	nop();
	nop();
}

void term_sleep(void)
{
	
	while(1) sleep();
	nop();
	nop();
}