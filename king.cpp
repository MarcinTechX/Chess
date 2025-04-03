#include "king.hpp"
#include "board.hpp"

#include <iostream>

King::King(const sf::Texture& texture, float x, float y, Color color, Board& boardGame)
    : Piece(texture, x, y, Type::King, color, boardGame)
    {

    }

bool King::canMoveImpl(int startRow, int startCol, int endRow, int endCol, bool testMove) 
{  
    auto king = dynamic_cast<King*>(boardGame->board[startRow][startCol].get());

    if (boardGame->board[endRow][endCol] && boardGame->board[endRow][endCol]->getColor() == pieceColor) 
    {
        return false;
    }

    if (movesCount == 0 && startRow == endRow) 
    {
        int leftCol = 2;
        int rightCol = 6;
        int rookLeftCol = 0;
        int rookRightCol = 7;
        int newRookLeftCol = 3;
        int newRookRightCol = 5;
    
        if (endCol == leftCol || endCol == rightCol) 
        {
            int rookCol = (endCol == leftCol) ? rookLeftCol : rookRightCol;
            auto& rook = boardGame->board[startRow][rookCol];
    
            if (rook && rook->getType() == Piece::Type::Rook && rook->movesCount == 0) 
            {   
                if (boardGame->canCastle(startRow, startCol, endCol, pieceColor)) 
                {   
                    if (!testMove)
                    {
                        int newRookCol = (endCol == leftCol) ? newRookLeftCol : newRookRightCol;
                        boardGame->board[startRow][newRookCol] = std::move(rook); 
                        boardGame->board[startRow][rookCol] = nullptr; 
                    }
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
        return true;
    }

    return false;
}

