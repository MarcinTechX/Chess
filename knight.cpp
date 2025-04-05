#include "knight.hpp"
#include "board.hpp"

Knight::Knight(const sf::Texture& texture, float x, float y, Color color, Board& boardGame)
    : Piece(texture, x, y, Type::Knight, color, boardGame) 
    {

    }

bool Knight::canMoveImpl(int startRow, int startCol, int endRow, int endCol, bool testMove, bool sameColor) 
{
    if (boardGame->board[endRow][endCol] && boardGame->board[endRow][endCol]->getColor() == pieceColor && !sameColor) 
    {
        return false;
    }

    if ((abs(endRow - startRow) == 2 && abs(endCol - startCol) == 1) || (abs(endRow - startRow) == 1 && abs(endCol - startCol) == 2))
    {
        return true;
    }

    return false;
}