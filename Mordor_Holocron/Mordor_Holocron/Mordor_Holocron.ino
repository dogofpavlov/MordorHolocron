#include <FastLED.h>

#include <Bounce2.h>
#include <ButtonEvents.h>
#include <DFPlayerMini_Fast.h>

//SOUND DECLARATIONS START
#include "SoftwareSerial.h"
const int RX_PIN = 11;
const int TX_PIN = 12;
SoftwareSerial mySerial(RX_PIN, TX_PIN);

DFPlayerMini_Fast myMP3;

//sound indexes are 1 based index not 0
const int SOUNDS_TOTAL = 13;
int CUR_SOUND = 1;//Starting on music

bool canPlaySound = true;
bool canLoopSound = false;
bool canLoopAllSound = false;

const int SOUND_MODE_SINGLE_PLAY = 0;
const int SOUND_MODE_SINGLE_STOP = 1;
const int SOUND_MODE_LOOP_PLAY = 2;
const int SOUND_MODE_LOOP_STOP = 3;
const int SOUND_MODE_CONTINUE_PLAY = 4;
const int SOUND_MODE_CONTINUE_STOP = 5;

int SOUND_MODE = SOUND_MODE_SINGLE_PLAY;

//SOUND END


#define NUM_LEDS 52
#define LED_PIN 2
#define BTN_PIN 10


CRGB leds[NUM_LEDS];

int animationModesTotal = 7;
int animationModeIndex = 0;//default fade in and out


ButtonEvents primaryButton = ButtonEvents(); 


void setup() {
  Serial.begin(115200);
  
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(60);

  initButtons();
  initSound();
}


void loop() {
  loopButtons();
  loopLEDs();
}

/**
 * HELPER FUNCTIONS
 */

int getRelativeIndex(int index){
    int relativeIndex = index;
    if(index>NUM_LEDS-1){
       relativeIndex = index-NUM_LEDS;
    }
    if(index<0){
      relativeIndex = NUM_LEDS+index;
    }
   return relativeIndex;
}


void nextSongAndAnimation(){
  nextAnimation();
  nextSound();
}
void prevSongAndAnimation(){
  prevAnimation();
  prevSound();
}

/**
 * SOUND
 */

void initSound(){
  mySerial.begin(9600);
  myMP3.begin(mySerial, true);
  delay(2000);
  myMP3.volume(30);
  delay(1000);
  myMP3.play(CUR_SOUND);
}

void playCurrentSound(){
  canPlaySound=true;
  myMP3.play(CUR_SOUND);  
}
void playLoopSound(){
  canLoopSound=true;
  myMP3.loop(CUR_SOUND);
}
void playLoopAllSound(){
  canLoopAllSound=true;
  myMP3.startRepeatPlay();
}

void stopSound(){

  if(canLoopAllSound){
    canLoopAllSound=false;
    myMP3.stopRepeatPlay();
  }else{
    canPlaySound=false;
    canLoopSound=false;
    myMP3.stop();
  }
}

void nextSoundIndex(){
  CUR_SOUND++;
  if(CUR_SOUND==SOUNDS_TOTAL+1){
    CUR_SOUND=1;
  }  
}
void prevSoundIndex(){
  CUR_SOUND--;
  if(CUR_SOUND==0){
    CUR_SOUND=SOUNDS_TOTAL;
  }
}

void nextSound(){
  if(canPlaySound){
    nextSoundIndex();
    playCurrentSound();
  }
  if(canLoopSound){
    nextSoundIndex();
    playLoopSound();
  }
  if(canLoopAllSound){
    myMP3.playNext();
  }
}
void prevSound(){
  if(canPlaySound){
    prevSoundIndex();
    playCurrentSound();
  }
  if(canLoopSound){
    prevSoundIndex();   
    playLoopSound(); 
  }
  if(canLoopAllSound){
    myMP3.playPrevious();
  }
}





/**
 * BUTTONS
 */

void initButtons(){

  primaryButton.attach(BTN_PIN, INPUT);  
  primaryButton.activeHigh();
  primaryButton.debounceTime(15);
  primaryButton.doubleTapTime(500);
  primaryButton.holdTime(2000);
  
}

void nextSoundMode(){
  switch(SOUND_MODE){
    case SOUND_MODE_SINGLE_PLAY:
      soundModeSingleStop();
    break;
    case SOUND_MODE_SINGLE_STOP:
      soundModeLoopPlay();
    break;
    case SOUND_MODE_LOOP_PLAY:
      soundModeLoopStop();
    break;
    case SOUND_MODE_LOOP_STOP:  
      soundModeContinuePlay(); 
    break;
    case SOUND_MODE_CONTINUE_PLAY:
      soundModeContinueStop();
    break;
    case SOUND_MODE_CONTINUE_STOP: 
      soundModeSinglePlay(); 
    break;
  }
}

void soundModeSinglePlay(){    
  SOUND_MODE = SOUND_MODE_SINGLE_PLAY;
  playCurrentSound();  
}
void soundModeSingleStop(){
  SOUND_MODE = SOUND_MODE_SINGLE_STOP;
  stopSound();
}
void soundModeLoopPlay(){
  SOUND_MODE = SOUND_MODE_LOOP_PLAY;
  playLoopSound();
}
void soundModeLoopStop(){
  SOUND_MODE = SOUND_MODE_LOOP_STOP;
  stopSound();
}
void soundModeContinuePlay(){
  SOUND_MODE = SOUND_MODE_CONTINUE_PLAY;
  playLoopAllSound();   
}
void soundModeContinueStop(){
  SOUND_MODE = SOUND_MODE_CONTINUE_STOP;
  stopSound();  
}


void loopButtons(){
  bool changed = primaryButton.update();

  // Was the button pushed?
  if (changed){
    int event = primaryButton.event(); // Get how the button was pushed

    switch(event){
      case(tap):
        Serial.println(F("Primary button single press..."));
        nextSongAndAnimation();  
        break;
      case (doubleTap):
        Serial.println(F("Primary button double press..."));
        prevSongAndAnimation();
        break;
      case (hold):
        Serial.println(F("Primary button long press..."));
        nextSoundMode();        
        break;
    }
  }
}


/*
 * LEDS
 */

void nextAnimation(){
  animationModeIndex++;
  if(animationModeIndex==animationModesTotal){
    animationModeIndex=0;
  }  
}
void prevAnimation(){
  animationModeIndex--;
  if(animationModeIndex<0){
    animationModeIndex=animationModesTotal-1;
  }  
}
 
void loopLEDs(){

  switch(animationModeIndex){
    case 0:
      modeFadeChaos();
    break;
    case 1:
      modeRain();
    break;
    case 2:
      resetLEDs();
      modeWaveForward();
    break;
    case 3:
      resetLEDs();
      modeWaveBackward();
    break;
    case 4:
      resetLEDs();
      modeWaveChaos();
    break;
    case 5:
      modeColorRed();
    break;
    case 6:
      modeColorBlue();
    break;
  }
  
  FastLED.show();
  
}

void resetLEDs(){
 //brightness reset
  for(int i = 0; i< NUM_LEDS; i++){
    leds[i] = CHSV(0,255,0);
  }
}

/**
 * FADE ANIMATION
 */

int fadeIndex = 0;
int fadeSteps = 50;
int fadePerStep = ((int)255/fadeSteps);
int fadeDir = 1;


int fadeStepsToChangeCur = 0;
int fadeStepsToChangeOnTotal = 200;
int fadeStepsToChangeOffTotal = 30;
int fadeColor = 0;
int fadeMinimum = 0;

 
void modeFadeChaos(){
  EVERY_N_MILLISECONDS(15){

    int brightness = fadePerStep*fadeIndex;
    if(brightness>255){
      brightness = 255;
    }
    if(brightness<fadeMinimum){
      brightness = fadeMinimum;
    }
    
    for(int i = 0; i< NUM_LEDS; i++){
      leds[i] = CHSV(fadeColor,255,brightness);
    }
    fadeIndex=fadeIndex+fadeDir;
    
    if(fadeIndex>=fadeSteps-1){

      fadeStepsToChangeCur++;
      if(fadeStepsToChangeCur==fadeStepsToChangeOnTotal){
        fadeStepsToChangeCur=0;
        fadeDir = -1;
        fadeIndex = fadeSteps-1;
        
        fadeMinimum = ((int)random(20,80));
        int randomForceMinimumZero = ((int)random(0,3));
        if(randomForceMinimumZero<1){
          fadeMinimum = 0;
        }
      }
      
    }
    if(fadeIndex<=-1){
      fadeStepsToChangeCur++;
      if(fadeStepsToChangeCur==fadeStepsToChangeOffTotal){
        fadeStepsToChangeCur=0;
        fadeDir = 1;
        fadeIndex = 0;
        
        fadeStepsToChangeOnTotal = ((int)random(100,300));

        if(fadeMinimum==0){
          fadeColor = ((int)random(0,15));
          

          int randomForceRed = ((int)random(0,5));
          if(randomForceRed>3){
            fadeColor = 0;
          }
        }


      }
      
    }
  } 
}


/**
 * SOLID COLOR RED
 */

 int modeColorValue = 0;
 int modeColorSat = 255;
 int modeColorBrightness = 255;

 void modeColorRed(){
  modeColorValue = 0;
  modeColorSat = 255;
  modeColorBrightness = 255;
  modeColor();
 }
 void modeColorBlue(){
  modeColorValue = 160;
  modeColorSat = 255;
  modeColorBrightness = 255;
  modeColor();
 }
 void modeColor(){
    for(int i = 0; i< NUM_LEDS; i++){
        leds[i] = CHSV(modeColorValue,modeColorSat,255);        
    }
 }

/**
 * RAIN ANIMATION
 */


void modeRain(){

  EVERY_N_MILLISECONDS(30){
    for(int i = 0; i< NUM_LEDS; i++){

      int rainColor = ((int)random(0,20));
      
      int brightness = ((int)random(0,255));

      int canChange = ((int)random(0,10));

      if(canChange>8){
        leds[i] = CHSV(rainColor,255,brightness);

        
      }
      
    }

  }

}


/**
 * WAVE ANIMATION
 */


int curWaveIndex = 0;
uint8_t waveSideBuffer = 7;
uint8_t waveSize = 3;

int waveColorFrom = 0;
int waveColorTo = 20; //orange
int waveColorIndex = 0;
int waveColorSteps = 20;
int waveColorDir = 1;

int waveColorRange = waveColorTo-waveColorFrom;
int waveColorStep =  ((int)waveColorRange/waveColorSteps);

void modeWaveForward(){
  wave();
  EVERY_N_MILLISECONDS(30){
    waveMoveForward(); 
  }
}
void modeWaveBackward(){
  wave();
  EVERY_N_MILLISECONDS(30){
    waveMoveBackward();
  } 
}
void modeWaveChaos(){
  wave();
  EVERY_N_MILLISECONDS(30){
    waveMoveChaos();
  } 
}

void wave(){
  int waveLoopColor = waveColorFrom+(waveColorStep*waveColorIndex);
  
  //wave
  for(int i=0; i<waveSize; i++){
    int index = getRelativeIndex(curWaveIndex+i);
    leds[index] = CHSV(waveLoopColor,255,255);    
  }
  
  int bufferStep = ((int)255/waveSideBuffer);
  
  //wave buffer
  for(int i=0; i<waveSideBuffer; i++){ 
    int brightness = 255-(bufferStep*i);
    
    //front
    int indexF = getRelativeIndex(curWaveIndex+waveSize+i);   
    leds[indexF] = CHSV(waveLoopColor,255,brightness);    

    //back
    int indexB = getRelativeIndex(curWaveIndex-(i+1));
    leds[indexB] = CHSV(waveLoopColor,255,brightness);
  }

  EVERY_N_MILLISECONDS(300){ 
    waveColorIndex = waveColorIndex+waveColorDir;
    if(waveColorIndex==waveColorSteps){
      waveColorDir = -1;
      waveColorIndex = waveColorSteps+waveColorDir;
    }
    if(waveColorIndex==-1){
      waveColorDir = 1;
      waveColorIndex = 1;
    }
  }
}



int chaosStepIndex = 0;
int chaosStepsToChange = 10;
int chaosDir = 1;

void waveMoveChaos(){

  chaosStepIndex++;
  if(chaosStepIndex == chaosStepsToChange){
    chaosStepIndex = 0;
    chaosStepsToChange = ((int)random(5,100));
    if(chaosDir==1){
      chaosDir=-1;
    }else{
      chaosDir=1;
    }    
  }
  curWaveIndex = curWaveIndex+chaosDir;  
  wrapWaveIndex();
}

void waveMoveForward(){
    curWaveIndex++;
    wrapWaveIndex();
}
void waveMoveBackward(){
    curWaveIndex--;
    wrapWaveIndex();  
}

void wrapWaveIndex(){
    if(curWaveIndex>NUM_LEDS-1){
      curWaveIndex = 0;
    }
    if(curWaveIndex==-1){
      curWaveIndex = NUM_LEDS-1;
    }
}
