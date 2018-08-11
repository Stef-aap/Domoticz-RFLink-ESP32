
// ****************************************************************************
// used in Raw signal
//
// original: 20=8 bits. Minimal number of bits*2 that need to have been received 
//    before we spend CPU time on decoding the signal.
//
// MAX might be a little to low ??
// ****************************************************************************
#define MIN_RAW_PULSES                 26  //   20      // =8 bits. Minimal number of bits*2 that need to have been received before we spend CPU time on decoding the signal.
#define MAX_RAW_PULSES                 150   
// ****************************************************************************


#define BUILDNR                          000                                    // shown in version
#define MIN_PULSE_LENGTH                  40                                    // Pulses shorter than this value in uSec. will be seen as garbage and not taken as actual pulses.
#define SIGNAL_TIMEOUT                     7                                    // Timeout, after this time in mSec. the RF signal will be considered to have stopped.
#define SIGNAL_REPEAT_TIME               500                                    // Time in mSec. in which the same RF signal should not be accepted again. Filters out retransmits.
#define BAUD                           57600                                    // Baudrate for serial communication.
#define TRANSMITTER_STABLE_DELAY         500                                    // delay to let the transmitter become stable (Note: Aurel RTX MID needs 500µS/0,5ms).
#define RAW_BUFFER_SIZE                  512                                    // Maximum number of pulses that is received in one go.
#define PLUGIN_MAX                        41                                    // Maximum number of Receive plugins
#define PLUGIN_TX_MAX                     20                                    // Maximum number of Transmit plugins
#define SCAN_HIGH_TIME                    50                                    // tijdsinterval in ms. voor achtergrondtaken snelle verwerking
#define FOCUS_TIME                        50                                    // Duration in mSec. that, after receiving serial data from USB only the serial port is checked. 
#define INPUT_COMMAND_SIZE                60                                    // Maximum number of characters that a command via serial can be.
#define PRINT_BUFFER_SIZE                 60                                    // Maximum number of characters that a command should print in one go via the print buffer.

#define VALUE_ALLOFF                    55
#define VALUE_OFF                       74
#define VALUE_ON                        75
#define VALUE_ALLON                     141

// PIN Definition 
#define PIN_BSF_0                   22                                          // Board Specific Function lijn-0
#define PIN_BSF_1                   23                                          // Board Specific Function lijn-1
#define PIN_BSF_2                   24                                          // Board Specific Function lijn-2
#define PIN_BSF_3                   25                                          // Board Specific Function lijn-3

#define PIN_RF_TX_VCC               15                                          // +5 volt / Vcc power to the transmitter on this pin
//#define PIN_RF_TX_DATA              14                                          // Data to the 433Mhz transmitter on this pin
#define TRANSMIT_PIN              5                                         // Data to the 433Mhz transmitter on this pin

#define PIN_RF_RX_VCC               16                                          // Power to the receiver on this pin
#define PIN_RF_RX_DATA              19                                          // On this input, the 433Mhz-RF signal is received. LOW when no signal.


//****************************************************************************************************************************************


byte dummy=1;                                                                   // get the linker going. Bug in Arduino. (old versions?)

void(*Reboot)(void)=0;                                                          // reset function on adres 0.
byte PKSequenceNumber=0;                                                        // 1 byte packet counter


// ****************************************************************************************
// ****************************************************************************************
//boolean RFDebug=false;                                                          // debug RF signals with plugin 001 
////boolean RFUDebug=false;                                                         // debug RF signals with plugin 254 
//boolean RFUDebug=false;                                                         // debug RF signals with plugin 254 
//boolean QRFDebug=false;                                                        // debug RF signals with plugin 254 but no multiplication

// LEARNING_MODE
String Learning_Modes = " \
0 : Production Mode\n\
1 : after a series detection, the length and code is displayed\n\
2 : as 1, but loops through all protocols\n\
3 : display pulstime\n\
4 : display pulstime as 01 (" ;
int Learning_Mode = 0 ;  // always start in production mode
// ****************************************************************************************
// ****************************************************************************************



uint8_t RFbit,RFport;                                                           // for processing RF signals.

char pbuffer[PRINT_BUFFER_SIZE];                                                // Buffer for printing data
char InputBuffer_Serial[INPUT_COMMAND_SIZE];                                    // Buffer for Seriel data

// Van alle devices die worden mee gecompileerd, worden in een tabel de adressen opgeslagen zodat hier naar toe gesprongen kan worden
//void PluginTXInit(void);
boolean (*PluginTX_ptr[PLUGIN_TX_MAX])(byte, char*);                            // Transmit plugins
byte PluginTX_id[PLUGIN_TX_MAX];

void PrintHex8(uint8_t *data, uint8_t length);                                  // prototype
void PrintHexByte(uint8_t data);                                                // prototype
void RFLinkHW( void );                                                          // prototype

struct RawSignalStruct                                                          // Raw signal variabelen places in a struct
  {
  int  Number;                                                                  // Number of pulses, times two as every pulse has a mark and a space.
  int  Min  ;
  int  Max  ;
  long Mean ;
  byte Repeats;                                                                 // Number of re-transmits on transmit actions.
  byte Delay;                                                                   // Delay in ms. after transmit of a single RF pulse packet
  //byte Multiply;                                                                // Pulses[] * Multiply is the real pulse time in microseconds 
  unsigned long Time;                                                           // Timestamp indicating when the signal was received (millis())
  int Pulses[RAW_BUFFER_SIZE+2];                                               // Table with the measured pulses in microseconds divided by RawSignal.Multiply. (halves RAM usage)
  //byte Pulses[RAW_BUFFER_SIZE+2];                                               // Table with the measured pulses in microseconds divided by RawSignal.Multiply. (halves RAM usage)
                                                                                // First pulse is located in element 1. Element 0 is used for special purposes, like signalling the use of a specific plugin
} RawSignal={0,0,0,0,0,0L};
// ===============================================================================
unsigned long RepeatingTimer=0L;
unsigned long SignalCRC=0L;                                                     // holds the bitstream value for some plugins to identify RF repeats
unsigned long SignalHash=0L;                                                    // holds the processed plugin number
unsigned long SignalHashPrevious=0L;                                            // holds the last processed plugin number

bool Serial_Command      = false ;
int  SerialInByteCounter = 0 ;                                                    // number of bytes counter 
byte SerialInByte ;                                                          // incoming character value




// ***********************************************************************************
// ***********************************************************************************
unsigned long HexString_2_Long ( String HexString ) {
  return ( strtoul ( HexString.c_str(), NULL, HEX ) ) ; 
//  unsigned long Value = 0 ;
//  int           Nibble ;
//  for ( int i = 0; i < HexString.length(); i++ ) {
//    Nibble = int ( HexString.charAt(i) ) ;
//    if ( Nibble >= 48 && Nibble <= 57  ) Nibble = map ( Nibble, 48, 57,  0,  9  ) ;
//    if ( Nibble >= 65 && Nibble <= 70  ) Nibble = map ( Nibble, 65, 70,  10, 15 ) ;
//    if ( Nibble >= 97 && Nibble <= 102 ) Nibble = map ( Nibble, 97, 102, 10, 15 ) ;
//    Nibble = constrain ( Nibble, 0, 15 );
//    Value = ( Value * 16 ) + Nibble ;
//  }
//  return Value;
} 



// >>>>>>>>>>   CLASSES   <<<<<<<<<<<<<<<<<<<
#include "RFL_Protocols.h" 
// >>>>>>>>>>   CLASSES   <<<<<<<<<<<<<<<<<<<




void setup() {
  Serial.begin(BAUD);                                                           // Initialise the serial port

  pinMode(PIN_RF_RX_DATA, INPUT);                                               // Initialise in/output ports
  //pinMode(PIN_RF_TX_DATA, OUTPUT);                                              // Initialise in/output ports
  pinMode( TRANSMIT_PIN, OUTPUT);                                              // Initialise in/output ports
  pinMode(PIN_RF_TX_VCC,  OUTPUT);                                              // Initialise in/output ports
  pinMode(PIN_RF_RX_VCC,  OUTPUT);                                              // Initialise in/output ports    
  digitalWrite(PIN_RF_RX_VCC,HIGH);                                             // turn VCC to RF receiver ON
  digitalWrite(PIN_RF_RX_DATA,INPUT_PULLUP);                                    // pull-up resister on (to prevent garbage)
  
  pinMode(PIN_BSF_0,OUTPUT);                                                    // rflink board switch signal
  digitalWrite(PIN_BSF_0,HIGH);                                                 // rflink board switch signal


  // >>>>>>>>>>   PROTOCOL CLASSES   <<<<<<<<<<<<<<<<<<<
  RFL_Protocols.Add ( new _RFL_Protocol_KAKU   () ) ;  
  RFL_Protocols.Add ( new _RFL_Protocol_EV1527 () ) ;  
  RFL_Protocols.Add ( new _RFL_Protocol_Paget_Door_Chime () ) ;  

  RFL_Protocols.setup () ;
  // >>>>>>>>>>   CLASSES   <<<<<<<<<<<<<<<<<<<


  RFbit=digitalPinToBitMask(PIN_RF_RX_DATA);
  RFport=digitalPinToPort(PIN_RF_RX_DATA);
  Serial.println(F("20;00;Nodo RadioFrequencyLink V1.1;"));
  PKSequenceNumber++;

  RawSignal.Time = millis() ;
}




// ***********************************************************************************
// ***********************************************************************************
void loop () {

  RFL_Protocols.loop () ;
  ScanEvent();                                                                // Scan for RF events
  
  //Serial.println ( "                         MMMMMMMMMMMMMMMMMMMMMM");
  
  Handle_Serial () ;
}



