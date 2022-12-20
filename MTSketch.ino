#include "MIDIUSB.h"

const byte HOWMANYBUTTONS = 4;
const byte activePins[HOWMANYBUTTONS] = {12, 11, 10, 9};
bool engagedPins[HOWMANYBUTTONS];
bool boostBool = false;

const byte LEDPIN = 13;

void setup() {
  for (byte i = 0; i < HOWMANYBUTTONS; i++) {
    pinMode(activePins[i], INPUT);
    engagedPins[i] = false;
  }
  //led di output
  pinMode(LEDPIN, OUTPUT);
}

void loop() {

  //Ho scritto bene il codice così non cerca di premere troppe volte troppo velocemente
  //le note e non si impallano i programmi. Va strabene
  for (byte i = 0; i < HOWMANYBUTTONS; i++) {

    //memore dei consigli di Mr Bradshaw tengo il valore fisso per un poco
    bool buttonState = digitalRead(activePins[i]);

    //bottone premuto preceduto da bottone off
    if (buttonState == HIGH && !engagedPins[i]) {
      engagedPins[i] = true;

      //se premo il boost
      if (i == 0) {
        //se il boost non e' gia' attivo
        if (!boostBool) {
          boostBool = true;
          //manda C0 per il boost
          noteOn(0, 0, 127);
          MidiUSB.flush();
          noteOff(0, 0, 127);
          //accendi LED
          digitalWrite(LEDPIN, HIGH);
        } else {
          boostBool = false;
          //manda C#0 per togliere il boost
          noteOn(0, 1, 127);
          MidiUSB.flush();
          noteOff(0, 1, 127);
          //spegni LED
          digitalWrite(LEDPIN, LOW);
        }
      }
      //manda note tramite l'indice
      else { //i PC non funzionano... programChange(0, i);
        
        //mando +1 cosi' non interferisce con il boost
        noteOn(0, i + 1, 127);
        MidiUSB.flush();

        //serve per non dimenticare il boost per strada
        if (boostBool) {
          //manda C0 per il boost
          noteOn(0, 0, 127);
          MidiUSB.flush();
          noteOff(0, 0, 127);
        }
      }
      /*for (int i = 0; i < 128; i++) {
        noteOn(0, i, 127);
        }//TEST ONLY sends all notes at once*/

      delay(250);
    }
    //bottone libero preceduto da bottone on
    else if (buttonState == LOW && engagedPins[i]) {

      engagedPins[i] = false;
      if (i != 0) {
        //il boost non funziona come gli altri pulsanti quindi lo escludo
        noteOff(0, i + 1, 127);
      }

      /*for (int i = 0; i < 128; i++) {
        noteOff(0, i, 127);
        }
        MidiUSB.flush();//*/
    }

    //flush finale, per efficenza?
    MidiUSB.flush();
  }
}

//METHODS
//////////////////////////////////////////////////////////////////////////////////
void noteOn(byte channel, byte pitch, byte velocity) {
  // First parameter is the event type (0x09 = note on, 0x08 = note off).
  // Second parameter is note-on/note-off, combined with the channel.
  // Channel can be anything between 0-15. Typically reported to the user as 1-16.
  // Third parameter is the note number (48 = middle C).
  // Fourth parameter is the velocity (64 = normal, 127 = fastest).
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}
void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}
void controlChange(byte channel, byte control, byte value) {
  // First parameter is the event type (0x0B = control change).
  // Second parameter is the event type, combined with the channel.
  // Third parameter is the control number number (0-119).
  // Fourth parameter is the control value (0-127).
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}
void programChange(byte channel, byte program) {
  midiEventPacket_t pc = {0x0C, 0xC0 | channel, program, 0};
  MidiUSB.sendMIDI(pc);
}
//////////////////////////////////////////////////////////////////////////////////
