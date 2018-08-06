
// Version 0.1

#ifndef RFL_Protocols_h
#define RFL_Protocols_h   0.1

#include <vector>


// ****************************************************************************
// Here all available Protocols are included
//   and the base class, from which all Protocols should be derived.
// ****************************************************************************
#include "RFL_Protocol_Base.h"
#include "RFL_Protocol_Start.h"
#include "RFL_Protocol_Finish.h"

#include "RFL_Protocol_EV1527.h"
#include "RFL_Protocol_KAKU.h"


// *************************************************************************
// *************************************************************************
class _RFL_Protocols {
  public :  

    // ***********************************************************************
    // ***********************************************************************
    _RFL_Protocols (){
//   RFL_Protocols.
Add ( new _RFL_Protocol_Start () ) ;  
   }

    // ***********************************************************************
    // ***********************************************************************
    void Add ( _RFL_Protocol_BaseClass* RFL_Protocol ) {
      //Serial.println ( "********************* Add RFL_Protocol &&&&&&&&&&&&&&&&&" ) ;
      //Serial.println ( _RFL_Protocol_List.size() ) ;
      _RFL_Protocol_List.push_back ( RFL_Protocol ) ;
      //Serial.println ( _RFL_Protocol_List.size() ) ;
    }

    // ***********************************************************************
    // ***********************************************************************
    void setup () {
      //RFL_Protocols.
	  Add ( new _RFL_Protocol_Finish () ) ;  
		
      for ( auto RFL_Protocol:_RFL_Protocol_List ){
        RFL_Protocol->setup () ;      
      }
    }

    // ***********************************************************************
    // ***********************************************************************
    void loop () {
      for ( auto RFL_Protocol:_RFL_Protocol_List ){
        RFL_Protocol->loop () ;   
      }
    }

    // ***********************************************************************
    // ***********************************************************************
    void Print () {
      Serial.println ( "=======================  My_Numbers  =======================" ) ;
      for ( auto RFL_Protocol:_RFL_Protocol_List ){
        Serial.println ( "Something Else" );
      }
    }

    // ***********************************************************************
    // ***********************************************************************
    boolean Decode () {
      if ( LEARNING_MODE == 0 ) {
        for ( auto RFL_Protocol:_RFL_Protocol_List ){
          if ( RFL_Protocol->Decode () ) {
            return true ;
          }
        }  
        return false ;
      } 
      else if ( ( LEARNING_MODE == 1 ) || (LEARNING_MODE == 2 ) ){
        int Found = 0 ;
        int S_Len = 0 ;
              S_Len = RawSignal.Number ;
        for ( auto RFL_Protocol:_RFL_Protocol_List ){
          if ( RFL_Protocol->Decode () ) {
            if ( RFL_Protocol->Name != "Start" ) {
            }
            if ( ( RFL_Protocol->Name != "Start" ) && ( RFL_Protocol->Name != "Finish" ) ) {
              Serial.print   ( "LM=" ) ;
              Serial.print   ( LEARNING_MODE ) ;      
              Serial.print   ( "    Protocol=" ) ;
              Serial.print   ( RFL_Protocol->Name ) ;      
              Serial.print   ( "    Len=" ) ;
              Serial.println ( S_Len ) ;
              Found += 1 ;
            }
            if ( LEARNING_MODE == 1 ) {
              return true;
            }
            RawSignal.Number = S_Len ;  // is cleared after a valid detection
          }
        }  
        if ( Found > 0 ) {
          Serial.println ( "--------") ;
          return true ;
        }
        return false ;
      }
    }

    // ***********************************************************************
    // ***********************************************************************
    private :
      std::vector <_RFL_Protocol_BaseClass*> _RFL_Protocol_List ;
};

// ********************************************************************************
// Create Sensor and Protocol list
// ********************************************************************************
_RFL_Protocols RFL_Protocols ;

#endif

