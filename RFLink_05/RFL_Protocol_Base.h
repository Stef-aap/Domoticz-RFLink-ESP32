
#ifndef RFL_Protocol_Base_h
#define RFL_Protocol_Base_h

//#include <Arduino.h>
//#include <ArduinoJson.h>

// ***********************************************************************************
// ***********************************************************************************
class _RFL_Protocol_BaseClass {
  public:
    String Name = "Unknown" ;

    // **********************************************************************************************
    // By defining the method virtual,
    // The method in the derived class (if available) will take over the functionality
    // If no method with the same name is in the derived class, this virtual method will be effective
    // **********************************************************************************************
    virtual void setup () {
      if ( LEARNING_MODE > 0 ) {
        Serial.println ( "Active Protocol " + Name ) ;
      }
    }

    virtual void   loop () {
      // ************************************************************
      // Not allowed to do here a print statement
      // because it's possible that the sensor doesn't need a loop !!
      // ************************************************************
    }
    
    // ***********************************************************************
    // ***********************************************************************
    virtual bool Decode (  ) {
      Serial.print   ( "DECODE of _RFL_Protocol_Class is Missing" ) ;
	  return false ;
    }

    // ***********************************************************************
    // ***********************************************************************
    virtual void Print_Help () {
      Serial.println ( "Help information is missing for this library" ) ;
    }

};
#endif
