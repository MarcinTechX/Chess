#include "king.hpp"
#include "board.hpp"

#include <iostream>

King::King(const sf::Texture& texture, float x, float y, Color color, Board& boardGame)
    : Piece(texture, x, y, Type::King, color, boardGame)
    {

    }

bool King::canMove(int startRow, int startCol, int endRow, int endCol) 
{  
    auto king = dynamic_cast<King*>(boardGame->board[startRow][startCol].get());
    
    if (endRow < 0 || endRow >= 8 || endCol < 0 || endCol >= 8) 
    {
        return false;
    }

    if (boardGame->board[endRow][endCol] && boardGame->board[endRow][endCol]->getColor() == pieceColor) 
    {
        return false;
    }

    if (movesCount == 0 && startRow == endRow) 
    {
        if (endCol == 2 || endCol == 6)
        {
            int rookCol = (endCol == 2) ? 0 : 7;
            auto& rook = boardGame->board[startRow][rookCol];

            if (rook && rook->getType() == Piece::Type::Rook && rook->movesCount == 0) 
            {   
                if (boardGame->canCastle(startRow, startCol, endCol, pieceColor)) 
                {   
                    int newRookCol = (endCol == 2) ? 3 : 5; 
                    boardGame->board[startRow][newRookCol] = std::move(rook); 
                    boardGame->board[startRow][rookCol] = nullptr; 
                    return true;
                }
            }
        }
    }
    
    for (int i = -1; i <= 1; i++) 
    {
        for (int j = -1; j <= 1; j++) 
        {
            int neighborRow = endRow + i;
            int neighborCol = endCol + j;
    
            if (neighborRow >= 0 && neighborRow < 8 && neighborCol >= 0 && neighborCol < 8) 
            {
                if (boardGame->board[neighborRow][neighborCol] && 
                    boardGame->board[neighborRow][neighborCol]->getType() == Type::King && 
                    boardGame->board[neighborRow][neighborCol]->getColor() != pieceColor) 
                {
                    return false;
                }
            }
        }
    }
    
    if (abs(endRow - startRow) <= 1 && abs(endCol - startCol) <= 1 && (!(abs(endRow - startRow) == 0 && abs(endCol - startCol) == 0)))
    {
        movesCount++;
        return true;
    }

    return false;
}

