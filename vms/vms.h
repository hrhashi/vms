/* FILE NAME "vms.h"  by H.Hashimoto  2005.12.2 */

#ifndef _VMS_H_
#define _VMS_H_

typedef unsigned int UINT;
typedef unsigned int ID;
typedef unsigned int ER;

#define loc_cpu()	BLon()
#define unl_cpu()	BLoff()
#define slp_tsk()	while(jwc_table[0])
#define iwup_tsk(a)

extern void vms_init_M(void);
extern void vms_main_S(void);
extern void vms_inh_M(void);
extern void vms_inh_S(void);
//extern void vms_disp(void);

#define SUCCESS		0
#define FAILURE		-1

extern ER thread_create(ID tskid, UINT *thread_id, void (*start_routine)(void *), void *arg);
extern int thread_create_multi(ID tskid, UINT thread_id[], void (*start_routine[])(void *), void *arg[], int n);
extern ER thread_join(UINT thread_id);
extern int thread_join_multi(UINT thread_id[], int n);

#endif /* _VMS_H_ */
