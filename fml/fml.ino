#include <Audio.h>
#include "AudioFML.h"

AudioFML fml;

AudioOutputAnalogStereo out;
AudioConnection conn(fml, 0, out, 0);
AudioConnection conn2(fml, 1, out, 1);

void handleNoteOn(byte inChannel, byte inNumber, byte inVelocity) {
  fml.process_note(true, inChannel - 1, inNumber, inVelocity);
  Serial.print("NoteOn  ");
  Serial.print(inNumber);
  Serial.print("\tvelocity: ");
  Serial.print(inVelocity);
  Serial.print("\tchan: ");
  Serial.println(inChannel);
}

void handleNoteOff(byte inChannel, byte inNumber, byte inVelocity) {
  fml.process_note(false, inChannel - 1, inNumber, inVelocity);
  Serial.print("NoteOff  ");
  Serial.print(inNumber);
  Serial.print("\tvelocity: ");
  Serial.print(inVelocity);
  Serial.print("\tchan: ");
  Serial.println(inChannel);
}

void handleCC(byte inChannel, byte inNumber, byte inVal) {
  fml.process_cc(inChannel - 1, inNumber, inVal);
  Serial.print("CC  ");
  Serial.print(inNumber);
  Serial.print("\tval: ");
  Serial.print(inVal);
  Serial.print("\tchan: ");
  Serial.println(inChannel);
}

void handleBend(byte inChannel, int value) {
  //fml.process_cc(inChannel, inNumber, inVal);
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  AudioMemory(15);
  usbMIDI.setHandleNoteOff(handleNoteOff);
  usbMIDI.setHandleNoteOn(handleNoteOn) ;
  usbMIDI.setHandleControlChange(handleCC);

  Serial.println("SETUP");
}

void loop() {
  usbMIDI.read();
}


//needed to avoid errors about kill and shit
void abort() {
  while(1);
}
