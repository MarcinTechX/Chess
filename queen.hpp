#ifndef QUEEN_HPP
#define QUEEN_HPP

#include "piece.hpp"

class Queen : public Piece 
{
public:
    Queen(const sf::Texture& texture, float x, float y, Color color)
        : Piece(texture, x, y, Type::Queen, color) {}
};

#endif 