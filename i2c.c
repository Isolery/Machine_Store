#include "i2c.h"
#include <util/delay.h>

void AT24C256_Init(void)
{
    DDRE  |= 0xC0;
	PORTE |= 0xC0;
}
/*
********************************************************
�������ƣ�static void IIC_delay(void)
�������ܣ���ʱ
ʱ�䣺�����ʱʱ��Ϊ1/8M��0.125��S
********************************************************
*/
void IIC_delay(void)
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
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
}

void IIC_Start(void)
{
    MODE_OUT;
	
	SDA_H;
	IIC_delay();

	SCL_H;
	IIC_delay();

	SDA_L;
	IIC_delay();
	
	SCL_L;
	IIC_delay();
}

void IIC_Stop(void)
{
    MODE_OUT;

    SCL_L;
	IIC_delay();

	SDA_L;
	IIC_delay();

	SCL_H;
	IIC_delay();

	SDA_H;
	IIC_delay(); 
}

unsigned char IIC_ACK_Check(void)
{
    unsigned char Bitmun;
	
	MODE_IN;
	
    SCL_L;
	IIC_delay();

	SDA_H;
	IIC_delay();
	
	SCL_H;
	IIC_delay();

	Bitmun = PINE;
	
	SCL_L;
	IIC_delay();

	Bitmun =(Bitmun >> 7)&0x01;
	
    return Bitmun;
}


void IIC_NO_ACK(void)
{
    MODE_OUT;
    
    SCL_L;
	IIC_delay();

	SDA_H;
	IIC_delay();

	SCL_H;
	IIC_delay();

	SCL_L;
	IIC_delay();
}

void IIC_ACK_Creat(void)
{
    MODE_OUT;
    
	SCL_L;
	IIC_delay();

	SDA_L;
	IIC_delay();

    SCL_H;
	IIC_delay();

	SCL_L;
	IIC_delay();
}

void IIC_Send8Bit(unsigned char SendByte)
{
    char counter;
		
	MODE_OUT;
		
	SDA_L;
	IIC_delay();

	for (counter = 8; counter >0; counter--)
	{
        SCL_L;
		IIC_delay();
	    if (SendByte & 0x80)
		{
		    SDA_H;
			IIC_delay();
		}
		else
		{
            SDA_L;
			IIC_delay();
		}
		
		SCL_H;
		IIC_delay();

		SendByte = SendByte << 1;
	}
	SCL_L;
	IIC_delay();
}

unsigned char IIC_Receive8Bit(void)
{
    char counter;
    unsigned char ReceiveByte=0;
	unsigned char IOstatus=0;
	
	MODE_IN;
	
	SCL_L;
	IIC_delay();

    SDA_H;
	IIC_delay();

    for (counter = 8; counter > 0; counter--)
	{
        SCL_L;
		IIC_delay();
			
		SCL_H;
		IIC_delay();
				
		IOstatus = PINE;
		
	    ReceiveByte  = ReceiveByte<<1;
        ReceiveByte	|= (IOstatus >>7)& 0x01;
	}
	SCL_L;
	IIC_delay();
			
	return ReceiveByte;
}

char IIC_Transmit(unsigned char lcv_device,unsigned int liv_addr,unsigned char *lcv_pdata, char counter)
{
    IIC_Start();
	
	/* ���ʹӻ���ַ */
	IIC_Send8Bit(lcv_device);
	if ( IIC_ACK_Check() ) //���ú���char IIC_ACK_Check(void),û�дӻ���Ӧ��������η��͡�
	{
	    IIC_Stop();
        return FAIL;
	}

	/* �����ڴ��ַ*/
    IIC_Send8Bit((unsigned char)(liv_addr >> 8) & 0x0FF);  //�߰�λ
	if ( IIC_ACK_Check()) //���ú���char IIC_ACK_Check(void),û�дӻ���Ӧ��������η��͡�
    {
        IIC_Stop();
		return FAIL;
    }	
    IIC_Send8Bit((unsigned char)((liv_addr)&0x0FF));  //�Ͱ�λ
	if ( IIC_ACK_Check()) //���ú���char IIC_ACK_Check(void),û�дӻ���Ӧ��������η��͡�
	{
        IIC_Stop();
		return FAIL;
	}	
		
	/* �������� */
    for(;counter > 0;counter-- )		
    {
        IIC_Send8Bit(*lcv_pdata);
	    if ( IIC_ACK_Check()) //���ú���char IIC_ACK_Check(void),û�дӻ���Ӧ��������η��͡�
		{
		    IIC_Stop();
		    return FAIL;
		}	
	    lcv_pdata++;
    }
	
	IIC_Stop();
	
	SDA_H;
	
	_delay_ms(18);  //��ʱ18MS

	return SUCCEED;
}

char IIC_Receive(unsigned char lcv_device, unsigned int liv_addr, unsigned char *lcv_pdata, char counter)
{
    
	/* �Ƚ��ӻ�����Ϊдģʽ */
    IIC_Start();
	/* ���ʹӻ���ַ */
    IIC_Send8Bit(lcv_device);
	if ( IIC_ACK_Check())//���ú���char IIC_ACK_Check(void),û�дӻ���Ӧ��������η��͡�
	{
        IIC_Stop();
        return FAIL;
	}
		
	   /*    �����ڴ��ַ   */
    IIC_Send8Bit((unsigned char)((liv_addr >> 8)&0x0FF));
	if ( IIC_ACK_Check()) //���ú���char IIC_ACK_Check(void),û�дӻ���Ӧ��������η��͡�
	{
	    IIC_Stop();
		return FAIL;
	}	
    IIC_Send8Bit((unsigned char)((liv_addr)&0x0FF));		
	if ( IIC_ACK_Check()) //���ú���char IIC_ACK_Check(void),û�дӻ���Ӧ��������η��͡�
	{
	    IIC_Stop();
		return FAIL;
	}	
	
	
	/* Ȼ�󽫴ӻ�����Ϊдģʽ */	
	IIC_Start();
    /* ���ʹӻ���ַ */
	IIC_Send8Bit(lcv_device+1); 
	if ( IIC_ACK_Check()) ////���ú���char IIC_ACK_Check(void),û�дӻ���Ӧ��������η��͡�
	{
        IIC_Stop();
		return FAIL;
    }	
		
	for( ; counter > 0; )
	{
        *lcv_pdata = IIC_Receive8Bit();
	    counter--;
	    if ( counter >0)
		{
		    IIC_ACK_Creat();
		    lcv_pdata++ ;
		}
	}
	
	IIC_NO_ACK();
	
	IIC_Stop();
	
	return SUCCEED;
}



