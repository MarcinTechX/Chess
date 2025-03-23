// ColorConverter.hpp
#ifndef COLOR_CONVERTER_HPP
#define COLOR_CONVERTER_HPP

#include <algorithm>
#include <cmath>

class ColorConverter 
{
public:
    struct RGB
    {
        int r, g, b; 
    };

    struct HSV 
    {
        double h;
        double s; 
        double v; 
    };

    static HSV rgbToHsv(const RGB& rgb);
    static RGB hsvToRgb(const HSV& hsv);
    static HSV adjustValue(const HSV& hsv);
    static HSV adjustSaturation(const HSV& hsv);
};

#endif 