#include "Nano100Series.h"
#include <stdio.h>
#include <stdlib.h>
//---
void peak_init(void);
uint32_t peak(uint32_t py);
uint32_t peak_AC_DC(uint32_t py, uint32_t* UP, uint32_t* DOWN);
//---
uint32_t plv=300;
uint32_t pmax=0, pmin=5000;

uint32_t mr[50], nr[50];
uint32_t px=0;

uint32_t pdir=1;
uint32_t maxn=0, minn=50000000;
uint32_t pnn=0, ni=0;
uint16_t an=10; //


uint32_t peak(uint32_t py){
	//uint32_t dd=0;
	px++;
	if (pdir ==1){
		if (py > pmax){
			pmax = py;
			maxn = px;
		}else{
			if((pmax - py) > plv){
				pdir = 0;
				pmin = pmax;
				PA12=1;
				//printf("%d 0\n", py);
				//---
				{
					uint32_t a, b, c;
					a = mr[ni];
					mr[ni++] = maxn;
					if(ni>(an-1)){ ni=0; }
					b = maxn;
					c = an*6000/(b-a);
					pnn++;
					if(pnn==5){ return 3*6000/(maxn-mr[1]);	}
					if(pnn<an+2){ printf("0 0\n"); return 0; }
					printf("%d 0\n", c);
					return c;
				}
			}
		}
	}else{
		if (py < pmin){
			pmin = py;
			minn = px;
		}else{
			if((py - pmin) > plv){
				double R = ((double)(pmax))/((double)(pmin));
				pdir = 1;
				pmax = pmin;
				PA12=0;
				printf("0 %f\n", R);
				return 0;
			}
		}
	}
	printf("0 0\n");
	return 0;
}

double peak2(uint32_t py){
	//uint32_t dd=0;
	px++;
	if (pdir ==1){
		if (py > pmax){
			pmax = py;
			maxn = px;
		}else{
			if((pmax - py) > plv){
				pdir = 0;
				pmin = pmax;
				PA12=1;
				//printf("%d 0\n", py);
				//---
				{
					uint32_t a, b;
					double c;
					a = mr[ni];
					mr[ni++] = maxn;
					if(ni>(an-1)){ ni=0; }
					b = maxn;
					c = (double)(an*6000/(b-a));
					pnn++;
					if(pnn==5){ return 3*6000/(maxn-mr[1]);	}
					if(pnn<an+2){ return 0; }
					//printf("%f 0\n", c);
					return -c;
				}
			}
		}
	}else{
		if (py < pmin){
			pmin = py;
			minn = px;
		}else{
			if((py - pmin) > plv){
				double R = ((double)(pmax))/((double)(pmin));
				pdir = 1;
				pmax = pmin;
				PA12=0;
				//printf("ggggg %f\n", R);
				return R;
			}
		}
	}
	return 0;
}

uint32_t peak_AC_DC(uint32_t py, uint32_t* UP, uint32_t* DOWN){
	px++;
	if (pdir ==1){
		if (py > pmax){
			pmax = py;
			maxn = px;
		}else{
			if((pmax - py) > plv){
				pdir = 0;
				pmin = pmax;
				*UP = pmax;
				PA12=1;
				//---
				{
					uint32_t a, b, c;
					a = mr[ni];
					mr[ni++] = maxn;
					if(ni>(an-1)){ ni=0; }
					b = maxn;
					c = an*6000/(b-a);
					pnn++;
					if(pnn==5){ return 3*6000/(maxn-mr[1]);	}
					if(pnn<an+2){ return 0; }
					return c;
				}
			}
		}
	}else{
		if (py < pmin){
			pmin = py;
			minn = px;
		}else{
			if((py - pmin) > plv){
				pdir = 1;
				pmax = pmin;
				*DOWN = pmin;
				PA12=0;
			}
		}
	}
	return 0;
}


void peak_init(void){
	px=0; pnn=0, ni=0;
	pmax=0, pmin=50000000;
}
