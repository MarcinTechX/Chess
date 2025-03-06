#include "king.hpp"
#include "board.hpp"

King::King(const sf::Texture& texture, float x, float y, Color color)
    : Piece(texture, x, y, Type::King, color) {}

bool King::canMove(int startRow, int startCol, int endRow, int endCol, std::array<std::array<std::unique_ptr<Piece>, 8>, 8>& board) 
{
    if (endRow < 0 || endRow >= 8 || endCol < 0 || endCol >= 8) 
    {
        return false;
    }

    if (board[endRow][endCol] && board[endRow][endCol]->getColor() == pieceColor) 
    {
        return false;
    }

    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            int neighborRow = endRow + i;
            int neighborCol = endCol + j;

            if (board[neighborRow][neighborCol] && board[neighborRow][neighborCol]->getType() == Type::King && board[neighborRow][neighborCol]->getColor() != pieceColor)
            {
                return false;
            }
        }
    }

    if (abs(endRow - startRow) <= 1 && abs(endCol - startCol) <= 1 && (!(abs(endRow - startRow) == 0 && abs(endCol - startCol) == 0)))
    {
        return true;
    }

    return false;
}

