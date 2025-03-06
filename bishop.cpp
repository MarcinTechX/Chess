#include "bishop.hpp"
#include "board.hpp"

Bishop::Bishop(const sf::Texture& texture, float x, float y, Color color)
    : Piece(texture, x, y, Type::Bishop, color) {}

bool Bishop::canMove(int startRow, int startCol, int endRow, int endCol, std::array<std::array<std::unique_ptr<Piece>, 8>, 8>& board) 
{
    if (endRow < 0 || endRow >= 8 || endCol < 0 || endCol >= 8) 
    {
        return false;
    }

    if (board[endRow][endCol] && board[endRow][endCol]->getColor() == pieceColor) 
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
            if (board[row][col])
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