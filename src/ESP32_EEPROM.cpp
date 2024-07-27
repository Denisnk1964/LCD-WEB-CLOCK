
#include "ESP32_EEPROM.h"

//===========================
//
//===========================
void EEPROM_Init( char FUNCAO ) {

  //=========================
  if (!EEPROM.begin( _EEPROM_SIZE_)) {
    Serial.println("failed to initialize EEPROM");
  }

  if (FUNCAO == 'W') {
    Serial.println();
    Serial.println(" bytes write from Flash. Values are:");
    u_int8_t value = 0;

    for (int i = 0; i < _EEPROM_SIZE_; i++) {
      EEPROM.write( i, value );
      EEPROM.writeUInt( i, value );


      EEPROM.commit();
      Serial.print( value ); Serial.print(" ");
      if (i % 10 == 0 ) Serial.println();
    }
  }

  //=========================
  if (FUNCAO == 'R') {
    Serial.println();
    Serial.println(" bytes read from Flash . Values are:");
    for( int i = 0; i < _EEPROM_SIZE_; i++) {
      u_int8_t value = EEPROM.readInt( i ) ;
      Serial.print( value ); Serial.print(" ");
      if (i % 10 == 0 ) Serial.println();
    }
  }
}

 
//===========================
//
//===========================
void EEPROM_Write( int address, int value ) {
    
 	EEPROM.write(address, value);
 	EEPROM.commit();
}

//===========================
//
//===========================
int EEPROM_Read( int address ) {
 	int value = EEPROM.read( address ); 
    return value;
}

