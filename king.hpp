#ifndef KING_HPP
#define KING_HPP

#include "piece.hpp"

class King : public Piece 
{
public:
    King(const sf::Texture& texture, float x, float y, Color color)
        : Piece(texture, x, y, Type::King, color) {}
};

#endif 