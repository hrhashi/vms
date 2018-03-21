/* file name "initsct.c"  by H.Hashimoto */

extern int *_B_BGN,*_B_END,*_D_BGN,*_D_END,*_D_ROM;

void _INITSCT(void)
{
	register int *p, *q;
	for (p=_B_BGN; p<_B_END; p++) {
		*p=0;
	}
	
	for (p=_D_BGN, q=_D_ROM; p<_D_END; p++, q++) {
		*p=*q;
	}
}
