#ifndef BISHOP_HPP
#define BISHOP_HPP

#include "piece.hpp"

class Bishop : public Piece 
{
public:
    Bishop(const sf::Texture& texture, float x, float y, Color color)
        : Piece(texture, x, y, Type::Bishop, color) {} 
};

#endif 