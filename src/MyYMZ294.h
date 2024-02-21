//参考URL
//https://elchika.com/article/5fa0b7db-359d-4d43-9acf-52c22ecdee3f/
//データシート
//https://akizukidenshi.com/goodsaffix/ymz294.pdf

#pragma once
#include "arduino.h"
#include "MusicalScalse.h"

enum Addresses{
  CH_A_1 = 0x00,
  CH_A_2 = 0x01,
  CH_B_1 = 0x02,
  CH_B_2 = 0x03,
  CH_C_1 = 0x04,
  CH_C_2 = 0x05,
  NOISE  = 0x06,
  MIXER  = 0x07,
  VOLUME_A = 0x08,
  VOLUME_B = 0x09,
  VOLUME_C = 0x0a,
  ENVELOPE_OB = 0x0b,
  ENVELOPE_OC = 0x0c,
  ENVELOPE_SHAPE = 0x0d
};

enum Channel{
    CH_A = 0x00,
    CH_B,
    CH_C,
    NOISE_A,
    NOISE_B,
    NOISE_C
};

enum MixerOnOff{
    ON,
    OFF
};

enum EnvelopeShapes{
    SH_1 = 0b1000,
    SH_2,
    SH_3,
    SH_4,
    SH_5,
    SH_6,
    SH_7, 
    SH_8
};

class MyYMZ294{
private:
    const int WRCS_PIN;
    const int A0_PIN;
    const int RESET_PIN;
    const int CS;
    const int D0;
    const int D1;
    const int D2;
    const int D3;
    const int D4;
    const int D5;
    const int D6;
    const int D7;
    bool isCsAvailable_;
    uint8_t currentMixerVal_ = 0xff;

    void init();
    void initMixer();
    void initSound();

    void setMixer(Channel _ch, MixerOnOff _mixer);
    void setFrequency(Channel _ch, word _freq);
    void setVolume(Channel _ch, uint8_t _vol);

    void setAddress(byte _addr);
    void setData(uint8_t _data);
    void setRegister(byte _data);

public:
    MyYMZ294(int _rt, int _wr, int _cs, int _ad, int _d0, int _d1, int _d2, int _d3, int _d4, int _d5, int _d6, int _d7);
    MyYMZ294(int _rt, int _wr, int _ad, int _d0, int _d1, int _d2, int _d3, int _d4, int _d5, int _d6, int _d7);

    void setup(){
        init();
    }
    
    void noteOn(Channel _ch, int _noteNum, uint8_t _vol);
    void noteOff(Channel _ch);
    
    void setEnvelopeFrequency(uint16_t _freq);
    void setEnvelopeShape(EnvelopeShapes _shape);
};
