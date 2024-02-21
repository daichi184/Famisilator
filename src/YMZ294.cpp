#include <Arduino.h>
#include "YMZ294.h"

YMZ294::YMZ294(int _rt, int _wr, int _cs, int _ad, int _d0, int _d1, int _d2, int _d3, int _d4, int _d5, int _d6, int _d7)
:RT(_rt),WR(_wr),CS(_cs),AD(_ad),D0(_d0),D1(_d1),D2(_d2),D3(_d3),D4(_d4),D5(_d5),D6(_d6),D7(_d7),isCsAvailable_(true)
{}

YMZ294::YMZ294(int _rt, int _wr, int _ad, int _d0, int _d1, int _d2, int _d3, int _d4, int _d5, int _d6, int _d7)
:RT(_rt),WR(_wr),AD(_ad),D0(_d0),D1(_d1),D2(_d2),D3(_d3),D4(_d4),D5(_d5),D6(_d6),D7(_d7),isCsAvailable_(false)
{}

void YMZ294::setRegister(byte _addr, byte _value){
    // addr
    if (isCsAvailable_) {
        digitalWrite(CS, LOW);
    }
    digitalWrite(WR, LOW);
    digitalWrite(AD, LOW);
    digitalWrite(D7, _addr & 0b10000000);
    digitalWrite(D6, _addr & 0b01000000);
    digitalWrite(D5, _addr & 0b00100000);
    digitalWrite(D4, _addr & 0b00010000);
    digitalWrite(D3, _addr & 0b00001000);
    digitalWrite(D2, _addr & 0b00000100);
    digitalWrite(D1, _addr & 0b00000010);
    digitalWrite(D0, _addr & 0b00000001);
    digitalWrite(WR, HIGH);
    if (isCsAvailable_) {
        digitalWrite(CS, HIGH);
    }

    // value
    digitalWrite(WR, LOW);
    digitalWrite(AD, HIGH);
    digitalWrite(D7, _addr & 0b10000000);
    digitalWrite(D6, _addr & 0b01000000);
    digitalWrite(D5, _addr & 0b00100000);
    digitalWrite(D4, _addr & 0b00010000);
    digitalWrite(D3, _addr & 0b00001000);
    digitalWrite(D2, _addr & 0b00000100);
    digitalWrite(D1, _addr & 0b00000010);
    digitalWrite(D0, _addr & 0b00000001);
    digitalWrite(WR, HIGH);
}

void YMZ294::begin(){
    pinMode(D0, OUTPUT);
    pinMode(D1, OUTPUT);
    pinMode(D2, OUTPUT);
    pinMode(D3, OUTPUT);
    pinMode(D4, OUTPUT);
    pinMode(D5, OUTPUT);
    pinMode(D6, OUTPUT);
    pinMode(D7, OUTPUT);
    pinMode(AD, OUTPUT);
    pinMode(WR, OUTPUT);
    pinMode(RT, OUTPUT);

    if (isCsAvailable_) {
        pinMode(CS, OUTPUT);
        digitalWrite(CS, HIGH);
    }

    setRegister(0x00, 0);
    setRegister(0x01, 0);
    setRegister(0x02, 0);
    setRegister(0x03, 0);
    setRegister(0x04, 0);
    setRegister(0x05, 0);
    setRegister(0x06, 01);
  
    setRegister(0x07, 0);
    setRegister(0x08, 0);
    setRegister(0x09, 0);
    setRegister(0x0a, 0);
  
    setRegister(0x0b, 0);
    setRegister(0x0c, 0);
    setRegister(0x0d, 0);

    digitalWrite(WR, HIGH);
    digitalWrite(AD, LOW);
    digitalWrite(RT, LOW);
    delay(10); 
    digitalWrite(RT, HIGH);


    setVolume(Channel::CH_A, 0x0f);
    setVolume(Channel::CH_B, 0x0f);
    setVolume(Channel::CH_C, 0x0f);
    setMixer(Channel::NOISE_1, MixerOnOff::OFF);
    setMixer(Channel::NOISE_2, MixerOnOff::OFF);
    setMixer(Channel::NOISE_3, MixerOnOff::OFF);

}

void YMZ294::noteOn(Channel _ch, uint8_t _noteNum){
    setVolume(Channel::CH_A, 0x0f);
    setFrequency(_ch, _noteNum);
    setMixer(_ch, MixerOnOff::ON);
}

void YMZ294::noteOff(Channel _ch){
    setMixer(_ch, MixerOnOff::OFF);
}

void YMZ294::selectAddressData(AddressData _ad){
    digitalWrite(AD, _ad);
}

void YMZ294::setData(uint8_t _data){
    selectAddressData(AddressData::DATA);
    setDataBus(_data);
}

void YMZ294::setDataBus(uint8_t _data){
    if (isCsAvailable_) {
        digitalWrite(CS, LOW);
    }
    digitalWrite(WR, LOW);    
    digitalWrite(D0, bitRead((byte)_data, 0));
    digitalWrite(D1, bitRead((byte)_data, 1));
    digitalWrite(D2, bitRead((byte)_data, 2));
    digitalWrite(D3, bitRead((byte)_data, 3));
    digitalWrite(D4, bitRead((byte)_data, 4));
    digitalWrite(D5, bitRead((byte)_data, 5));
    digitalWrite(D6, bitRead((byte)_data, 6));
    digitalWrite(D7, bitRead((byte)_data, 7));
    digitalWrite(WR, HIGH);
    if (isCsAvailable_) {
        digitalWrite(CS, HIGH);
    }
}

void YMZ294::setAddress(Addresses _data){
    selectAddressData(AddressData::ADDRESS);
    setDataBus(_data);
}

void YMZ294::setFrequency(Channel _ch, word _freq){
    Addresses MSBaddr;
    Addresses LSBaddr;
    switch (_ch)
    {
        case Channel::CH_A:
            LSBaddr = Addresses::CH_A_1;
            MSBaddr = Addresses::CH_A_2;
            break;
        case Channel::CH_B:
            LSBaddr = Addresses::CH_B_1;
            MSBaddr = Addresses::CH_B_2;
            break;
        case Channel::CH_C:
            LSBaddr = Addresses::CH_C_1;
            MSBaddr = Addresses::CH_C_2;
            break;
        default:
            return;
    }

    word cal_freqency = 0;
    if (_freq != 0) {
        cal_freqency = 125000 / _freq;
    }
    cal_freqency &= 0b0000111111111111;

    setAddress(LSBaddr);
    setData(cal_freqency & 0xff);
    setAddress(MSBaddr);
    setData((cal_freqency >> 8) & 0xff);
}


void YMZ294::setVolume(Channel _ch, uint8_t _volume){
    Addresses addr;
    switch (_ch)
    {
    case Channel::CH_A:
        addr = Addresses::VOLUME_A;
        break;
    case Channel::CH_B:
        addr = Addresses::VOLUME_B;
        break;
    case Channel::CH_C:
        addr = Addresses::VOLUME_C;
        break;
    default:
        return;
    }
    Serial.println("setVolume");
    setAddress(addr);
    setData(_volume & 0x0f);
}

void YMZ294::setMixer(Channel _ch, MixerOnOff _mixer){
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
        case Channel::NOISE_1:
            mixerValue = 0b1000;
            break;
        case Channel::NOISE_2:
            mixerValue = 0b10000;
            break;
        case Channel::NOISE_3:
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

void YMZ294::setEnvelopeFrequency(uint16_t _freq){
    setAddress(Addresses::ENVELOPE_FREQ_LSB);
    setData(_freq & 0x00ff);
    setAddress(Addresses::ENVELOPE_FREQ_MSB);
    setData(_freq >> 8);
}

void YMZ294::setEnvelopeShape(EnvelopeShapes _shape){
    setAddress(Addresses::ENVELOPE_SHAPE);
    setData(_shape);
    setAddress(Addresses::VOLUME_A);
    setData(0x0f);
}