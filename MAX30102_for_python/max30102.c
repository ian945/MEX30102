#include "Nano100Series.h"
#include	<stdio.h>

void lcd_print(uint8_t pos, char *s);
void lcd_init(void);
void delay_ms(uint32_t ms);
uint32_t peak_AC_DC(uint32_t py, uint32_t* UP, uint32_t* DOWN);
uint32_t peak_SpO2_1(uint32_t py, uint32_t* UP, uint32_t* DOWN);
uint32_t peak_SpO2_2(uint32_t py, uint32_t* UP, uint32_t* DOWN);
double peak2(uint32_t py);
double peak3(uint32_t py);
uint32_t peak(uint32_t py);

//---myi2c---
void i2c_start(void);
void i2c_stop(void);
uint8_t i2c_write(uint8_t d);
uint8_t i2c_read(uint8_t ack);
uint16_t i2c_scan(void);
//---myi2c---
#define WR 0
#define RE 1
#define ACK 0
#define NACK 1
void writeReg(uint8_t reg, uint8_t val){
	i2c_start();
	i2c_write(0xAE);
	i2c_write(reg);
	i2c_write(val);
	i2c_stop();
}
uint8_t readReg(uint8_t reg){
	uint8_t r;
	i2c_start();
	i2c_write(0xAE+WR);
	i2c_write(reg);
	i2c_start();
	i2c_write(0xAE+RE);
	r = i2c_read(NACK);
	i2c_stop();
	return r;
}
void readFifo( uint32_t *v1, uint32_t* v2){
	int i=0;
	uint8_t ru[3];
	//uint8_t r;
	i2c_start();
	i2c_write(0xAE+WR);
	i2c_write(0x07); 	//0x07
	i2c_start();
	i2c_write(0xAE+RE);
	
	/*
	*v1 = 0;
	r = i2c_read(ACK);
	*v1 += (uint32_t)r* 65536;
	r = i2c_read(ACK);
	*v1 += (uint32_t)r* 256;
	r = i2c_read(ACK);
	*v1 += (uint32_t)r;
	r = i2c_read(ACK);
	*v2 += (uint32_t)r* 65536;
	r = i2c_read(ACK);
	*v2 += (uint32_t)r* 256;
	r = i2c_read(NACK);
	*v2 += (uint32_t)r;
	*/
	
	for(i=0;i<3;i++){
		ru[i] = i2c_read(ACK);
	}
	*v1 = ru[2] | (ru[1] << 8) | (ru[0] << 16);
	for(i=0;i<3;i++){
		if(i==2){
			ru[i] = i2c_read(NACK);
			break;
		}
		ru[i] = i2c_read(ACK);
	}
	*v2 = ru[2] | (ru[1] << 8) | (ru[0] << 16);
	
	i2c_stop();
}

void max30102_init(void){
	uint16_t r;
	char df[32];
	int t1=20;
	r=readReg(0xFF); 	delay_ms(t1);
	r=readReg(0xFE);	delay_ms(t1);
	r=readReg(0x09); 	delay_ms(t1);
	writeReg(0x09, 0x43);
	r=readReg(0x09);	delay_ms(t1);
	r=readReg(0x08);	delay_ms(t1);
	writeReg(0x08, 0x4F);
	r=readReg(0x08);	delay_ms(t1);
	writeReg(0x08, 0x5F); //400/4==25/sec -> 100/sec
	r=readReg(0x09);	delay_ms(t1);
	writeReg(0x09, 0x03);
	r=readReg(0x0A);	delay_ms(t1);
	writeReg(0x0A, 0x20);
	r=readReg(0x0A);	delay_ms(t1);
	//
	writeReg(0x0A, 0x24);
	r=readReg(0x0A);	delay_ms(t1);
	writeReg(0x0A, 0x2F);  //  SPO2_SR[2:0]==011
	
	
	writeReg(0x0C, 0x1E);		//IR
	writeReg(0x0D, 0x1E);  	//red
	//writeReg(0x0E, 0x3C);
	writeReg(0x10, 0x3C);
	r=readReg(0x11);	delay_ms(t1);
	writeReg(0x11, 0x01);
	r=readReg(0x11);	delay_ms(t1);
	writeReg(0x11, 0x21);
	writeReg(0x04, 0x00);
	writeReg(0x05, 0x00);
	writeReg(0x06, 0x00);
	sprintf(df,"%x", r);
	lcd_print(0x40,df);
}
double ared=0, air=0, o2c=0, av2=0;
void max30102_read_HR(uint32_t* v1,uint32_t* v2){
	uint8_t r4, r6;
	r4 = readReg(0x04);
	r6 = readReg(0x06);
	if(r4==r6){ return; }
	//---
	{
		double av, av3;
		char bf[32];
		
		readFifo(v1, v2);
//		lcd_print(0x40, "                ");
//		sprintf(bf, "%8X%8X", v1, v2);
//		lcd_print(0x40, bf);
		
		printf("%d %d ", *v1, *v2);
		av = peak2(*v1);
		if(ared==0 && av>0){
			ared=av;
		}
		av3 = peak3(*v2);
		if(air==0 && av3>0){
			air=av3;
		}
		if(av<0){
			av2=-av;
		}
		//printf("%d   ", av);
		if(av<0){
			lcd_print(0x40, "                ");
			sprintf(bf, "%3d", (int)(-av));
			lcd_print(0x40, "HR=");
  		lcd_print(0x43, bf);
			printf("%d %f\n", (int)(-av), o2c);
			return;
		}
		if(ared!=0 && air!=0){
			o2c=((air/ared)-0.9)*100000;
			printf("%d %f\n", (int)(av2), o2c);
			ared=0;
			air=0;
			return;
		}
		if(av==0 || av3==0){
			printf("%d %f\n", (int)(av2), o2c);
		}
	}
}

void max30102_read_PI(uint32_t* v1,uint32_t* v2){
	uint16_t r4, r6;
	uint32_t UP, DOWN;		//ADCup, ADCdown
	int av=0;
	r4 = readReg(0x04);
	r6 = readReg(0x06);
	if(r4!=r6){
		readFifo( v1, v2);
		av=peak_AC_DC(*v1, &UP, &DOWN);
		if(av>0){
			uint32_t AC=UP-DOWN;
			double P_I=((double)(AC)*0.01);
			char bf[32];
			printf("%d %d     ", UP, DOWN);
			printf("%f\n", P_I);
			lcd_print(0x40, "                ");
			sprintf(bf, "%.2f", P_I);
			lcd_print(0x40, "P.I=");
  		lcd_print(0x44, bf);
		}
		
	}
	
}

void max30102_read_SpO2(uint32_t* v1,uint32_t* v2){
	uint16_t r4, r6;
	uint32_t UP1, DOWN1;		//ADCup, ADCdown
	uint32_t UP2, DOWN2;		//ADCup, ADCdown
	uint32_t U1, D1, U2, D2;
	int av1=0, av2=0;
	uint8_t catch1=0, catch2=0;
	
	r4 = readReg(0x04);
	r6 = readReg(0x06);
	if(r4!=r6){
		readFifo( v1, v2);
		
		av1=peak_SpO2_1(*v1, &U1, &D1);
		av2=peak_SpO2_2(*v2, &U2, &D2);
		if(av1>0){
			catch1=1;
			UP1=U1;
			DOWN1=D1;
		}
		if(av2>0){
			catch2=1;
			UP2=U2;
			DOWN2=D2;
		}
		
		if(catch1==1 && catch2==1){
			double ACred=(double)(UP1-DOWN1);
			double ACir=(double)(UP2-DOWN2);
			double DCred=(double)(DOWN1);
			double DCir=(double)(DOWN2);
			double R = (ACred/DCred)/(ACir/DCir);
			double SpO2 = -45.060*R*R + 30.354 *R + 94.845;
			//double SpO2 = 110-25*R;
			char bf[32];
			printf("%d %d  %d %d  %f ", UP1, DOWN1, UP2, DOWN2, R);
			printf("%f\n", SpO2);
			lcd_print(0x40, "                ");
			sprintf(bf, "%.3f", SpO2);
			lcd_print(0x40, "SpO2=");
  		lcd_print(0x45, bf);
			catch1=0;
			catch2=0;
		}
	}
}

void max30102_IR_RED(void){
	uint16_t r;
	int t1=20;
	r=readReg(0x09);	delay_ms(t1);
	if((r&1)==1){
		writeReg(0x09, r&=~(1<<0));
	}else{
		writeReg(0x09, r|=(1<<0));
	}
}
