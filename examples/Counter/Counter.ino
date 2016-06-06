/*
      RoboCore - Serial Display example
                    (06/06/2016)

  Written by François.
  
  Examples of functions in the Serial Display library.
  A single display is used as a counter from 1 to 10
  with 1 s interval.

*/

#include <SerialDisplay.h>

SerialDisplay displays(4,5,1); // (data, clock, number of modules)
byte count = 0;

void setup(){
  Serial.begin(19200);
  displays.Off(0);
  displays.Info(&Serial);
}


void loop(){
  if(count < 10){
    displays.Print(count);
    count++;
  } else {
    displays.Off(1);
    count = 0;
  }
  delay(1000);
}






