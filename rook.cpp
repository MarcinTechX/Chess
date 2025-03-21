#include "rook.hpp"
#include "board.hpp"

Rook::Rook(const sf::Texture& texture, float x, float y, Color color, Board& boardGame)
    : Piece(texture, x, y, Type::Rook, color, boardGame)
    {

    }

bool Rook::canMoveImpl(int startRow, int startCol, int endRow, int endCol) 
{
    if (boardGame->board[endRow][endCol] && boardGame->board[endRow][endCol]->getColor() == pieceColor) 
    {
        return false;
    }

    if (startCol == endCol) 
    {
        int rowDirection = (endRow > startRow) ? 1 : -1;
        int row = startRow + rowDirection;

        while (row != endRow)
        {
            if (boardGame->board[row][startCol])  
            {
                return false;
            }
            row += rowDirection;
        }
        return true;
    }

    if (startRow == endRow) 
    {
        int colDirection = (endCol > startCol) ? 1 : -1;
        int col = startCol + colDirection;

        while (col != endCol)
        {
            if (boardGame->board[startRow][col])  
            {
                return false;
            }
            col += colDirection;
        }
        return true;
    }

    return false;
}