/*
██╗      ██████╗██████╗     ██████╗  ██████╗ ██╗  ██╗██╗  ██╗                                        
██║     ██╔════╝██╔══██╗    ╚════██╗██╔═████╗╚██╗██╔╝██║  ██║                                        
██║     ██║     ██║  ██║     █████╔╝██║██╔██║ ╚███╔╝ ███████║                                        
██║     ██║     ██║  ██║    ██╔═══╝ ████╔╝██║ ██╔██╗ ╚════██║                                        
███████╗╚██████╗██████╔╝    ███████╗╚██████╔╝██╔╝ ██╗     ██║                                        
╚══════╝ ╚═════╝╚═════╝     ╚══════╝ ╚═════╝ ╚═╝  ╚═╝     ╚═╝                                        
                                                                                                     
██████╗ ██╗ ██████╗      ██████╗██╗  ██╗ █████╗ ██████╗      ██████╗██╗      ██████╗  ██████╗██╗  ██╗
██╔══██╗██║██╔════╝     ██╔════╝██║  ██║██╔══██╗██╔══██╗    ██╔════╝██║     ██╔═══██╗██╔════╝██║ ██╔╝
██████╔╝██║██║  ███╗    ██║     ███████║███████║██████╔╝    ██║     ██║     ██║   ██║██║     █████╔╝ 
██╔══██╗██║██║   ██║    ██║     ██╔══██║██╔══██║██╔══██╗    ██║     ██║     ██║   ██║██║     ██╔═██╗ 
██████╔╝██║╚██████╔╝    ╚██████╗██║  ██║██║  ██║██║  ██║    ╚██████╗███████╗╚██████╔╝╚██████╗██║  ██╗
╚═════╝ ╚═╝ ╚═════╝      ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝     ╚═════╝╚══════╝ ╚═════╝  ╚═════╝╚═╝  ╚═╝
                                                                                                     
//===========================
GPIO-13 - TOUCH
GPIO-14 - TOUCH
GPIO-12 - TOUCH
GPIO-15 - LCD VCC - ANALOG PWM
GPIO-21 - I2C SCL | LCD 20x4 | AHX10 |
GPIO-22 - I2C SDA | LCD 20x4 | AHX10 |
                      0x27      0x39 
*/



#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <Adafruit_AHTX0.h>
#include <WebServer.h>
#include <Wire.h> 
#include <Preferences.h>
#include <ESPmDNS.h>

Preferences prefs;


// Historico de temperatura

typedef struct {
  tm              DATAHORA;
  float           TEMPERATURA_GRAUS;
  float           HUMIDADE_PORCENTAGEM;
} stRegistroDadosTemperatura;


#define _MAX_HISTORICO_TEMPERATURA_ 50 
bool INICIALIZA_TABELA = true; 

struct tm _ULTIMA_DATA_HORA_LIDA;
stRegistroDadosTemperatura _TEMP_HISTORICO [ _MAX_HISTORICO_TEMPERATURA_ ];

stRegistroDadosTemperatura  _COLETA_ATUAL;

float                       _TEMP_MAXIMA;
float                       _TEMP_MEDIA;
float                       _TEMP_MINIMA;

#include "lcdbigchar.h"
#include "WeedDay.h"
//#include "ESP32_EEPROM.h"

byte zero = 0x00; 

#define TOUCH_PIN_1   13
#define TOUCH_PIN_2   12
#define TOUCH_PIN_3   14

#define LCR_BRILHO_PIN   16

#define MAX_BRILHO_LCD   0xff
#define MIN_BRILHO_LCD   0x00

int VALOR_BRILHO = 255;



//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
//===========================
// DISPLAY LCD 20x4 I2C
#define I2C_SCL       22
#define I2C_SDA       21

#define I2C_ADDR    0x27 // Padrao sem Strapes

#define LCD_COLUMNS   20
#define LCD_LINES      4

LiquidCrystal_I2C lcd( I2C_ADDR, LCD_COLUMNS, LCD_LINES);


Adafruit_AHTX0 aht;
Adafruit_Sensor *aht_humidity, *aht_temp;

WebServer server(80);

String _DATA_TXT = "";
String _HORA_TXT = "";

String _TEMPERATURA_E_HUMIDADE_TXT = "";
//String _TEMPERATURA_GRAUS ;
//String _HUMIDADE_PORCENTAGEM ;

String _ULTIMA_TEMPERATURA_LIDA_GRAUS ;  
String _TEMPERATURAS_LIDAS_GRAUS[10] = { "--", "--", "--", "--", "--", "--", "--", "--", "--", "--" };

sensors_event_t _HUMIDADE_PORCENTAGEM;
sensors_event_t _TEMPERATURA_GRAUS;

int _COL = 0;


//===========================
// WIFI
WiFiManager wifiManager;

const char* WiFi_Name = "WEB-CLOCK";

const char* ssid = "IOT";
const char* password = "12345678";
const char* host = "relogio";

//===========================
// NTP
// #define NTP_SERVERS "us.pool.ntp.org", "time.nist.gov", "pool.ntp.org"
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";


const long  gmtOffset_sec = 3600 * -4;
const int   daylightOffset_sec = 3600;

#define EEPROM_SIZE 12

void Mostra_Relogio();
void GetLocalTime();
void WIFI_Config();
void GetAHTx_Data();
void Html_Result();

//===========================
//
//===========================
void setup() {
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LCR_BRILHO_PIN, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);  
  analogWrite(LCR_BRILHO_PIN, VALOR_BRILHO);  

  Serial.begin(115200);
  Serial.println("\n\n\n");
  Serial.println("==============================");
  Serial.println("LCD 2-x4 WEB CLOCK 01.001 B001");

  prefs.begin("FBRILHO");                              
//  prefs.putInt("FBRILHO", VALOR_BRILHO );
  prefs.getUInt("FBRILHO", VALOR_BRILHO );
  Serial.println(VALOR_BRILHO);


// EEPROM_Init( 'W' );
// EEPROM_Init( 'R' );
// EEPROM_Init( 'R' ); 
// VALOR_BRILHO = EEPROM_Read( 0 );


  Wire.begin(I2C_SDA, I2C_SCL);

  lcd.init();
  lcd.begin(20, 4); 
  
  
  
  lcd.clear();
  lcd.backlight();
  lcd.noBlink();
  lcd.noAutoscroll();

  //Asscia cada segmento criado, a um número

  lcd.createChar( _TES_, _TES ); // 0
  lcd.createChar( _TEI_, _TEI ); // 1
  lcd.createChar( _TDS_, _TDS ); // 2
  lcd.createChar( _TDI_, _TDI ); // 3
  lcd.createChar( _TMS_, _TMS ); // 4
  lcd.createChar( _TMI_, _TMI ); // 5
  lcd.createChar( _TMC_, _TMC ); // 6
  lcd.createChar( _TPO_, _TPO ); // 7



  lcd.clear();
                             //  :01234567890623466789  
  lcd.setCursor(0, 0); lcd.print("| Relogiok |========" );
  lcd.setCursor(0, 1); lcd.print("LCD WEB CLOCK       ");
  lcd.setCursor(0, 2); lcd.print("DENIS NESTOR KIELING");
  lcd.setCursor(0, 3); lcd.print("  SW 01.001 - B001  ");
  delay(1000);


  lcd.clear();
                               //:01234567890623466789  
  lcd.setCursor(0, 0); lcd.print("Conectando no WIFI !");      
  lcd.setCursor(0, 1); lcd.print("Espere 15 seg...    ");      

  //===========================
  wifiManager.setConfigPortalTimeout(15);
  if ( !wifiManager.autoConnect( WiFi_Name ) ) {
      WIFI_Config();
    }  

 lcd.clear();
                               //:01234567890623466789    
  lcd.setCursor(0, 0); lcd.print("Conectando no WIFI");      
  lcd.setCursor(0, 1); lcd.print(WiFi.localIP());
  lcd.setCursor(0, 2); lcd.print(WiFi.macAddress());
  lcd.setCursor(0, 3); lcd.print("OK!");    
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);  

  lcd.clear();
                               //:01234567890623466789    
  lcd.setCursor(0, 0); lcd.print("Relogio da INTERNET ");      
  lcd.setCursor(0, 1); lcd.print(ntpServer1);      
  lcd.setCursor(0, 2); lcd.print(ntpServer2);      
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
  Serial.println("NTP Connect");  
  GetLocalTime();
  lcd.setCursor(0, 3); lcd.print("OK!");    
  delay(500);

 // Initialize AHTx sensor
  lcd.clear();
                               //:01234567890623466789    
  lcd.setCursor(0, 0); lcd.print("AHTx Temperatura");      
  int CNT = 0;
  if (!aht.begin()) {
  Serial.println("Failed to find AHT10/AHT20 chip");
  while (1) {
    Serial.print(".");  delay(100);
    if (CNT++ > 100 ) { 
      Serial.print("No AHTx find..");
      break; 
      }
    }
  }

  lcd.setCursor(0, 3); lcd.print("OK!");    
  Serial.println("AHTx OK");  
  delay(500);


  // Start web server
  server.on("/", Html_Result);
  server.begin();
  Serial.println("Web server started");
  Serial.println( WiFi.localIP() );

  MDNS.begin(host);
  
  lcd.clear();
  lcd.setCursor(0,0); lcd.print( _DATA_TXT ); 
  GetAHTx_Data();       
}

//===========================
//
//===========================
void GetAHTx_Data() {

   aht.getEvent(&_HUMIDADE_PORCENTAGEM, &_TEMPERATURA_GRAUS);

  _COLETA_ATUAL.DATAHORA               = _ULTIMA_DATA_HORA_LIDA;
  _COLETA_ATUAL.TEMPERATURA_GRAUS      = _TEMPERATURA_GRAUS.temperature;
  _COLETA_ATUAL.HUMIDADE_PORCENTAGEM   = _HUMIDADE_PORCENTAGEM.relative_humidity;

  //  |01234567890623466789|  
  //                   [34]| 
  //  +23 28 34 | 23.6C 90%+  


  _TEMPERATURA_E_HUMIDADE_TXT  =   String( _TEMP_MINIMA , 0)
                                 + " "
                                 + String( _TEMP_MEDIA , 0)
                                 + " "
                                 + String( _TEMP_MAXIMA, 0)
                                 + "  "
                                 + String( _COLETA_ATUAL.TEMPERATURA_GRAUS   , 2 )
                                 + "C "
                                 + String( _COLETA_ATUAL.HUMIDADE_PORCENTAGEM, 0 )
                                 + "% "
                                 ;

  return;
}     


//===========================
//
//===========================
void loop() {
  static long currentTime = 0;

  static long previousTime1000ms = 0;
  static long previousTime100ms = 0;
  
  static long T_1000MS = 1000;
  static long T_100MS = 100;
  static bool FLAG = true;

  currentTime = millis();

  server.handleClient();

  if ((currentTime - previousTime100ms) > T_100MS) {
    previousTime100ms = currentTime;
  
    Mostra_Relogio();   

    int valor_tecla1 = touchRead( TOUCH_PIN_1 ); 
    int valor_tecla2 = touchRead( TOUCH_PIN_2 ); 
    int valor_tecla3 = touchRead( TOUCH_PIN_3 );  if (valor_tecla3 <= 20 ) Serial.println("3");

    if (valor_tecla1 <= 20 ) {
      Serial.println("1");
      VALOR_BRILHO += 1;
      if ( VALOR_BRILHO > MAX_BRILHO_LCD ) VALOR_BRILHO = MAX_BRILHO_LCD;
      analogWrite(LCR_BRILHO_PIN, VALOR_BRILHO);
      Serial.println(VALOR_BRILHO);      
      prefs.putInt("FBRILHO", VALOR_BRILHO);
    }

    if (valor_tecla2 <= 20 ) {
      Serial.println("2");
      VALOR_BRILHO -= 5;
      if ( VALOR_BRILHO < MIN_BRILHO_LCD ) VALOR_BRILHO = MIN_BRILHO_LCD;
      analogWrite(LCR_BRILHO_PIN, VALOR_BRILHO);
      Serial.println(VALOR_BRILHO);      
      prefs.putInt("FBRILHO", VALOR_BRILHO );
    }
  
  }

  if ((currentTime - previousTime1000ms) > T_1000MS) {
    previousTime1000ms = currentTime;

    FLAG = !FLAG;
    if (FLAG) {
      digitalWrite(LED_BUILTIN, HIGH);  
      lcd.setCursor( _DIGITO_P1, _LINHA_1); lcd.write( _TPO_ );
      lcd.setCursor( _DIGITO_P1, _LINHA_2); lcd.write( _TPO_ );

      lcd.setCursor( _DIGITO_P2, _LINHA_1); lcd.write( _TPO_ );
      lcd.setCursor( _DIGITO_P2, _LINHA_2); lcd.write( _TPO_ );
    } else {
    
      digitalWrite(LED_BUILTIN, LOW);  
      lcd.setCursor( _DIGITO_P1, _LINHA_1); lcd.write( _SPC_ );
      lcd.setCursor( _DIGITO_P1, _LINHA_2); lcd.write( _SPC_ );

      lcd.setCursor( _DIGITO_P2, _LINHA_1); lcd.write( _SPC_ );
      lcd.setCursor( _DIGITO_P2, _LINHA_2); lcd.write( _SPC_ );
    }
  }

 
}


//===========================
void MostaDigito( char COL, char LIN , char DIGITO ) {

  for(int n = 0; n < 3; n++) {
    lcd.setCursor( COL+n, LIN   ); lcd.write( _SPC_ );    
    lcd.setCursor( COL+n, LIN+1 ); lcd.write( _SPC_ );    
  }


  // Plota Numero
  for(int n = 0; n < 3; n++) {
    lcd.setCursor( COL+n, LIN  ); lcd.write( real_digits[ DIGITO ][ n   ] ); delay(20); 
    lcd.setCursor( COL+n, LIN+1); lcd.write( real_digits[ DIGITO ][ n+3 ] ); delay(20);    
  }



}

//===========================
void Ajusta_Tabela_Historico_Temperatura() {

// Ajusta Historico de Temperatura
  //  0 = ATUAL
  //  1 =  0 
  //  2 =  1 
  //   ...
  // 24 = 23 
  // 25 = 24 

if ( INICIALIZA_TABELA ) {
  INICIALIZA_TABELA = false;   
  for(int n = 0; n < _MAX_HISTORICO_TEMPERATURA_ -1 ; n++ ) {
     _TEMP_HISTORICO[n] =_COLETA_ATUAL;
    }
  }

  // Copia Dados 
  for (int i = _MAX_HISTORICO_TEMPERATURA_ - 2; i >= 0; i--) {
    _TEMP_HISTORICO[i + 1] = _TEMP_HISTORICO[i];
   }

  _TEMP_HISTORICO[0] = _COLETA_ATUAL;


  _TEMP_MAXIMA = -200;
  _TEMP_MINIMA = +200;
  _TEMP_MEDIA  =    0;

  for(int n = 0; n < _MAX_HISTORICO_TEMPERATURA_ ; n++ ) {
    _TEMP_MAXIMA  = max(_TEMP_MAXIMA, _TEMP_HISTORICO [ n ].TEMPERATURA_GRAUS);
    _TEMP_MINIMA  = min(_TEMP_MAXIMA, _TEMP_HISTORICO [ n ].TEMPERATURA_GRAUS);
    _TEMP_MEDIA  += _TEMP_HISTORICO [ n ].TEMPERATURA_GRAUS;
    }

  _TEMP_MEDIA = _TEMP_MEDIA / _MAX_HISTORICO_TEMPERATURA_;

}



//===========================
void Mostra_Relogio()
{
  static char    numero = -1; 
  static char    segundoanterior1 = -1;
  static char    segundoanterior2 = -1;
  static char    minutoanterior1 = -1;
  static char    minutoanterior2 = -1;
  static char    horaanterior1 = -1;
  static char    horaanterior2 = -1;

  byte minut[2];
  String minutostring;
  String digitosegundos;
  char buf;
  
  GetLocalTime();

  byte segundos    = _ULTIMA_DATA_HORA_LIDA.tm_sec;
  byte minutos     = _ULTIMA_DATA_HORA_LIDA.tm_min;
  byte horas       = _ULTIMA_DATA_HORA_LIDA.tm_hour; 
  byte diadasemana = _ULTIMA_DATA_HORA_LIDA.tm_wday; 
  byte diadomes    = _ULTIMA_DATA_HORA_LIDA.tm_mday;
  byte mes         = _ULTIMA_DATA_HORA_LIDA.tm_mon; 
  byte ano         = _ULTIMA_DATA_HORA_LIDA.tm_year;

  //================================ 
  _COL = _DIGITO_1;
  numero = horas/10;
  if (horaanterior1 != horas / 10) {
    horaanterior1 = horas / 10;
    MostaDigito( _COL, _LINHA_1 , numero );    
  }

  //================================ 
  _COL = _DIGITO_2;
  numero = horas % 10;
  if (horaanterior2 != horas %10) {
    horaanterior2 = horas % 10;
    MostaDigito( _COL, _LINHA_1 , numero );   
    Ajusta_Tabela_Historico_Temperatura();
  }
  
  //================================ 
  _COL = _DIGITO_3;
  numero = minutos / 10;
  if (minutoanterior1 != minutos / 10) {
    minutoanterior1 = minutos / 10;
    MostaDigito( _COL, _LINHA_1 , numero );   
  }

  //================================ 
  _COL = _DIGITO_4;
  numero = minutos % 10;
  if (minutoanterior2 != minutos % 10)  {
    minutoanterior2 = minutos % 10;
    MostaDigito( _COL, _LINHA_1 , numero );  

    GetAHTx_Data();          
    lcd.setCursor(0,0); lcd.print( _DATA_TXT );  
    lcd.setCursor(0,3); lcd.print( _TEMPERATURA_E_HUMIDADE_TXT );   
  }
 
  //================================ 
  _COL = _DIGITO_5;
  numero = segundos / 10;
  if (segundoanterior1 != segundos / 10) {
    segundoanterior1 = segundos / 10;
    MostaDigito( _COL, _LINHA_1 , numero );    
  }

  //================================ 
  _COL = _DIGITO_6;
  numero = segundos % 10;
  if (segundoanterior2 != segundos % 10)  {
    segundoanterior2 = segundos % 10;
    MostaDigito( _COL, _LINHA_1 , numero );    

    GetAHTx_Data();          
    lcd.setCursor(0,3); lcd.print( _TEMPERATURA_E_HUMIDADE_TXT );   
  }

}

//=================================================
// Pega Data e Hora
//==================================================
void GetLocalTime() {


  if (!getLocalTime( &_ULTIMA_DATA_HORA_LIDA) ) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);    
    Serial.println("No time available (yet)");
    return;
  }
  
 
  char TXT[ 21 ];

  // :01234567890123456789
  //  Dom 23/07/2024  [23]
  strftime( TXT, sizeof(TXT), "%d/%m/%Y  [%W]", &_ULTIMA_DATA_HORA_LIDA );
  _DATA_TXT = _DIASEMANA[ _ULTIMA_DATA_HORA_LIDA.tm_wday ] + String(TXT);  
 
  strftime( TXT, sizeof(TXT), "%H:%M:%S", &_ULTIMA_DATA_HORA_LIDA );
  _HORA_TXT = String(TXT);  

  
}


//=======================
// Ativar WIFI Manager
//=======================
void WIFI_Config () {

  Serial.println("Ligado WIFI Manager");
  lcd.clear();  lcd.setCursor(0, 0); lcd.print("Config WIFI to ESP32");   
  lcd.setCursor(0, 1); lcd.print("Connect to AP-ESP32");   
  lcd.setCursor(0, 2); lcd.print("Use WEB-Brower");   
  lcd.setCursor(0, 3); lcd.print("HTTP://192.168.4.1");   

  wifiManager.setConfigPortalTimeout( 90 );
  digitalWrite(LED_BUILTIN, HIGH);    

  if (!wifiManager.startConfigPortal(WiFi_Name)) {
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("failed to connect");   
    Serial.println("failed to connect and hit timeout");
  } 
  else {
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("connected...");   
    Serial.println("connected...yeey :)");  
    digitalWrite(LED_BUILTIN, LOW);      
  }

  delay(1000);
  ESP.restart();
  }



//=======================
// Create HTML page with sensor data
//=======================
void Html_Result () {
  String html = "<html>";
  html += "<head> <meta http-equiv='refresh' content='10'> </head>"; 
  html += "<body>";
  html += "<h1>ESP32";
  html += "<h1>LCD Web Clock";
  html += "<h2>Humiddade e Temperatura</h2>";
  html += "<p>Data: " + String( _DATA_TXT ) + "</p>";
  html += "<p>Hora: " + String( _HORA_TXT ) + "</p>";
  html += "<p>Temperature Atual: "  + String( _COLETA_ATUAL.TEMPERATURA_GRAUS , 2)  + "C</p>";
  html += "<p>Temperature Maxima: " + String( _TEMP_MAXIMA , 2)  + "C</p>";
  html += "<p>Temperature Media: "  + String( _TEMP_MEDIA  , 2)  + "C</p>";
  html += "<p>Temperature Minima: " + String( _TEMP_MINIMA , 2)  + "C</p>";
  

  html += "<p>Humididade: "    + String( _HUMIDADE_PORCENTAGEM.relative_humidity , 0)  + " % rH</p>";
  html += "<p>Ultimas Temperaturas por Hora: </p>";

    char TXT [30];
    String LINHA;
  for(int n = 0; n < _MAX_HISTORICO_TEMPERATURA_ ; n++ ) {
    strftime( TXT, sizeof(TXT), "%d/%m/%Y %H:%M:%S", &_TEMP_HISTORICO[n].DATAHORA );

    LINHA = String(n) + " :" 
            + String(TXT) 
            + " " 
            + String(_TEMP_HISTORICO[n].TEMPERATURA_GRAUS   , 1) + "C"
            + " " 
            + String(_TEMP_HISTORICO[n].HUMIDADE_PORCENTAGEM, 1) + "%"
            ;
    html += "<p>"  + LINHA + "</p>";
    }
  
  html += "</body>";
  html += "</html>";

  server.send(200, "text/html", html);  

  return;
}

