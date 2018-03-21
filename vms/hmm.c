/* FILE NAME "hmm.c"  by H.Hashimoto  2005.12.9 */

#include "vms.h"

#define NR_SYMBOLS	5
#define N	10
#define M	5
const int o[N] = {4,2,1,3,0,1,2,1,4,3};

/*
#define NR_SYMBOLS	2
#define N	3
#define M	2
const int o[N] = {0, 1, 1};
*/

float a[M + 1][M + 1];
float b[M][NR_SYMBOLS];

int backptr[N][M];
int goalbp;

float prob[N][M];
float goalp;

int afo;
#define afo_n (*((volatile int*)(0xa0000000 + (char*)&afo)))

unsigned int myrand(void);
void mysrand(unsigned int);

void hmm_init(void)
{
	int i, j;
	float sum;
	
	afo = 9;
	afo_n = 10;
	
	for(i = 0; i <= M - 1; i++) {
		sum = 0;
		for(j = 0; j <= M; j++) {
			a[i][j] = (float)myrand();
			sum += a[i][j];
		}
		for(j = 0; j <= M; j++) {
			a[i][j] /= sum;
		}
	}
	
	sum = 0;
	for(j = 0; j <= M - 1; j++) {
		a[M][j] = (float)myrand();
		sum += a[M][j];
	}
	for(j = 0; j <= M - 1; j++) {
		a[M][j] /= sum;
	}
	
	a[M][M] = 0;
	
	for(i = 0; i <= M - 1; i++) {
		sum = 0;
		for(j = 0; j <= NR_SYMBOLS - 1; j++) {
			b[i][j] = (float)myrand();
			sum += b[i][j];
		}
		for(j = 0; j <= NR_SYMBOLS - 1; j++) {
			b[i][j] /= sum;
		}
	}
	
	/*a[M][0] = 1.0;
	a[M][1] = 0.0;
	a[M][M] = 0.0;
	a[0][0] = 0.6;
	a[0][1] = 0.4;
	a[0][M] = 0.0;
	a[1][0] = 0.0;
	a[1][1] = 0.2;
	a[1][M] = 0.8;
	
	b[0][0] = 0.7;
	b[0][1] = 0.3;
	b[1][0] = 0.6;
	b[1][1] = 0.4;*/
}

void hmm_likelihood(void)
{
	int i, n, m;
	double temp;
	
	for(i = 0; i <= M - 1; i++) {
		prob[0][i] = a[M][i];
		backptr[0][i] = M;
	}
	
	for(n = 0; n <= N - 2; n++) {
		for(i = 0; i <= M - 1; i++) {
			prob[n + 1][i] = 0;
			for(m = 0; m <= M - 1; m++) {
				temp =  prob[n][m] * a[m][i] * b[m][o[n]];
				if(prob[n + 1][i] < temp) {
					prob[n + 1][i] = temp;
					backptr[n + 1][i] = m;
				}
			}
		}
	}
	
	goalp = 0;
	for(m = 0; m <= M - 1; m++) {
		temp =  prob[N - 1][m] * a[m][M] * b[m][o[N - 1]];
		if(goalp < temp) {
			goalp = temp;
			goalbp = m;
		}
	}
}

static unsigned long next = 1;

static unsigned int myrand(void)
{
	next = next * 1103515245 + 12345;
	return (unsigned int)(next / 65536) % 32768;
}

static void mysrand(unsigned int seed)
{
	next = seed;
}
