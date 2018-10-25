//
// #################################################
//
//    HTLAB.NET YM2413 Shield
//      http://htlab.net/electronics/
//
//    Copyright (C) 2018
//      Hideto Kikuchi / PJ (@pcjpnet) - http://pc-jp.net/
//
// #################################################
//

//
// ########## Compatible Boards ##########
//    - Arduino Uno Rev3 (USB-MIDI is available using MOCO)
//    - Arduino Leonardo (USB-MIDI is available)
//

//
// ########## Require Libraries ##########
//    - MIDI Library 4.3
//
//

//
// ########## Optional Libraries ##########
//    - MIDIUSB Library 1.0.3 (for Arduino Leonardo)
//
//

// ########## Settings ##########
// Use MIDIUSB Library
//#define USE_MIDIUSB
//
// ########## Complete ##########

#include <math.h>

// Use MIDI Library
#include <MIDI.h>
#if (MIDI_LIBRARY_VERSION_MAJOR < 4) || \
  ((MIDI_LIBRARY_VERSION_MAJOR == 4) && (MIDI_LIBRARY_VERSION_MINOR < 3))
  #error This version of MIDI library is not supported. Please use version 4.3 or later.
#endif


// Use MIDIUSB Library (for Arduino Leonardo, Micro)
#if defined(USE_MIDIUSB) && defined(USBCON) && \
    (defined(ARDUINO_AVR_LEONARDO) || defined(ARDUINO_AVR_MICRO))
  #include <midi_UsbTransport.h>
  static const unsigned sUsbTransportBufferSize = 16;
  typedef midi::UsbTransport<sUsbTransportBufferSize> UsbTransport;
  UsbTransport sUsbTransport;
  MIDI_CREATE_INSTANCE(UsbTransport, sUsbTransport, MIDI);
#else
  MIDI_CREATE_DEFAULT_INSTANCE();
#endif


// Pin Define
const byte pin_bus[] = { 2, 3, 4, 5, 6, 7, 8, 9 };
const byte pin_CS = 12;
const byte pin_WE = 11;
const byte pin_A0 = 10;
const byte pin_IC = 13;


// F-Number table
const int fnum[12] = { 172, 181, 192, 204, 216, 229, 242, 257, 272, 288, 305, 323 };
const byte muteNum = 255;


class YM2413 {
  public:
    YM2413() {}
      void begin() {
        // pin mode setup
        for (int i = 0; i < 8; i++) {
          pinMode(pin_bus[i], OUTPUT);
        }
        pinMode(pin_CS, OUTPUT);
        pinMode(pin_A0, OUTPUT);
        pinMode(pin_IC, OUTPUT);
        pinMode(pin_WE, OUTPUT);
        
        // pin output (Inactive)
        digitalWrite(pin_CS, HIGH);
        
        // YM2413 reset
        digitalWrite(pin_IC, LOW);
        delayMicroseconds(23);
        // 80CLK @ 4MHz = 20us / 80CLK @  3.579545MHz = 22.35us
        digitalWrite(pin_IC, HIGH);
        delayMicroseconds(23);
        
        // note slot
        for (int i = 0; i < 9; i++) {
          for (int j = 0; j < 2; j++) {
            note_slot[i][j] = muteNum;
          }
          use_mod[i] = false;
        }
        max_num = 9;
        
        // drum default settings
        drum_stat_flag = false;
      }
      
      void rom_note_on(byte inst, byte vol, byte num) {
        int sel;
        sel = note_slot_add(num);
        
        if (sel == muteNum) {
          sel = note_slot_remove(note_slot[0][0]);
          note_slot[max_num-1][0] = num;
          note_slot[max_num-1][1] = sel;
          write((0x20 + sel), 0); 
        }

          // Note(9ch) 0x10 - 15 (0 - 5)
          write((0x10 + sel), fnum_calc(num));
          // Inst & Vol(9ch)
          write((0x30 + sel), (inst << 4) | (vol_calc(vol) & 0b1111));
          // NoteON & Oct & Note
          write((0x20 + sel), (0b10000 | ((oct_calc(num) << 1) & 0b1110) | ((fnum_calc(num) >> 8) & 0b1)));
          use_mod[sel] = true;
      }
      
      void rom_note_off(byte num) {
        int sel;
        sel = note_slot_remove(num);
        
        if (sel != muteNum) {
          // 0x20 - 25 (0 - 5) noteoff
          write((0x20 + sel), 0);
          use_mod[sel] = false;
        }
      }
      
      void drum_mode(boolean mode) {
        // drum settings
        drum_stat_flag = mode;
        drum_stat = mode << 5;
        if (drum_stat_flag) {
          max_num = 6;
        } else {
          max_num = 9;
        }
        drum_vol[0] = 0;
        drum_vol[1] = 0;
        drum_vol[2] = 0;
        drum_vol[3] = 0;
        drum_vol[4] = 0;
        write(0x16, 0x20);
        write(0x17, 0x50);
        write(0x18, 0xC0);
        write(0x26, 0x5);
        write(0x27, 0x5);
        write(0x28, 0x1);
        write(0x36, drum_vol[4]);
        write(0x37, drum_vol[0] << 4 | (drum_vol[3] & 0b1111));
        write(0x38, drum_vol[2] << 4 | (drum_vol[1] & 0b1111));
        write(0x0E, drum_stat);
      }
      
      void drum_note_on(byte inst, byte vol) {
        if (drum_stat_flag && inst < 5) {
          // drum volume
          drum_vol[inst] = vol_calc(vol);
          switch (inst) {
            case 0:
            case 3:
              write(0x37, drum_vol[0] << 4 | (drum_vol[3] & 0b1111));
            break;
            case 1:
            case 2:
              write(0x38, drum_vol[2] << 4 | (drum_vol[1] & 0b1111));
            break;
            case 4:
              write(0x36, drum_vol[4]);
            break;
          }
          
          // drum status
          drum_stat |= _BV(inst);
          write(0x0E, drum_stat);
        }
      }
      
      void drum_note_off(byte inst) {
        if (drum_stat_flag && inst < 5) {
          drum_stat &= ~(_BV(inst));
          write(0x0E, drum_stat);
        }
      }
      
      void sound_off() {
        for (int i = 0; i < max_num; i++) {
          rom_note_off(note_slot[i][0]);
        }
        drum_note_off(0);
        drum_note_off(1);
        drum_note_off(2);
        drum_note_off(3);
        drum_note_off(4);
      }
      
      void write(byte adr, byte dat) {
        // address write
        digitalWrite(pin_A0, LOW);
        write_data(adr);
        digitalWrite(pin_CS, LOW);
        delayMicroseconds(4);
        // 12CLK @ 4MHz = 3us / 12CLK @ 3.579545MHz = 3.35us
        digitalWrite(pin_CS, HIGH);
        
        // data write
        digitalWrite(pin_A0, HIGH);
        write_data(dat);
        digitalWrite(pin_CS, LOW);
        delayMicroseconds(24);
        // 84CLK @ 4MHz = 21us / 84CLK @  3.579545MHz = 23.47us
        digitalWrite(pin_CS, HIGH);
      }
  private:
    // member variable
    byte note_slot[9][2];
    byte max_num;
    boolean use_mod[9];
    
    boolean drum_stat_flag;
    byte drum_stat;
    byte drum_vol[5];
    
    // data bus
    void write_data(byte dat) {
      for (int i = 0; i < 8; i++) {
        digitalWrite(pin_bus[i], bitRead(dat, i));
      }
    }
    
    int fnum_calc(byte num) {
      // num = 0 - 127 -> (12 - 107[8oct]) (MIDI Note Numbers)
      return fnum[num % 12];
    }
    
    byte oct_calc(byte num) {
      // oct = 0 - 7
      if (num < 12) {
        return 0;
      } else if (num > 107) {
        return 7;
      } else {
        return num / 12;
      }
    }
    
    byte vol_calc(byte num) {
      return log10(pow(10, ( (127 - num) / 26)));
    }
    
    byte note_slot_add(byte num) {
      byte i,j;
      for(i = 0; i < max_num; i++) {
        if (!use_mod[i]) {
          for(j = 0; j < max_num; j++) {
            if (note_slot[j][0] == muteNum) {
              note_slot[j][0] = num;
              note_slot[j][1] = i;
              return i;
            }
          }
        }
      }
      return muteNum;
    }
    
    byte note_slot_remove(byte num) {
      for(byte i = 0; i < max_num; i++) {
        if (note_slot[i][0] == num) {
          int sel;
          sel = note_slot[i][1];
          for (byte j = i; j < max_num; j++) {
            if (j != max_num -1) {
              note_slot[j][0] = note_slot[j+1][0];
              note_slot[j][1] = note_slot[j+1][1];
            } else {
              note_slot[j][0] = muteNum;
              note_slot[j][1] = muteNum;
            }
          }
          return sel;
        }
      }
      return muteNum;
    }
};


// global variable
YM2413 g_ym;
byte pc_map[16];


void setup() {
  // YM2413 initialize
  g_ym.begin();
  g_ym.drum_mode(true);
  
  // MIDI Lib
  MIDI.setHandleNoteOn(isr_midi_noteon);
  MIDI.setHandleNoteOff(isr_midi_noteoff);
  MIDI.setHandleProgramChange(isr_midi_programchange);
  MIDI.setHandleControlChange(isr_midi_controlchange);
  MIDI.begin(MIDI_CHANNEL_OMNI);
  
  // Program Change <-> Channel Tone Map
  for(byte i = 0; i < 16; i++) {
    pc_map[i] = 10;
  }
}


// Main Loop
void loop() {

  // MIDI Tasks
  MIDI.read();

}


// Interrupt MIDI NoteON Tasks
void isr_midi_noteon(byte ch, byte num, byte vel) {
  if (ch != 10) {
    g_ym.rom_note_on(pc_map[ch], vel, num);
  } else {
    switch (num) {
      case 35:
      case 36:
        //BD
        g_ym.drum_note_on(4, vel);
        break;

      case 38:
      case 40:
        //SD
        g_ym.drum_note_on(3, vel);
        break;

      case 41:
      case 43:
      case 45:
      case 47:
      case 48:
      case 50:
        //TOM
        g_ym.drum_note_on(2, vel);
        break;

      case 42:
      case 44:
      case 46:
        //HH
        g_ym.drum_note_on(0, vel);
        break;

      case 49:
      case 51:
        //TCY
        g_ym.drum_note_on(1, vel);
        break;
    }
  }
}


// Interrupt MIDI NoteOFF Tasks
void isr_midi_noteoff(byte ch, byte num, byte vel) {
  if (ch != 10) {
    g_ym.rom_note_off(num);
  } else {
    switch (num) {
      case 35:
      case 36:
        //BD
        g_ym.drum_note_off(4);
        break;

      case 38:
      case 40:
        //SD
        g_ym.drum_note_off(3);
        break;

      case 41:
      case 43:
      case 45:
      case 47:
      case 48:
      case 50:
        //TOM
        g_ym.drum_note_off(2);
        break;

      case 42:
      case 44:
      case 46:
        //HH
        g_ym.drum_note_off(0);
        break;

      case 49:
      case 51:
        //TCY
        g_ym.drum_note_off(1);
        break;
    }
  }
}


// Interrupt MIDI ProgramChange Tasks
void isr_midi_programchange(byte ch, byte num) {
  switch (num) {
    case 0:  //00  Acoustic Piano
    case 1:  //01 Bright Piano
    case 2:  //02 Electric Grand Piano
    case 3:  //03 Honky-tonk Piano
    case 4:  //04 Electric Piano
    case 5:  //05 Electric Piano 2
      //Piano
      pc_map[ch] = 3;
      break;

    case 6:  //06 Harpsichord
    case 7:  //07 Clavi
      //Harpsichord
      pc_map[ch] = 11;
      break;

    case 8:  //08 Celesta
    case 9:  //09 Glockenspiel
    case 10:  //0A Musical Box
    case 11:  //0B Vibraphone
      //Vibraphone
      pc_map[ch] = 12;
      break;

    case 16:  //10 Drawbar Organ
    case 17:  //11 Percussive Organ
    case 18:  //12 Rock Organ
    case 19:  //13 Church Organ
    case 20:  //14 Reed Organ
      //Organ
      pc_map[ch] = 8;
      break;

    case 24:  //18 Acoustic Guitar (nylon)
    case 25:  //19 Acoustic Guitar (steel)
      //Guitar
      pc_map[ch] = 2;
      break;

    case 26:  //1A Electric Guitar (jazz)
    case 27:  //1B Electric Guitar (clean)
    case 28:  //1C Electric Guitar (muted)
    case 29:  //1D Overdriven Guitar
    case 30:  //1E Distortion Guitar
    case 31:  //1F Guitar harmonics
      //Electric Guitar
      pc_map[ch] = 15;
      break;

    case 32:  //20 Acoustic Bass
    case 33:  //21 Electric Bass (finger)
    case 34:  //22 Electric Bass (pick)
    case 35:  //23 Fretless Bass
      //Bass
      pc_map[ch] = 14;
      break;

    case 38:  //26 Synth Bass 1
    case 39:  //27 Synth Bass 2
      //Synth Bass
      pc_map[ch] = 13;
      break;

    case 40:  //28 Violin
      //Violin
      pc_map[ch] = 1;
      break;

    case 56:  //38 Trumpet
    case 59:  //3B Muted Trumpet
      //Trumpet
      pc_map[ch] = 7;
      break;

    case 60:  //3C French Horn
      //French Horn
      pc_map[ch] = 9;
      break;

    case 68:  //44 Oboe
      //Oboe
      pc_map[ch] = 6;
      break;

    case 71:  //47 Clarinet
      //Clarinet
      pc_map[ch] = 5;
      break;

    case 73:  //49 Flute
      //Flute
      pc_map[ch] = 4;
      break;

    default:
      //Synth (Default)
      pc_map[ch] = 10;
      break;
  }
}


// Interrupt MIDI ControlChange Tasks
void isr_midi_controlchange(byte ch, byte num, byte val) {
  switch(num) {
    case 120: // CC#120 All Sound Off
    case 123: // CC#123 All Notes Off
      g_ym.sound_off();
      break;
  }
}
