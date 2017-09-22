/*
 * MUDI_2000
 * 
 * Final code used in MIDI Controller.
 * 
 * Note different options for Note, Velocity and Channel arrays. 
 * The versions commented out are for notes in the key of C major 
 * and the second version can be used for debugging - as each note 
 * value identifies it's position in the array.
 * These arrays can be modified to send any MIDI messages.
 * 
 * The three arrays currently active are set up to allow a user to play
 * a version of Shape Of You by Ed Sheeran - the percussion, bass line and
 * melody can all be played
 * 
 */

// Include the MIDI library, needed for the MIDI specific functions
// such as MIDI.sendNoteOn().
#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

// Array to hold the select pins for the MUXs
// pin 10 is connected to s0, pin 11 to s1 and pin 12
// to s2 on the multiplexers.
const int selectPin[] = {10, 11, 12};

// Array holds the pins the outputs of the MUXs are connected to
// The first element in the array corresponds to MUX0, the second
// to MUX1 etc. 
// Multiplexers can be added and removed by changing this array.
const int MUXarr[] = {4, 5, 6, 7};
const int MUXarrLength = sizeof(MUXarr);

// There are only 30 buttons, but 32 possibilities with the MUXs, 
// so NUMBUTTONS prevents the program looking for one of the extra two in an array,
// finding the index out of bounds and crashing.
// Even if a message is sent from one of the last two buttons, the values are set so
// it won't make any noise. 
// If the number of buttons is changed, NUMBUTTONS must be updated to reflect this.
const int NUMBUTTONS = 30;

int currentButtonState[NUMBUTTONS]; // holds the current state of each button
int prevButtonState[NUMBUTTONS]; // holds the previous state of each button

// holds the last MIDI message sent for each button
// e.g. if the last message was noteOn, element will be HIGH
int msgState[NUMBUTTONS];

// note[], velocity[] and channel[] hold the arguments for the MIDI message
// assigned to each button.
// They are arranged in the same orientation as they are on the physical box, 
// so that mapping can be easily done.
// Note the final two elements in each array are set to 0, this is so if for
// some reason the Uno tries to send a message from either of these buttons 
// the buttons will not affect any software running, as they will have a velocity
// of 0 (no sound)
/* Arrays set up in the key of C major:

const byte note[] = {42, 48, 55, 57, 65,
                     41, 45, 53, 59, 67, 
                     38, 43, 52, 60, 69,
                     36, 41, 50, 62, 71, 
                     34, 36, 48, 64, 72, 
                     1,   2,  3,  4,  5,      0, 0};
                        
const byte velocity[] = {80,80,80,80,80,
                         80,80,80,80,80,
                         80,80,80,80,80,
                         80,80,80,80,80,
                         80,80,80,80,80,
                         100,5,55,5,05,        0,0};
                            
const byte channel[] = {1,2,3,3,4,
                        1,2,3,3,4,
                        1,2,3,3,4,
                        1,2,3,4,4,
                        1,2,3,4,4,
                        5,5,5,5,5,         0,0};
*/

/* Debugging setup - each note value corresponds to position in array,
 * so should match position pressed on unit

const byte note[] = {11, 12, 13, 14, 15,
                     21, 22, 23, 24, 25,
                     31, 32, 33, 34, 35, 
                     41, 42, 43, 44, 45,
                     51, 52, 53, 54, 55,  0, 0};
                        
const byte velocity[] = {80,80,80,80,80,
                         80,80,80,80,80,
                         80,80,80,80,80,
                         80,80,80,80,80,
                         80,80,80,80,80,
                         80 80,80,80,80,        0,0};
                            
const byte channel[] = {1,1,1,1,1,
                        1,1,1,1,1,
                        1,1,1,1,1,
                        1,1,1,1,1,
                        1,1,1,1,1,        0,0};

*/

// Shape of You by Ed Sheeran
// Currently the active notes.
// Drums are set up for the first(left) column.
// Bass line can be played using the second column.
// Chords can be played using the third and fourth column,
// with the riff played primarily on the fifth(right) column
// All velocities (volumes) are set to 80, as it is easier to
// adjust the volume on the computer side.

const byte note[] = {42, 35, 49, 57, 71,
                     41, 37, 51, 59, 73, 
                     38, 39, 52, 61, 75,
                     36, 40, 54, 63, 76, 
                     34, 49, 56, 69, 78, 
                     1,   2,  3,  4,  5,      0, 0};
                        
const byte velocity[] = {80,80,80,80,80,
                         80,80,80,80,80,
                         80,80,80,80,80,
                         80,80,80,80,80,
                         80,80,80,80,80,
                         100,5,55,5,05,        0,0};
                            
const byte channel[] = {1,2,3,3,4,
                        1,2,3,3,4,
                        1,2,3,3,4,
                        1,2,3,4,4,
                        1,2,3,4,4,
                        5,5,5,5,5,         0,0};

byte selects;// holds decimal number to be passed to select pins
int inputNum;// used to keep track of which button is being assessed
byte MUX;// used in outer for loop, holds which MUX is being 
         // currently checked


// variables used in timing to eliminate bouncing of contacts
unsigned long timer[NUMBUTTONS];
const unsigned long timeConst = 1;


void setup() {

  // begin MIDI and serial transmissions
  MIDI.begin(MIDI_CHANNEL_OFF);
  Serial.begin(115200);
  
  // set up pins and initialise values for all buttons
  for (int i = 0; i < NUMBUTTONS; i++) {

    // set previousButtonState to 0
    prevButtonState[i] = 0;

    // set msgState to LOW
    msgState[i] = LOW;

    // start timer for each button
    timer[i] = millis();    
  }

  // set each select pin to an output
  for (byte i = 0; i <= 2; i++) {
    pinMode(selectPin[i], OUTPUT);
  }

  // set each MUX pin to an input
  for (byte i = 0; i < MUXarrLength; i++) {
    pinMode(MUXarr[i], INPUT);
  }
}

void loop() {

  // outer loop starts at MUX 0 and works through each MUX
  for (MUX = 0; MUX < MUXarrLength; MUX ++) {

    // cycle through each select for this MUX
    for (selects = 0; selects < 8; selects ++) {

      //set select pins to the input being checked
      setSelectPins(selects);

      // calculates the button number, used to identify
      // the element in each array that should be read/ adjusted
      inputNum = (MUX * 8) + (selects);

      // read state of current button
      currentButtonState[inputNum] = digitalRead(MUXarr[MUX]);

      // if the state of the current button is different from it's
      // last recorded state
      if (currentButtonState[inputNum] != prevButtonState[inputNum]) {

        // if the time since the last time this button was checked is greater
        // than the time constant set. 
        // This timing is used to contact bounce - which is the contact of the
        // button bouncing back and forward, which would cause a constant change
        // MIDI message being sent.
        if ((millis() - timer[inputNum]) > timeConst) {

          // Update the state of the message for this button to whatever
          // it's current state is. e.g. if buttonpin[inputNum] is HIGH, then
          // msgState[inputNum] will also be high, determining a MIDI note on
          // message must be sent
          msgState[inputNum] = currentButtonState[inputNum];
        
          // send new midi message on this button
          sendMIDI(inputNum);
        
          // reset timer for this button
          timer[inputNum] = millis();

        } // end if millis()...
    
    }//end if currentButtonState...

    // Make the current button state of this button the previous state 
    // of the button, for when it is next assessed
    prevButtonState[inputNum] = currentButtonState[inputNum];

    }// end selects loop
    
  }// end MUX loop

}// end loop



/* Sets the three select pins s2, s1, s0 to the binary
 * equivilant of the value passed to it.
 * Instead of 1 and 0 being used, HIGH and LOW are used instead
 */
void setSelectPins(byte val) {
  
  byte i; // used in for loop
  
  // cycle through all select pins
  for (i = 0; i < 3; i++) {
    
    // read specific bits from val, if bit being read is 
    // HIGH then set same select Pin HIGH
    if (bitRead(val, i) == 1) {
      digitalWrite(selectPin[i], HIGH);
    }
    else {
      digitalWrite(selectPin[i], LOW);
    }
    
  } // end for loop
  
}// end setSelectPins



/*  sendMIDI takes the inputNum of the button as it's argument, 
 *  and uses this value to determine the paramenters of the
 *  MIDI message it will send
*/  
void sendMIDI(int inputNum) {
  
  if (msgState[inputNum] == HIGH) {
    MIDI.sendNoteOn(note[inputNum], velocity[inputNum], channel[inputNum]);
  }
  else {
    MIDI.sendNoteOff(note[inputNum], velocity[inputNum], channel[inputNum]);
  }
}// end sendMIDI

