#include "eeprom.h"
#include <avr/io.h>
#include <util/delay.h>

/*
********************************************************
�������ƣ� void EEPROM_write_8B(INT16U Address, INT8U Data)
�������ܣ� ��ATmega64��EEPROMд������
           Address    16λ�ĵ�ַ
		   Data       8λ������
********************************************************
*/
void EEPROM_write_8B(INT16U Address, INT8U Data)
{
    /* �ȴ���һ��д��� */
    while(EECR &(1 <<EEMWE))
	{    
	}
	
	/* ���õ�ַ�����ݼĴ��� */
	EEAR = Address;
	EEDR = Data;
	
	/* ��λEEMWE */
	EECR |= (1 << EEMWE);
	
	/* ��λEEWE������д���� */
	EECR |= (1 << EEWE);

}
/*
********************************************************
�������ƣ� INT8U EEPROM_read_8B(INT16U Address)
�������ܣ� ��ATmega64��EEPROM��ȡ����
           Address    16λ�ĵ�ַ
           ����һ��8λ������
********************************************************
*/
INT8U EEPROM_read_8B(INT16U Address)
{
    /* �ȴ���һ�ζ���� */
	while (EECR &(1 << EEWE))
	{
	}

	/* ���õ�ַ�Ĵ��� */
	EEAR = Address;

	/*����EERE������������*/
	EECR |= (1 <<EERE );
	
	return EEDR;
}
/*
********************************************************
�������ƣ� void EEPROM_write_16B(INT16U Address, INT16U Data)
�������ܣ� ��ATmega64��EEPROMд������������ַ��16λ����
           Address    16λ�ĵ�ַ
		   Data       16λ������
********************************************************
*/
void EEPROM_write_16B(INT16U Address, INT16U Data)
{
    INT8U lcv_data=0;
	
	/*�߰�λ*/
	lcv_data = (INT8U)((Data>>8) & 0xFF);
    EEPROM_write_8B(Address, lcv_data);
	_delay_ms(9);
	
	/*�Ͱ�λ*/
	lcv_data = (INT8U)(Data & 0xFF);
	EEPROM_write_8B(Address+1, lcv_data);
	_delay_ms(9);
}

/*
********************************************************
�������ƣ� INT16U EEPROM_read_16B(INT16U Address)
�������ܣ� ��ATmega64��EEPROM��ȡ����������ַ��16λ����
           Address    16λ�Ŀ�ʼ��ַ
                    ����һ��16λ������
********************************************************
*/
INT16U EEPROM_read_16B(INT16U Address)
{
    INT16U liv_data = 0;
	
	/*�߰�λ*/
	liv_data = (INT16U)EEPROM_read_8B(Address);
     
	/*�Ͱ�λ*/
	liv_data = liv_data<<8;
	liv_data = liv_data + (INT16U)EEPROM_read_8B(Address+1);
	
    return liv_data;
}
