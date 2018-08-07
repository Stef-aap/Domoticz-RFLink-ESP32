// *********************************************************************************************
// *********************************************************************************************
#ifdef PLUGIN_016

int PulseCount = 50 ;
int PulseMid    =  600 / RAWSIGNAL_SAMPLE_RATE ;
int PulseMin    =  150 / RAWSIGNAL_SAMPLE_RATE ;
int PulseMax    = 1300 / RAWSIGNAL_SAMPLE_RATE ;

boolean Plugin_016(byte function, char *string) {
  // ****************************************************
  // Check the length of the sequence
  // ****************************************************
  if ( RawSignal.Number != PulseCount ) return false;

  // ****************************************************
  //  Translate the sequence in bit-values and 
  //     jump out this method if errors are detected
  // 0-bit = a short high followed by a long low
  // 1-bit = a long  high followed by a short low
  // ****************************************************
  unsigned long bitstream = 0L ;
  unsigned long P1 ;
  unsigned long P2 ;  
  for ( byte x=1; x < 48; x=x+2 ) {
    P1 = RawSignal.Pulses [ x ]  ;
	P2 = RawSignal.Pulses [ x+1 ]  ;
    if ( (  P1 < PulseMin ) || ( P1 > PulseMax ) ) return false;   // first pulse too short or too long
    if ( (  P2 < PulseMin ) || ( P2 > PulseMax ) ) return false;   // first pulse too short or too long

	if ( P1 > PulseMid ) {
	   bitstream = (bitstream << 1) | 0x1;    // append "1"
	} else { 
	   bitstream = bitstream << 1;           // append "0"
	}
  }
 
  // ****************************************************
  // Now we have a valid detection, send the Output command
  // ****************************************************
  #define Device "EV1527" 
  unsigned long Id = bitstream >> 4 ;
  unsigned long Switch = bitstream & 0xF ;
  sprintf ( pbuffer, "20;%02X;%s;ID=%05X;SWITCH=%02X;CMD=ON;", PKSequenceNumber++, Device, Id, Switch ) ; 
  Serial.println  (  pbuffer ) ;
  
  // ****************************************************
  // do some housekeeping
  // ****************************************************
  RawSignal.Repeats=true;                       // suppress repeats of the same RF packet
      RawSignal.Number=0;
      return true;
}

#endif
