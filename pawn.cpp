#include "pawn.hpp"
#include "board.hpp"

#include <iostream>

Pawn::Pawn(const sf::Texture& texture, float x, float y, Color color, Board& boardGame)
    : Piece(texture, x, y, Type::Pawn, color, boardGame)
    {

    };

bool Pawn::canMove(int startRow, int startCol, int endRow, int endCol) 
{
    if (endRow < 0 || endRow >= 8 || endCol < 0 || endCol >= 8) 
    {
        return false;
    }

    if (boardGame->board[endRow][endCol] && boardGame->board[endRow][endCol]->getColor() == pieceColor) 
    {
        return false;
    }

    int direction = (pieceColor == Color::White) ? 1 : -1; 

    if (startCol == endCol) {

        if (boardGame->board[endRow][endCol] != nullptr) 
        {
            return false; 
        }

        if (movesCount == 0) 
        {
            if (endRow == startRow + 2 * direction && boardGame->board[startRow + direction][startCol] == nullptr) 
            {
                movesCount++;
                boardGame->roundEnPassant = boardGame->rounds;
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
        if (boardGame->board[endRow][endCol] && boardGame->board[endRow][endCol]->getColor() != pieceColor) 
        {
            movesCount++;
            return true;
        }

        //en passant
        if (boardGame->board[startRow][endCol] && boardGame->board[startRow][endCol]->getType() == Type::Pawn) 
        {
            auto opponentPawn = dynamic_cast<Pawn*>(boardGame->board[startRow][endCol].get());
            if (opponentPawn && opponentPawn->getColor() != pieceColor &&
                boardGame->rounds == boardGame->roundEnPassant + 1 &&
                opponentPawn->movesCount == 1 &&
                startRow == (opponentPawn->getColor() == Color::White ? 3 : 4) && 
                boardGame->board[endRow][endCol] == nullptr) 
            {
                boardGame->board[startRow][endCol].reset();
                movesCount++;
                return true;
            }
        }
    
    }
    return false;
}