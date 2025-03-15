#ifndef KNIGHT_HPP
#define KNIGHT_HPP

#include "piece.hpp"
#include "board.hpp"

class Board;

class Knight : public Piece 
{
public:
    Knight(const sf::Texture& texture, float x, float y, Color color, Board& boardGame);

    bool canMoveImpl(int startRow, int startCol, int endRow, int endCol) override;
};

#endif 