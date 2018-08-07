/*********************************************************************************************/
boolean ScanEvent(void) {                                         // Deze routine maakt deel uit van de hoofdloop en wordt iedere 125uSec. doorlopen
  unsigned long Timer=millis()+SCAN_HIGH_TIME;

//Serial.println();

  while(Timer>millis() || RepeatingTimer>millis()) {
       if (FetchSignal(PIN_RF_RX_DATA,HIGH)) {                    // RF: *** data start ***
//if (FetchSignal(PIN_RF_RX_DATA,LOW)) {                    // RF: *** data start ***
//Serial.print ( "." ) ;

if ( RFL_Protocols.Decode ()) {

             RepeatingTimer=millis()+SIGNAL_REPEAT_TIME;
             return true;
          }
       }
  }// while
  return false;
}
/**********************************************************************************************\
 * Haal de pulsen en plaats in buffer. 
 * bij de TSOP1738 is in rust is de uitgang hoog. StateSignal moet LOW zijn
 * bij de 433RX is in rust is de uitgang laag. StateSignal moet HIGH zijn
 * 
 \*********************************************************************************************/



// ************************************************************
//   LoopsPerMilli      maxloops       time [msec)
//      500               3500          0.8 ... 1.7  (zeer wisselend)
//     1500              10500          2.4
//     4000              28000          6.3
// detectie van Notenhout knop A ( QIAchip = EV1527)
//  1500:  2370,270,690,270,690,270,690,270,690,270,690,270,690,270,690,270,720,270,690,750,210,240,690,750,210,750,210,750,210,240,720,750,240,750,210,720,240,720,240,720,240,240,720,240,720,240,720,720,210,210,
//              2640;
//  4000:  660,270,690,270,690,270,690,240,690,270,690,270,690,270,750,210,690,270,690,720,240,240,720,750,210,750,210,750,210,270,690,750,210,750,210,750,210,750,240,720,240,240,690,240,720,210,720,690,270,240,
//            7050;
// We zien de beginpuls korter worden en de eindpuls langer (dus 1500 was toch wel een hele mooie waarde
// ************************************************************
//const unsigned long LoopsPerMilli=345;   // <<< OORSPRONKELIJKE WAARDE VOOR ARDUINO MEGA
const unsigned long LoopsPerMilli=1500;
//const unsigned long LoopsPerMilli=4000;
// ************************************************************
// ************************************************************



const unsigned long Overhead=0;  

// Because this is a time critical routine, we use global variables so that the variables 
// do not need to be initialized at each function call. 
int RawCodeLength=0;
unsigned long PulseLength=0L;
unsigned long numloops=0L;
unsigned TEMP_numloops ;
unsigned long maxloops=0L;

boolean Ftoggle=false;
uint8_t Fbit=0;
uint8_t Fport=0;
uint8_t FstateMask=0;
/*********************************************************************************************/
boolean FetchSignal(byte DataPin, boolean StateSignal) {
   unsigned long LastPulse;
   //uint8_t Fbit = digitalPinToBitMask(DataPin);
   //uint8_t Fport = digitalPinToPort(DataPin);
   //uint8_t FstateMask = ( StateSignal ? Fbit : 0 ) ;
   bool    FstateMask = LOW ;  //DIT WERKT HET BESTE VOOR QIACHIP !! //! StateSignal ;


   // ************************************************************
   // Wacht totdat er geen signaal is  ???
   // ************************************************************
//   if ((*portInputRegister(Fport) & Fbit) == FstateMask) {                       // Als er signaal is
if (  digitalRead(PIN_RF_RX_DATA) == FstateMask) {                       // Als er signaal is
     
    // ************************************************************
    // Wacht totdat de pulsen voorbij zijn
    //    doet niets, dus verwijderd:  if (RawSignal.Time)
    // #define SIGNAL_REPEAT_TIME  500   // Time in mSec. in which the same RF signal should not be accepted again. Filters out retransmits.
    // #define SIGNAL_TIMEOUT       7    // Timeout, after this time in mSec. the RF signal will be considered to have stopped.
    // RawSignal.Time = tijdstip van de vorige detectie
    // RawSignal.Repeats wordt door protocol 005 en 061 op true gezet na een valide detectie
    // ************************************************************
    // ************************************************************
    // als herhalingen niet worden geaccepteerd
    //    en
    // als er minder dan 500 msec zijn verstreken sinds de vorige valide detectie van een signaal
    // ************************************************************
    if ( RawSignal.Repeats && ( millis() < ( RawSignal.Time + SIGNAL_REPEAT_TIME ) ) ) { 
      PulseLength = millis() + SIGNAL_TIMEOUT ;                             

      // ************************************************************
      // wacht tot minstens
      //    500 msec zijn verstreken sinds de vorige valide detectie van een signaal
      //         EN
      //      7 msec sinds begin van deze if-loop ????
      // ************************************************************
      while ( ( millis() < ( RawSignal.Time + SIGNAL_REPEAT_TIME ) ) && ( millis() < PulseLength ) ) 
        // ************************************************************
        // als er nog steeds signaal is, 
        //   blijf weer voor 7 msec in de while-loop
        // ************************************************************
        if ( digitalRead ( PIN_RF_RX_DATA ) == FstateMask )
          PulseLength = millis() + SIGNAL_TIMEOUT ;                             
        
      // ************************************************************
      // dus hier is er geen signaal meer (tenzij het binnen 7 msec is opgekomen)
      // ************************************************************
      // wacht tot minstens 500 msec zijn verstreken sinds de vorige valide detectie
      //   OF
      // er geen signaal meer is
      // ************************************************************
      while ( ( millis() < ( RawSignal.Time + SIGNAL_REPEAT_TIME ) ) &&  ( digitalRead ( PIN_RF_RX_DATA ) != FstateMask ) ) ;
    }
     
//     RawCodeLength=1;   // Start at 1 for legacy reasons. Element 0 can be used to pass special information like plugin number etc.
     RawCodeLength = 0 ;                              
     Ftoggle       = false ;                  
     maxloops      = ( SIGNAL_TIMEOUT * LoopsPerMilli );

       
    // ************************************************************
    // Na een start, wordt hier de hele serie pulsen gemeten
    // ************************************************************
    do{                                                                       
      // ************************************************************
      // Meet de breedte van het huidige nivo
      //   break als het nivo te lang duurt
      // ************************************************************
      numloops  = 0 ;
      LastPulse = micros () ;
      while ( ( digitalRead ( PIN_RF_RX_DATA ) == FstateMask ) ^ Ftoggle )     // while() loop *A*
        if ( numloops++ == maxloops ) break ;                                  // timeout 
      PulseLength = micros() - LastPulse;                                      // Contains pulslength in microseconds
      
      // ************************************************************
      // spring uit de loop als we een te korte puls detecteren
      // ************************************************************
      if ( PulseLength < MIN_PULSE_LENGTH ) break;
      
      // ************************************************************
      // Inverteer het nivo naar waar we gaan zoeken
      // ************************************************************
      Ftoggle = !Ftoggle ;    
      
      // ************************************************************
      // Geldige puls gevonden, dus opbergen
      // ************************************************************
      RawSignal.Pulses [ RawCodeLength++ ] = PulseLength / (unsigned long)( RAWSIGNAL_SAMPLE_RATE ) ;
      
    } while ( ( RawCodeLength < RAW_BUFFER_SIZE ) && ( numloops <= maxloops ));
    //For as long as there is space in the buffer, no timeout etc.

    // ************************************************************
    // We hebben nu het einde van een signaal bereikt
    //   als we genoeg pulsen hebben,
    //     return true
    // ************************************************************
    if ( RawCodeLength >= MIN_RAW_PULSES ) {
      RawSignal.Repeats  = 0 ;                          // no repeats
      RawSignal.Multiply = RAWSIGNAL_SAMPLE_RATE ;      // sample size.
      RawSignal.Number   = RawCodeLength-1 ;            // Number of received pulse times (pulsen *2)
      RawSignal.Pulses [ RawSignal.Number + 1 ] = 0 ;   // Last element contains the timeout. 
      RawSignal.Time = millis() ;                       // Time the RF packet was received (to keep track of retransmits
      return true ;
    } 
    else {
      RawSignal.Number = 0 ;    
    }
  }
  return false;
}



/*********************************************************************************************/
// RFLink Board specific: Generate a short pulse to switch the Aurel Transceiver from TX to RX mode.
void RFLinkHW( void ) {
     delayMicroseconds(36);
     digitalWrite(PIN_BSF_0,LOW);
     delayMicroseconds(16);
     digitalWrite(PIN_BSF_0,HIGH);
     return;
}
/*********************************************************************************************\
 * Send rawsignal buffer to RF
\*********************************************************************************************/
void RawSendRF(void) {
  int x;
  digitalWrite(PIN_RF_RX_VCC,LOW);                                        // Spanning naar de RF ontvanger uit om interferentie met de zender te voorkomen.
  digitalWrite(PIN_RF_TX_VCC,HIGH);                                       // zet de 433Mhz zender aan
  delayMicroseconds(TRANSMITTER_STABLE_DELAY);                            // short delay to let the transmitter become stable (Note: Aurel RTX MID needs 500µS/0,5ms)
  
  // LET OP: In de Arduino versie 1.0.1 zit een bug in de funktie delayMicroSeconds(). Als deze wordt aangeroepen met een nul dan zal er
  // een pause optreden van 16 milliseconden. Omdat het laatste element van RawSignal af sluit met een nul (omdat de space van de stopbit 
  // feitelijk niet bestaat) zal deze bug optreden. Daarom wordt deze op 1 gezet om de bug te omzeilen. 
  RawSignal.Pulses[RawSignal.Number]=1;

  for(byte y=0; y<RawSignal.Repeats; y++) {                               // herhaal verzenden RF code
     x=1;
     noInterrupts();
     while(x<RawSignal.Number) {
        digitalWrite(PIN_RF_TX_DATA,HIGH);
        delayMicroseconds(RawSignal.Pulses[x++]*RawSignal.Multiply-5);    // min een kleine correctie  
        digitalWrite(PIN_RF_TX_DATA,LOW);
        delayMicroseconds(RawSignal.Pulses[x++]*RawSignal.Multiply-7);    // min een kleine correctie
     }
     interrupts();
     if (y+1 < RawSignal.Repeats) delay(RawSignal.Delay);                 // Delay buiten het gebied waar de interrupts zijn uitgeschakeld! Anders werkt deze funktie niet.
  }

  delayMicroseconds(TRANSMITTER_STABLE_DELAY);                            // short delay to let the transmitter become stable (Note: Aurel RTX MID needs 500µS/0,5ms)
  digitalWrite(PIN_RF_TX_VCC,LOW);                                        // zet de 433Mhz zender weer uit
  digitalWrite(PIN_RF_RX_VCC,HIGH);                                       // Spanning naar de RF ontvanger weer aan.
  RFLinkHW();
}
/*********************************************************************************************/

