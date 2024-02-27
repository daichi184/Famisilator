#include <Arduino.h>
#include "common.h"

#include "MyYMZ294.h";
const int WR_PIN = 10;
const int AD_PIN = 11;
const int RT_PIN = 12;
const int D0_PIN = 14;
const int D1_PIN = 15;
const int D2_PIN = 16;
const int D3_PIN = 17;
const int D4_PIN = 18;
const int D5_PIN = 19;
const int D6_PIN = 8;
const int D7_PIN = 9;
MyYMZ294 ymz294_(RT_PIN, WR_PIN, AD_PIN, D0_PIN, D1_PIN, D2_PIN, D3_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN);

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
const int NEOPIXEL_PIN = 13;
const int PIXEL_LEN = 13;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_LEN, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

int const IN_PIN_SL = 2;
int const IN_PIN_CLK = 3;
int const IN_PIN_SER = 4;

const int OUT_PIN_SER   = 7;
const int OUT_PIN_LATCH = 5;
const int OUT_PIN_CLK   = 6;

const int threshold_1 = 1;

byte patterns[2];
void initPatterns(){
    //74HC595が一枚増えるごとに8bitずつ増える
    patterns[0] = 0b0000000000000000;
    patterns[1] = 0b1000000000000000;
}


const int PAD_LEN = 13;
bool bTouchedPads_[PAD_LEN];
bool bTouchedKeys_[PAD_LEN];
int gCnt_[PAD_LEN];
int releasedCnt_[PAD_LEN];

bool bCurSwState_[PAD_LEN];
bool bOldSwState_[PAD_LEN];
const int DEFAULT_OCTAVE = 5;
const int MAX_OCTAVE = 7;
const int MIN_OCTAVE = 1;
int octave_ = DEFAULT_OCTAVE;
const int TONE_SCALE = 12;
int keyboardNo_ = 12+(octave_-1)*TONE_SCALE;

bool bBegin_ = true;

char debugBuf_[50];

uint16_t myShiftIn(int dataPin, int clockPin, int loadPin);
void init74HC165();
void init74HC595();
void onTouched();
bool inputSW(uint16_t _inputCh);
void resetOctave();
void upOcatave();
void downOctave();
void onPressSW(const bool& _cur, bool& _old, int _ch, void (*pFunc)(void));
void onReleaseSW(const bool& _cur, bool& _old, int _ch, void (*pFunc)(void));
void colorWipe(uint32_t c, uint8_t wait);
void blinkNeoPixel(int _pos);
void allOffNeoPixel();

void setup() {
    Serial.begin(115200);
    Serial.println("\n--------------MY TOUCH BOARD--------------");

    init74HC165();
    init74HC595();
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    colorWipe(strip.Color(0, 127, 0), 30);
    colorWipe(strip.Color(0, 0, 0), 30);

    ymz294_.setup();

    initPatterns();

    memset(bTouchedKeys_, 0, PAD_LEN);
    memset(bTouchedPads_, 0, PAD_LEN);
    memset(gCnt_, 0, PAD_LEN);
    memset(bOldSwState_, 0, 3);
    memset(bCurSwState_, 0, 3);
    
    
    Serial.println("--------------START--------------");
}

void loop() {
    digitalWrite(OUT_PIN_LATCH, LOW );
        shiftOut(OUT_PIN_SER, OUT_PIN_CLK, LSBFIRST, patterns[1]);
    digitalWrite(OUT_PIN_LATCH, HIGH );

    onTouched();

    digitalWrite(OUT_PIN_LATCH, LOW );
        shiftOut(OUT_PIN_SER, OUT_PIN_CLK, LSBFIRST, patterns[0]);
    digitalWrite(OUT_PIN_LATCH, HIGH );

    int releasePad;
    for(int pad=0; pad<PAD_LEN; pad++){
        if(bTouchedKeys_[pad] == true){
            keyboardNo_ = 12 + (octave_-1)*TONE_SCALE;

            ymz294_.noteOn(Channel::CH_A, noteNumber[pad+keyboardNo_], 15);

            releasedCnt_[pad] = 0;

            for(uint16_t i=0; i<strip.numPixels(); i++) {
                strip.setPixelColor(i, strip.Color(0, 0, 0));
            }
            
            blinkNeoPixel(pad);
        }
        else{
            //ログを入れるかどうかでwaitの時間が変わるので注意
            int wait = 60;
            releasedCnt_[pad]++;
            
            if(releasedCnt_[pad] > wait){                
                releasePad++;
                releasedCnt_[pad] = wait;
            }
        }
    }

    if(releasePad >= PAD_LEN){
        ymz294_.noteOff(Channel::CH_A);
        allOffNeoPixel();
    }

    int sw1 = 1;
    bCurSwState_[0] = inputSW(sw1+PAD_LEN);
    onPressSW(bCurSwState_[0], bOldSwState_[0], sw1, downOctave);

    int sw2 = 2;
    if(inputSW(sw2+PAD_LEN) == true){
        if(bBegin_) {
            Serial.println("SW_2 OnPress");
            octave_++;
            bBegin_ = false;
        }
    }
    else{
        if(!bBegin_){
            octave_--;
            bBegin_ = true;
        }
    }
    
    int sw3 = 3;
    bCurSwState_[2] = inputSW(sw3+PAD_LEN);
    onPressSW(bCurSwState_[2], bOldSwState_[2], sw3, upOcatave);
}


uint16_t myShiftIn(int dataPin, int clockPin, int loadPin){
    //int 2byteなので、16bitにすると上位1bitが１になるとマイナス値になる
    //マイナス値になると4byte、32bit扱いになり上位ビットに不要なデータが入る
    uint16_t data = 0;
  
    digitalWrite(loadPin, LOW); //A-Hを格納
    digitalWrite(loadPin, HIGH); //確定
    
    data = digitalRead(dataPin); //Hの値を読む
    
    for (int i=1; i<16; i++){
      digitalWrite(clockPin, HIGH);
      data = data << 1 | (digitalRead(dataPin)); //G,F,E...Aの値を読む
      digitalWrite(clockPin, LOW);
    }
    
    return data;
}

void init74HC165(){
    pinMode(IN_PIN_CLK, OUTPUT);
    pinMode(IN_PIN_SL,  OUTPUT);
    pinMode(IN_PIN_SER, INPUT);
  
    digitalWrite(IN_PIN_SL, HIGH);
    digitalWrite(IN_PIN_CLK, LOW);
    
    Serial.println("INIT 74HC165");
}

void init74HC595(){
    pinMode(OUT_PIN_SER,   OUTPUT);
    pinMode(OUT_PIN_LATCH, OUTPUT);
    pinMode(OUT_PIN_CLK,   OUTPUT);

    Serial.println("INIT 74HC595");
}


void onTouched(){  
    uint16_t incomingData = myShiftIn(IN_PIN_SER, IN_PIN_CLK, IN_PIN_SL);
    unsigned ch = 0B0000000000000001;
     
    for(int inputCh=0; inputCh<13; inputCh++){
        bTouchedPads_[inputCh] = incomingData & (ch << inputCh);
        if(bTouchedPads_[inputCh]) gCnt_[inputCh]++;
        else                       gCnt_[inputCh] = 0;

        if(gCnt_[inputCh] >= threshold_1){
            sprintf(debugBuf_, "gCnt[%d] %d", inputCh, gCnt_[inputCh]);
            Serial.println(debugBuf_);
            bTouchedKeys_[inputCh] = true;
            // sprintf(debugBuf_, "OnTouched %d, %d", inputCh, gCnt[inputCh]);
            // Serial.println(debugBuf_);
        }
        else
            bTouchedKeys_[inputCh] = false;
    }
}

bool inputSW(uint16_t _inputCh){
    uint16_t data = 0;
  
    digitalWrite(IN_PIN_SL, LOW); //A-Hを格納
    digitalWrite(IN_PIN_SL, HIGH); //確定

    data = digitalRead(IN_PIN_SER); //Hの値を読む

    for (int i=1; i<16; i++){
      digitalWrite(IN_PIN_CLK, HIGH);
      data = data << 1 | (digitalRead(IN_PIN_SER)); //G,F,E...Aの値を読む
      digitalWrite(IN_PIN_CLK, LOW);
    }

    int a = data >> (_inputCh-1);
    int aa = a & 1;

     
    if(aa == 0) return true;
    
    return false;
}

void resetOctave(){
    octave_ = DEFAULT_OCTAVE;
    sprintf(debugBuf_, "OCTAVE RESET");
    Serial.println(debugBuf_);
}

void upOcatave(){
    if(octave_ < MAX_OCTAVE){
        octave_++;
    }
    sprintf(debugBuf_, "OCTAVE UP %d, KeyBoard No.%d ~", octave_, 3+(octave_-1)*TONE_SCALE);
    Serial.println(debugBuf_);
}


void downOctave(){
    if(octave_ > MIN_OCTAVE){
        octave_--;
    }
    sprintf(debugBuf_, "OCTAVE DOWN %d, KeyBoard No.%d ~", octave_, 3+(octave_-1)*TONE_SCALE);
    Serial.println(debugBuf_);
}

void onPressSW(const bool& _cur, bool& _old, int _ch, void (*pFunc)(void)){
    if(_cur != _old){
        _old = _cur;
        if(_cur){
            sprintf(debugBuf_, "SW_%d OnPress", _ch);
            Serial.println(debugBuf_);
            pFunc();
        }
    }
}

void onReleaseSW(const bool& _cur, bool& _old, int _ch, void (*pFunc)(void)){
    if(_cur != _old){
        _old = _cur;
        if(!_cur){
            sprintf(debugBuf_, "SW_%d OnRelease", _ch);
            Serial.println(debugBuf_);
            pFunc();
        }
    }
}

void colorWipe(uint32_t c, uint8_t wait) {
    for(uint16_t i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
        strip.show();
        delay(wait);
    }
}

void blinkNeoPixel(int _pos){
    float h = octave_ / 7.0;
    uint8_t r, g, b;
    hsv2rgb(h, 1.f, 1.f, r, g, b);
    uint32_t color = strip.Color(r/4, g/4, b/4);
    strip.setPixelColor(_pos, color);
    strip.show();
}

void allOffNeoPixel(){
    for(uint16_t i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
}