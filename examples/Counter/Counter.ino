/*
      RoboCore - Serial Display example
                    (25/05/2016)

  Written by François.
  
  Examples of functions in the Serial Display library.
  A single display is used as a counter from 1 to 10
  with 1 s interval.
  
*/

#include "SerialDisplay.h"

SerialDisplay displays(4,5); // (data, clock)
byte count = 0;

void setup(){
  Serial.begin(19200);
  displays.Off(0);
  displays.Info(&Serial);
}


void loop(){
  displays.Update();
  
  count++;
  switch(count){
      case 1:  displays.Set(SerialDisplay::CHAR_1);  break;
      case 2:  displays.Set(SerialDisplay::CHAR_2);  break;
      case 3:  displays.Set(SerialDisplay::CHAR_3);  break;
      case 4:  displays.Set(SerialDisplay::CHAR_4);  break;
      case 5:  displays.Set(SerialDisplay::CHAR_5);  break;
      case 6:  displays.Set(SerialDisplay::CHAR_6);  break;
      case 7:  displays.Set(SerialDisplay::CHAR_7);  break;
      case 8:  displays.Set(SerialDisplay::CHAR_8);  break;
      case 9:  displays.Set(SerialDisplay::CHAR_9);  break;
      case 10:  displays.Set(SerialDisplay::CHAR_0);  break;
      default:
        displays.Off(1);
        count = 0;
        break;
  }
  delay(1000);
}






