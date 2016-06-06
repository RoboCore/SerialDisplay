/*
      RoboCore - Serial Display example
                    (25/05/2016)

  Written by François.
  
  Examples of functions in the Serial Display library.
  Use the serial monitor to control the displays.
  
*/

#include "SerialDisplay.h"

const int pinOE = 6;
SerialDisplay displays(4,5,4); // (data, clock)
int pwm = 0;

void setup(){
  Serial.begin(19200);
  displays.setBrightnessPin(pinOE);
  displays.Off(0);
  displays.Info(&Serial);
}


void loop(){
  displays.Update();
  
  if(Serial.available()){
    byte b = Serial.read();
    Serial.println((char)b);
    switch(b){
      case '0':  displays.Set(SerialDisplay::CHAR_0);  break;
      case '1':  displays.Set(SerialDisplay::CHAR_1);  break;
      case '2':  displays.Set(SerialDisplay::CHAR_2);  break;
      case '3':  displays.Set(SerialDisplay::CHAR_3);  break;
      case '4':  displays.Set(SerialDisplay::CHAR_4);  break;
      case '5':  displays.Set(SerialDisplay::CHAR_5);  break;
      case '6':  displays.Set(SerialDisplay::CHAR_6);  break;
      case '7':  displays.Set(SerialDisplay::CHAR_7);  break;
      case '8':  displays.Set(SerialDisplay::CHAR_8);  break;
      case '9':  displays.Set(SerialDisplay::CHAR_9);  break;
      
      case 'A':  displays.Set(SerialDisplay::CHAR_A);  break;
      case 'B':  displays.Set(SerialDisplay::CHAR_B);  break;
      case 'C':  displays.Set(SerialDisplay::CHAR_C);  break;
      case 'D':  displays.Set(SerialDisplay::CHAR_D);  break;
      case 'E':  displays.Set(SerialDisplay::CHAR_E);  break;
      case 'F':  displays.Set(SerialDisplay::CHAR_F);  break;
      
//      case '-':  displays.Set(SerialDisplay::CHAR_SEPARATOR);  break;
      
      case 'i':  displays.Info(&Serial);  break;
      case 'o':  displays.On(0);  break;
      case 'O':  displays.Off(0);  break;
      case '.':  displays.Dot();  break;
      case ',':  displays.noDot();  break;
      case 'u':  displays.Invert(SERIAL_DISPLAY_INVERT_CHAR);  break;
      case '[':  displays.Cascade(SERIAL_DISPLAY_CASCADE_UP, 300);  break;
      case ']':  displays.Cascade(SERIAL_DISPLAY_CASCADE_DOWN, 300);  break;
      case '*':  displays.Blink(1000);  break;
      
      case 's':{
        byte array[] = { SerialDisplay::CHAR_1 , SerialDisplay::CHAR_2 , SerialDisplay::CHAR_3 , SerialDisplay::CHAR_4 };
        displays.Scroll(array, 4, 1000);
        break;
      }
      
      case '+':{
        pwm += 10;
        displays.Brightness(pwm);
        Serial.print("pwm:");
        Serial.println(pwm);
        break;
      }
      case '-':{
        pwm -= 10;
        displays.Brightness(pwm);
        Serial.print("pwm:");
        Serial.println(pwm);
        break;
      }
    }
  }
  
}






