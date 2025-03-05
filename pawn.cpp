#include "pawn.hpp"
#include "board.hpp"

#include <iostream>

Pawn::Pawn(const sf::Texture& texture, float x, float y, Color color)
    : Piece(texture, x, y, Type::Pawn, color), hasMoved(false), movesCount(0) {}

bool Pawn::canMove(int startRow, int startCol, int endRow, int endCol, std::array<std::array<std::unique_ptr<Piece>, 8>, 8>& board) 
{

    if (endRow < 0 || endRow >= 8 || endCol < 0 || endCol >= 8) 
    {
        return false;
    }

    if (board[endRow][endCol] && board[endRow][endCol]->getColor() == pieceColor) 
    {
        return false;
    }

    int direction = (pieceColor == Color::White) ? 1 : -1; 

    if (startCol == endCol) {

        if (pieceColor == Color::White) 
        {
            if (board[endRow][endCol] != nullptr) 
            {
                return false;  
            }
        } else 
        {
            if (board[endRow][endCol] != nullptr) 
            {
                return false; 
            }
        }

        if (!hasMoved) 
        {
            if (endRow == startRow + 2 * direction && board[startRow + direction][startCol] == nullptr) 
            {
                movesCount++;
                return true;
            }
            if (endRow == startRow + direction) 
            {   
                movesCount++;
                return true;
            }
        } else {
            if (endRow == startRow + direction) 
            {
                movesCount++;
                return true;
            }
        }
    }

    if (abs(startCol - endCol) == 1 && endRow == startRow + direction) 
    {
        if (board[endRow][endCol] && board[endRow][endCol]->getColor() != pieceColor) 
        {
            movesCount++;
            return true;
        }

        if (board[startRow][endCol] && board[startRow][endCol]->getType() == Type::Pawn) 
        {
            auto opponentPawn = dynamic_cast<Pawn*>(board[startRow][endCol].get());
            if (opponentPawn && opponentPawn->getColor() != pieceColor &&
                opponentPawn->movesCount == 1 &&
                startRow == (opponentPawn->getColor() == Color::White ? 3 : 4) && 
                board[endRow][endCol] == nullptr) 
            {
                board[startRow][endCol].reset();
                movesCount++;
                return true;
            }
        }
    
    }
    return false;
}

void Pawn::setHasMoved(bool moved) 
{
    hasMoved = moved;
}
