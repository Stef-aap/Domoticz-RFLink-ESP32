
// Version 0.1

#ifndef RFLink_File_h
#define RFLink_File_h   0.1

#include "FS.h"
#include "SPIFFS.h"


// ***********************************************************************************
// typical usage:
//
//String Filename = "/RFLink.txt" ;
//_RFLink_File  RFLink_File ( Filename ) ;
//
//void setup(){
//  Serial.begin(115200);
//  delay ( 500 ) ;
//
//  RFLink_File.Begin () ;
//  //RFLink_File.Clear () ;
//  Serial.println ( "1:" + RFLink_File.Known_Devices ) ;
//  RFLink_File.Add ( "Device;ID=55AAP;" ) ;
//  Serial.println ( "2:" + RFLink_File.Known_Devices ) ;
// ***********************************************************************************
class _RFLink_File {
  public:
    String Known_Devices = "" ;

    // **************************************************  
    // STRANGE, call without a Filename is not accepted by the compiler  
    // **************************************************    
    _RFLink_File ( String Filename = "/RFLink.txt" ) {
      _Filename = Filename ;
    }
  
    // **************************************************    
    // **************************************************    
    bool Begin () {
      if ( !SPIFFS.begin() ) {
        // ********************************
        // if begin failed, format the disk
        // ********************************
        SPIFFS.format () ;
      }
      _Read () ;
    }
    
    // **************************************************    
    // **************************************************    
    void Dir () {
      String Path = "/" ;
      Serial.println ( "Files in SPIFFS" ) ;

      File root = SPIFFS.open ( Path ) ;
      if ( !root ){
        Serial.println("- failed to open SPIFFS");
        return;
      }

      File file = root.openNextFile () ;
      while ( file ) {
        Serial.print("  FILE: ");
        Serial.print(file.name());
        Serial.print("\tSIZE: ");
        Serial.println(file.size());
        file = root.openNextFile();
      }
    }
    
    // **************************************************    
    // **************************************************    
    void Dump ( String Filename ) {
      Serial.printf ( "Contents of %s\n", Filename.c_str() ) ;
      File file = SPIFFS.open ( Filename, FILE_READ ) ;
      if ( file ) {
        while ( file.available() ){
          Serial.print ( file.readString() ) ;
        }
        file.close () ;
        Serial.println () ;
      }
    }

    // **************************************************    
    // **************************************************    
    bool Delete ( String Filename ) {
      Serial.printf ( "Deleting file: %s  ", Filename.c_str() );
      if ( SPIFFS.remove ( Filename ) ) {
        Serial.println ( "- file deleted" ) ;
        return true ;
      } 
      else {
        Serial.println ( "- delete failed" ) ;
        return false ;
      }
    }
    
    // **************************************************    
    // **************************************************    
    bool Add ( String Line ) {
      if ( Known_Devices.indexOf ( Line ) < 0 ) {
        Known_Devices += Line ;
        _Write ( Known_Devices ) ;
      }
    }

    // **************************************************    
    // **************************************************    
    bool Remove ( String Line ) {
      int pos = Known_Devices.indexOf ( Line ) ;
      if ( pos >= 0 ) {
        Known_Devices = Known_Devices.substring ( 0, pos ) + Known_Devices.substring ( pos + Line.length() ) ;
        _Write ( Known_Devices ) ;
      }
    }

    // **************************************************    
    // **************************************************    
    bool Clear () {
      SPIFFS.remove ( _Filename );
      Known_Devices = "" ;
    }

    // **************************************************    
    // **************************************************    
    void Print () {
      Serial.println ( "=====  Known Devices =====" ) ;
      int x0 = 0 ;
      while ( x0 < Known_Devices.length() )  {
        int x1 = Known_Devices.indexOf ( ";", x0+1 ) ; 
        int x2 = Known_Devices.indexOf ( ";", x1+1 ) ; 
        if (( x2 < 0 ) || ( x2 < 0 )) return ;
        Serial.println ( "    " + Known_Devices.substring ( x0, x2+1 ) ); 
        x0 = x2+1 ;
      }  
    }

  // **************************************************    
  // **************************************************    
  private:
    String _Filename ;  

    // **************************************************    
    // **************************************************    
    bool _Write ( String Line ) {
      File file = SPIFFS.open ( _Filename, FILE_WRITE ) ;
      file.print ( Line ) ;
      file.close () ;
      return true;
    }

    // **************************************************    
    // **************************************************    
    void _Read () {
      File file = SPIFFS.open ( _Filename, FILE_READ ) ;
      Known_Devices = "" ;
      if ( file ) {
        while ( file.available() ){
          Known_Devices += file.readString() ;
        }
        file.close () ;
      }
    }

};

#endif
