#include "eeprom.h"
#include <avr/io.h>
#include <util/delay.h>

/*
********************************************************
函数名称： void EEPROM_write_8B(INT16U Address, INT8U Data)
函数功能： 向ATmega64的EEPROM写入数据
           Address    16位的地址
		   Data       8位的数据
********************************************************
*/
void EEPROM_write_8B(INT16U Address, INT8U Data)
{
    /* 等待上一次写完成 */
    while(EECR &(1 <<EEMWE))
	{    
	}
	
	/* 设置地址和数据寄存器 */
	EEAR = Address;
	EEDR = Data;
	
	/* 置位EEMWE */
	EECR |= (1 << EEMWE);
	
	/* 置位EEWE以启动写操作 */
	EECR |= (1 << EEWE);

}
/*
********************************************************
函数名称： INT8U EEPROM_read_8B(INT16U Address)
函数功能： 从ATmega64的EEPROM读取数据
           Address    16位的地址
           返回一个8位的数据
********************************************************
*/
INT8U EEPROM_read_8B(INT16U Address)
{
    /* 等待上一次读完成 */
	while (EECR &(1 << EEWE))
	{
	}

	/* 设置地址寄存器 */
	EEAR = Address;

	/*设置EERE以启动读操作*/
	EECR |= (1 <<EERE );
	
	return EEDR;
}
/*
********************************************************
函数名称： void EEPROM_write_16B(INT16U Address, INT16U Data)
函数功能： 向ATmega64的EEPROM写入两个连续地址的16位数据
           Address    16位的地址
		   Data       16位的数据
********************************************************
*/
void EEPROM_write_16B(INT16U Address, INT16U Data)
{
    INT8U lcv_data=0;
	
	/*高八位*/
	lcv_data = (INT8U)((Data>>8) & 0xFF);
    EEPROM_write_8B(Address, lcv_data);
	_delay_ms(9);
	
	/*低八位*/
	lcv_data = (INT8U)(Data & 0xFF);
	EEPROM_write_8B(Address+1, lcv_data);
	_delay_ms(9);
}

/*
********************************************************
函数名称： INT16U EEPROM_read_16B(INT16U Address)
函数功能： 从ATmega64的EEPROM读取两个连续地址的16位数据
           Address    16位的开始地址
                    返回一个16位的数据
********************************************************
*/
INT16U EEPROM_read_16B(INT16U Address)
{
    INT16U liv_data = 0;
	
	/*高八位*/
	liv_data = (INT16U)EEPROM_read_8B(Address);
     
	/*低八位*/
	liv_data = liv_data<<8;
	liv_data = liv_data + (INT16U)EEPROM_read_8B(Address+1);
	
    return liv_data;
}
