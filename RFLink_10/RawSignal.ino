/*********************************************************************************************/
boolean ScanEvent(void) {                                     // Deze routine maakt deel uit van de hoofdloop en wordt iedere 125uSec. doorlopen
  unsigned long Timer = millis() + SCAN_HIGH_TIME;           // +50 msec

  while ( ( millis() < Timer ) || ( millis() < RepeatingTimer ) ) {
    if ( FetchSignal (PIN_RF_RX_DATA, HIGH ) ) {
      if ( RFL_Protocols.Decode () ) {
        RepeatingTimer = millis() + SIGNAL_REPEAT_TIME ;      // +500 msec
        return true;
      }
    }
  }
  return false;
}


// ***********************************************************************************
// Some time critical parameters are declared global
// ***********************************************************************************
unsigned long FETCH_Pulse_Plus_1 ;  //  here 3320,  inside 3540
// ***********************************************************************************
boolean FetchSignal ( byte DataPin, boolean StateSignal ) {
  // ************************************************************
  //   LoopsPerMilli      maxloops       timeout [msec)
  //      500               3500          0.8 ... 1.7  (zeer wisselend)
  //     1500              10500          2.4
  //     4000              28000          6.3
  // detectie van Notenhout knop A ( QIAchip = EV1527)
  //  1500:  2370,270,690,270,690,270,690,270,690,270,690,270,690,270,690,270,720,270,690,750,210,240,690,750,210,750,210,750,210,240,720,750,240,750,210,720,240,720,240,720,240,240,720,240,720,240,720,720,210,210,
  //              2640;
  //  4000:  660,270,690,270,690,270,690,240,690,270,690,270,690,270,750,210,690,270,690,720,240,240,720,750,210,750,210,750,210,270,690,750,210,750,210,750,210,750,240,720,240,240,690,240,720,210,720,690,270,240,
  //            7050;
  // We zien de beginpuls korter worden en de eindpuls langer (dus 1500 was toch wel een hele mooie waarde
  // BELANGRIJK: de laatste puls is dus de timeout in usec !!!
  // ************************************************************
  //const unsigned long LoopsPerMilli = 345;   // <<< OORSPRONKELIJKE WAARDE VOOR ARDUINO MEGA
  // 2500 was optimaal, maar we hebben de loop sneller gemaakt 3540 naar 3320 usec
  // dus verhogen we met 3540/3320 =>  2670
  const unsigned long LoopsPerMilli = 2700;
  // ************************************************************
  // ************************************************************

  // ******************************************************************************************
  // WEIRD een aantal parameters niet globaal definieeren, maar hier, maakt de routine sneller
  // in de comment staat de breedte van de laatste puls (getriggered door een timeout, dus korter is sneller)
  // ******************************************************************************************
  unsigned long LastPulse            ;   // 3320 i.p.v. 3540
  bool          Toggle        = true ;   // 3320 i.p.v. 3750               
  int           RawCodeLength = 0    ;   // maakt niet uit              
  unsigned long PulseLength          ;   // 3320 i.p.v. 3540
  unsigned long numloops             ;   // 3320 i.p.v. 4200
  bool          Start_Level   = LOW  ;   // via #define maakt niet uit
  unsigned      maxloops      = ( SIGNAL_TIMEOUT * LoopsPerMilli );  // via #define maakt niet uit
  // ******************************************************************************************


  // ************************************************************
  // wacht op lang laag nivo (Start_Level) = start van nieuwe sequence
  // ************************************************************
  bool Started = false ;
  unsigned long Start_Time = 500 + millis() ;  // 100 .. 500 msec, maakt niet zo veel uit
  while ( !Started ){
    // ************************************************************
    // als het nivo laag is, wacht tot het einde van dit laag nivo
    // ************************************************************
//    while ( ( digitalRead ( PIN_RF_RX_DATA ) == Start_Level ) ) ;
    while ( ( digitalRead ( PIN_RF_RX_DATA ) == Start_Level ) && ( millis() < Start_Time ) ) ;
    
    // ************************************************************
    // hier is het nivo hoog, wacht totdat het naar laag springt
    // als we de vorige while lus niet doorlopen zijn,
    //   zijn we halverwege een positieve puls deze routine binnen gekomen
    //   en kunnen we deze positieve puls niet nauwkeurig meten
    //   maar vanwege de herhalende sequences zullen we toch regelmatig de juiste waarde meten
    // ************************************************************
    FETCH_Pulse_Plus_1 = micros() ;
//    while ( ( digitalRead ( PIN_RF_RX_DATA ) != Start_Level ) ) ;
    while ( ( digitalRead ( PIN_RF_RX_DATA ) != Start_Level ) && ( millis() < Start_Time ) ) ;
    
    // ************************************************************
    // Wacht tot het einde van de laag periode
    // ************************************************************
    LastPulse = micros() ;
//    while ( ( digitalRead ( PIN_RF_RX_DATA ) == Start_Level ) ) ;
    while ( ( digitalRead ( PIN_RF_RX_DATA ) == Start_Level ) && ( millis() < Start_Time ) ) ;
    PulseLength = micros() - LastPulse;

    // ************************************************************
    // Als de laag periode voldoende lang is, is hetde startpuls
    // Berg dan ook de positieve en negatieve startpuls op
    // ************************************************************
    if ( PulseLength > 5000 ) {
      RawSignal.Pulses [ RawCodeLength++ ] = LastPulse - FETCH_Pulse_Plus_1 ;
      RawSignal.Pulses [ RawCodeLength++ ] = PulseLength ;
      Started = true ;      
    }
    if ( millis() > Start_Time ) return false;
  }

  RawSignal.Min  = 10000 ;
  RawSignal.Max  = 0     ;
  RawSignal.Mean = 0     ;
  // ************************************************************
  // Na een start, wordt hier de hele serie pulsen gemeten
  // en er wordt gestopt als een (te) lange puls wordt gevonden (of het buffer vol is)
  // ************************************************************
  do {                                                                       
    // ************************************************************
    // Meet de breedte van het huidige nivo
    //   break als het nivo te lang duurt
    // ************************************************************
    numloops  = 0 ;
    LastPulse = micros () ;
    while ( ( digitalRead ( PIN_RF_RX_DATA ) == Start_Level ) ^ Toggle )
      if ( numloops++ == maxloops ) break ;
    PulseLength = micros() - LastPulse; 
    
    // ************************************************************
    // spring uit de loop als we een te korte puls detecteren
    // ************************************************************
    if ( PulseLength < MIN_PULSE_LENGTH ) break ;
    
    // ************************************************************
    // Inverteer het nivo waar naar we gaan zoeken
    // ************************************************************
    Toggle = !Toggle ;    
    
    // ************************************************************
    // Geldige puls gevonden, dus opbergen
    // ************************************************************
    RawSignal.Pulses [ RawCodeLength++ ] = PulseLength ;

    // ************************************************************
    // keep track of ststistics
    // ************************************************************
    if ( numloops < maxloops ) {
      if ( PulseLength < RawSignal.Min ) RawSignal.Min = PulseLength ;
      if ( PulseLength > RawSignal.Max ) RawSignal.Max = PulseLength ;
      RawSignal.Mean += PulseLength ;
    }
    
  // ************************************************************
  // stop als er een lange puls is gevonden of als het buffer vol is
  // ************************************************************
  } while ( ( RawCodeLength < RAW_BUFFER_SIZE ) && ( numloops < maxloops ) ) ;


  // ************************************************************
  // We hebben nu het einde van een signaal bereikt
  //   als we genoeg pulsen hebben,
  //     return true
  // ************************************************************
  if ( ( RawCodeLength >= MIN_RAW_PULSES ) && ( RawCodeLength <= MAX_RAW_PULSES ) ) {
    RawSignal.Mean = RawSignal.Mean / ( RawCodeLength - 3 ) ;
    RawSignal.Number   = RawCodeLength-1 ;            // Number of received pulse times (pulsen *2)
    RawSignal.Pulses [ RawSignal.Number + 1 ] = 0 ;   // Last element contains the timeout. 
    RawSignal.Time = millis() ;                       // Time the RF packet was received (to keep track of retransmits
    return true ;
  } 

  // ************************************************************
  // anders opnieuw beginnen
  // ************************************************************
  else {
    RawSignal.Number = 0 ;    
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
//  int x;
//  digitalWrite(PIN_RF_RX_VCC,LOW);                                        // Spanning naar de RF ontvanger uit om interferentie met de zender te voorkomen.
//  digitalWrite(PIN_RF_TX_VCC,HIGH);                                       // zet de 433Mhz zender aan
//  delayMicroseconds(TRANSMITTER_STABLE_DELAY);                            // short delay to let the transmitter become stable (Note: Aurel RTX MID needs 500µS/0,5ms)
//  
//  // LET OP: In de Arduino versie 1.0.1 zit een bug in de funktie delayMicroSeconds(). Als deze wordt aangeroepen met een nul dan zal er
//  // een pause optreden van 16 milliseconden. Omdat het laatste element van RawSignal af sluit met een nul (omdat de space van de stopbit 
//  // feitelijk niet bestaat) zal deze bug optreden. Daarom wordt deze op 1 gezet om de bug te omzeilen. 
//  RawSignal.Pulses[RawSignal.Number]=1;
//
//  for(byte y=0; y<RawSignal.Repeats; y++) {                               // herhaal verzenden RF code
//     x=1;
//     noInterrupts();
//     while(x<RawSignal.Number) {
//        digitalWrite(PIN_RF_TX_DATA,HIGH);
////??        delayMicroseconds(RawSignal.Pulses[x++]*RawSignal.Multiply-5);    // min een kleine correctie  
//        digitalWrite(PIN_RF_TX_DATA,LOW);
////??        delayMicroseconds(RawSignal.Pulses[x++]*RawSignal.Multiply-7);    // min een kleine correctie
//     }
//     interrupts();
//     if (y+1 < RawSignal.Repeats) delay(RawSignal.Delay);                 // Delay buiten het gebied waar de interrupts zijn uitgeschakeld! Anders werkt deze funktie niet.
//  }
//
//  delayMicroseconds(TRANSMITTER_STABLE_DELAY);                            // short delay to let the transmitter become stable (Note: Aurel RTX MID needs 500µS/0,5ms)
//  digitalWrite(PIN_RF_TX_VCC,LOW);                                        // zet de 433Mhz zender weer uit
//  digitalWrite(PIN_RF_RX_VCC,HIGH);                                       // Spanning naar de RF ontvanger weer aan.
//  RFLinkHW();
}
/*********************************************************************************************/

