#include "pawn.hpp"
#include "board.hpp"

#include <iostream>

Pawn::Pawn(const sf::Texture& texture, float x, float y, Color color, Board& boardGame)
    : Piece(texture, x, y, Type::Pawn, color, boardGame)
    {

    }

bool Pawn::canMoveImpl(int startRow, int startCol, int endRow, int endCol, bool testMove) 
{   
    if (boardGame->board[endRow][endCol] && boardGame->board[endRow][endCol]->getColor() == pieceColor) 
    {
        return false;
    }

    int direction = (pieceColor == Color::White) ? 1 : -1; 

    if (boardGame->isFlipped)  
    {
        direction = -direction;
    }

    if (startCol == endCol) {

        if (boardGame->board[endRow][endCol] != nullptr) 
        {
            return false; 
        }

        if (movesCount == 0) 
        {
            if (endRow == startRow + 2 * direction && boardGame->board[startRow + direction][startCol] == nullptr) 
            {
                boardGame->roundEnPassant = boardGame->rounds;
                roundEnPassant = boardGame->rounds;
                return true;
            }
            if (endRow == startRow + direction) 
            {   
                return true;
            }
        } 
        else 
        {
            if (endRow == startRow + direction) 
            {
                return true;
            }
        }
    }

    if (abs(startCol - endCol) == 1 && endRow == startRow + direction) 
    {
        if (boardGame->board[endRow][endCol] && boardGame->board[endRow][endCol]->getColor() != pieceColor) 
        {
            return true;
        }

        //en passant
        if (abs(startCol - endCol) == 1 && endRow == startRow + direction) 
        {
            auto* opponentPiece = boardGame->board[startRow][endCol].get();
            
            if (opponentPiece && opponentPiece->getType() == Type::Pawn) 
            {
                auto* opponentPawn = dynamic_cast<Pawn*>(opponentPiece);

                if (opponentPawn && 
                    opponentPawn->getColor() != pieceColor &&
                    opponentPawn->movesCount == 1 &&
                    boardGame->rounds == opponentPawn->roundEnPassant + 1 &&
                    startRow == (boardGame->isFlipped ? (opponentPawn->getColor() == Color::White ? 4 : 3)
                                                    : (opponentPawn->getColor() == Color::White ? 3 : 4)) &&
                    boardGame->board[endRow][endCol] == nullptr) 
                {   
                    boardGame->hasEnPassantMade = true;
                    if (!testMove) 
                    {
                        boardGame->board[startRow][endCol].reset();
                    }
                    return true;
                }
            }
        }
    }
    return false;
}

