#ifndef QUEEN_HPP
#define QUEEN_HPP

#include "piece.hpp"
#include "board.hpp"

class Board;

class Queen : public Piece 
{
public:
    Queen(const sf::Texture& texture, float x, float y, Color color, Board& boardGame);

    bool canMove(int startRow, int startCol, int endRow, int endCol) override; 
};

#endif 