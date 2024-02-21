#include "MyYMZ294.h";

MyYMZ294::MyYMZ294(int _rt, int _wr, int _cs, int _ad, int _d0, int _d1, int _d2, int _d3, int _d4, int _d5, int _d6, int _d7)
:RESET_PIN(_rt),WRCS_PIN(_wr),CS(_cs),A0_PIN(_ad),D0(_d0),D1(_d1),D2(_d2),D3(_d3),D4(_d4),D5(_d5),D6(_d6),D7(_d7),isCsAvailable_(true)
{}

MyYMZ294::MyYMZ294(int _rt, int _wr, int _ad, int _d0, int _d1, int _d2, int _d3, int _d4, int _d5, int _d6, int _d7)
:RESET_PIN(_rt),WRCS_PIN(_wr),A0_PIN(_ad),D0(_d0),D1(_d1),D2(_d2),D3(_d3),D4(_d4),D5(_d5),D6(_d6),D7(_d7),isCsAvailable_(false),CS(10)
{}

void MyYMZ294::init(){
    pinMode(WRCS_PIN, OUTPUT);
    pinMode(A0_PIN, OUTPUT);
    pinMode(RESET_PIN, OUTPUT);
    pinMode(D0, OUTPUT);
    pinMode(D1, OUTPUT);
    pinMode(D2, OUTPUT);
    pinMode(D3, OUTPUT);
    pinMode(D4, OUTPUT);
    pinMode(D5, OUTPUT);
    pinMode(D6, OUTPUT);
    pinMode(D7, OUTPUT);
    if(isCsAvailable_){
        pinMode(CS, OUTPUT);
    }

    digitalWrite(WRCS_PIN, HIGH);
    digitalWrite(A0_PIN, LOW);
    digitalWrite(RESET_PIN, LOW);
    delay(10);
    digitalWrite(RESET_PIN, HIGH);

    Serial.println("INIT YMZ294");

    
    initMixer();    
    initSound();
}

void MyYMZ294::initMixer(){
    setMixer(Channel::NOISE_A, MixerOnOff::OFF);
    setMixer(Channel::NOISE_B, MixerOnOff::OFF);
    setMixer(Channel::NOISE_C, MixerOnOff::OFF);
}

void MyYMZ294::noteOn(Channel _ch, int _noteNum, uint8_t _vol){
    setVolume(_ch, _vol);
    setFrequency(_ch, _noteNum);
    setMixer(_ch, MixerOnOff::ON);
}

void MyYMZ294::noteOff(Channel _ch){
    setMixer(_ch, MixerOnOff::OFF);
}

void MyYMZ294::setMixer(Channel _ch, MixerOnOff _mixer){
    uint8_t mixerValue = 0;
    setAddress(Addresses::MIXER);

    switch (_ch)
    {
        case Channel::CH_A:
            mixerValue = 0b1;
            break;
        case Channel::CH_B:
            mixerValue = 0b10;
            break;
        case Channel::CH_C:
            mixerValue = 0b100;
            break;
        case Channel::NOISE_A:
            mixerValue = 0b1000;
            break;
        case Channel::NOISE_B:
            mixerValue = 0b10000;
            break;
        case Channel::NOISE_C:
            mixerValue = 0b100000;
            break;
        default:
            return;
    }

    if (_mixer == MixerOnOff::ON){
        currentMixerVal_ &= ~mixerValue;
    }
    else{
        currentMixerVal_ |= mixerValue;
    }
    
    setData(currentMixerVal_);
}

void MyYMZ294::setFrequency(Channel _ch, word _freq){
    word cal_freqency = 0;
    if (_freq != 0) {
        cal_freqency = 125000 / _freq;
    }
    cal_freqency &= 0b0000111111111111;

    setAddress(0x00 + (_ch * 2));
    setData(cal_freqency & 0xff);
    setAddress(0x01 + (_ch * 2));
    setData((cal_freqency >> 8) & 0xff);
}

void MyYMZ294::setVolume(Channel _ch, uint8_t _vol){
    byte addr = Addresses::VOLUME_A + _ch;
    uint8_t vol = 0xff;
    vol &= _vol;
    setAddress(addr);
    setData(vol);
}

void MyYMZ294::setEnvelopeFrequency(uint16_t _freq){
    setAddress(Addresses::ENVELOPE_OB);
    setData(_freq & 0x00ff);
    setAddress(Addresses::ENVELOPE_OC);
    setData(_freq >> 8);
}

void MyYMZ294::setEnvelopeShape(EnvelopeShapes _shape){
    setAddress(Addresses::ENVELOPE_SHAPE);
    setData(_shape);

    setVolume(Channel::CH_A, 15);
}

void MyYMZ294::setAddress(byte _addr){
    digitalWrite(A0_PIN, LOW);
    setRegister((byte)_addr);
}

void MyYMZ294::setData(uint8_t _data){
    digitalWrite(A0_PIN, HIGH);
    setRegister((byte)_data);
}

void MyYMZ294::setRegister(byte _data){
    if (isCsAvailable_) 
        digitalWrite(CS, LOW);
    digitalWrite(WRCS_PIN, LOW);
    digitalWrite(D7, bitRead(_data, 7));
    digitalWrite(D6, bitRead(_data, 6));
    digitalWrite(D5, bitRead(_data, 5));
    digitalWrite(D4, bitRead(_data, 4));
    digitalWrite(D3, bitRead(_data, 3));
    digitalWrite(D2, bitRead(_data, 2));
    digitalWrite(D1, bitRead(_data, 1));
    digitalWrite(D0, bitRead(_data, 0));
    digitalWrite(WRCS_PIN, HIGH);
    if (isCsAvailable_) 
        digitalWrite(CS, HIGH);
}


void MyYMZ294::initSound(){
    int vol = 15;

    noteOn(Channel::CH_A, noteNumber[79], vol);
    // noteOn(Channel::CH_B, noteNumber[83], vol);
    // noteOn(Channel::CH_C, noteNumber[86], vol);
    delay(250);
    noteOff(Channel::CH_A);
    // noteOff(Channel::CH_B);
    // noteOff(Channel::CH_C);
    delay(250);
    
    noteOn(Channel::CH_A, noteNumber[79], vol);
    // noteOn(Channel::CH_B, noteNumber[83], vol);
    // noteOn(Channel::CH_C, noteNumber[86], vol);
    delay(250);   
    noteOff(Channel::CH_A);
    // noteOff(Channel::CH_B);
    // noteOff(Channel::CH_C);
    delay(250);
    
    noteOn(Channel::CH_A, noteNumber[79], vol);
    // noteOn(Channel::CH_B, noteNumber[83], vol);
    // noteOn(Channel::CH_C, noteNumber[86], vol);
    delay(250);
    noteOff(Channel::CH_A);
    // noteOff(Channel::CH_B);
    // noteOff(Channel::CH_C);
    
    noteOn(Channel::CH_A, noteNumber[76], vol);
    // noteOn(Channel::CH_B, noteNumber[79], vol);
    // noteOn(Channel::CH_C, noteNumber[83], vol);
    delay(250);

    noteOn(Channel::CH_A, noteNumber[84], vol);
    // noteOn(Channel::CH_B, noteNumber[88], vol);
    // noteOn(Channel::CH_C, noteNumber[91], vol);
    delay(500);
    noteOff(Channel::CH_A);
    // noteOff(Channel::CH_B);
    // noteOff(Channel::CH_C);
}

