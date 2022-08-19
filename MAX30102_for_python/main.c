#include "Nano100Series.h"              // Device header
#include	<stdio.h>

void lcd_print(uint8_t pos, char *s);
void lcd_init(void);

uint32_t g_u32TICK;


//-------------- HCLK  ------------- CPU clock
void init_HCLK(void){
  SYS_UnlockReg();
    CLK_EnableXtalRC(CLK_PWRCTL_HXT_EN_Msk);
    CLK_WaitClockReady( CLK_CLKSTATUS_HXT_STB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HXT,CLK_HCLK_CLK_DIVIDER(1));
  SYS_LockReg();
}

//--------------- Systick ------------------ system time
uint32_t tick=0;
void SysTick_Handler(void){
    tick++;
};
 
void delay_ms(uint32_t ms){
	uint32_t t0;
	t0=tick;
    while( (tick-t0)<ms){}
}
 
void init(void){
    SysTick_Config( SystemCoreClock /1000);
}
void init_systick(void){
	SysTick_Config(SystemCoreClock/1000);
}
 //----------- UART1 --------------
void init_UART1(uint32_t bps){
  SYS_UnlockReg();
    //--- CLK
    CLK_EnableModuleClock(UART1_MODULE);
    CLK_SetModuleClock( UART1_MODULE, CLK_CLKSEL1_UART_S_HIRC, CLK_UART_CLK_DIVIDER(1) );
    //--- PIN
    SYS->PB_L_MFP &=~ SYS_PB_L_MFP_PB4_MFP_Msk;
    SYS->PB_L_MFP |=  SYS_PB_L_MFP_PB4_MFP_UART1_RX;
    SYS->PB_L_MFP &=~ SYS_PB_L_MFP_PB5_MFP_Msk;
    SYS->PB_L_MFP |=  SYS_PB_L_MFP_PB5_MFP_UART1_TX;
  SYS_LockReg();
    //--- OPEN
    UART_Open(UART1, bps);
    //--- NVIC
    UART_ENABLE_INT(UART1, UART_IER_RDA_IE_Msk);
    NVIC_EnableIRQ(UART1_IRQn);
}
const char connect[]="CONNECT";
const char closed[]="CLOSED";
char cmp_str(char *src, const char *dst){
	while(*dst){
		if(*src++ != *dst++){ return 0; }
	}
	return 1;
}

  //--- IRQ
char rb[256];
uint16_t ri=0, rn=0, WIF=0;
int conn=-1, ln=0;
char ss[256];
void UART1_IRQHandler(void){	
	char r;
	r = UART1->RBR;
    UART0->THR = r;
	rb[ri++] = r;
	if(r=='\n'){
		rb[ri]=0; ri=0;
		if( cmp_str(rb+2, connect) ){ WIF=1; conn = rb[0]-'0'; }
		if( cmp_str(rb+2, closed)  ){ WIF=0; }
	}
}

//----------- UART0 --------------
void init_UART0(int bps){
  SYS_UnlockReg();
    //--- CLK
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_UART_CLK_DIVIDER(1));
    //--- PIN
    SYS->PB_L_MFP &=~ SYS_PB_L_MFP_PB0_MFP_Msk;
    SYS->PB_L_MFP |=  SYS_PB_L_MFP_PB0_MFP_UART0_RX;
    SYS->PB_L_MFP &=~ SYS_PB_L_MFP_PB1_MFP_Msk;
    SYS->PB_L_MFP |=  SYS_PB_L_MFP_PB1_MFP_UART0_TX;
  SYS_LockReg();
    //--- OPEN
    UART_Open(UART0, bps);
    //--- NVIC
    UART_ENABLE_INT(UART0, UART_IER_RDA_IE_Msk);
    NVIC_EnableIRQ(UART0_IRQn);
}
  //--- IRQ
void UART0_IRQHandler(void){
    UART1->THR = UART0->RBR;
}

//----------------- TIMER1 --------------------
void init_TIMER1(int freq){
  SYS_UnlockReg();
    //--- CLK
    CLK_EnableModuleClock(TMR1_MODULE);
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1_S_HIRC, 0);
    //--- PIN
    SYS->PB_H_MFP &= ~SYS_PB_H_MFP_PB9_MFP_Msk;
    SYS->PB_H_MFP |= SYS_PB_H_MFP_PB9_MFP_TMR1_EXT;
  SYS_LockReg();
    //--- OPEN
    TIMER_Open(TIMER1, TIMER_TOGGLE_MODE, freq);
    TIMER_Start(TIMER1);
    //--- NVIC
    TIMER_EnableInt(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);
}
    //--- ISR
char TMR1F=0;
void TMR1_IRQHandler(void){
    TIMER_ClearIntFlag( TIMER1 );
	TMR1F=1;
}
//------------------ ADC ---------------------
void init_ADC(void){
  SYS_UnlockReg();
    //--- ADC CLK
    CLK_EnableModuleClock(ADC_MODULE);
    CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL1_ADC_S_HXT, CLK_ADC_CLK_DIVIDER(12));
    //--- ADC PIN
    SYS->PA_L_MFP &= ~SYS_PA_L_MFP_PA0_MFP_Msk;
    SYS->PA_L_MFP |=  SYS_PA_L_MFP_PA0_MFP_ADC_CH0;
    SYS->PA_L_MFP &= ~SYS_PA_L_MFP_PA1_MFP_Msk;
    SYS->PA_L_MFP |=  SYS_PA_L_MFP_PA1_MFP_ADC_CH1;
 
    GPIO_DISABLE_DIGITAL_PATH( PA, (BIT0+BIT1));
  SYS_LockReg();
    //--- ADC OPEN
    ADC_Open(ADC, ADC_INPUT_MODE_SINGLE_END, ADC_OPERATION_MODE_SINGLE_CYCLE, BIT0+BIT1+BIT15 );
    ADC_POWER_ON();
    //--- ADC NVIC
    ADC_EnableInt(ADC, ADC_ADF_INT);
    NVIC_EnableIRQ(ADC_IRQn);
}
//--- ADC ISR
void ADC_IRQHandler(void){
	char bf[32];
	int v = ADC_GET_CONVERSION_DATA(ADC, 0);
	printf("%d\n",v);
	sprintf(bf, "%4d", v);
	lcd_print(0x40,bf);
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
}

//---I2C----
void i2c_start(void);
void i2c_stop(void);
uint8_t i2c_write(uint8_t d);
uint8_t i2c_read(uint8_t ack);
uint16_t i2c_scan(void);

int NACK=0;
int ACK=1;
uint16_t r;
char bf[32];
int ti=10;

void writeReg(uint8_t reg, uint8_t val);
uint8_t readReg(uint8_t reg);
void readFifo(uint8_t reg, uint32_t *v1, uint32_t* v2);
void max30102_init(void);
void max30102_read_HR(uint32_t* v1,uint32_t* v2);
void max30102_read_SpO2(uint32_t* v1,uint32_t* v2);
void max30102_read_PI(uint32_t* v1,uint32_t* v2);
void max30102_IR_RED(void);

//int ans=1;
uint32_t V1=0;
uint32_t V2=0;
uint8_t REVERSE1=0;
uint8_t REVERSE2=0;

void peak_init(void);
uint32_t peak(uint32_t py);
int ccccc=0;
int main(void){
	//uint8_t ans=0;
	init_HCLK();
	init_systick();
	init_UART0(115200);
	lcd_init();
	lcd_print(0,"I2C DEMO");
	GPIO_SetMode(PB, BIT2, GPIO_PMD_OUTPUT);
	GPIO_SetMode(PA, BIT12+BIT13+BIT14, GPIO_PMD_OUTPUT);
	PA12=0; PA13=0; PA14=0;
	
	
	//CLK_PowerDown();
	
	//RTC_IRQHandler();
	peak_init();
	//--- I2C scan
	{
		r = i2c_scan();
		sprintf(bf, "%02X", r);
		lcd_print(0x40,bf);
		delay_ms(500);
		//--------------
		max30102_init();
		init_TIMER1(1);
		
	}
	/*
	//--------wifi-----------------
		
	init_UART1(115200);
	printf("AT\r\n"); delay_ms(1000);
	
	printf("AT+RST\r\n"); delay_ms(5000);
	printf("AT+CWMODE=2\r\n"); delay_ms(1000);
	printf("AT+CWSAP=\"STU25\",\"12345678\",2,3\r\n"); delay_ms(3000);
	printf("AT+CIPMUX=1\r\n");  delay_ms(1000);
	printf("AT+CIPSERVER=1,8000\r\n"); delay_ms(1000);
		
	while(1){
		if(WIF){
			if(TMR1F){
				uint32_t v1, v2;
				max30102_read_HR(&v1, &v2);
				TMR1F=0;
				//if(dn>481){ dn=0; }
				printf("AT+CIPSEND=%d,6\r\n", conn); delay_ms(2);
				printf("%4d\n", v1);
			}
		}
	*/

	while(1){
		uint32_t v1, v2;
		max30102_read_HR(&v1, &v2);
		//max30102_read_PI(&v1, &v2);
		//max30102_read_SpO2(&v1, &v2);
		//delay_ms(1);
		/*
		if(tick-ccccc>1000){    //control 1s to action
			ccccc=tick;
			printf("-----%d\n", tick);
		}
		*/
		/*
		//------button
		if(4==((PB->PIN)&(1<<2))){
		}else{
			while(4!=((PB->PIN)&(1<<2))){
					max30102_IR_RED();
					ans^=1;
				}
		}
		if(ans==0){
			max30102_read_SpO2(&v1, &v2);
		}else{
			max30102_read_HR();
		}
		*/
	}
}
