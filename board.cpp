#define rowsAndColumnsNumber 8

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <map>
#include "piece.hpp"
#include "pawn.hpp"
#include "rook.hpp"
#include "knight.hpp"
#include "bishop.hpp"
#include "queen.hpp"
#include "king.hpp"
#include "board.hpp"

#include <iostream>

Board::Board() 
{
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            board[row][col] = nullptr;
        }
    }
}

Board::Board(float newHeight)
    : newHeight(newHeight), isWhiteTurn(true) 
{
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            board[row][col] = nullptr;
        }
    }
}

Board::~Board()
{

}

void Board::setScaleForAllPieces(float newHeight) 
{
    for (auto& row : board) {
        for (auto& piece : row) {
            if (piece) {  
                sf::FloatRect bounds = piece->getSprite().getGlobalBounds();
                float scaleFactor = (newHeight / 8.0f) / bounds.size.x;
                piece->getSprite().setScale({scaleFactor, scaleFactor});
            }
        }
    }
}

void Board::setupBoard(const std::map<std::string, sf::Texture>& textures) 
{
    board[0][0] = std::make_unique<Rook>(textures.at("white-rook"), 0, 0, Piece::Color::White, *this);
    board[0][1] = std::make_unique<Knight>(textures.at("white-knight"), 1, 0, Piece::Color::White, *this);
    board[0][2] = std::make_unique<Bishop>(textures.at("white-bishop"), 2, 0, Piece::Color::White, *this);
    board[0][3] = std::make_unique<Queen>(textures.at("white-queen"), 3, 0, Piece::Color::White, *this);
    board[0][4] = std::make_unique<King>(textures.at("white-king"), 4, 0, Piece::Color::White, *this);
    board[0][5] = std::make_unique<Bishop>(textures.at("white-bishop"), 5, 0, Piece::Color::White, *this);
    board[0][6] = std::make_unique<Knight>(textures.at("white-knight"), 6, 0, Piece::Color::White, *this);
    board[0][7] = std::make_unique<Rook>(textures.at("white-rook"), 7, 0, Piece::Color::White, *this);

    for (int col = 0; col < 8; col++) 
    {
        board[1][col] = std::make_unique<Pawn>(textures.at("white-pawn"), col, 1, Piece::Color::White, *this);
    }

    board[7][0] = std::make_unique<Rook>(textures.at("black-rook"), 0, 7, Piece::Color::Black, *this);
    board[7][1] = std::make_unique<Knight>(textures.at("black-knight"), 1, 7, Piece::Color::Black, *this);
    board[7][2] = std::make_unique<Bishop>(textures.at("black-bishop"), 2, 7, Piece::Color::Black, *this);
    board[7][3] = std::make_unique<Queen>(textures.at("black-queen"), 3, 7, Piece::Color::Black, *this);
    board[7][4] = std::make_unique<King>(textures.at("black-king"), 4, 7, Piece::Color::Black, *this);
    board[7][5] = std::make_unique<Bishop>(textures.at("black-bishop"), 5, 7, Piece::Color::Black, *this);
    board[7][6] = std::make_unique<Knight>(textures.at("black-knight"), 6, 7, Piece::Color::Black, *this);
    board[7][7] = std::make_unique<Rook>(textures.at("black-rook"), 7, 7, Piece::Color::Black, *this);

    for (int col = 0; col < 8; col++) 
    {
        board[6][col] = std::make_unique<Pawn>(textures.at("black-pawn"), col, 6, Piece::Color::Black, *this);
    }

    setScaleForAllPieces(newHeight);
}

void Board::draw(sf::RenderWindow& window, float newPosX, float newPosY, float newHeight) 
{
    float offset = newHeight / 8.0f;

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (board[row][col] && board[row][col] != selectedPiece) {
                float posX = newPosX + col * offset;
                float posY = newPosY + (7 - row) * offset;
                board[row][col]->setPosition(posX, posY);
                board[row][col]->draw(window);
            }
        }
    }

    if (isDragging && selectedPiece) {
        selectedPiece->draw(window);
    }
}

void Board::handleMouseClick(const sf::Vector2i& mousePos) 
{
    for (int row = 0; row < 8; row++) 
    {
        for (int col = 0; col < 8; col++) 
        {
            if (board[row][col]) 
            {
                if (board[row][col]->getColor() == (isWhiteTurn ? Piece::Color::White : Piece::Color::Black)) 
                {
                    if (board[row][col]->contains(mousePos)) 
                    {
                        selectedPiece = std::move(board[row][col]);
                        selectedPieceOriginalPos = {col, row};
                        isDragging = true;
                        return;
                    }
                }
            }
        }
    }
}

void Board::handleMouseMove(const sf::Vector2i& mousePos) 
{
    if (isDragging && selectedPiece) {
        selectedPiece->setPosition(mousePos.x - (newHeight / 16.0f), mousePos.y - (newHeight/ 16.0f)); 
    }
}

void Board::handleMouseRelease(const sf::Vector2i& mousePos, float newPosX, float newPosY) 
{
    isMoveCorrect = false;

    if (!isDragging || !selectedPiece) 
        return;

    int newCol = (mousePos.x - newPosX) / (newHeight / 8.0f);
    int newRow = 8 - (mousePos.y - newPosY) / (newHeight / 8.0f);

    std::unique_ptr<Piece> tempPiece;

    if (newCol >= 0 && newCol < 8 && newRow >= 0 && newRow < 8) 
    {   
        if (selectedPiece->canMove(selectedPieceOriginalPos.y, selectedPieceOriginalPos.x, newRow, newCol)) 
        {   
            auto kingsPositions = getKingsPositions();
            sf::Vector2i whiteKingPos = kingsPositions.first.first;
            Piece::Color whiteKingColor = kingsPositions.first.second;

            sf::Vector2i blackKingPos = kingsPositions.second.first;
            Piece::Color blackKingColor = kingsPositions.second.second;

            bool isWhiteKingChecked = isKingInCheck(whiteKingPos.y, whiteKingPos.x, whiteKingColor);
            bool isBlackKingChecked = isKingInCheck(blackKingPos.y, blackKingPos.x, blackKingColor);

            tempPiece = std::move(board[newRow][newCol]);
            board[newRow][newCol] = std::move(selectedPiece);

            if ((isWhiteTurn && isWhiteKingChecked) || (!isWhiteTurn && isBlackKingChecked)) 
            {
                board[selectedPieceOriginalPos.y][selectedPieceOriginalPos.x] = std::move(board[newRow][newCol]);
                board[newRow][newCol] = std::move(tempPiece);
            } 
            else 
            {
                isMoveCorrect = true;
            }
        }
    }

    if (!isMoveCorrect) 
    {
        if (selectedPiece) 
        {
            board[selectedPieceOriginalPos.y][selectedPieceOriginalPos.x] = std::move(selectedPiece);
        }
    }

    selectedPiece.reset();
    isDragging = false;

    if (isMoveCorrect)
    {
        isWhiteTurn = !isWhiteTurn;
        rounds++;
    }
}

std::pair<std::pair<sf::Vector2i, Piece::Color>, std::pair<sf::Vector2i, Piece::Color>> Board::getKingsPositions() 
{
    sf::Vector2i whiteKingPos = sf::Vector2i(-1, -1);
    sf::Vector2i blackKingPos = sf::Vector2i(-1, -1);

    for (int row = 0; row < 8; row++) 
    {
        for (int col = 0; col < 8; col++) 
        {
            std::unique_ptr<Piece>& piece = board[row][col];

            if (piece) 
            {
                if (piece->getType() == Piece::Type::King) 
                {
                    if (piece->getColor() == Piece::Color::White) 
                    {
                        whiteKingPos = sf::Vector2i(col, row);
                    } 
                    else if (piece->getColor() == Piece::Color::Black) 
                    {
                        blackKingPos = sf::Vector2i(col, row);
                    }
                }
            }
        }
    }

    return {{whiteKingPos, Piece::Color::White}, {blackKingPos, Piece::Color::Black}};
}

bool Board::isKingInCheck(int row, int col, Piece::Color kingColor) 
{
    for (int r = 0; r < 8; r++) 
    {
        for (int c = 0; c < 8; c++) 
        {
            std::unique_ptr<Piece>& piece = board[r][c];

            if (piece) 
            {
                if (piece->getColor() != kingColor) 
                {
                    if (piece->canMove(r, c, row, col)) 
                    {
                        return true; 
                    }
                }
            }
        }
    }
    return false;
}

bool Board::canCastle(int row, int kingCol, int targetCol, Piece::Color kingColor) 
{
    int step = (targetCol > kingCol) ? 1 : -1;

    for (int col = kingCol + step; col != targetCol; col += step) 
    {
        if (board[row][col]) 
        {
            return false; 
        }
    }

    int midCol = (targetCol == 2) ? 3 : 5; 

    if (isKingInCheck(row, kingCol, kingColor)) 
    {
        return false;
    }

    if (isKingInCheck(row, midCol, kingColor)) 
    {
        return false;
    }

    if (isKingInCheck(row, targetCol, kingColor)) 
    {
        return false;
    }

    return true;
}

/*
void Board::drawKingCheckBoundary(sf::RenderWindow& window, const sf::Vector2i& kingPos, float newPosX, float newPosY, float newHeight)
{
    if (kingPos.x != -1 && kingPos.y != -1) {  
        float squareSize = newHeight / 8.0f;
        
        float posX = newPosX + kingPos.x * squareSize;
        float posY = newPosY + kingPos.y * squareSize;
        
        sf::RectangleShape border(sf::Vector2f(squareSize, squareSize));
        border.setPosition({posX, posY});
        border.setOutlineColor(sf::Color::Red); 
        border.setOutlineThickness(5);  
        border.setFillColor(sf::Color::Transparent);  

        window.draw(border);
    }
}
*/






