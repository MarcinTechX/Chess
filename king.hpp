#ifndef KING_HPP
#define KING_HPP

#include "piece.hpp"
#include "board.hpp"
#include "pawn.hpp"
#include "king.hpp"
#include "queen.hpp"
#include "bishop.hpp"
#include "knight.hpp"
#include "rook.hpp"

class Board;

class King : public Piece 
{
public:
    King(const sf::Texture& texture, float x, float y, Color color, Board& boardGame);

    bool canMoveImpl(int startRow, int startCol, int endRow, int endCol, bool testMove, bool sameColor) override;
};

#endif 