#ifndef RC_SERIAL_DISPLAY_H
#define RC_SERIAL_DISPLAY_H

/*
	   RoboCore Serial Display Library
		    (v1.0 - 25/05/2016)

  Serial Display functions for Arduino
    (tested with Arduino 1.0.1)

  Copyright 2016 RoboCore (François) ( http://www.RoboCore.net )
  
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

#include <Arduino.h>

// ----------------------------------------------------------------------------------------------------

#define SERIAL_DISPLAY_MAX_DISPLAYS 10

#define SERIAL_DISPLAY_DELAY_DATA           5 // [µs]
#define SERIAL_DISPLAY_DELAY_CLOCK_HIGH     5 // [µs]
#define SERIAL_DISPLAY_DELAY_CLOCK_LOW     20 // [µs]
#define SERIAL_DISPLAY_DELAY_LATCH       1100 // [µs]

#define SERIAL_DISPLAY_OFF      0
#define SERIAL_DISPLAY_ON       1
#define SERIAL_DISPLAY_BLINK    0x10
#define SERIAL_DISPLAY_NONE     0xFF

#define SERIAL_DISPLAY_INVERT_CHAR     0x01
#define SERIAL_DISPLAY_INVERT_DISPLAY  0x02
#define SERIAL_DISPLAY_INVERT_BOTH     (SERIAL_DISPLAY_INVERT_CHAR | SERIAL_DISPLAY_INVERT_DISPLAY)

#define SERIAL_DISPLAY_CASCADE_UP    0
#define SERIAL_DISPLAY_CASCADE_DOWN  1

#define SERIAL_DISPLAY_DEBUG

//#define ENABLE_INFINITE_STREAM

// ----------------------------------------------------------------------------------------------------

class SerialDisplay {
  public:
    static const byte PIN_A = 0x04;
    static const byte PIN_B = 0x02;
    static const byte PIN_C = 0x40;
    static const byte PIN_D = 0x20;
    static const byte PIN_E = 0x10;
    static const byte PIN_F = 0x01;
    static const byte PIN_G = 0x80;
    static const byte PIN_P = 0x08;
    
    static const byte CHAR_0 = (PIN_A | PIN_B | PIN_C | PIN_D | PIN_E | PIN_F);
    static const byte CHAR_1 = (PIN_B | PIN_C);
    static const byte CHAR_2 = (PIN_A | PIN_B | PIN_D | PIN_E | PIN_G);
    static const byte CHAR_3 = (PIN_A | PIN_B | PIN_C | PIN_D | PIN_G);
    static const byte CHAR_4 = (PIN_B | PIN_C | PIN_F | PIN_G);
    static const byte CHAR_5 = (PIN_A | PIN_C | PIN_D | PIN_F | PIN_G);
    static const byte CHAR_6 = (PIN_A | PIN_C | PIN_D | PIN_E | PIN_F | PIN_G);
    static const byte CHAR_7 = (PIN_A | PIN_B | PIN_C);
    static const byte CHAR_8 = (PIN_A | PIN_B | PIN_C | PIN_D | PIN_E | PIN_F | PIN_G);
    static const byte CHAR_9 = (PIN_A | PIN_B | PIN_C | PIN_F | PIN_G);
    static const byte CHAR_A = (PIN_A | PIN_B | PIN_C | PIN_E | PIN_F | PIN_G);
    static const byte CHAR_B = (PIN_C | PIN_D | PIN_E | PIN_F | PIN_G);
    static const byte CHAR_C = (PIN_A | PIN_D | PIN_E | PIN_F);
    static const byte CHAR_D = (PIN_B | PIN_C | PIN_D | PIN_E | PIN_G);
    static const byte CHAR_E = (PIN_A | PIN_D | PIN_E | PIN_F | PIN_G);
    static const byte CHAR_F = (PIN_A | PIN_E | PIN_F | PIN_G);
    static const byte CHAR_SEPARATOR = PIN_G;
  
    SerialDisplay(int pinData, int pinClock, byte qty = 1);
    boolean Blink(word interval, byte display = 0);
    void Brightness(int value);
    void Cascade(byte type, word interval);
    boolean Dot(byte display = 1);
    byte GetData(byte display = 1);
    byte GetState(byte display = 1);
#ifdef SERIAL_DISPLAY_DEBUG
    void Info(HardwareSerial *stream, byte format = HEX);
#endif
    void Invert(byte type = SERIAL_DISPLAY_INVERT_BOTH, boolean keep = true);
    boolean noDot(byte display = 1);
    boolean Off(byte display);
    boolean On(byte display);
    boolean Scroll(byte *array, byte array_length, word interval);
    boolean Set(byte mask, byte display = 1, boolean send = true);
    boolean setBrightnessPin(int pin);
    boolean SetPin(byte pin, byte state, byte display = 1, boolean send = true);
    void Update(void);
    void Write(void);
  
  private:
    int _pinClock;
    int _pinData;
    int _pinOE;
    byte _display_qty;
    byte _data[SERIAL_DISPLAY_MAX_DISPLAYS];
    byte _state[SERIAL_DISPLAY_MAX_DISPLAYS];
    byte _inverted;
    byte _brightness;
    word _blink_interval;
    unsigned long _blink_next;
    boolean _tosend; // TRUE if data to send
    
    // Send the data
    void Send(void);
  
};

// ----------------------------------------------------------------------------------------------------

#endif // RC_SERIAL_DISPLAY_H





