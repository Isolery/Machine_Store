#include "ds1302.h"
#include <util/delay.h>

static void DS1302_Delay(void)
{
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
}

void DS1302_TransmitByte(unsigned char lcv_address, unsigned char lcv_data)
{
	unsigned char lcv_loop;
	
	DS1302MODE_OUT;
	CE_L;
	SCLK_L;
	DS1302_Delay();
	CE_H;
	DS1302_Delay();
	
	/****************写控制字或者说地址**************/
	for (lcv_loop = 0; lcv_loop < 8; lcv_loop++)
	{
		if(lcv_address & 0x01) DIO_H;
		else DIO_L;
		
		lcv_address = lcv_address>>1;
		SCLK_H;
		DS1302_Delay();
		SCLK_L;
		DS1302_Delay();	
	}

	/****************写数据**************/
	for (lcv_loop = 0; lcv_loop < 8; lcv_loop++)
	{	
		if(lcv_data & 0x01) DIO_H;
		else DIO_L;

		lcv_data = lcv_data>>1;
		SCLK_H;	
		DS1302_Delay();
		SCLK_L;
		DS1302_Delay();		
	}
	
	SCLK_L;
			
	DIO_L;
	
	CE_L;
	
	DS1302_Delay();
}


unsigned char DS1302_ReceiveByte(unsigned char lcv_address)
{
	unsigned char lcv_loop = 0;
	unsigned char lcv_data = 0x00;
			
	DS1302MODE_OUT;
			
	CE_L;
	DS1302_Delay();
	SCLK_L;
	DS1302_Delay();
	CE_H;
	DS1302_Delay();
			
/****************写控制字**************/
	for (lcv_loop = 0; lcv_loop < 8; lcv_loop++)
	{			
		if(lcv_address & 0x01) DIO_H;
		else	DIO_L;
		
		lcv_address = lcv_address>>1;
				
		SCLK_H;	
		DS1302_Delay();		
		SCLK_L;		
		DS1302_Delay();	
	}
				
	/****************读取一个8位的数据**************/	
	DS1302MODE_IN; /****高阻态输入，保证正常接收数据***/
	DIO_L;		
	DS1302_Delay();
	for (lcv_loop = 0; lcv_loop < 8; lcv_loop++)
	{			
		lcv_data = lcv_data>> 1;
		if (RPIN )	lcv_data = lcv_data | 0x80;
					
		SCLK_H;
		DS1302_Delay();
					
		SCLK_L;
		DS1302_Delay();
	}
	
	CE_L;
	DS1302_Delay();
	SCLK_H;
	DS1302_Delay();
	DIO_L;
	DS1302_Delay();
	DIO_H;
	DS1302_Delay();
					
	return(lcv_data);
}

void DS1302_Init(void)
{			
	DDRG  |= 0x18;
	PORTG |= 0x18;
					
	DDRB  |= 0x10;
	PORTB &= 0xEF;
					
	CE_L ;   //初始化引脚
	SCLK_L ;
}
							
void DS1302_SetTime(unsigned char* pTime)
{
	/*  将写保护设置为0，即写不保护 */
	DS1302_TransmitByte(0x8E,0x00);
	_delay_ms(10);
	DS1302_TransmitByte(0x8c, pTime[0]);
	_delay_ms(10);
	DS1302_TransmitByte(0x88, pTime[1]);
	_delay_ms(10);
	DS1302_TransmitByte(0x86, pTime[2]);
	_delay_ms(10);
	DS1302_TransmitByte(0x84, pTime[3]);
	_delay_ms(10);
	DS1302_TransmitByte(0x82, pTime[4]);
	_delay_ms(10);
	DS1302_TransmitByte(0x80, pTime[5]);
	_delay_ms(10);
	/*  将写保护设置为1，即写保护 */
	DS1302_TransmitByte(0x8E, 0x80);
}

void Ds1302_ReceiveTime(uint8_t *pData)
{
	pData[0] = DS1302_ReceiveByte(0x8d);    //year
	_delay_ms(5);
	pData[1] = DS1302_ReceiveByte(0x89);    //month
	_delay_ms(5);
	pData[2] = DS1302_ReceiveByte(0x87);    //day
	_delay_ms(5);
	pData[3] = DS1302_ReceiveByte(0x85);    //year
	_delay_ms(5);
	pData[4] = DS1302_ReceiveByte(0x83);    //year
	_delay_ms(5);
	pData[5] = DS1302_ReceiveByte(0x81);    //year
	_delay_ms(5);
}
