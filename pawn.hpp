#ifndef PAWN_HPP
#define PAWN_HPP

#include "piece.hpp"
#include "board.hpp"

class Board;

class Pawn : public Piece 
{
public:
    Pawn(const sf::Texture& texture, float x, float y, Color color, Board& boardGame);

    bool canMoveImpl(int startRow, int startCol, int endRow, int endCol, bool testMove) override;

    int roundEnPassant = 0;
};

#endif
