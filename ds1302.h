//#ifndef DS1302_H_
//#define DS1302_H_
//
//#include <avr/io.h>
//#include <util/delay.h>
//#include "predefine.h"

//#define SEC       0x80
//#define MIN      0x82
//#define HOUR    0x84
//#define DATE     0x86
//#define MON     0x88
//#define DAY       0x8a
//#define YEAR      0x8c
//#define CTRL       0x8e
//#define CHARGE  0x90
//#define LOCK       0x80
//#define UNLOCK  0x00
//#define CLKON    0x00
//#define CLKOFF    0x80
//
//#define PINIO      3
//#define PINSCLK  4
//#define PINRST    4
//#define PORTIO        PORTG
//#define PORTSCLK    PORTG
//#define PORTRST      PORTB
//#define MODEIO       DDRG
//#define MODESCLK   DDRG
//#define MODERST     DDRB
//#define INIO              PING
//
//extern void ds1302_write(uint8_t addr, uint8_t dat);
//extern uint8_t ds1302_read(uint8_t addr);
//extern void ds1302_set_timer(uint8_t *pTimer);
//extern void ds1302_get_timer(uint8_t *pTimer);
//extern void protect(void);
//extern void unprotect();
//extern void ds1302_init();
//extern void clock_on();
//extern void clock_off();

//#endif /* DS1302_H_ */


#ifndef  DS1302_H_
#define DS1302_H_

#include <avr/io.h>

#define SCLK_H  PORTG |= 0x10
#define SCLK_L  PORTG &= 0xEF
#define CE_H    PORTB |= 0x10
#define CE_L    PORTB &= 0xEF
#define DIO_H   PORTG |= 0x08
#define DIO_L   PORTG &= 0xF7
#define DS1302MODE_IN    DDRG &= 0xF7
#define DS1302MODE_OUT   DDRG |= 0x08
#define RPIN    (PING & 0x08)

extern void DS1302_TransmitByte(unsigned char lcv_address, unsigned char lcv_data);
extern void DS1302_Transmit(unsigned char lcv_address, unsigned char lcv_data);
extern unsigned char DS1302_ReceiveByte(unsigned char lcv_address);
extern unsigned char DS1302_Receive(unsigned char lcv_address);
extern void DS1302_Init(void);
extern void DS1302_SetTime(unsigned char* pTime);
extern void Ds1302_ReceiveTime(uint8_t *pData);

#endif 

