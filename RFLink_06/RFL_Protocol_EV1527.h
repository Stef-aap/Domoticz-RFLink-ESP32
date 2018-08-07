
#ifndef RFL_Protocol_EV1527_h
#define RFL_Protocol_EV1527_h

//#include <Arduino.h>

#include "Plugins/NEW_Plugin_016.h"

// ***********************************************************************************
// ***********************************************************************************
class _RFL_Protocol_EV1527 : public _RFL_Protocol_BaseClass {
	
  public:

    // ***********************************************************************
    // Creator, 
    // ***********************************************************************
    _RFL_Protocol_EV1527 () {
      Name = "EV1527" ;
    }
 
    // ***********************************************************************
    // ***********************************************************************
    bool Decode (  ) {
      //Serial.print   ( "DECODE of " + _My_Name ) ;
	  bool Result = NEW_Plugin_016 ( 0, 0 ) ;
	  //Serial.println( "LLLL\nPPPP" ) ;
	  return Result ;
	  //return true ;
    }

};
#endif
