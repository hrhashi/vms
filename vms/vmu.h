/* FILE NAME "vmu.h"  by H.Hashimoto  2005.12.8 */

#ifndef _VMU_H_
#define _VMU_H_

//#include <s_services.h>

#define BIT_SLAVE(i)	(0x00000001 << (i)) /* 0 <= i < NR_SLAVES */

struct st_vmu {
	unsigned int CORE_ID;
	unsigned int CORE_ID_SEND;
	unsigned int CORE_ID_STATES;
	unsigned int CORE_ID_RECEIVE;
	unsigned int CORE_ID_IRQOUT;
};

#define P_VMU	(*(volatile struct st_vmu*)	0xA7000000) /* VMU Address */

#pragma inline(vmu_amim_MS)
#pragma inline(vmu_sid_S)
#pragma inline(vmu_send_signal_M)
#pragma inline(vmu_receive_signal_M)
#pragma inline(vmu_send_signal_S)
#pragma inline(vmu_receive_signal_S)

static int vmu_amim_MS(void)
{
	return (P_VMU.CORE_ID == 0x00000000);	
}

static int vmu_sid_S(void)
{
	switch(P_VMU.CORE_ID) {
	case 0x00000000:
		return -1;
	case 0x00000001:
		return 0;
	case 0x00000002:
		return 1;
	case 0x00000004:
		return 2;
	case 0x00000008:
		return 3;
	case 0x00000010:
		return 4;
	case 0x00000020:
		return 5;
	case 0x00000040:
		return 6;
	case 0x00000080:
		return 7;
	case 0x00000100:
		return 8;
	default:
		return -1;
	}			
}

static void vmu_send_signal_M(unsigned int bitptn)
{
	P_VMU.CORE_ID_SEND = bitptn;
	while(P_VMU.CORE_ID_SEND);
	//sil_wrw_mem(P_VMU.CORE_ID_SEND, bitptn);
	//while(sil_rew_mem(P_VMU.CORE_ID_SEND));
}

static unsigned int vmu_receive_signal_M(void)
{
	unsigned int bitptn;
	
	bitptn = P_VMU.CORE_ID_RECEIVE;
	P_VMU.CORE_ID_IRQOUT |= bitptn;
	return bitptn;
	//bitptn = sil_rew_mem(P_VMU.CORE_ID_RECEIVE);
	//return bitptn;
}

static void vmu_send_signal_S(void)
{
	P_VMU.CORE_ID_SEND = 0x00000001;
	while(P_VMU.CORE_ID_SEND);
}

static void vmu_receive_signal_S(void)
{
	P_VMU.CORE_ID_STATES = 1;
}

#endif /* _VMU_H_ */
