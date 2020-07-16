/*
 * Created by: Herman Lin
 * GitHub: https://github.com/HermanLin/Arduino-Simon-Says
 * 7-16-2020
 */


const int difficulty = 300; // the lower, the more difficult


// LEDs
const int RED = 2;
const int BLU = 3;
const int GRN = 4;
const int YLW = 5;

// Pushbuttons and buzzer
const int redPin = 6;
const int bluPin = 7;
const int grnPin = 8;
const int ylwPin = 9;
const int simonPin = A1;
const int startPin = 10;
const int buzPin = 11;

// frequencies for the buzzer
const int light_tones[] = {329, 195, 392, 261};
const int start_tones_order[] = {3,3,3,3,0,0,0,0,1,1,1,1,2,2,2,2,3,3,0,1,2};
int START_TONE_SIZE = sizeof(start_tones_order) / sizeof(start_tones_order[0]);
const int lose_tones[] = {440, 415, 392, 370};

// variables for game operation
bool lost = false;
int seq_count = 0;
int sequence[256]; // maximum sequence length of 256
int new_led, note, reply;

/**************
 * void setup()
 **************/
void setup() {  
  // set pin modes
  pinMode(RED, OUTPUT);
  pinMode(BLU, OUTPUT);
  pinMode(GRN, OUTPUT);
  pinMode(YLW, OUTPUT);
  pinMode(buzPin, OUTPUT);
  pinMode(simonPin, INPUT);
  pinMode(startPin, INPUT);

  // initialize a random seed for random to use
  randomSeed(analogRead(0));
}

/*************************************************
 *  void sequencer()
 * - adds another LED to the Simon Says sequence
 * - blinks each LED and plays its respective note
 *************************************************/
void sequencer() {
  // add a new LED to the sequence
  new_led = random(2, 6);
  sequence[seq_count++] = new_led;
  
  // play entire sequence
  for (size_t i = 0; i < seq_count; i++) {
    tone(buzPin, light_tones[sequence[i] - 2]);
    digitalWrite(sequence[i], HIGH);
    delay(difficulty);
    digitalWrite(sequence[i], LOW);
    noTone(buzPin);
    delay(difficulty);
  }

  changeTurn();
}

/**************************************************
 * bool checker()
 * - checks the player's input against the sequence
 * - returns true if a wrong input is recorded
 * - otherwise, false
 **************************************************/
bool checker() {
  for (size_t i = 0; i < seq_count; i++) {
    reply = ledSelect();

    // blink corresponding LED
    digitalWrite(reply, HIGH);
    tone(buzPin, light_tones[reply - 2]);
    delay(difficulty);
    digitalWrite(reply, LOW);
    noTone(buzPin);
    
    if (reply != sequence[i]) {
      return true;
    }
    // this delay can be altered to account for reaction timings
    // when pressing down the pushbutton. a longer delay will allow
    // for longer presses to be counted as one input
    delay(150);
  }
  return false;
}

/*********************************************
 * void soundPlayer(int type)
 * - plays notes depending on type
 * - 0, plays the start sounds when game loads
 * - 1, plays losing sound when player loses
 *********************************************/
void soundPlayer(int type) {
  if (type == 0) { // start sounds
    for (size_t i = 0; i < START_TONE_SIZE; i++) {
      tone(buzPin, light_tones[start_tones_order[i]]);
      delay(150);
      noTone(buzPin);
    }
  }
  else if (type == 1) { // lose sounds
    for (size_t i = 0; i < 4; i++) {
      tone(buzPin, lose_tones[i]);
      delay(600);
    }
    delay(400);
    noTone(buzPin);
  }
}

/**************************************************
 * int ledSelect()
 * - returns the respective led pin for each button
 * - waits until input is received
 **************************************************/
int ledSelect() {
  while (true) { 
    if (note = analogRead(A1)) {
      if (note >= 1020) return 2; // red LED
      else if (note >= 990 && note <= 1010) return 3; // blue LED
      else if (note >= 505 && note <= 550) return 4; // green LED
      else if (note >= 4 && note <= 20) return 5; // yellow LED
    }
    delay(150);
  }
}

/***********************************************
 * void changeTurn()
 * - blinks all the LEDs to signal a turn change
 * - happens between Simon's and player's turns
 ***********************************************/
void changeTurn() {
  for (size_t i = 2; i < 6; i++) digitalWrite(i, HIGH);
  delay(650);
  for (size_t i = 2; i < 6; i++) digitalWrite(i, LOW);
  delay(200);
}

/********************************************
 * void blinkScore()
 * - blinks the final score on the yellow LED
 ********************************************/
void blinkScore() {
  for (size_t i = 0; i < seq_count - 1; i++) {
    digitalWrite(YLW, HIGH);
    delay(400);
    digitalWrite(YLW, LOW);
    delay(400);
  }
}

/*************
 * void loop()
 *************/
void loop() {
  if (digitalRead(startPin) == HIGH) {
    delay(50);
    if (digitalRead(startPin) == HIGH) { // simple debounce
      soundPlayer(0);
      // reset variables
      randomSeed(random(analogRead(0))); // generate random new sequence
      seq_count = 0; 
      lost = false;  
      delay(1500);

      while (true) {
        sequencer(); // run sequence
        lost = checker(); // run player check
        if (lost) {
          delay(100);
          soundPlayer(1); // play losing tones
          delay(100);
          blinkScore();
          break;
        }
        delay(500);
        changeTurn();
      }
    }
  }
}
