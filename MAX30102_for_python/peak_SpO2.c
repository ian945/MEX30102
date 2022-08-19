#include "Nano100Series.h"
#include <stdio.h>
//---
uint32_t peak_SpO2_1(uint32_t py, uint32_t* UP, uint32_t* DOWN);
uint32_t peak_SpO2_2(uint32_t py, uint32_t* UP, uint32_t* DOWN);
//---1
uint32_t plv1=300;
uint32_t pmax1=0, pmin1=5000;

uint32_t mr1[50], nr1[50];
uint32_t px1=0;

uint32_t pdir1=1;
uint32_t maxn1=0, minn1=50000000;
uint32_t pnn1=0, ni1=0;
uint16_t an1=10; //


//---2
uint32_t plv2=200;
uint32_t pmax2=0, pmin2=5000;

uint32_t mr2[50], nr2[50];
uint32_t px2=0;

uint32_t pdir2=1;
uint32_t maxn2=0, minn2=50000000;
uint32_t pnn2=0, ni2=0;
uint16_t an2=10; //


double peak3(uint32_t py){
	//uint32_t dd=0;
	px1++;
	if (pdir1 ==1){
		if (py > pmax1){
			pmax1 = py;
			maxn1 = px1;
		}else{
			if((pmax1 - py) > plv1){
				pdir1 = 0;
				pmin1 = pmax1;
				PA12=1;
				//printf("%d 0\n", py);
				//---
				{
					uint32_t a, b;
					double c;
					a = mr1[ni1];
					mr1[ni1++] = maxn1;
					if(ni1>(an1-1)){ ni1=0; }
					b = maxn1;
					c = (double)(an1*6000/(b-a));
					pnn1++;
					if(pnn1==5){ return 3*6000/(maxn1-mr1[1]);	}
					if(pnn1<an1+2){ return 0; }
					//printf("%f 0\n", c);
					return -c;
				}
			}
		}
	}else{
		if (py < pmin1){
			pmin1 = py;
			minn1 = px1;
		}else{
			if((py - pmin1) > plv1){
				double R = ((double)(pmax1))/((double)(pmin1));
				pdir1 = 1;
				pmax1 = pmin1;
				PA12=0;
				//printf("ggggg %f\n", R);
				return R;
			}
		}
	}
	return 0;
}



uint32_t peak_SpO2_1(uint32_t py, uint32_t* UP, uint32_t* DOWN){
	px1++;
	if (pdir1 ==1){
		if (py > pmax1){
			pmax1 = py;
			maxn1 = px1;
		}else{
			if((pmax1 - py) > plv1){
				pdir1 = 0;
				pmin1 = pmax1;
				*UP = pmax1;
				PA12=1;
				//---
				{
					uint32_t a, b, c;
					a = mr1[ni1];
					mr1[ni1++] = maxn1;
					if(ni1>(an1-1)){ ni1=0; }
					b = maxn1;
					c = an1*6000/(b-a);
					pnn1++;
					if(pnn1==5){ return 3*6000/(maxn1-mr1[1]);	}
					if(pnn1<an1+2){ return 0; }
					return c;
				}
			}
		}
	}else{
		if (py < pmin1){
			pmin1 = py;
			minn1 = px1;
		}else{
			if((py - pmin1) > plv1){
				pdir1 = 1;
				pmax1 = pmin1;
				*DOWN = pmin1;
				PA12=0;
			}
		}
	}
	return 0;
}


uint32_t peak_SpO2_2(uint32_t py, uint32_t* UP, uint32_t* DOWN){
	px2++;
	if (pdir2 ==1){
		if (py > pmax2){
			pmax2 = py;
			maxn2 = px2;
		}else{
			if((pmax2 - py) > plv2){
				pdir2 = 0;
				pmin2 = pmax2;
				*UP = pmax2;
				PA12=1;
				//---
				{
					uint32_t a, b, c;
					a = mr2[ni2];
					mr2[ni2++] = maxn2;
					if(ni2>(an2-1)){ ni2=0; }
					b = maxn2;
					c = an2*6000/(b-a);
					pnn2++;
					if(pnn2==5){ return 3*6000/(maxn2-mr2[1]);	}
					if(pnn2<an2+2){ return 0; }
					return c;
				}
			}
		}
	}else{
		if (py < pmin2){
			pmin2 = py;
			minn2 = px2;
		}else{
			if((py - pmin2) > plv2){
				pdir2 = 1;
				pmax2 = pmin2;
				*DOWN = pmin2;
				PA12=0;
			}
		}
	}
	return 0;
}

/*
void peak_init(void){
	px=0; pnn=0, ni=0;
	pmax=0, pmin=50000000;
}
*/
