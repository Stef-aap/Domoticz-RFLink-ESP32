
// ***********************************************************************************
// ***********************************************************************************
bool On_Off_Command () {
  byte kar = InputBuffer_Serial [ SerialInByteCounter - 3 ] ;
  return kar == 'N' || kar == 'n' ;
}

// ***********************************************************************************
// ***********************************************************************************
void Handle_Serial () {
    // *********************************************
  // If a complete line is received
  // *********************************************
  if ( Serial_Command ) {
    
    // *********************************************
    // 10;   // COMMAND
    // *********************************************
    if ( strncmp ( InputBuffer_Serial, "10;", 3 ) == 0 ) {  

      // *********************************************
      // LIST all Commands
      // *********************************************
      if ( strcasecmp ( InputBuffer_Serial+3, "LIST;" ) == 0 ) {
        sprintf ( InputBuffer_Serial, "20;%02X;LIST;", PKSequenceNumber++ ) ;
        Serial.println ( InputBuffer_Serial ) ; 
        Serial.println ( "10;LIST;         // list all commands"            );
        Serial.println ( "10;PING;         // return PONG"                  );
        Serial.println ( "10;REBOOT;       // reboot RFLink"                );
        Serial.println ( "10;VERSION;      // displays version information" );
        Serial.println ( "10;RFDEBUG=ON;   //" );
        Serial.println ( "10;RFUDENUG=ON;  // " );
        Serial.println ( "10;QRFDEBUG=ON;  // " );
        Serial.println ( "10;OPDRACHT ....;" );
        Serial.println ( "11;LEARN ....;" );
      }

      // *********************************************
      // PING
      // *********************************************
      else if ( strcasecmp ( InputBuffer_Serial+3, "PING;" ) == 0 ) {
        sprintf ( InputBuffer_Serial, "20;%02X;PONG;", PKSequenceNumber++ ) ;
        Serial.println ( InputBuffer_Serial ) ; 
      }

      // *********************************************
      // REBOOT
      // *********************************************
      else if ( strcasecmp ( InputBuffer_Serial+3, "REBOOT;" ) == 0 ) {
        ESP.restart () ;
      }

      // *********************************************
      // VERSION
      // *********************************************
      else if ( strcasecmp ( InputBuffer_Serial+3, "VERSION;" ) == 0 ) {
        sprintf ( InputBuffer_Serial, "20;%02X;VER=1.1;REV=28;BUILD=%03d;", PKSequenceNumber++, BUILDNR ) ;
        Serial.println ( InputBuffer_Serial ) ; 
      }

      // *********************************************
      // *********************************************
      else if ( strncasecmp ( InputBuffer_Serial+3, "RFDEBUG=O", 9 ) == 0 ) {
        if ( On_Off_Command() ) {
          RFDebug  = true ;
          RFUDebug = false ;
          QRFDebug = false ;
          sprintf ( InputBuffer_Serial, "20;%02X;RFDEBUG=ON;", PKSequenceNumber++ ) ;
        } else {
          RFDebug = false ; 
          sprintf ( InputBuffer_Serial, "20;%02X;RFDEBUG=OFF;", PKSequenceNumber++) ;
        }
        Serial.println ( InputBuffer_Serial ) ; 
      }

      // *********************************************
      // *********************************************
      else if ( strncasecmp ( InputBuffer_Serial+3, "RFUDEBUG=O", 10 ) == 0 ) {
        if ( On_Off_Command() ) {
          RFDebug  = false ;
          RFUDebug = true ;
          QRFDebug = false ;
          sprintf ( InputBuffer_Serial, "20;%02X;RFDEBUG=ON;", PKSequenceNumber++ ) ;
        } else {
          RFUDebug = false ; 
          sprintf ( InputBuffer_Serial, "20;%02X;RFDEBUG=OFF;", PKSequenceNumber++) ;
        }
        Serial.println ( InputBuffer_Serial ) ; 
      }

      // *********************************************
      // *********************************************
      else if ( strncasecmp ( InputBuffer_Serial+3, "QRFDEBUG=O", 10 ) == 0 ) {
        if ( On_Off_Command() ) {
          RFDebug  = false ;
          RFUDebug = false ;
          QRFDebug = true ;
          sprintf ( InputBuffer_Serial, "20;%02X;RFDEBUG=ON;", PKSequenceNumber++ ) ;
        } else {
          QRFDebug = false ; 
          sprintf ( InputBuffer_Serial, "20;%02X;RFDEBUG=OFF;", PKSequenceNumber++) ;
        }
        Serial.println ( InputBuffer_Serial ) ; 
      }


      // *********************************************
      // Unhandled Commands
      // 10;TRISTATEINVERT; => Toggle Tristate ON/OFF inversion
      // 10;RTSCLEAN; => Clean Rolling code table stored in internal EEPROM
      // 10;RTSRECCLEAN=9 => Clean Rolling code record number (value from 0 - 15)
      // 10;RTSSHOW; => Show Rolling code table stored in internal EEPROM (includes RTS settings)
      // 10;RTSINVERT; => Toggle RTS ON/OFF inversion
      // 10;RTSLONGTX; => Toggle RTS long transmit ON/OFF 
      //
      // 11;20;0B;NewKaku;ID=000005;SWITCH=2;CMD=ON; => 11; 
      //    is the required node info it can be followed by any custom data which will be echoed 
      // *********************************************


      
      // *********************************************
      // Handle Generic Commands / Translate protocol data into Nodo text commands 
      // *********************************************
      else {
        // check plugins
        if ( InputBuffer_Serial [ SerialInByteCounter - 1 ] == ';' ) { 
          InputBuffer_Serial [ SerialInByteCounter - 1 ] = 0 ;         // remove last ";" char
        }

//      if ( PluginTXCall ( 0, InputBuffer_Serial ) ) {
//        ValidCommand = 1 ;
//      } else {
//        // Answer that an invalid command was received?
//        ValidCommand=2;
//      }
        
      }
    
    }
    // *********************************************
    // *********************************************
    Serial_Command      = false ;  
    SerialInByteCounter = 0 ;  
  } 
  // *********************************************
  // Collect serial bytes until "\n" is detected
  // *********************************************
  //else if ( Serial.available () ) {
  else while ( Serial.available () && !Serial_Command ) {
    SerialInByte = Serial.read ();                

    // *********************************************
    // If "\n", line is complete
    // *********************************************
    if ( SerialInByte == '\n' ) {                                              // new line character
      InputBuffer_Serial [ SerialInByteCounter++ ] = 0 ;
      Serial_Command = true ;   
    }
    // *********************************************
    // otherwise collect as long as there's space in the buffer
    // *********************************************
    else if ( isprint ( SerialInByte ) ) {
      if ( SerialInByteCounter < ( INPUT_COMMAND_SIZE - 1 ) ) {
        InputBuffer_Serial [ SerialInByteCounter++ ] = SerialInByte ;
      }
      // *********************************************
      // otherwise clear the buffer and start all over again
      // *********************************************
      else {
        SerialInByteCounter = 0 ;  
      }
    }
  }

  
}

