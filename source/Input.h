#pragma once

#include <cstring>
#include <cstdint>

class Input {
public:
    //!Constructor
    Input() = default;

    //!Destructor
    virtual ~Input() = default;

    typedef struct {
        uint32_t buttons_h;
        uint32_t buttons_d;
        uint32_t buttons_r;
        bool validPointer;
        bool touched;
        float pointerAngle;
        int32_t x;
        int32_t y;
    } PadData;

    PadData data{};
    PadData lastData{};
};
