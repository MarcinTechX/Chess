#ifndef ROOK_HPP
#define ROOK_HPP

#include "piece.hpp"

class Rook : public Piece 
{
public:
    Rook(const sf::Texture& texture, float x, float y, Color color)
        : Piece(texture, x, y, Type::Rook, color) {}
};

#endif 