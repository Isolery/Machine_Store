#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>
#include <util/delay.h>
#include "predefine.h"

#define SCL_H     PORTE|=0x40
#define SCL_L     PORTE&=0xBF

#define SDA_H     PORTE|=0x80
#define SDA_L     PORTE&=0x7F

#define MODE_IN   DDRE&=0x7F
#define MODE_OUT  DDRE|=0x80

#define  FAIL      1
#define  SUCCEED   0

extern  void AT24C256_Init(void);
extern  void IIC_delay(void);
extern  void IIC_delayMS(int x);
extern  void IIC_Start(void);
extern  void IIC_Stop(void);
extern  unsigned char IIC_ACK_Check(void);
extern  void IIC_NO_ACK(void);
extern  void IIC_ACK_Creat(void);
extern  void IIC_Send8Bit(unsigned char SendByte);
extern  unsigned char IIC_Receive8Bit(void);
extern char IIC_Transmit(unsigned char lcv_device,unsigned int liv_addr,unsigned char *lcv_pdata, char counter);
extern char IIC_Receive(unsigned char lcv_device, unsigned int liv_addr, unsigned char *lcv_pdata, char counter);

#endif
