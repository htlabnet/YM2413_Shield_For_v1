typedef unsigned char uchar;

// Pin Define
const int pins[] = { 2, 3, 4, 5, 6, 7, 8, 9 };
const int CS = 12;
const int A0_ = 10; // A0だとconflict
const int IC = 13;
const int WE = 11;

class YM2413 {
 public:
  YM2413() {}
  void write(uchar adr, uchar dat) {
    // address write
    digitalWrite(A0_, LOW);
    write_data(adr);
    digitalWrite(CS, LOW);
    delayMicroseconds(12);
    digitalWrite(CS, HIGH);
    // data write
    digitalWrite(A0_, HIGH);
    write_data(dat);
    digitalWrite(CS, LOW);
    delayMicroseconds(84);
    digitalWrite(CS, HIGH);
  }
 private:
   void write_data(uchar dat) {
    for (int i = 0; i < 8; i++) {
      // hard cording 2..9
      digitalWrite(pins[i], bitRead(dat, i));
    }
   }
};

// Global Variable
YM2413 g_ym;
int i;

int fnum[12] = {172,181,192,204,216,229,242,257,272,288,305,323};


void noteon(int ch, int num, int inst, int vol) {
  // num = 12 - 107 (MIDI Note Numbers)
  // oct = 0 - 7
  int oct;
  if (num >= 12) {num = num - 12;};
  oct = num / 12;
  if (oct >= 8) {oct = 7;};
  num = fnum[num % 12];
  
  // Note(9ch) 0x10 - 15 (0 - 5)
  g_ym.write((0x10 + ch), num);

  // Inst & Vol(9ch)
  g_ym.write((0x30 + ch), (inst << 4) | vol);

  // NoteON & Oct & Note
  g_ym.write((0x20 + ch), (16 | (oct << 1) | (num >> 8)));
}

void noteoff(int ch) {
  // 0x20 - 25 (0 - 5)
  g_ym.write((0x20 + ch), 0);
}

void drumvol(int bd, int sd, int tom, int tcy, int hh) {
  g_ym.write(0x36, bd);
  g_ym.write(0x37, hh << 4 | sd);
  g_ym.write(0x38, tom << 4 | tcy);
}

void drum(int val) {
  g_ym.write(0x0E, 0);
  g_ym.write(0x0E, (val + 32));
}

void setup() {
  // Setup Pins
  for (int i = 0; i < 8; i++) {
    pinMode(pins[i], OUTPUT);
  }
  pinMode(CS, OUTPUT);
  pinMode(A0_, OUTPUT);
  pinMode(IC, OUTPUT);
  pinMode(WE, OUTPUT);
  
  digitalWrite(WE, LOW);
  
  // RESET IC
  digitalWrite(IC, LOW);
  delayMicroseconds(100);
  digitalWrite(IC, HIGH);
  delay(100);
  
  // Drum Volume (BD, SD, TOM, TCY, HH) [0 - 15]
  drumvol(0,0,0,0,0);
  
  delay(1000);

}

void wait() {
  //Tempo
  delay(110);
}

void loop(){
  
  //ch 0 : Synth1
  //ch 1 : Synth2
  //ch 2 : Synth3
  //ch 3 : Bass1
  //ch 4 : Bass2
  
  // [1- 1/2] ------------------------------
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,76,10,0); //E6
  noteon(1,64,10,0); //E5
  noteon(2,71,10,0); //B5
  noteon(3,40,13,0); //E3
  noteon(4,40,14,0); //E3
  drum(0B11011);
  wait();wait();
  
  noteoff(3);
  noteoff(4);
  noteon(3,52,13,0); //E4
  noteon(4,52,14,0); //E4
  drum(0B00001);
  wait();wait();

  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,71,10,0); //B5
  noteon(1,59,10,0); //B4
  noteon(2,68,10,0); //G#5
  noteon(3,40,13,0); //E3
  noteon(4,40,14,0); //E3
  drum(0B01011);
  wait();wait();
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,72,10,0); //C6
  noteon(1,60,10,0); //C5
  noteon(2,69,10,0); //A5
  noteon(3,52,13,0); //E4
  noteon(4,52,14,0); //E4
  drum(0B10001);
  wait();wait();
  
  // [1- 2/2] ------------------------------
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,74,10,0); //D6
  noteon(1,62,10,0); //D5
  noteon(2,71,10,0); //B5
  noteon(3,40,13,0); //E3
  noteon(4,40,14,0); //E3
  drum(0B01011);
  wait();wait();
  
  noteoff(3);
  noteoff(4);
  noteon(3,52,13,0); //E4
  noteon(4,52,14,0); //E4
  drum(0B10001);
  wait();wait();

  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,72,10,0); //C6
  noteon(1,60,10,0); //C5
  noteon(2,69,10,0); //A5
  noteon(3,40,13,0); //E3
  noteon(4,40,14,0); //E3
  drum(0B01011);
  wait();wait();
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,71,10,0); //B5
  noteon(1,59,10,0); //B4
  noteon(2,68,10,0); //G#5
  noteon(3,52,13,0); //E4
  noteon(4,52,14,0); //E4
  drum(0B00001);
  wait();wait();
  
  // [2- 1/2] ------------------------------
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,69,10,0); //A5
  noteon(1,57,10,0); //A4
  noteon(2,64,10,0); //E5
  noteon(3,45,13,0); //A3
  noteon(4,45,14,0); //A3
  drum(0B11011);
  wait();wait();
  
  noteoff(3);
  noteoff(4);
  noteon(3,57,13,0); //A4
  noteon(4,57,14,0); //A4
  drum(0B00001);
  wait();wait();

  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,69,10,0); //A5
  noteon(1,57,10,0); //A4
  noteon(2,64,10,0); //E5
  noteon(3,45,13,0); //A3
  noteon(4,45,14,0); //A3
  drum(0B01011);
  wait();wait();
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,72,10,0); //C6
  noteon(1,60,10,0); //C5
  noteon(2,69,10,0); //A5
  noteon(3,57,13,0); //A4
  noteon(4,57,14,0); //A4
  drum(0B10001);
  wait();wait();
  
  // [2- 2/2] ------------------------------
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,76,10,0); //E6
  noteon(1,64,10,0); //E5
  noteon(2,72,10,0); //C6
  noteon(3,45,13,0); //A3
  noteon(4,45,14,0); //A3
  drum(0B11011);
  wait();wait();
  
  noteoff(3);
  noteoff(4);
  noteon(3,57,13,0); //A4
  noteon(4,57,14,0); //A4
  drum(0B00001);
  wait();wait();

  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,74,10,0); //D6
  noteon(1,62,10,0); //D5
  noteon(2,71,10,0); //B5
  noteon(3,45,13,0); //A3
  noteon(4,45,14,0); //A3
  drum(0B01011);
  wait();wait();
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,72,10,0); //C6
  noteon(1,60,10,0); //C5
  noteon(2,69,10,0); //A5
  noteon(3,57,13,0); //A4
  noteon(4,57,14,0); //A4
  drum(0B10001);
  wait();wait();
  
  // [3- 1/2] ------------------------------
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,71,10,0); //B5
  noteon(1,59,10,0); //B4
  noteon(2,68,10,0); //G#5
  noteon(3,44,13,0); //G#3
  noteon(4,44,14,0); //G#3
  drum(0B11011);
  wait();wait();
  
  noteoff(3);
  noteoff(4);
  noteon(3,56,13,0); //G#4
  noteon(4,56,14,0); //G#4
  drum(0B00001);
  wait();wait();

  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,71,10,0); //B5
  noteon(1,59,10,0); //B4
  noteon(2,68,10,0); //G#5
  noteon(3,44,13,0); //G#3
  noteon(4,44,14,0); //G#3
  drum(0B01011);
  wait();wait();
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,72,10,0); //C6
  noteon(1,60,10,0); //C5
  noteon(2,69,10,0); //A5
  noteon(3,56,13,0); //G#4
  noteon(4,56,14,0); //G#4
  drum(0B10001);
  wait();wait();
  
  // [3- 2/2] ------------------------------
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,74,10,0); //D6
  noteon(1,62,10,0); //D5
  noteon(2,71,10,0); //B5
  noteon(3,40,13,0); //E3
  noteon(4,40,14,0); //E3
  drum(0B11011);
  wait();wait();
  
  noteoff(3);
  noteoff(4);
  noteon(3,52,13,0); //E4
  noteon(4,52,14,0); //E4
  drum(0B00001);
  wait();wait();

  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,76,10,0); //E6
  noteon(1,64,10,0); //E5
  noteon(2,72,10,0); //C6
  noteon(3,40,13,0); //E3
  noteon(4,40,14,0); //E3
  drum(0B01011);
  wait();wait();
  
  noteoff(3);
  noteoff(4);
  noteon(3,52,13,0); //E4
  noteon(4,52,14,0); //E4
  drum(0B10001);
  wait();wait();
  
  // [4- 1/2] ------------------------------
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,72,10,0); //C6
  noteon(1,60,10,0); //C5
  noteon(2,69,10,0); //A5
  noteon(3,45,13,0); //A3
  noteon(4,45,14,0); //A3
  drum(0B11011);
  wait();wait();
  
  noteoff(3);
  noteoff(4);
  noteon(3,57,13,0); //A4
  noteon(4,57,14,0); //A4
  drum(0B00001);
  wait();wait();

  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,69,10,0); //A5
  noteon(1,57,10,0); //A4
  noteon(2,64,10,0); //E5
  noteon(3,45,13,0); //A3
  noteon(4,45,14,0); //A3
  drum(0B01011);
  wait();wait();
  
  noteoff(3);
  noteoff(4);
  noteon(3,57,13,0); //A4
  noteon(4,57,14,0); //A4
  drum(0B10001);
  wait();wait();
  
  // [4- 2/2] ------------------------------
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,69,10,0); //A5
  noteon(1,57,10,0); //A4
  noteon(2,64,10,0); //E5
  noteon(3,45,13,0); //A3
  noteon(4,45,14,0); //A3
  drum(0B11011);
  wait();wait();
  
  noteoff(3);
  noteoff(4);
  noteon(3,57,13,0); //A4
  noteon(4,57,14,0); //A4
  drum(0B00001);
  wait();wait();

  noteoff(3);
  noteoff(4);
  noteon(3,47,13,0); //B3
  noteon(4,47,14,0); //B3
  drum(0B01011);
  wait();wait();
  
  noteoff(3);
  noteoff(4);
  noteon(3,48,13,0); //C4
  noteon(4,48,14,0); //C4
  drum(0B10011);
  wait();wait();
  
  // [5- 1/2] ------------------------------
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(3,50,13,0); //D4
  noteon(4,50,14,0); //D4
  drum(0B11011);
  wait();wait();
  
  noteoff(3);
  noteoff(4);
  noteon(0,74,10,0); //D6
  noteon(1,62,10,0); //D5
  noteon(2,65,10,0); //F5
  noteon(3,38,13,0); //D3
  noteon(4,38,14,0); //D3
  drum(0B00001);
  wait();wait();

  drum(0B01011);
  wait();wait();
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,77,10,0); //F6
  noteon(1,65,10,0); //F5
  noteon(2,69,10,0); //A5
  noteon(3,38,13,0); //D3
  noteon(4,38,14,0); //D3
  drum(0B10001);
  wait();
  drum(0B01000);
  wait();
  
  // [5- 2/2] ------------------------------
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteon(0,81,10,0); //A6
  noteon(1,69,10,0); //A5
  noteon(2,72,10,0); //C6
  drum(0B11011);
  wait();
  drum(0B01000);
  wait();
  
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(2,72,10,0); //C6
  noteon(3,38,13,0); //D3
  noteon(4,38,14,0); //D3
  drum(0B00001);
  wait();
  
  noteoff(2);
  noteon(2,72,10,0); //C6
  wait();
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,79,10,0); //G6
  noteon(1,67,10,0); //G5
  noteon(2,71,10,0); //B5
  noteon(3,45,13,0); //A3
  noteon(4,45,14,0); //A3
  drum(0B01011);
  wait();wait();
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,77,10,0); //F6
  noteon(1,65,10,0); //F5
  noteon(2,69,10,0); //A5
  noteon(3,41,13,0); //F3
  noteon(4,41,14,0); //F3
  drum(0B10001);
  wait();
  drum(0B01000);
  wait();
  
  // [6- 1/2] ------------------------------
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,76,10,0); //E6
  noteon(1,64,10,0); //E5
  noteon(2,67,10,0); //G5
  noteon(3,36,13,0); //C3
  noteon(4,36,14,0); //C3
  drum(0B11011);
  wait();wait();
  
  noteoff(3);
  noteoff(4);
  
  noteon(3,48,13,0); //C4
  noteon(4,48,14,0); //C4
  drum(0B00001);
  wait();wait();

  drum(0B01011);
  wait();wait();
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,72,10,0); //C6
  noteon(1,60,10,0); //C5
  noteon(2,64,10,0); //E5
  noteon(3,48,13,0); //C4
  noteon(4,48,14,0); //C4
  drum(0B10001);
  wait();
  drum(0B01000);
  wait();
  
  // [6- 2/2] ------------------------------
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,76,10,0); //E6
  noteon(1,64,10,0); //E5
  noteon(2,67,10,0); //G5
  noteon(3,36,13,0); //C3
  noteon(4,36,14,0); //C3
  drum(0B11011);
  wait();
  drum(0B01000);
  wait();
  
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(2,69,10,0); //A5
  noteon(3,43,13,0); //G3
  noteon(4,43,14,0); //G3
  drum(0B00001);
  wait();
  
  noteoff(2);
  noteon(2,67,10,0); //G5
  wait();
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,74,10,0); //D6
  noteon(1,62,10,0); //D5
  noteon(2,65,10,0); //F5
  noteon(3,43,13,0); //G3
  noteon(4,43,14,0); //G3
  drum(0B01011);
  wait();wait();
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteon(0,72,10,0); //C6
  noteon(1,60,10,0); //C5
  noteon(2,64,10,0); //E5
  drum(0B10001);
  wait();
  drum(0B01000);
  wait();
  
  // [7- 1/2] ------------------------------
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,71,10,0); //B5
  noteon(1,59,10,0); //B4
  noteon(2,68,10,0); //G#5
  noteon(3,47,13,0); //B3
  noteon(4,47,14,0); //B3
  drum(0B11011);
  wait();wait();
  
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(2,64,10,0); //E5
  noteon(3,59,13,0); //B4
  noteon(4,59,14,0); //B4
  drum(0B00001);
  wait();wait();

  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteon(0,71,10,0); //B5
  noteon(1,59,10,0); //B4
  noteon(2,68,10,0); //G#5
  drum(0B01011);
  wait();wait();
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,72,10,0); //C6
  noteon(1,60,10,0); //C5
  noteon(2,69,10,0); //A5
  noteon(3,59,13,0); //B4
  noteon(4,59,14,0); //B4
  drum(0B10001);
  wait();
  drum(0B01000);
  wait();
  
  // [7- 2/2] ------------------------------
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteon(0,74,10,0); //D6
  noteon(1,62,10,0); //D5
  noteon(2,71,10,0); //B5
  drum(0B11011);
  wait();
  drum(0B01000);
  wait();
  
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(2,68,10,0); //G#5
  noteon(3,52,13,0); //E4
  noteon(4,52,14,0); //E4
  drum(0B00001);
  wait();wait();
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteon(0,76,10,0); //E6
  noteon(1,64,10,0); //E5
  noteon(2,72,10,0); //C6
  drum(0B01011);
  wait();wait();
  
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(2,68,10,0); //G#5
  noteon(3,56,13,0); //G#4
  noteon(4,56,14,0); //G#4
  drum(0B10001);
  wait();
  drum(0B01000);
  wait();
  
  // [8- 1/2] ------------------------------
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,72,10,0); //C6
  noteon(1,60,10,0); //C5
  noteon(2,69,10,0); //A5
  noteon(3,45,13,0); //A3
  noteon(4,45,14,0); //A3
  drum(0B11011);
  wait();wait();
  
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(2,64,10,0); //E5
  noteon(3,52,13,0); //E4
  noteon(4,52,14,0); //E4
  drum(0B00001);
  wait();wait();

  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,69,10,0); //A5
  noteon(1,57,10,0); //A4
  noteon(2,64,10,0); //E5
  noteon(3,45,13,0); //A3
  noteon(4,45,14,0); //A3
  drum(0B01011);
  wait();wait();
  
  noteoff(3);
  noteoff(4);
  noteon(3,52,13,0); //E4
  noteon(4,52,14,0); //E4
  drum(0B10001);
  wait();
  drum(0B01000);
  wait();
  
  // [8- 2/2] ------------------------------
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  noteon(0,69,10,0); //A5
  noteon(1,57,10,0); //A4
  noteon(2,64,10,0); //E5
  noteon(3,45,13,0); //A3
  noteon(4,45,14,0); //A3
  drum(0B11011);
  wait();
  drum(0B01000);
  wait();
  
  drum(0B00001);
  wait();wait();
  
  noteoff(0);
  noteoff(1);
  noteoff(2);
  noteoff(3);
  noteoff(4);
  drum(0B01011);
  wait();wait();
  
  drum(0B10001);
  wait();wait();
  
  // end  ------------------------------
  
  //delay (10000);

}




