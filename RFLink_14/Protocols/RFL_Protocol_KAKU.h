
#ifndef RFL_Protocol_KAKU_h
#define RFL_Protocol_KAKU_h

// ***********************************************************************************
// ***********************************************************************************
class _RFL_Protocol_KAKU : public _RFL_Protocol_BaseClass {
	
  public:

    // ***********************************************************************
    // Creator, 
    // ***********************************************************************
    _RFL_Protocol_KAKU () {
      //Name = "KAKU" ;
      Name = "NewKaku" ;
    }
 
    // ***********************************************************************
    // ***********************************************************************
    bool Decode (  ) {
	  
//#define NewKAKU_RawSignalLength      132        // regular KAKU packet length
//#define NewKAKUdim_RawSignalLength   148        // KAKU packet length including DIM bits
#define NewKAKU_RawSignalLength      133        // regular KAKU packet length
#define NewKAKUdim_RawSignalLength   149        // KAKU packet length including DIM bits

#define NewKAKU_1T                   225        // 275        // us
#define NewKAKU_mT                   650 // us, approx. in between 1T and 4T 
#define NewKAKU_4T                  1225        // 1100       // us
#define NewKAKU_8T                  2600        // 2200       // us, Duration of the space after the start bit

      // nieuwe KAKU bestaat altijd uit start bit + 32 bits + evt 4 dim bits. Ongelijk, dan geen NewKAKU

      if ( (RawSignal.Number != NewKAKU_RawSignalLength) && (RawSignal.Number != NewKAKUdim_RawSignalLength) ) return false;
//Serial.print ( "K" ) ;
      boolean Bit;
      int i;
      //int P0,P1,P2,P3;
//      byte P0,P1,P2,P3;
int P0,P1,P2,P3;
      byte dim=0;
      unsigned long bitstream=0L;
      
      // RawSignal.Number bevat aantal pulsRawSignal.Multiplyen * 2  => negeren
      // RawSignal.Pulses[1] bevat startbit met tijdsduur van 1T => negeren
      // RawSignal.Pulses[2] bevat lange space na startbit met tijdsduur van 8T => negeren
//      i=3; // RawSignal.Pulses[3] is de eerste van een T,xT,T,xT combinatie
i=4; // RawSignal.Pulses[3] is de eerste van een T,xT,T,xT combinatie
      //if ( RawSignal.Number==(NewKAKU_RawSignalLength-2) || RawSignal.Number==(NewKAKUdim_RawSignalLength-2) ) i=1;
      do {
          P0=RawSignal.Pulses[i]  ; // * RawSignal.Multiply;
          P1=RawSignal.Pulses[i+1]; // * RawSignal.Multiply;
          P2=RawSignal.Pulses[i+2]; // * RawSignal.Multiply;
          P3=RawSignal.Pulses[i+3]; // * RawSignal.Multiply;
          
          if (P0<NewKAKU_mT && P1<NewKAKU_mT && P2<NewKAKU_mT && P3>NewKAKU_mT) { 
              Bit=0; // T,T,T,4T
              //Serial.print("0");
          } else 
          if (P0<NewKAKU_mT && P1>NewKAKU_mT && P2<NewKAKU_mT && P3<NewKAKU_mT) {
              Bit=1; // T,4T,T,T
              //Serial.print("1");
          } else 
          if (P0<NewKAKU_mT && P1<NewKAKU_mT && P2<NewKAKU_mT && P3<NewKAKU_mT) {      // T,T,T,T Deze hoort te zitten op i=111 want: 27e NewKAKU bit maal 4 plus 2 posities voor startbit
/* Serial.print( " P");
Serial.print(i);
Serial.print( " ");
Serial.print(P0);
Serial.print( " ");
Serial.print(P1);
Serial.print( " ");
Serial.print(P2);
Serial.print( " ");
Serial.print(P3);
 */
             if(RawSignal.Number!=NewKAKUdim_RawSignalLength) {    // als de dim-bits er niet zijn
               //Serial.println("No dim bits");
               return false;
             }
          } else {
            //Serial.println("Unknown pattern");
//Serial.print( "O");
            return false;                                          // andere mogelijkheden zijn niet geldig in NewKAKU signaal.  
          }
          
//          if(i<130) {                                              // alle bits die tot de 32-bit pulstrein behoren 32bits * 4posities per bit + pulse/space voor startbit
if(i<131) {                                              // alle bits die tot de 32-bit pulstrein behoren 32bits * 4posities per bit + pulse/space voor startbit
            bitstream=(bitstream<<1) | Bit;
          } else {                                                 // de resterende vier bits die tot het dimlevel behoren 
            dim=(dim<<1) | Bit;
          }       
          i+=4;                                                    // volgende pulsenquartet
      } while(i<RawSignal.Number-2);                             //-2 omdat de space/pulse van de stopbit geen deel meer van signaal uit maakt.
      //==================================================================================
      // Prevent repeating signals from showing up
      //==================================================================================
      if(SignalHash!=SignalHashPrevious || ( millis() > 700 + Last_Detection_Time ) || SignalCRC != bitstream ) { // 1000
         // not seen the RF packet recently
         SignalCRC=bitstream;
      } else {
         // already seen the RF packet recently
         //Serial.println("Skip");
//Serial.print( "R");
         return true;
      } 
/*       //==================================================================================
      // Output
      // ----------------------------------
      sprintf(pbuffer, "20;%02X;", PKSequenceNumber++);    // Node and packet number 
      Serial.print( pbuffer );
      // ----------------------------------
      //Serial.print(F("NewKaku;"));                         // Label
      Serial.print(F("NewKaku;"));                         // Label
      sprintf(pbuffer, "ID=%08lx;",((bitstream) >> 6) );   // ID   
      Serial.print( pbuffer );
      sprintf(pbuffer, "SWITCH=%x;", ((bitstream)&0x0f)+1 );
      Serial.print( pbuffer );
      Serial.print(F("CMD="));                    

      int command = (bitstream >> 4) & 0x03;
      if (command > 1) command ++;
      if (i>140) {                                         // Command and Dim part
          sprintf(pbuffer, "SET_LEVEL=%d;", dim );     
          Serial.print( pbuffer );
      } else {
          if ( command == 0 ) Serial.print(F("OFF;"));
          if ( command == 1 ) Serial.print(F("ON;"));
          if ( command == 3 ) Serial.print(F("ALLOFF;"));
          if ( command == 4 ) Serial.print(F("ALLON;"));
      }
      Serial.println();
 */      
      String On_Off = "OFF" ;
      if ( ( bitstream & 0x10 ) != 0 ) On_Off = "ON" ; 
      int Switch = bitstream & 0x0F ;
      unsigned long Id = bitstream >> 5 ;

      //sprintf ( pbuffer, "20;%02X;%s;ID=%05X;SWITCH=%0X;CMD=%s;", PKSequenceNumber++, Name.c_str(), Id, Switch, On_Off.c_str() ) ; 
      //Serial.println ( pbuffer ) ;

      sprintf ( pbuffer, "%s;ID=%05X;", Name.c_str(), Id ) ; 
      if ( Unknown_Device ( pbuffer ) ) return false ;
/*       if ( ( Learning_Mode == 0 ) && ( RFLink_File.Known_Devices.indexOf ( pbuffer ) < 0 ) ) {
//      if ( ( Learning_Mode == 0 ) && ( Known_Devices.indexOf ( pbuffer ) < 0 ) ) {
          Serial.print   ( "Unknown Device: 12;" ) ;
Serial.println ( pbuffer ) ;
        return false ;
      }  
 */      
      Serial.print   ( PreFix ) ;
      Serial.print   ( pbuffer ) ;
      sprintf ( pbuffer2, "SWITCH=%0X;CMD=%s;", Switch, On_Off.c_str() ) ; 
      Serial.println ( pbuffer2 ) ;
      
      
      
//Serial.println ( bitstream, HEX ) ;
      
      // ----------------------------------
      RawSignal.Number=0;
      return true;
	  
    }

    // ***********************************************************************************
    // ***********************************************************************************
    bool Home_Command ( String Device, unsigned long ID, int Switch, String On ) {
      if ( Device !=  Name ) return false ;

      unsigned long Data  = ( ID << 5 ) | Switch ;
      if ( On == "ON" ) Data = Data | 0x10 ;
      int           NData = 32 ;
      unsigned long Mask ;
      bool          Zero ;
      int           uSec  = 260 ; 
      
      // ************************************
      // send the sequence a number of times
      // ************************************
      for ( int R=0; R<9; R++ ) {
        // *************************
        // start at the most significant bit position
        // *************************
        Mask = 0x01 << ( NData -1 ) ;

        // *************************
        // Start Bit = T, 10*T
        // *************************
        digitalWrite ( TRANSMIT_PIN, HIGH ) ;
        delayMicroseconds (      uSec ) ;
        digitalWrite ( TRANSMIT_PIN, LOW ) ;
        delayMicroseconds ( 10 * uSec ) ;
        
        // *************************
        // 32 information bits
        // *************************
        #define NT 4     // 3,4,5 all work correctly
        for ( int i=0; i<NData; i++ ) {
          Zero = ( Data & Mask ) == 0 ;
          if ( Zero ) {                             // T, T, T, 4*T
            digitalWrite      ( TRANSMIT_PIN, HIGH ) ;
            delayMicroseconds (      uSec          ) ;
            digitalWrite      ( TRANSMIT_PIN, LOW  ) ;
            delayMicroseconds (      uSec          ) ;
            digitalWrite      ( TRANSMIT_PIN, HIGH ) ;
            delayMicroseconds (      uSec          ) ;
            digitalWrite      ( TRANSMIT_PIN, LOW  ) ;
            delayMicroseconds ( NT * uSec          ) ;
          } else {                                  // T, 4*T, T, T
            digitalWrite      ( TRANSMIT_PIN, HIGH ) ;
            delayMicroseconds (      uSec          ) ;
            digitalWrite      ( TRANSMIT_PIN, LOW  ) ;
            delayMicroseconds ( NT * uSec          ) ;
            digitalWrite      ( TRANSMIT_PIN, HIGH ) ;
            delayMicroseconds (      uSec          ) ;
            digitalWrite      ( TRANSMIT_PIN, LOW  ) ;
            delayMicroseconds (      uSec          ) ;
          }
          // *************************
          // go to the next bit
          // *************************
          Mask = Mask >> 1 ;
        }
        // *************************
        // Stop Bit = T, 39*T
        // *************************
        digitalWrite ( TRANSMIT_PIN, HIGH ) ;
        delayMicroseconds (      uSec ) ;
        digitalWrite ( TRANSMIT_PIN, LOW ) ;
        delayMicroseconds ( 39 * uSec ) ;
      }
      return true ;
    }
    
    
};
#endif
