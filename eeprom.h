#ifndef EEPROM_H_
#define EEPROM_H_

#define INT8U     unsigned char
#define INT16U   unsigned int

extern void EEPROM_write_8B(INT16U Address, INT8U Data);
extern INT8U EEPROM_read_8B(INT16U Address);
extern void EEPROM_write_16B(INT16U Address, INT16U Data);
extern INT16U EEPROM_read_16B(INT16U Address);

#endif /* EEPROM_H_ */