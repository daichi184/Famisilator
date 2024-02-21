#pragma once
#include "MusicalScalse.h"

enum AddressData{
    ADDRESS = 0,
    DATA = 1
};

enum Addresses{
    CH_A_1,
    CH_A_2,
    CH_B_1,
    CH_B_2,
    CH_C_1,
    CH_C_2,
    NOISE,
    MIXER,
    VOLUME_A,
    VOLUME_B,
    VOLUME_C,
    ENVELOPE_FREQ_LSB,
    ENVELOPE_FREQ_MSB,
    ENVELOPE_SHAPE
};

enum Channel{
    CH_A,
    CH_B, 
    CH_C,
    NOISE_1,
    NOISE_2,
    NOISE_3
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

class YMZ294{
private:
    const int WR;
    const int CS;
    const int RT;
    const int AD;
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

public:
    YMZ294(int _rt, int _wr, int _cs, int _ad, int _d0, int _d1, int _d2, int _d3, int _d4, int _d5, int _d6, int _d7);
    YMZ294(int _rt, int _wr, int _ad, int _d0, int _d1, int _d2, int _d3, int _d4, int _d5, int _d6, int _d7);

    void begin();
    void noteOn(Channel _ch, uint8_t _noteNum);
    void noteOff(Channel _ch);
    void selectAddressData(AddressData _ad);
    void setDataBus(uint8_t _data);
    void setData(uint8_t _data);
    void setAddress(Addresses _data);
    void setFrequency(Channel _ch, word _freq);
    void setVolume(Channel _ch, uint8_t _volume);
    void setMixer(Channel _ch, MixerOnOff _mixer);
    void setEnvelopeFrequency(uint16_t _freq);
    void setEnvelopeShape(EnvelopeShapes _shape);

    void setRegister(byte _addr, byte _value);
};