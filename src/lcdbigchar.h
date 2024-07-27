/*
██╗      ██████╗██████╗     ██████╗ ██╗ ██████╗      ██████╗██╗  ██╗ █████╗ ██████╗ 
██║     ██╔════╝██╔══██╗    ██╔══██╗██║██╔════╝     ██╔════╝██║  ██║██╔══██╗██╔══██╗
██║     ██║     ██║  ██║    ██████╔╝██║██║  ███╗    ██║     ███████║███████║██████╔╝
██║     ██║     ██║  ██║    ██╔══██╗██║██║   ██║    ██║     ██╔══██║██╔══██║██╔══██╗
███████╗╚██████╗██████╔╝    ██████╔╝██║╚██████╔╝    ╚██████╗██║  ██║██║  ██║██║  ██║
╚══════╝ ╚═════╝╚═════╝     ╚═════╝ ╚═╝ ╚═════╝      ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝
https://manytools.org/hacker-tools/ascii-banner/                                                                                    
*/
#ifndef DEF__LCD_BIG_CHAR
#define DEF__LCD_BIG_CHAR

#include <Arduino.h>

// https://github.com/upiir/character_display_big_digits/blob/main/small_16x2_display_big_digits__upir.ino
// https://www.youtube.com/watch?v=SXSujfeN_QI&t=395s

//    3   7  10  15 17
// |1||2|:|3||4|:|5||6|
// |  |   |  |   |  |  
// 01234567890123456789
// 0  3   7   10   5   9
//       :6     :13


#define _DIGITO_1   0
#define _DIGITO_2   3
#define _DIGITO_P1  6
#define _DIGITO_3   7
#define _DIGITO_4  10
#define _DIGITO_P2 13
#define _DIGITO_5  14
#define _DIGITO_6  17

#define _LINHA_1   1
#define _LINHA_2   2

//Arrays para criação dos segmentos e customização dos números

// T--  -TABELA
// -E-  -ESQUERDA
// -D-  -DIREITA
// -M-  -MEIO
// --S  -SUPERIOR
// --I  -INFERIOR
// --C  -CENTRO


// T--  -TABELA
// -E-  -ESQUERDA
// --S  -SUPERIOR
byte _TES[8] PROGMEM = {
                B01111,  
                B11111,  
                B11111,  
                B11111,  
                B11111,  
                B11111,  
                B11111,  
                B11111 
                };


// T--  -TABELA
// -E-  -ESQUERDA
// --I  -INFERIOR
byte _TEI[8] PROGMEM = {
                B11111,  
                B11111,  
                B11111,  
                B11111,  
                B11111,  
                B11111,  
                B11111,  
                B01111 
                };


// T--  -TABELA
// -D-  -DIREITA
// --S  -SUPERIOR
byte _TDS [8] PROGMEM = {
                B11110,  
                B11111,  
                B11111,  
                B11111,  
                B11111,  
                B11111,  
                B11111,  
                B11111 
                };

// T--  -TABELA
// -D-  -DIREITA
// --I  -INFERIOR
byte _TDI [8] PROGMEM = {
                B11111,  
                B11111,  
                B11111,  
                B11111,  
                B11111,  
                B11111,  
                B11111,  
                B11110 
                };


// 2 PONTOS 
byte _TPO [8] PROGMEM = {
                B00000,  
                B00000,  
                B01110,  
                B01110, 
                B01110,  
                B01110,  
                B00000,  
                B00000  
                };


// T--  -TABELA
// -M-  -MEIO
// --S  -SUPERIOR
byte _TMS [8] PROGMEM= {
                B11111,  
                B11111,  
                B11111,  
                B00000,  
                B00000,  
                B00000,  
                B00000,  
                B00000 
                };


// T--  -TABELA
// -M-  -MEIO
// --I  -INFERIOR
byte _TMI [8] PROGMEM = { 
                B00000,  
                B00000,  
                B00000,  
                B00000,  
                B00000,  
                B11111,  
                B11111,  
                B11111 
                };

// T--  -TABELA
// -M-  -MEIO
// --C  -CENTRO
byte _TMC [8] PROGMEM = { 
                B11111,  
                B11111,  
                B11111,  
                B00000,  
                B00000,  
                B11111,  
                B11111,  
                B11111 
                };



  #define _TES_   0
  #define _TEI_   1
  #define _TDS_   2
  #define _TDI_   3
  #define _TMS_   4
  #define _TMI_   5
  #define _TMC_   6
  #define _TPO_   7
  // #define _TPO_  B10100001

  #define _SPC_  ' '
  #define _TCH_  B11111111
  #define _X01_  B10111011


byte real_digits[10][6] PROGMEM = { 
  { _TES_, _TMS_, _TDS_ ,
    _TEI_, _TMI_, _TDI_ }, // 0

  { _SPC_, _TES_, _SPC_, 
    _TMI_, _TCH_, _TMI_ }, // 1 

  { _TMS_, _TMC_, _TDS_, 
    _TEI_, _TMI_, _TMI_ }, // 2

  { _TMS_, _TMC_, _TDS_, 
    _TMI_, _TMI_, _TDI_ }, // 3

  { _TES_, _TMI_, _TCH_, 
    _SPC_, _SPC_, _TDI_ }, // 4

  { _TES_, _TMC_, _TMS_, 
    _TMI_, _TMI_, _TDI_ }, // 5

  { _TES_, _TMC_, _TMC_, 
    _TEI_, _TMI_, _TDI_ }, // 6

  { _TMS_, _TMS_, _TDI_, 
    _SPC_, _TEI_, _SPC_ }, // 7

  { _TES_, _TMC_, _TDS_, 
    _TEI_, _TMI_, _TDI_ }, // 8

  { _TES_, _TMC_, _TDS_, 
    _SPC_, _TMI_, _TDI_ }  // 9
  }; 




#endif // DEF__LCD_BIG_CHAR