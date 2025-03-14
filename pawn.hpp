#ifndef PAWN_HPP
#define PAWN_HPP

#include "piece.hpp"
#include "board.hpp"

class Board;

class Pawn : public Piece 
{
public:
    Pawn(const sf::Texture& texture, float x, float y, Color color, Board& boardGame);

    bool canMove(int startRow, int startCol, int endRow, int endCol) override;
};

#endif
