
void hsv2rgb(float _h, float _s, float _v, uint8_t& _r, uint8_t& _g, uint8_t& _b){
    float phase, ofs, d1, d2, d3;

    if(_s == 0){
        _r = floor(_v*255);
        _g = floor(_v*255);
        _b = floor(_v*255);
    }
    else {
        phase = (_h - floor(_h))*6.f;
        ofs = phase - floor(phase);
        d1 = _v * (1.f - _s);
        d2 = _v * (1.f - _s * ofs);
        d3 = _v * (1.f - _s * (1.f-ofs));

        switch((int)phase){
            case 0:
                _r = floor(_v * 255.f);
                _g = floor(d3 * 255.f);
                _b = floor(d1 * 255.f);
                break;

            case 1:
                _r = floor(d2 * 255.f);
                _g = floor(_v * 255.f);
                _b = floor(d1 * 255.f);
                break;

            case 2:
                _r = floor(d1 * 255.f);
                _g = floor(_v * 255.f);
                _b = floor(d3 * 255.f);
                break;

            case 3:
                _r = floor(d1 * 255.f);
                _g = floor(d2 * 255.f);
                _b = floor(_v * 255.f);
                break;

            case 4:
                _r = floor(d3 * 255.f);
                _g = floor(d1 * 255.f);
                _b = floor(_v * 255.f);
                break;

            case 5:
                _r = floor(_v * 255.f);
                _g = floor(d1 * 255.f);
                _b = floor(d2 * 255.f);
                break;
        }
    }
    
}
