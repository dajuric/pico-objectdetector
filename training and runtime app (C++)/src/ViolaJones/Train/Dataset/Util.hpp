#pragma once

#include <System.h>
using namespace System;

namespace ViolaJones
{
    /// @brief A YOLOv3 rectangle containing normalized coordinates.
    struct Rect
    {
        float CenterX;
        float CenterY;
        float Width;
        float Height;

        operator Tuple<float, float, float, float>()
        {
            return Tuple<float, float, float, float>(CenterX, CenterY, Width, Height);
        }
    };
}