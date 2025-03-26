#ifndef BISHOP_HPP
#define BISHOP_HPP

#include "piece.hpp"
#include "board.hpp"

class Board;

class Bishop : public Piece 
{
public:
    Bishop(const sf::Texture& texture, float x, float y, Color color, Board& boardGame);

    bool canMoveImpl(int startRow, int startCol, int endRow, int endCol, bool testMove) override;
};

#endif 