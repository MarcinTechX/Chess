#ifndef ROOK_HPP
#define ROOK_HPP

#include "piece.hpp"
#include "board.hpp"

class Board;

class Rook : public Piece 
{
public:
    Rook(const sf::Texture& texture, float x, float y, Color color, Board& boardGame);

    bool canMoveImpl(int startRow, int startCol, int endRow, int endCol) override;
};

#endif 