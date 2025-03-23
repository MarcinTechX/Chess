#include "ColorConverter.hpp"

ColorConverter::HSV ColorConverter::rgbToHsv(const RGB& rgb) 
{
    double r = rgb.r / 255.0;
    double g = rgb.g / 255.0;
    double b = rgb.b / 255.0;

    double maxVal = std::max({r, g, b});
    double minVal = std::min({r, g, b});
    double delta = maxVal - minVal;

    HSV hsv;
    hsv.v = maxVal;

    if (delta == 0) 
    {
        hsv.h = 0;
        hsv.s = 0;
    } 
    else 
    {
        hsv.s = delta / maxVal;

        if (maxVal == r) 
        {
            hsv.h = 60 * fmod(((g - b) / delta), 6);
        } else if (maxVal == g) 
        {
            hsv.h = 60 * (((b - r) / delta) + 2);
        } else 
        {
            hsv.h = 60 * (((r - g) / delta) + 4);
        }

        if (hsv.h < 0) 
        {
            hsv.h += 360;
        }
    }

    return hsv;
}

ColorConverter::RGB ColorConverter::hsvToRgb(const HSV& hsv) 
{
    double c = hsv.v * hsv.s;
    double x = c * (1 - fabs(fmod(hsv.h / 60.0, 2) - 1));
    double m = hsv.v - c;

    double r = 0, g = 0, b = 0;

    if (hsv.h >= 0 && hsv.h < 60) 
    {
        r = c; g = x; b = 0;
    } else if (hsv.h >= 60 && hsv.h < 120) 
    {
        r = x; g = c; b = 0;
    } else if (hsv.h >= 120 && hsv.h < 180) 
    {
        r = 0; g = c; b = x;
    } else if (hsv.h >= 180 && hsv.h < 240) 
    {
        r = 0; g = x; b = c;
    } else if (hsv.h >= 240 && hsv.h < 300) 
    {
        r = x; g = 0; b = c;
    } else 
    {
        r = c; g = 0; b = x;
    }

    RGB rgb;
    rgb.r = static_cast<int>((r + m) * 255);
    rgb.g = static_cast<int>((g + m) * 255);
    rgb.b = static_cast<int>((b + m) * 255);

    return rgb;
}

ColorConverter::HSV ColorConverter::adjustValue(const HSV& hsv) 
{
    HSV adjustedHSV = hsv;

    if (adjustedHSV.v < 0.5) 
    {
        adjustedHSV.v = std::clamp(adjustedHSV.v + 0.3, 0.0, 1.0);
    } else 
    {
        adjustedHSV.v = std::clamp(adjustedHSV.v - 0.3, 0.0, 1.0);
    }

    return adjustedHSV;
}

ColorConverter::HSV ColorConverter::adjustSaturation(const HSV& hsv) 
{
    HSV adjustedHSV = hsv;

    if (adjustedHSV.s < 0.5) 
    {
        adjustedHSV.s = std::clamp(adjustedHSV.s + 0.3, 0.0, 1.0);
    } else 
    {
        adjustedHSV.s = std::clamp(adjustedHSV.s - 0.3, 0.0, 1.0);
    }

    return adjustedHSV;
}