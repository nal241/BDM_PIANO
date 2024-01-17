#include <MozziGuts.h>
#include <Oscil.h>
#include <ADSR.h>
#include "piano2048_int8.h"
#include <Keyboard.h>

#define CONTROL_RATE 64
#define SOUND_OUT 9

#define IN0 3
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7

#define OUT0 11
#define OUT1 12
#define OUT2 13

const float pitch[7][14] = {
  //{ 32.703, 34.648, 36.708, 38.891, 41.203, 43.654, 46.249, 48.999, 51.913, 55.000, 58.270, 61.735, 65.406},
  //{65.406, 	69.296, 73.416, 77.782, 82.407, 87.307, 92.499, 97.999, 103.826, 110.000, 116.541, 123.471, 130.813},
  // {0, 130.813, 138.591, 146.832, 155.563, 164.814, 174.614, 184.997, 195.998, 207.652, 220.000, 233.082, 246.942, 261.626},
  {0.0, 32.703, 34.648, 36.708, 38.891, 41.203, 43.654, 46.249, 48.999, 51.913, 55.000, 58.270, 61.735, 65.406},
  {0.0, 65.406, 69.296, 73.416, 77.782, 82.407, 87.307, 92.499, 97.999, 103.826, 110.000, 116.541, 123.471, 130.813},
  {0.0, 130.813, 138.591, 146.832, 155.563, 164.814, 174.614, 184.997, 195.998, 207.652, 220.000, 233.082, 246.942, 261.626},
  {0.0, 261.626, 277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440.000, 466.164, 493.883, 523.251},
  {0.0, 523.251, 554.365, 587.330, 622.254, 659.255, 698.456, 739.989, 783.991, 830.609, 880.000, 932.328, 987.767, 1046.502},
  {0.0, 1046.502, 1108.731, 1174.659, 1244.508, 1318.510, 1396.913, 1479.978, 1567.982, 1661.219, 1760.000, 1864.655, 1975.533, 2093.005},
  {0.0, 2093.005, 2217.461, 2349.318, 2489.016, 2637.020, 2793.826, 2959.955, 3135.963, 3322.438, 3520.000, 3729.310, 3951.066, 4186.009},
};

int8_t key[15];
int8_t key_previous[15];
int8_t octave = 3;


Oscil <PIANO2048_NUM_CELLS, AUDIO_RATE> aPiano(PIANO2048_DATA);
Oscil <PIANO2048_NUM_CELLS, AUDIO_RATE> bPiano(PIANO2048_DATA);
Oscil <PIANO2048_NUM_CELLS, AUDIO_RATE> cPiano(PIANO2048_DATA);
Oscil <PIANO2048_NUM_CELLS, AUDIO_RATE> dPiano(PIANO2048_DATA);

ADSR<AUDIO_RATE, AUDIO_RATE> envelopea;
// ADSR<AUDIO_RATE, AUDIO_RATE> envelopeb;
// ADSR<AUDIO_RATE, AUDIO_RATE> envelopec;


void setup() {
  Serial.begin(9600);
  startMozzi(CONTROL_RATE);
  //Keyboard.begin();
  aPiano.setFreq(0.f);
  bPiano.setFreq(0.f);
  cPiano.setFreq(0.f);
  dPiano.setFreq(0.f);

  pinMode(IN0, INPUT);
  pinMode(IN1, INPUT);
  pinMode(IN2, INPUT);
  pinMode(IN3, INPUT);
  pinMode(IN4, INPUT);

  pinMode(OUT0, OUTPUT);
  pinMode(OUT1, OUTPUT);
  pinMode(OUT2, OUTPUT);

  envelopea.setADLevels(255, 210);
  envelopea.setTimes(10, 1200, 10, 500);

  // envelopeb.setADLevels(255, 210);
  // envelopeb.setTimes(10, 1200, 10, 500);

  // envelopec.setADLevels(255, 210);
  // envelopec.setTimes(10, 1200, 10, 500);
}

void updateControl(){

  digitalWrite(OUT0, HIGH);
  key[0] = digitalRead(IN0);
  key[3] = digitalRead(IN1);
  key[6] = digitalRead(IN2);
  key[9] = digitalRead(IN3);
  key[12] = digitalRead(IN4);
  digitalWrite(OUT0, LOW);
  
  digitalWrite(OUT1, HIGH);
  key[1] = digitalRead(IN0);
  key[4] = digitalRead(IN1);
  key[7] = digitalRead(IN2);
  key[10] = digitalRead(IN3);
  key[13] = digitalRead(IN4);
  digitalWrite(OUT1, LOW);

  digitalWrite(OUT2, HIGH);
  key[2] = digitalRead(IN0);
  key[5] = digitalRead(IN1);
  key[8] = digitalRead(IN2);
  key[11] = digitalRead(IN3);
  key[14] = digitalRead(IN4);
  digitalWrite(OUT2, LOW);

  int8_t play_keyboard[4] = {0, 0, 0, 0};
  int8_t play_keyboard_arg = 0;
  int8_t envelopea_on = 0;

  for(int i = 0; i < 13; i++){
    if(key[i] == LOW){
      key_previous[i] = 0;
    }else{
      Serial.println(i);
      if(key_previous[i] == 0){
        envelopea_on = 1;
        Keyboard.write('a');
      }
      play_keyboard[play_keyboard_arg] = i + 1;
      play_keyboard_arg = play_keyboard_arg+1;
      if(play_keyboard_arg == 5)break;
      key_previous[i] = 1;
    }
  }

if(key[13] == LOW){
  key_previous[13] = 0;
}else if(key_previous[13] == 0){
  key_previous[13] = 1;
  if(octave > 0)octave -= 1;
}

if(key[14] == LOW){
  key_previous[14] = 0;
}else if(key_previous[14] == 0){
  key_previous[14] = 1;
  if(octave < 6)octave += 1;
}


  if(envelopea_on){
    envelopea.noteOn();
    envelopea_on = 0;
  }

  aPiano.setFreq(pitch[octave][play_keyboard[0]]);
  bPiano.setFreq(pitch[octave][play_keyboard[1]]);
  cPiano.setFreq(pitch[octave][play_keyboard[2]]);
  dPiano.setFreq(pitch[octave][play_keyboard[3]]);

}


int updateAudio(){
  envelopea.update();
  //envelopeb.update();
  //envelopec.update();
  return (int)(envelopea.next() * ((aPiano.next() + bPiano.next() + cPiano.next() + dPiano.next())>>2))>>8;
}

void loop() {
  audioHook();
}
