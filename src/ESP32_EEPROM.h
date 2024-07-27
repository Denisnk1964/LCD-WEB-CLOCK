
//https://github.com/espressif/arduino-esp32/tree/master/libraries/EEPROM

#ifndef DEF__EEPROM
#define DEF__EEPROM

#include <Arduino.h>
#include <EEPROM.h> 

#define _EEPROM_SIZE_ 64


void  EEPROM_Init( char FUNCAO );
void EEPROM_Write( int address, int value );
int   EEPROM_Read( int address );


#endif // __EEPROM