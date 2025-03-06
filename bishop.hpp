#ifndef BISHOP_HPP
#define BISHOP_HPP

#include "piece.hpp"

class Bishop : public Piece 
{
public:
    Bishop(const sf::Texture& texture, float x, float y, Color color);

    bool canMove(int startRow, int startCol, int endRow, int endCol, std::array<std::array<std::unique_ptr<Piece>, 8>, 8>& board);
};

#endif 