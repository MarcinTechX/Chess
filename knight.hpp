#ifndef KNIGHT_HPP
#define KNIGHT_HPP

#include "piece.hpp"

class Knight : public Piece 
{
public:
    Knight(const sf::Texture& texture, float x, float y, Color color)
        : Piece(texture, x, y, Type::Knight, color) {}
};

#endif 