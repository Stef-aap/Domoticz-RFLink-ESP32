
#ifndef RFL_Protocol_Base_h
#define RFL_Protocol_Base_h

// ***********************************************************************************
// ***********************************************************************************
class _RFL_Protocol_BaseClass {
  public:
    String Name = "Unknown" ;
    int    ID   = 0         ;

    // **********************************************************************************************
    // By defining the method virtual,
    // The method in the derived class (if available) will take over the functionality
    // If no method with the same name is in the derived class, this virtual method will be effective
    //
    // if derived class overrides this method, it should call theis base:
    //   _RFL_Protocol_BaseClass::setup() ;
    // **********************************************************************************************
    virtual void setup () {
      if ( Learning_Mode > 0 ) {
        Serial.print   ( "Active Protocol " + Name ) ;
        Serial.print   ( "    ID = " ) ;
        Serial.println ( ID ) ;
      }
    }

    virtual void loop () {
      // ************************************************************
      // Not allowed to do here a print statement
      // because it's possible that the sensor doesn't need a loop !!
      // ************************************************************
    }
    
    // ***********************************************************************
    // ***********************************************************************
    virtual bool Decode () {
      Serial.print   ( "DECODE of _RFL_Protocol_Class is Missing " + Name ) ;
	    return false ;
    }

    // ***********************************************************************
    // ***********************************************************************
    virtual bool Home_Command ( String Device, unsigned long ID, int Switch, String On ) {
      return false ;
    } 

    // ***********************************************************************
    // ***********************************************************************
    virtual void Print_Help () {
      Serial.println ( "Help information is missing for this library " + Name ) ;
    }

};
#endif
