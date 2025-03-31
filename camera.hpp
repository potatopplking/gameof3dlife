#pragma once

#include "utilities.hpp"

class CameraBase
{
    public:
        virtual void Pan() = delete;
        virtual void Rotate() = delete;
        virtual void Zoom() = delete;

        
}
