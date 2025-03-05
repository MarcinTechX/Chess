#ifndef KING_HPP
#define KING_HPP

#include "Piece.hpp"

class King : public Piece {
public:
    King(const sf::Texture& texture, float x, float y, Color color);
    bool isValidMove(float x, float y) const override; 
};

#endif
