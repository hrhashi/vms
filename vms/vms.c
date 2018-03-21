/* FILE NAME "vms.c"  by H.Hashimoto  2005.12.2 */

//#include <t_services.h>
#include "list.h"
#include "global.h"
#include "vmu.h"
#include "vms.h"

typedef struct list_head* THST;
typedef struct list_head* CRST;

struct thread {
	struct list_head link;	// linked list
	ID tskid;				// task ID
	int id; 				// thread ID
	UINT bit;				// thread state
	void (*start_routine)(void*); // startup routine
	void* arg;				// argument of startup routine
};

struct core {
	struct list_head link;	// linked list
	int id;					// core ID
	UINT bit;				// core state
	struct thread* pthread;	// pointer to thread
};

static LIST_HEAD(init_thread_list);
static LIST_HEAD(runnable_thread_list);
static LIST_HEAD(running_thread_list);
static LIST_HEAD(joinnable_thread_list);

static LIST_HEAD(idle_core_list);
static LIST_HEAD(busy_core_list);

#define TH_STATE_INIT		((THST)(&init_thread_list))
#define TH_STATE_RUNNABLE	((THST)(&runnable_thread_list))
#define TH_STATE_RUNNING	((THST)(&running_thread_list))
#define TH_STATE_JOINNABLE	((THST)(&joinnable_thread_list))

#define CORE_STATE_IDLE		((CRST)(&idle_core_list))
#define CORE_STATE_BUSY		((CRST)(&busy_core_list))


#define TH_BIT_INIT		0x00
#define TH_BIT_RUN		0x01
#define TH_BIT_JOIN		0x02
#define TH_BIT_ACTIVE	0x04

#define CORE_BIT_IDLE	0x00
#define CORE_BIT_BUSY	0x01

#define THREAD_NULL		0
#define NULL			0


#pragma inline(th_set_attr)
#pragma inline(th_change_state)
#pragma inline(th_set_bit)
#pragma inline(th_get_bit)
#pragma inline(th_test_bit)
#pragma inline(th_get_tskid)
#pragma inline(th_init)
#pragma inline(th_reset)
#pragma inline(state_get_thread)
#pragma inline(state_empty)

#pragma inline(core_set_attr)
#pragma inline(core_change_state)
#pragma inline(core_get_bit)
#pragma inline(core_init)
#pragma inline(core_reset)
#pragma inline(state_get_core)
#pragma inline(core_get_thread)


static void th_set_attr(struct thread* pth, ID tskid, void (*start_routine)(void *), void* arg)
{
	pth->tskid = tskid;
	pth->start_routine = start_routine;
	pth->arg = arg;
}

static void th_change_state(struct thread* pth, THST state)
{
	if(state == TH_STATE_INIT)
		pth->bit = TH_BIT_INIT;

	else if(state == TH_STATE_RUNNABLE) {
		pth->bit ^= TH_BIT_ACTIVE;
		pth->bit |= TH_BIT_RUN;
	}
	else if(state == TH_STATE_RUNNING) {
		pth->bit |= TH_BIT_ACTIVE;
		pth->bit |= TH_BIT_RUN;
	}
	else if(state ==  TH_STATE_JOINNABLE)
		pth->bit = TH_BIT_JOIN;

	list_move_tail(&pth->link, (struct list_head*)state);
}

static void th_set_bit(struct thread* pth, const UINT bit)
{
	pth->bit |= bit;
}

static UINT th_get_bit(const struct thread* pth, const UINT mask)
{
	return pth->bit & mask;
}

static int th_test_bit(const struct thread* pth, const UINT mask)
{
	return th_get_bit(pth, mask) == mask;
}

static ID th_get_tskid(const struct thread* pth)
{
	return pth->tskid;
}

static void th_init(struct thread* pth, int id)
{
	pth->id = id;
	pth->bit = TH_BIT_INIT;
	th_set_attr(pth, -1, NULL, NULL);
	list_add_tail(&pth->link, TH_STATE_INIT);
}

static void th_reset(struct thread* pth)
{
	th_set_attr(pth, -1, NULL, NULL);
	th_change_state(pth, TH_STATE_INIT);
}

static struct thread* state_get_thread(const THST state)
{	
	return container_of(state->next, struct thread, link);
}

static int state_empty(const THST state)
{
	return list_empty(state);
}

static void core_set_attr(struct core* pcore, struct thread* pth)
{
	pcore->pthread = pth;
}

static void core_change_state(struct core* pcore, CRST state)
{
	if(state == CORE_STATE_IDLE)
		pcore->bit = CORE_BIT_IDLE;

	else if(state == CORE_STATE_BUSY)
		pcore->bit |= CORE_BIT_BUSY;

	list_move_tail(&pcore->link, state);
}

static UINT core_get_bit(const struct core* pcore, const UINT mask)
{
	return pcore->bit & mask;
}

static void core_init(struct core* pcore, int id)
{
	pcore->id = id;
	core_set_attr(pcore, THREAD_NULL);
	list_add_tail(&pcore->link, CORE_STATE_IDLE);
}

static void core_reset(struct core* pcore)
{
	core_set_attr(pcore, THREAD_NULL);
	core_change_state(pcore, CORE_STATE_IDLE);
}

static struct core* state_get_core(const CRST state)
{
	return container_of(state->next, struct core, link);
}

static struct thread* core_get_thread(const struct core* pcore)
{
	return pcore->pthread;
}

/* ALLOCATE THREAD, CORE TABLE */
#pragma section VMS
static struct thread thread_table[NR_THREADS];
static struct core core_table[NR_SLAVES];
#pragma section

static volatile UINT jwc_table[NR_TSKS];

#pragma inline(vms_amim_MS)
int vms_amim_MS(void)
{
	return vmu_amim_MS();
}

void vms_init_M(void)
{
	int i;
	
	loc_cpu();
	
	for(i = 0; i < NR_THREADS; i++)
		th_init(&thread_table[i], i);
	
	for(i = 0; i < NR_SLAVES; i++)
		core_init(&core_table[i], i);

	for(i = 0; i < NR_TSKS; i++)
		jwc_table[i] = 0;

	unl_cpu();
}

void vms_main_S(void)
{
	while(1) {
		//sleep();
	}
}

void vms_inh_M(void)
{
	UINT bitptn;
	UINT mask;
	int i;
	
	struct thread* pth;
	struct core* pcore;
	
	bitptn = vmu_receive_signal_M();
	
	for(i = 0, pcore = &core_table[0]; i < NR_SLAVES; i++, pcore++) {
		mask = BIT_SLAVE(i);
		
		if(bitptn & mask) {
			pth = core_get_thread(pcore);
			
			if(th_get_bit(pth, TH_BIT_JOIN)) {
				if(--jwc_table[th_get_tskid(pth)] == 0)
					iwup_tsk(th_get_tskid(pth));
				th_reset(pth);
			} else {
				th_change_state(pth, TH_STATE_JOINNABLE);
			}
			
			if(state_empty(TH_STATE_RUNNABLE)) {
				core_reset(pcore);
				bitptn ^= mask;
			} else {
				pth = state_get_thread(TH_STATE_RUNNABLE);
				th_change_state(pth, TH_STATE_RUNNING);
				core_set_attr(pcore, pth);
			}
		}
	}
	
	if(bitptn)
		vmu_send_signal_M(bitptn);
}

void vms_inh_S(void)
{
	struct thread *pth;
	
	vmu_receive_signal_S();
	pth = core_table[vmu_sid_S()].pthread;
	(pth->start_routine)(pth->arg);
	vmu_send_signal_S();
}

ER thread_create(ID tskid, UINT* thread_id, void (*start_routine)(void *), void* arg)
{
	struct core* pcore;
	struct thread* pth;
	
	loc_cpu();
	
	if(state_empty(TH_STATE_INIT)) {
		unl_cpu();
		return FAILURE;
	}
	
	pth = state_get_thread(TH_STATE_INIT); 
	th_set_attr(pth, tskid, start_routine, arg);
	
	if(state_empty(CORE_STATE_IDLE)) {
		th_change_state(pth, TH_STATE_RUNNABLE);
	} else {
		th_change_state(pth, TH_STATE_RUNNING);
		pcore = state_get_core(CORE_STATE_IDLE);
		core_set_attr(pcore, pth);
		core_change_state(pcore, CORE_STATE_BUSY);
		
		vmu_send_signal_M(BIT_SLAVE(pcore->id));
	}
	
	(*thread_id) = pth->id;
	
	unl_cpu();
	return SUCCESS;
}

int thread_create_multi(ID tskid, UINT thread_id[], void (*start_routine[])(void *), void* arg[], int n)
{
	struct core* pcore;
	struct thread* pth;
	
	UINT bitptn;
	int i;
	
	loc_cpu();
	
	for(i = 0, bitptn = 0x00000000; i < n && !state_empty(CORE_STATE_IDLE); i++) {
		if(state_empty(TH_STATE_INIT)) {
			vmu_send_signal_M(bitptn);
			unl_cpu();
			return i;
		} else {
			pth = state_get_thread(TH_STATE_INIT);
			th_set_attr(pth, tskid, start_routine[i], arg[i]);
			th_change_state(pth, TH_STATE_RUNNING);
			thread_id[i] = pth->id;
			
			pcore = state_get_core(CORE_STATE_IDLE);
			core_set_attr(pcore, pth);
			core_change_state(pcore, CORE_STATE_BUSY);
			bitptn |= BIT_SLAVE(pcore->id);
		}
	}
	
	if(bitptn)
		vmu_send_signal_M(bitptn);
	
	for( ; i < n; i++) {
		if(state_empty(TH_STATE_INIT)) {
			unl_cpu();
			return i;
		} else {
			pth = state_get_thread(TH_STATE_INIT);
			th_set_attr(pth, tskid, start_routine[i], arg[i]);
			th_change_state(pth, TH_STATE_RUNNABLE);
			thread_id[i] = pth->id;
		}
	}
	
	unl_cpu();
	return i;
}

ER thread_join(UINT thread_id)
{
	struct thread* pth;

	loc_cpu();
	
	pth = &thread_table[thread_id];
	
	if(th_get_bit(pth, TH_BIT_JOIN)) {
		th_reset(pth);
		unl_cpu();
	} else {
		th_set_bit(pth, TH_BIT_JOIN);
		jwc_table[th_get_tskid(pth)]++;
		unl_cpu();
		slp_tsk();
	}
	
	return SUCCESS;
}

int thread_join_multi(UINT thread_id[], int n)
{
	int i;
	struct thread* pth;
	
	loc_cpu();
	
	for(i = 0; i < n; i++) {
		
		pth = &thread_table[thread_id[i]];
		
		if(th_get_bit(pth, TH_BIT_JOIN)) {
			th_reset(pth);
		} else {
			th_set_bit(pth, TH_BIT_JOIN);
			jwc_table[th_get_tskid(pth)]++;
		}
	}
	
	if(jwc_table[th_get_tskid(pth)] == 0) {
		unl_cpu();
	} else {
		unl_cpu();
		slp_tsk();
	}
	
	return n;
}

/*
void vms_disp(void)
{
	int i;
	UINT bit;
	struct thread* pth;
	char str[3];
	loc_cpu();
	
	for(i = 0; i < NR_THREADS; i++) {
		pth = &thread_table[i];
		if(th_test_bit(pth, TH_BIT_RUN))
			str[2] = 'R';
		else
			str[2] = '_';

		if(th_test_bit(pth, TH_BIT_JOIN))
			str[1] = 'J';
		else
			str[1] = '_';
		
		if(th_test_bit(pth, TH_BIT_ACTIVE))
			str[0] = 'A';
		else
			str[0] = '_';
		
		syslog_4(LOG_NOTICE, "thread[%d] = %c%c%c", i, str[0], str[1], str[2]);
	}
	
	unl_cpu();
}
*/
