
/*
     RoboCore Serial Display Library
        (v1.1 - 05/04/2018)

  Serial Display functions for Arduino
    (tested with Arduino 1.0.1)

  Copyright 2018 RoboCore ( http://www.RoboCore.net )
    - François;
  
  ------------------------------------------------------------------------------
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
  ------------------------------------------------------------------------------
  
*/

#include "SerialDisplay.h"

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------

// Constructor
SerialDisplay::SerialDisplay(int pinData, int pinClock, byte qty){
  // set & configure pins
  _pinData = pinData;
  _pinClock = pinClock;
  _pinOE = -1; // default
  pinMode(_pinData, OUTPUT);
  pinMode(_pinClock, OUTPUT);
  digitalWrite(_pinData, LOW);
  digitalWrite(_pinClock, LOW);
  
  // check and set display qty
  _display_qty = qty;
  if(qty > SERIAL_DISPLAY_MAX_DISPLAYS)
    _display_qty = SERIAL_DISPLAY_MAX_DISPLAYS;
#ifndef ENABLE_INFINITE_STREAM
  if(qty == 0)
    _display_qty = 1;
#endif
  
  // set default values
  _tosend = false;
  _inverted = 0;
  _blink_next = 0;
  for(int i=0 ; i < SERIAL_DISPLAY_MAX_DISPLAYS ; i++){
    _data[i] = 0;
    _state[i] = SERIAL_DISPLAY_OFF;
  }
}

// ----------------------------------------------------------------------------------------------------

// Blink a display (1-based)
//  (returns false if invalid parameter)
//  NOTE: set display to 0 to blink all displays
boolean SerialDisplay::Blink(word interval, byte display){
  // check index
  if(display > _display_qty)
    return false;
  
  // check if turn OFF
  if(interval == 0){
    if(display == 0){
      for(int i=0 ; i < _display_qty ; i++)
        _state[i] &= ~SERIAL_DISPLAY_BLINK; // reset
    } else {
      _state[display - 1] &= ~SERIAL_DISPLAY_BLINK; // reset
    }
    
    _blink_next = 0; // reset
  } else {
    // turn ON
    if(display == 0){
      for(int i=0 ; i < _display_qty ; i++)
        _state[i] |= SERIAL_DISPLAY_BLINK; // reset
    } else {
      _state[display - 1] |= SERIAL_DISPLAY_BLINK; // reset
    }
    
    _blink_interval = interval; // set
    _blink_next = millis() + _blink_interval; // update
  }
  
  return true;
}

// ----------------------------------------------------------------------------------------------------

// Set the brightness of the displays
void SerialDisplay::Brightness(int value){
  value = map(value, 0, 255, 255, 0); // convert (OE is active LOW)
  
  // check limits
  if(value < 0)
    value = 0;
  if(value > 255)
    value = 255;
  
  _brightness = value; // set
  
  if((_pinOE > 0) && (_brightness == 0)){
    digitalWrite(_pinOE, LOW);
  } else if((_pinOE > 0) && (_brightness == 255)){
    digitalWrite(_pinOE, HIGH);
  } else {
    analogWrite(_pinOE, _brightness);
  }
}

// ----------------------------------------------------------------------------------------------------

// Cascade the characters
void SerialDisplay::Cascade(byte type, word interval){
  byte temp[SERIAL_DISPLAY_MAX_DISPLAYS];
  
  // copy the data
  for(int i=0 ; i < _display_qty ; i++)
    temp[i] = _data[i];
  
  // cascade
  byte mask = 0;
  for(int i=0 ; i < 6 ; i++){
    switch(i){
      case 0:  break; // do nothing, mask is empty
      case 1:  mask |= (type == SERIAL_DISPLAY_CASCADE_UP) ? (PIN_D | PIN_P) : PIN_A;            break;
      case 2:  mask |= (type == SERIAL_DISPLAY_CASCADE_UP) ? (PIN_C | PIN_E) : (PIN_B | PIN_F);  break;
      case 3:  mask |= PIN_G;                                                                    break;
      case 4:  mask |= (type == SERIAL_DISPLAY_CASCADE_UP) ? (PIN_B | PIN_F) : (PIN_C | PIN_E);  break;
      case 5:  mask |= (type == SERIAL_DISPLAY_CASCADE_UP) ? PIN_A : (PIN_D | PIN_P);            break;
    }
    
    for(int d=0 ; d < _display_qty ; d++){
      _data[d] = temp[d] & mask;
    }
    
    Send();
    
    delay(interval);
  }
}

// ----------------------------------------------------------------------------------------------------

// Turn the dot ON
//  (returns false if invalid parameters)
boolean SerialDisplay::Dot(byte display){
  // check index
  if((display == 0) || (display > _display_qty))
    return false;
  
  _data[display - 1] |= PIN_P;
  _state[display - 1] |= SERIAL_DISPLAY_ON;
  _tosend = true; // set
  Send();
  
  return true;
}

// ----------------------------------------------------------------------------------------------------

// Get the state of the relay (1-based)
byte SerialDisplay::GetData(byte display){
  // check index
  if((display == 0) || (display > _display_qty))
    return SERIAL_DISPLAY_NONE;
  
  return _data[display - 1];
}

// ----------------------------------------------------------------------------------------------------

// Get the state of the relay (1-based)
byte SerialDisplay::GetState(byte display){
  // check index
  if((display == 0) || (display > _display_qty))
    return SERIAL_DISPLAY_NONE;
  
  return _state[display - 1];
}

// ----------------------------------------------------------------------------------------------------

#ifdef SERIAL_DISPLAY_DEBUG
// Print all information
void SerialDisplay::Info(HardwareSerial *stream, byte format){
  stream->print("Qty:");
  stream->println(_display_qty);
  for(int i=0 ; i < _display_qty ; i++){
    stream->print('[');
    stream->print(i+1);
    stream->print("] - ");
    stream->print("D:");
    if(format == HEX)
      stream->print("0x");
    else if(format == BIN)
      stream->print("0b");
    else if(format == OCT)
      stream->print('0');
    stream->print(_data[i], format);
    stream->print(" : S:0x");
    stream->println(_state[i], HEX);
  }
}
#endif

// ----------------------------------------------------------------------------------------------------

// Invert the displays (modules and/or characters)
void SerialDisplay::Invert(byte type){
  // check if an update is needed
  if(type != _inverted)
    _tosend = true;
  
  _inverted = type;
}

// ----------------------------------------------------------------------------------------------------

// Turn the dot OFF
//  (returns false if invalid parameters)
boolean SerialDisplay::noDot(byte display){
  // check index
  if((display == 0) || (display > _display_qty))
    return false;
  
  _data[display - 1] &= ~PIN_P;
  _tosend = true; // set
  Send();
  
  return true;
}

// ----------------------------------------------------------------------------------------------------

// Turn a display OFF
//  (returns false if invalid parameter)
//  NOTE: set display to 0 to turn off all displays
boolean SerialDisplay::Off(byte display){
  // check index
  if(display > _display_qty)
    return false;
  
  if(display == 0){
    for(int i=0 ; i < _display_qty ; i++)
      _state[i] = SERIAL_DISPLAY_OFF;
  } else {
    _state[display - 1] = SERIAL_DISPLAY_OFF;
  }
  _tosend = true; // set
  Send();
  
  return true;
}

// ----------------------------------------------------------------------------------------------------

// Turn a display ON
//  (returns false if invalid parameter)
//  NOTE: set display to 0 to turn on all displays
boolean SerialDisplay::On(byte display){
  // check index
  if(display > _display_qty)
    return false;
  
  if(display == 0){
    for(int i=0 ; i < _display_qty ; i++)
      _state[i] = SERIAL_DISPLAY_ON;
  } else {
    _state[display - 1] = SERIAL_DISPLAY_ON;
  }
  _tosend = true; // set
  Send();
  
  return true;
}

// ----------------------------------------------------------------------------------------------------

// Print a value (unsigned long)
//  (returns false if value is negative)
boolean SerialDisplay::Print(int value){
  if(value < 0)
    return false;
  
  return Print((unsigned long)value);
}

// ---------------------------------------

// Print a value (unsigned long)
//  (returns true)
boolean SerialDisplay::Print(word value){
  return Print((unsigned long)value);
}

// ---------------------------------------

// Print a value (unsigned long)
//  (returns true)
boolean SerialDisplay::Print(unsigned long value){
  byte toprint;
  for(int i = _display_qty ; i > 0 ; i--){
    if((value == 0) & (i < _display_qty)){
      // turn off the displays with leading zeroes
      // (must print if given value is 0
      _data[i-1] &= PIN_P; // reset, but keep the dot on
      // no need to change the state, because the dot might be on and the data is reset
    } else {
      toprint = value % 10;
      Print(toprint,i,false);
      value /= 10; // update
    }
  }
  InvertDisplays();
  Send();
  
  return true;
}

// ----------------------------------------------------------------------------------------------------

// Print a character on a display (1-based)
//  (returns true if valid character)
boolean SerialDisplay::Print(char c, byte display, boolean send){
  byte toprint = toByteMask(c);
  
  // check if valid value
  if(toprint != 0){
    toprint |= (_data[display - 1] & PIN_P); // add the dot if necessary
    Set(toprint, display, send);
    return true;
  }
  
  return false;
}

// ----------------------------------------------------------------------------------------------------

// Scroll an array of characters
//  (returns false if invalid parameters)
boolean SerialDisplay::Scroll(byte *array, byte array_length, word interval){
  // check parameters
  if((array_length == 0) || (interval == 0))
    return false;
  
  // reset all displays
  for(int i=0 ; i < _display_qty ; i++){
    _data[i] = 0;
    _state[i] = SERIAL_DISPLAY_ON;
  }
  
  byte length = array_length + _display_qty;
  byte index = 0;
  for(int i=0 ; i < length ; i++){
    // shift left
    for(int d=0 ; d < (_display_qty - 1) ; d++){
      _data[d] = _data[d+1];
    }
    
    // shift array
    if(index < array_length){
      _data[_display_qty - 1] = array[index];
      index++; // update
    } else {
      _data[_display_qty - 1] = 0; // reset
    }
    
    Send(); // send the data
    
    delay(interval); // wait
  }
  
  return true;
}

// ----------------------------------------------------------------------------------------------------

// Set a display (1-based)
//  (returns false if invalid parameters)
//  NOTE: set send to FALSE to manually write to the shift register
boolean SerialDisplay::Set(byte mask, byte display, boolean send){
  // check index
  if((display == 0) || (display > _display_qty))
    return false;
  
  _data[display - 1] = mask;
  _state[display - 1] |= SERIAL_DISPLAY_ON;
  _tosend = true; // set
  
  InvertChar(display - 1);
  
  if(send)
    Send();
  
  return true;
}

// ----------------------------------------------------------------------------------------------------

// Set the brightness pin of the displays
//  (returns false if invalid parameter)
boolean SerialDisplay::setBrightnessPin(int pin){
  // check pin
  if(pin < 0)
    return false;
  
  _pinOE = pin;
  pinMode(_pinOE, OUTPUT);
  Brightness(255); // set to maximum
  
  return true;
}

// ----------------------------------------------------------------------------------------------------

// Set the pin state on a display (2x 1-based)
//  (returns false if invalid parameters)
//  NOTE: set send to FALSE to manually write to the shift register
boolean SerialDisplay::SetPin(byte pin, byte state, byte display, boolean send){
  // check index
  if((pin == 0) || (pin > 8) || (display == 0) || (display > _display_qty))
    return false;
  
  if(state == HIGH){
    _data[display - 1] |= pin;
    _state[display - 1] |= SERIAL_DISPLAY_ON;
  } else {
    _data[display - 1] &= ~pin;
  }
  _tosend = true; // set
  
  if(send)
    Send();
  
  return true;
}

// ----------------------------------------------------------------------------------------------------

// Update time based functions (blink)
void SerialDisplay::Update(void){
  // check blink
  if(_blink_next > 0){
    if(millis() > _blink_next){
      for(int i=0 ; i < _display_qty ; i++){
        if(_state[i] & SERIAL_DISPLAY_BLINK){
          if(_state[i] & SERIAL_DISPLAY_ON)
            _state[i] &= ~SERIAL_DISPLAY_ON;
          else
            _state[i] |= SERIAL_DISPLAY_ON;
        } else {
          _blink_next = 0; // reset
        }
      }
      
      Send();
      
      _blink_next = millis() + _blink_interval; // update
    }
  }
}

// ----------------------------------------------------------------------------------------------------

// Write changes
void SerialDisplay::Write(void){
  if(_tosend)
    Send();
}

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------

// Invert a character in a selected display (0-based)
void SerialDisplay::InvertChar(byte display){
  // check index
  if(display >= _display_qty)
    return;
  
  // invert characters
  if(_inverted & SERIAL_DISPLAY_INVERT_CHAR){
    byte temp = _data[display] & (PIN_G | PIN_P);
    if(_data[display] & PIN_A)
      temp |= PIN_D;
    if(_data[display] & PIN_B)
      temp |= PIN_E;
    if(_data[display] & PIN_C)
      temp |= PIN_F;
    if(_data[display] & PIN_D)
      temp |= PIN_A;
    if(_data[display] & PIN_E)
      temp |= PIN_B;
    if(_data[display] & PIN_F)
      temp |= PIN_C;
    
    _data[display] = temp;
  }
}

// ----------------------------------------------------------------------------------------------------

void SerialDisplay::InvertDisplays(void){
  // invert modules
  if(_inverted & SERIAL_DISPLAY_INVERT_DISPLAY){
    byte temp;
    byte qty = _display_qty / 2; // for ODD & EVEN
    for(int i=0 ; i < qty ; i++){
      // data
      temp = _data[i];
      _data[i] = _data[_display_qty - 1 - i];
      _data[_display_qty - 1 - i] = temp;
      
      // state
      temp = _state[i];
      _state[i] = _state[_display_qty - 1 - i];
      _state[_display_qty - 1 - i] = temp;
    }
  }
}

// ----------------------------------------------------------------------------------------------------

// Convert a char to printable byte
//  (returns 0 on failure or the converted char)
byte SerialDisplay::toByteMask(char c){
  byte res = 0;
  switch(c){
    case 0:  res = SerialDisplay::CHAR_0;  break;
    case 1:  res = SerialDisplay::CHAR_1;  break;
    case 2:  res = SerialDisplay::CHAR_2;  break;
    case 3:  res = SerialDisplay::CHAR_3;  break;
    case 4:  res = SerialDisplay::CHAR_4;  break;
    case 5:  res = SerialDisplay::CHAR_5;  break;
    case 6:  res = SerialDisplay::CHAR_6;  break;
    case 7:  res = SerialDisplay::CHAR_7;  break;
    case 8:  res = SerialDisplay::CHAR_8;  break;
    case 9:  res = SerialDisplay::CHAR_9;  break;
    
    case '0':  res = SerialDisplay::CHAR_0;  break;
    case '1':  res = SerialDisplay::CHAR_1;  break;
    case '2':  res = SerialDisplay::CHAR_2;  break;
    case '3':  res = SerialDisplay::CHAR_3;  break;
    case '4':  res = SerialDisplay::CHAR_4;  break;
    case '5':  res = SerialDisplay::CHAR_5;  break;
    case '6':  res = SerialDisplay::CHAR_6;  break;
    case '7':  res = SerialDisplay::CHAR_7;  break;
    case '8':  res = SerialDisplay::CHAR_8;  break;
    case '9':  res = SerialDisplay::CHAR_9;  break;
    
    
    case 'A':  res = SerialDisplay::CHAR_A;  break;
    case 'B':  res = SerialDisplay::CHAR_B;  break;
    case 'C':  res = SerialDisplay::CHAR_C;  break;
    case 'D':  res = SerialDisplay::CHAR_D;  break;
    case 'E':  res = SerialDisplay::CHAR_E;  break;
    case 'F':  res = SerialDisplay::CHAR_F;  break;
    
    case 'a':  res = SerialDisplay::CHAR_A;  break;
    case 'b':  res = SerialDisplay::CHAR_B;  break;
    case 'c':  res = SerialDisplay::CHAR_C;  break;
    case 'd':  res = SerialDisplay::CHAR_D;  break;
    case 'e':  res = SerialDisplay::CHAR_E;  break;
    case 'f':  res = SerialDisplay::CHAR_F;  break;
    
    case '-':  res = SerialDisplay::CHAR_SEPARATOR;  break;
  }
  
  return res;
}

// ----------------------------------------------------------------------------------------------------

// Send the data
void SerialDisplay::Send(void){
  byte mask, data;
  boolean last_display = false;
  byte display_number = _display_qty; // start from MAX because the do loop will set it to 0-base at the beginning
  do {
    // loop infinite stream or increment
    if(_display_qty != 0){
      display_number--;
      
      // check for last relay
      if(display_number == 0)
        last_display = true;
    }
    
    // send pulse
    mask = 0x80; // reset
    for(int i=1 ; i <= 8 ; i++){
      data = (_state[display_number] & SERIAL_DISPLAY_ON) ? _data[display_number] : 0; // get the corresponding data
      
      // set Data line
      if(data & mask)
        digitalWrite(_pinData, HIGH);
      else
        digitalWrite(_pinData, LOW);
      
      delayMicroseconds(SERIAL_DISPLAY_DELAY_DATA); // delay between Data and Clock signals
      
      // set Clock line
      digitalWrite(_pinClock, HIGH); // rising edge
      if((i == 8) && last_display)
        delayMicroseconds(SERIAL_DISPLAY_DELAY_LATCH); // latch
      else
        delayMicroseconds(SERIAL_DISPLAY_DELAY_CLOCK_HIGH); // shift
      digitalWrite(_pinClock, LOW);
      delayMicroseconds(SERIAL_DISPLAY_DELAY_CLOCK_LOW); // it is acceptable to have 5µs delay after the last bit has been sent
      
      mask >>= 1; // update mask
    }
  } while(display_number > 0);
  digitalWrite(_pinData, LOW); // reset to maintain LOW level when not in use
  
  _tosend = false; // reset
}

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------





