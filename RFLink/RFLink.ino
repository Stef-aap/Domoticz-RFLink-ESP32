
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


#define Version  "2.0"
#define Revision  0x01
#define Build     0x01


#define MIN_PULSE_LENGTH                  40                                    // Pulses shorter than this value in uSec. will be seen as garbage and not taken as actual pulses.
#define SIGNAL_TIMEOUT                     7                                    // Timeout, after this time in mSec. the RF signal will be considered to have stopped.
#define SIGNAL_REPEAT_TIME               500                                    // Time in mSec. in which the same RF signal should not be accepted again. Filters out retransmits.

#define RAW_BUFFER_SIZE                  512                                    // Maximum number of pulses that is received in one go.

#define INPUT_COMMAND_SIZE                60                                    // Maximum number of characters that a command via serial can be.
#define PRINT_BUFFER_SIZE                 60                                    // Maximum number of characters that a command should print in one go via the print buffer.

#define TRANSMIT_PIN              5                                         // Data to the 433Mhz transmitter on this pin
#define RECEIVE_PIN              19                                          // On this input, the 433Mhz-RF signal is received. LOW when no signal.
//****************************************************************************************************************************************

byte PKSequenceNumber=0;                                                        // 1 byte packet counter


// ***********************************************************************************
// ***********************************************************************************
//String Known_Devices = "\
//KAKU;ID=2508A7C;\
//KAKU;ID=1C38A04;" ;

#include "RFLink_File.h"
_RFLink_File  RFLink_File ( "/RFLink.txt" ) ;

// ***********************************************************************************
// ***********************************************************************************

char PreFix [20]       ;
unsigned long Last_Detection_Time = 0L ;
int Learning_Mode = 0 ;  // always start in production mode



char pbuffer[PRINT_BUFFER_SIZE];                                                // Buffer for printing data
char pbuffer2[30];
char InputBuffer_Serial[INPUT_COMMAND_SIZE];                                    // Buffer for Seriel data

struct RawSignalStruct                                                          // Raw signal variabelen places in a struct
  {
  int  Number;                                                                  // Number of pulses, times two as every pulse has a mark and a space.
  int  Min  ;
  int  Max  ;
  long Mean ;
  byte Delay;                                                                   // Delay in ms. after transmit of a single RF pulse packet
  //byte Multiply;                                                                // Pulses[] * Multiply is the real pulse time in microseconds 
  unsigned long Time;                                                           // Timestamp indicating when the signal was received (millis())
  int Pulses[RAW_BUFFER_SIZE+2];                                               // Table with the measured pulses in microseconds divided by RawSignal.Multiply. (halves RAM usage)
  //byte Pulses[RAW_BUFFER_SIZE+2];                                               // Table with the measured pulses in microseconds divided by RawSignal.Multiply. (halves RAM usage)
                                                                                // First pulse is located in element 1. Element 0 is used for special purposes, like signalling the use of a specific plugin
} RawSignal={0,0,0,0,0,0L};
// ===============================================================================
unsigned long SignalCRC=0L;                                                     // holds the bitstream value for some plugins to identify RF repeats
unsigned long SignalHash=0L;                                                    // holds the processed plugin number
unsigned long SignalHashPrevious=0L;                                            // holds the last processed plugin number

bool Serial_Command      = false ;
int  SerialInByteCounter = 0 ;                                                    // number of bytes counter 
byte SerialInByte ;                                                          // incoming character value

String Unknown_Device_ID = "" ;


#include "RFL_Protocols.h" 



// ***********************************************************************************
// ***********************************************************************************
void setup() {
  Serial.begin ( 57600 ) ;                    
  //Serial.begin ( 115200 ) ;                    

  RFLink_File.Begin () ;
  //RFLink_File.Clear () ;   // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

  pinMode      ( RECEIVE_PIN,  INPUT        ) ;
  pinMode      ( TRANSMIT_PIN, OUTPUT       ) ;
  digitalWrite ( RECEIVE_PIN,  INPUT_PULLUP ) ;  // pull-up resister on (to prevent garbage)

  // >>>>>>>>>>   PROTOCOL CLASSES   <<<<<<<<<<<<<<<<<<<
  RFL_Protocols.Add ( new _RFL_Protocol_KAKU             () ) ;  
  RFL_Protocols.Add ( new _RFL_Protocol_EV1527           () ) ;  
  RFL_Protocols.Add ( new _RFL_Protocol_Paget_Door_Chime () ) ;  
  RFL_Protocols.setup () ;
  // ***************************************************

  delay ( 200 ) ;
  Serial.printf ( "20;%02X;Nodo RadioFrequencyLink - MiRa V%s - R%02x\r\n", 
                  PKSequenceNumber++, Version, Revision );

  RawSignal.Time = millis() ;
}

 
// ***********************************************************************************
// ***********************************************************************************
void loop () {
  if ( FetchSignal () ) {
    RFL_Protocols.Decode ();
  }
  Handle_Serial () ;
}



