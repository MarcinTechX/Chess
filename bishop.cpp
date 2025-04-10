#include "bishop.hpp"
#include "board.hpp"

#include <iostream>

Bishop::Bishop(const sf::Texture& texture, float x, float y, Color color, Board& boardGame)
    : Piece(texture, x, y, Type::Bishop, color, boardGame) 
    {
        this->boardGame = &boardGame;
    }

bool Bishop::canMoveImpl(int startRow, int startCol, int endRow, int endCol, bool testMove, bool sameColor)
{
    if (boardGame->board[endRow][endCol] && boardGame->board[endRow][endCol]->getColor() == pieceColor && !sameColor) 
    {
        return false;
    }

    if (abs(endRow - startRow) == abs(endCol - startCol))
    {
        int rowDirection = (endRow > startRow) ? 1 : -1;
        int colDirection = (endCol > startCol) ? 1 : -1;

        int row = startRow + rowDirection;
        int col = startCol + colDirection;
        while (row != endRow && col != endCol)
        {
            if (boardGame->board[row][col])
            {
                return false;
            }
            row += rowDirection;
            col += colDirection;
        }

        return true;
    }

    return false;
}
