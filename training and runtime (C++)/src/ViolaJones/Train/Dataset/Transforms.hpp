#pragma once

#include <System.h>
#include "../../Shared/Config.hpp"
#include "Util.hpp"

using namespace System;

namespace ViolaJones
{
    /// @brief ROI transform base class.
    abstract class RoiTransform
    {
        public:
            /// @brief Transforms the provided ROI.
            /// @param source Source ROI.
            /// @return Modified ROI.
            virtual Rect Transform(const Rect& source) = 0;
    };

    /// @brief ROI transform that jitters ROI center coordinate and its size.
    class RoiRandomJitterTransform: public RoiTransform
    {
        public:
            /// @brief Constructs a transform.
            /// @param wTranslatePerc X offset jitter expressed as width percent.
            /// @param hTranslatePerc Y offset jitter expressed as height percent.
            /// @param whScalePerc Width scale jitter percent.
            RoiRandomJitterTransform(float wTranslatePerc = 0.03f, float hTranslatePerc = 0.03f, float whScalePerc = 0.05f)
            {
                this->wTranslatePerc = wTranslatePerc;
                this->hTranslatePerc = hTranslatePerc;
                this->whScalePerc = whScalePerc;
            }

            /// @brief Jitters the coordinates of the provided ROI.
            /// @param source Source ROI to transform.
            /// @return Modified ROI.
            Rect Transform(const Rect& source) override
            {
                var [xC, yC, w, h] = source;
                xC += ((rand.NextDouble() * 2 - 1) * w * wTranslatePerc);
                yC += ((rand.NextDouble() * 2 - 1) * h * hTranslatePerc);

                var whRatio = (float)w / h;
                h += ((rand.NextDouble() * 2 - 1) * h * whScalePerc);
                w =  (h * whRatio); //keep the width height ratio

                var rect = Rect { .CenterX = xC, .CenterY = yC, .Width = w, .Height = h };
                rect = Clip(rect);
                return rect;
            }

        private:
            float wTranslatePerc; 
            float hTranslatePerc; 
            float whScalePerc; 

            Random rand;

            /// @brief Clips the source ROI using normalized image boundaries [0, 1].
            /// @param rect Source ROI.
            /// @return Clipped ROI.
            Rect Clip(Rect rect)
            {
                var xC = Math::Min(1.0f, Math::Max(0.0f, rect.CenterX));
                var yC = Math::Min(1.0f, Math::Max(0.0f, rect.CenterY));
                var w  = Math::Min(Math::Min(xC - 0, 1.0f - xC), rect.Width);
                var h  = Math::Min(Math::Min(yC - 0, 1.0f - yC), rect.Height);

                return Rect { .CenterX = xC, .CenterY = yC, .Width = w, .Height = h };
            }
    };
}
