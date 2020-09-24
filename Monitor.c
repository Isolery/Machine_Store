#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> 
#include <stdio.h>
#include "predefine.h"
#include "usart.h"
#include "i2c.h"
#include "ds1302.h"
#include "ch376.h"
#include "eeprom.h"

#define UART0_EN          UCSR0B |= (1<< RXEN0)
#define UART0_DEN        UCSR0B &= ~(1<< RXEN0)

#define TRUE    1
#define FALSE   0
#define BEGIN   1
#define END      0

uint8_t flag_RxFinish = FALSE;

uint32_t wddr = 0x0000;
uint32_t Rddr = 0x0000;
uint16_t ddrcout = 0;
int benginitCh376_flag = 0, reinitCh376_count = 0, reinitCh376_flag = 0;

uint8_t rxUart0[20];    
uint8_t rxUart1[20];    

uint8_t timer[] = {0x20,0x08,0x28,0x09,0x50,0x00};  //年，月，日，时，分，秒 
uint8_t test[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xAA};
uint8_t savedata[16] = {0xAB, 0x11, 0x10, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB2};    //20 08 26 16 32 18 aa bb e1 02 52 32 44 02 53 55 

void SavedataProcess(void);
void SystemInit(void);
void UsbSave(void);
void EEPROM_Init(void);

/**********************************************************************************************
* 函 数 名  ：main
* 功能说明：主函数
* 输入参数：void
* 返 回 值 ：0
**********************************************************************************************/
int main(void)
{
	SystemInit();
	
	//EEPROM_write_16B(0x0000, 0xFFFF); _delay_ms(10);
	//EEPROM_write_16B(0x0002, 0xFFFF); _delay_ms(10);
	//EEPROM_write_16B(0x0004, 0xFFFF); _delay_ms(10);
	//EEPROM_write_16B(0x0006, 0xFFFF); _delay_ms(10);
	//EEPROM_write_16B(0x0008, 0xFFFF); _delay_ms(10);
	//DS1302_SetTime(timer);    //设置时间
	_delay_ms(5);
	
	while(1)
	{
		if(flag_RxFinish)
		{
			UART0_DEN;
			cli();
			
			Ds1302_ReceiveTime(timer);    //读取时间
			SavedataProcess();
			
			if( !( IIC_Transmit(0xA0, wddr, savedata, 16)) ) 
			{
				wddr+=16;
				ddrcout++;
			}
			
			EEPROM_write_16B(0x0000, wddr>>16);    //将新的wddr值存入eeprom
			_delay_ms(10);
			EEPROM_write_16B(0x0002, (uint16_t)wddr); 
			_delay_ms(10);
				
			EEPROM_write_16B(0x0008, ddrcout);    //将新的ddrcout值存入eeprom
			_delay_ms(10);
			
			if(wddr >= 0x7FF0) wddr = 0x0000;   //地址存满重新覆盖
			if(ddrcout > 2047)
			{
				ddrcout = 2047 ;	
				Rddr = 0 ;
			}
			
			flag_RxFinish = FALSE;
			_delay_ms(500);
			
			UART0_EN;
			sei();
		}

		UsbSave();    //usb存储
	}
	return 0;
}

/**********************************************************************************************
* 功能说明：串口0中断接收程序
* 输入参数：无
* 返 回 值 ：无
**********************************************************************************************/
ISR(USART0_RX_vect)
{
	volatile static uint8_t temp, lastdata, countRx;
	temp = UDR0;
	
	switch(temp)
	{
		case 0xAA:
		{
			if(lastdata == 0xFF)
			{
				rxUart0[countRx++] = temp;
				lastdata = temp;
			}
			else
			{
				countRx = 0;
				rxUart0[countRx++] = temp;
				lastdata = temp;
			}
			break;
		}
		
		case 0xFF:
		{
			if(lastdata == 0xFF)
			{
				rxUart0[countRx++] = temp;
			}
			else
			{
				lastdata = temp;
			}
			break;
		}
		
		case 0x55:
		{
			if(lastdata == 0xFF)
			{
				rxUart0[countRx++] = temp;
				lastdata = temp;
			}
			else if(countRx < 0x03)
			{
				countRx = 0;
			}
			else
			{
				rxUart0[countRx++] = temp;
				countRx = 0;
				flag_RxFinish = TRUE;
			}
			break;
		}
		
		default:
		{
			rxUart0[countRx++] = temp;
			lastdata = temp;
		}
		break;
	}
}

/**********************************************************************************************
* 函 数 名  ：SavedataProcess
* 功能说明：将读到的数据的有效部分存入savedata数组中
* 输入参数：void
* 返 回 值 ：void
**********************************************************************************************/
void SavedataProcess(void)
{
	uint8_t i;

	for(i = 0; i < 6; i++) savedata[i] = timer[i];
	
	savedata[6]  = rxUart0[5];
	savedata[7]  = rxUart0[6];
	savedata[8]  = rxUart0[7];
	savedata[9]  = rxUart0[12];
	savedata[10] = rxUart0[13];
	savedata[11] = rxUart0[14];
	savedata[12] = rxUart0[15];
	savedata[13] = rxUart0[16];
	savedata[14] = rxUart0[17];
	savedata[15] = rxUart0[18];
}

/**********************************************************************************************
* 函 数 名  ：SystemInit
* 功能说明：初始化串口0，DS1302，CH376
* 输入参数：void
* 返 回 值 ：void
**********************************************************************************************/
void SystemInit(void)
{
	DDRE = 0xFF;
	USART0_Init(9600, 0);
	//USART1_Init(9600, 0);
	DS1302_Init();
	CH376_Init();
	EEPROM_Init();
	sei();
}

/**********************************************************************************************
* 函 数 名  ：UsbSave
* 功能说明：检测U盘是否插上，将数据写入U盘
* 输入参数：void
* 返 回 值 ：void
**********************************************************************************************/
void UsbSave(void)
{
	uint8_t s;
	uint8_t lcv_data;
	uint8_t lcv_Usbflag = 0;
	//uint8_t lcv_Setflag = 0;
	
	uint8_t day[14] = {'\\', 0x32, 0x30, 0x32, 0x30, 0x30, 0x38, 0x32, 0x36, 0x2E, 0x54, 0x58, 0x54, 0x00};    //20200826   
	uint8_t data[16];
	uint8_t Usbdata[36] = {0x31, 0x32, 0x33, 0x30, 0x65, 0x0D, 0x0A};	

	xWriteCH376Cmd(CMD_DISK_CONNECT);
	
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	
	lcv_data = xReadCH376Data();
	
	if (lcv_data == 0x15 || lcv_data == 0x14)
	{
		lcv_Usbflag = 1;
		_delay_ms(100);
	}
	
	while(lcv_Usbflag == 1)
	{
		for (int i = 0; i < 10; i ++ )    /* 最长等待时间,10*50mS */
		{  
	     	if (CH376DiskMount()) break;    /* 初始化磁盘并测试磁盘是否就绪 */
			_delay_ms(50);
	    }
		
		Ds1302_ReceiveTime(timer);
		day[3] = timer[0]/16 + 0x30;
		day[4] = timer[0]%16 + 0x30;
		day[5] = timer[1]/16 + 0x30;
		day[6] = timer[1]%16 + 0x30;
		day[7] = timer[2]/16 + 0x30;
		day[8] = timer[2]%16 + 0x30;
		
		s = CH376FileOpen(day);    /* 打开文件,该文件在根目录下 */
		_delay_ms(30);
		
		if ( s == CMD_RET_SUCCESS )	/* 文件存在并且已经被打开,移动文件指针到尾部以便添加数据 */
		{  
	     	s = CH376ByteLocate(0xFFFFFFFF);   /* 移到文件的尾部 */	
			_delay_ms(50);
     	}
     	
     	else if (s == ERR_MISS_FILE)   /* 没有找到文件,必须新建文件 */
		{  
	     	s = CH376FileCreate(day);  /* 新建文件并打开,如果文件已经存在则先删除后再新建,不必再提供文件名,刚才已经提供给CH376FileOpen */	
			_delay_ms(200);
     	}
		
		for ( ; ddrcout > 0; ddrcout--)   /* 用循环方式添加12行数据 */
		{  
			IIC_Receive(0xA0, Rddr, data, 16);
	    
			Usbdata[0] = 0x5B;    //  ' [ '
				
			Usbdata[1] = data[0]/16 + 0x30;
			Usbdata[2] = data[0]%16 + 0x30;
				
			Usbdata[3] = 0x2F;    //  ' / '
				
			Usbdata[4] = data[1]/16 + 0x30;
			Usbdata[5] = data[1]%16 + 0x30;
				
			Usbdata[6] = 0x2F;    //  ' / '
				
			Usbdata[7] = data[2]/16 + 0x30;
			Usbdata[8] = data[2]%16 + 0x30;
				
			Usbdata[9] = 0x20;    // 空格
				
			Usbdata[10] = data[3]/16 + 0x30;
			Usbdata[11] = data[3]%16 + 0x30;
				
			Usbdata[12] = 0x3A;    //  ' : '
				
			Usbdata[13] = data[4]/16 + 0x30;
			Usbdata[14] = data[4]%16 + 0x30;
				
			Usbdata[15] = 0x3A;	  //  ' : '
				
			Usbdata[16] = data[5]/16 + 0x30;
			Usbdata[17] = data[5]%16 + 0x30;
				
			Usbdata[18] = 0x5D;	  //  ' ] '
				
			Usbdata[19] = 0x20;    // 空格
				
			Usbdata[20] = data[9]/16 + 0x30;
			Usbdata[21] = data[9]%16 + 0x30;
			Usbdata[22] = data[10]/16 + 0x30;
			Usbdata[23] = data[10]%16 + 0x30;
			Usbdata[24] = data[11]/16 + 0x30;
			Usbdata[25] = data[11]%16 + 0x30;
			Usbdata[26] = data[12]/16 + 0x30;
			Usbdata[27] = data[12]%16 + 0x30;
			Usbdata[28] = data[13]/16 + 0x30;
			Usbdata[29] = data[13]%16 + 0x30;
			Usbdata[30] = data[14]/16 + 0x30;
			Usbdata[31] = data[14]%16 + 0x30;
			Usbdata[32] = data[15]/16 + 0x30;
			Usbdata[33] = data[15]%16 + 0x30;
				
			Usbdata[34] = 0x0D;
			Usbdata[35] = 0x0A;
			
			CH376ByteWrite(Usbdata, 36, 0);
			_delay_ms(50);
			
			Rddr += 16;
	     	
	     	if(Rddr >= 0x7ff0) Rddr = 0x0000;    //判断地址是否读完
		}
		_delay_ms(50);
		
		EEPROM_write_16B(0x0004, Rddr>>16);
     	_delay_ms(10);
		EEPROM_write_16B(0x0006, (uint16_t)Rddr);
		_delay_ms(10);
     	
     	EEPROM_write_16B(0x0008, ddrcout);
     	_delay_ms(5);
		
		CH376CloseFile(1);
		
		while (1)  
		{
			benginitCh376_flag = 1;
				 
			xWriteCH376Cmd(CMD_DISK_CONNECT);
				 
			if( reinitCh376_flag == 1 )
			{
				 reinitCh376_flag = 0 ;
				 CH376_Init();
			}
				 
			_delay_ms(50);	
				 
			lcv_data = xReadCH376Data();
				 
		    if(lcv_data == 0x16 || lcv_data == 0x82)
		    {
				//lcv_Setflag = 0 ;  
				lcv_Usbflag = 0 ;
				benginitCh376_flag = 0 ;
				
				 _delay_ms(50);	
				break;
			}
		}
	}
}

/**********************************************************************************************
* 函 数 名  ：EEPROM_Init
* 功能说明：芯片上电时，从EEPROM中读取上次保存的数据
* 输入参数：void
* 返 回 值 ：void
**********************************************************************************************/
void EEPROM_Init(void)
{
	if( (EEPROM_read_16B(0x0000) != 0xFFFF) && (EEPROM_read_16B(0x0002) != 0xFFFF) ) 
	{
		_delay_ms(5);
		wddr = EEPROM_read_16B(0x0000);
		wddr = wddr<< 16;
		wddr += EEPROM_read_16B(0x0002);
	}
	
	if( (EEPROM_read_16B(0x0004) != 0xFFFF) && (EEPROM_read_16B(0x0006) != 0xFFFF) ) 
	{
		_delay_ms(5);
		Rddr = EEPROM_read_16B(0x0004);
		Rddr = Rddr<< 16;
		Rddr += EEPROM_read_16B(0x0006);
	}
	
	if( EEPROM_read_16B(0x0008) != 0xFFFF ) 
	{
		_delay_ms(5);
		ddrcout = EEPROM_read_16B(0x0008);
	}
}

