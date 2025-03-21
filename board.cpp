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
    for (int row = 0; row < 8; row++)       
    {
        for (int col = 0; col < 8; col++) 
        {
            board[row][col] = nullptr;
        }
    }
}

Board::Board(float newHeight) 
    : newHeight(newHeight), isWhiteTurn(true) 
{
    for (int row = 0; row < 8; row++) 
    {
        for (int col = 0; col < 8; col++) 
        {
            board[row][col] = nullptr;
        }
    }
}

Board::~Board()
{

}

void Board::setScaleForAllPieces() 
{
    for (auto& row : board) 
    {
        for (auto& piece : row) 
        {
            if (piece) 
            {  
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

    setScaleForAllPieces();
}

void Board::draw(sf::RenderWindow& window, float newPosX, float newPosY) 
{
    float offset = newHeight / 8.0f;
    float posX, posY;

    for (int row = 0; row < 8; row++) 
    {
        for (int col = 0; col < 8; col++) 
        {
            if (board[row][col] && board[row][col] != selectedPiece) 
            {
                posX = newPosX + col * offset;
                posY = newPosY + (7 - row) * offset;

                board[row][col]->setPosition(posX, posY);
                board[row][col]->draw(window);
            }
        }
    }

    if (isDragging && selectedPiece) 
    {
        selectedPiece->draw(window);
    }
}

void Board::flipBoard()
{
    for (int i = 0; i < 4; i++) 
    {
        std::swap(board[i], board[7 - i]);
    }
    
    for (int i = 0; i < 8; i++) 
    {
        for (int j = 0; j < 4; j++) 
        {
            std::swap(board[i][j], board[i][7 - j]);
        }
    }
    
    isFlipped = !isFlipped;
}

std::tuple<Piece::Color, int, int> Board::getPromotePawnPos()
{
    Piece::Color color = isWhiteTurn ? Piece::Color::Black : Piece::Color::White;

    int row = (color == Piece::Color::White) ? 7 : 0;
    if (isFlipped) row = (color == Piece::Color::White) ? 0 : 7; 

    int col = -1;

    for (int i = 0; i < 8; i++) 
    {
        if (board[row][i] && dynamic_cast<Pawn*>(board[row][i].get())) 
        {
            col = i;
            break;
        }
    }

    return {color, row, col};
}

sf::FloatRect Board::drawPromotionWindow(sf::RenderWindow& window, float newPosX, float newPosY, unsigned int screenWidth, unsigned int screenHeight, std::map<std::string, sf::Texture>& textures) 
{
    std::tuple<Piece::Color, int, int> pawnPos = getPromotePawnPos();

    Piece::Color color = std::get<0>(pawnPos);
    int row = std::get<1>(pawnPos);
    int col = std::get<2>(pawnPos);

    sf::RectangleShape promotionWindow({newHeight / 8.0f, newHeight / 2.0f});
    if (color == Piece::Color::White)
    {   
        if (isFlipped)
        {
            promotionWindow.setPosition({newPosX + col * newHeight / 8.0f, newPosY - (row - 4) * newHeight / 8.0f});
        }
        else
        {
            promotionWindow.setPosition({newPosX + col * newHeight / 8.0f, newPosY + (7 - row) * newHeight / 8.0f});
        }
    }
    else
    {           
        if (isFlipped)
        {
            promotionWindow.setPosition({newPosX + col * newHeight / 8.0f, newPosY - (row - 7) * newHeight / 8.0f});
        }
        else
        {
            promotionWindow.setPosition({newPosX + col * newHeight / 8.0f, newPosY - (row - 7 + 3) * newHeight / 8.0f});
        }
    }

    window.draw(promotionWindow);

    std::string colorPrefix = isWhiteTurn ? "black-" : "white-";
    std::vector<std::string> pieceNames = {"queen", "rook", "knight", "bishop"};

    std::vector<std::pair<sf::Sprite, std::string>> sprites;

    float iconSize = promotionWindow.getSize().x;

    for (int i = 0; i < 4; i++) 
    {
        sf::Sprite sprite(textures[colorPrefix + pieceNames[i]]);
        sprite.setScale({iconSize / sprite.getLocalBounds().size.x, iconSize / sprite.getLocalBounds().size.y});
        
        if (color == Piece::Color::White)
        {   
            if (isFlipped)
            {
                sprite.setPosition({promotionWindow.getPosition().x, promotionWindow.getPosition().y + (3 - i) * iconSize});  
            }
            else
            {
                sprite.setPosition({promotionWindow.getPosition().x, promotionWindow.getPosition().y + i * iconSize});    
            }
        }
        else
        {
            if (isFlipped)
            {
                sprite.setPosition({promotionWindow.getPosition().x, promotionWindow.getPosition().y + i * iconSize});  
            }
            else
            {
                sprite.setPosition({promotionWindow.getPosition().x, promotionWindow.getPosition().y + (3 - i) * iconSize});
            }
        }
        
        window.draw(sprite);
    }

    return promotionWindow.getGlobalBounds();
}

bool Board::isMouseInPromotionWindow(sf::RenderWindow& window, sf::FloatRect promotionWindowPos)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    return promotionWindowPos.contains({(float)mousePos.x, (float)(mousePos.y)});
}

Piece::Type Board::getPromotionPiece(const sf::Vector2i& mousePos, float newPosX, float newPosY)
{   
    std::tuple<Piece::Color, int, int> pawnPos = getPromotePawnPos();

    Piece::Color color = std::get<0>(pawnPos);
    int row = std::get<1>(pawnPos);
    int col = std::get<2>(pawnPos);

    std::vector<Piece::Type> pieceTypes = {
        Piece::Type::Queen,
        Piece::Type::Rook,
        Piece::Type::Knight,
        Piece::Type::Bishop
    };

    float iconSize = newHeight / 8.0f; 
    int rowMouse = (mousePos.y - newPosY) / iconSize;   
    int colMouse = (mousePos.x - newPosX) / iconSize;

    if (colMouse == col)
    {
        if (color == Piece::Color::White)
        {
            if (isFlipped)
            {
                if (rowMouse <= 7 && rowMouse > 3)
                {
                    return pieceTypes[7 - rowMouse];
                }
            }
            else 
            if (rowMouse >= 0 && rowMouse < 4)
            {
                return pieceTypes[rowMouse];
            }
        }
        else 
        {   
            if (isFlipped)
            {
                if (rowMouse >= 0 && rowMouse < 4)
                {
                    return pieceTypes[rowMouse];
                }
            }
            if (rowMouse <= 7 && rowMouse > 3)
            {
                return pieceTypes[7 - rowMouse];
            }
        }
    }

    return Piece::Type::None; 
}

void Board::promotePawn(Piece::Type promotionPiece, std::map<std::string, sf::Texture>& textures)
{
    std::tuple<Piece::Color, int, int> pawnPos = getPromotePawnPos();

    Piece::Color color = std::get<0>(pawnPos);
    int row = std::get<1>(pawnPos);
    int col = std::get<2>(pawnPos);

    std::unique_ptr<Piece> newPiece;

    if (promotionPiece == Piece::Type::Queen) 
    {
        newPiece = std::make_unique<Queen>(textures.at((color == Piece::Color::White) ? "white-queen" : "black-queen"), col, row, color, *this);
    }
    else if (promotionPiece == Piece::Type::Rook) 
    {
        newPiece = std::make_unique<Rook>(textures.at((color == Piece::Color::White) ? "white-rook" : "black-rook"), col, row, color, *this);
    }
    else if (promotionPiece == Piece::Type::Knight) 
    {
        newPiece = std::make_unique<Knight>(textures.at((color == Piece::Color::White) ? "white-knight" : "black-knight"), col, row, color, *this);
    }
    else if (promotionPiece == Piece::Type::Bishop) 
    {
        newPiece = std::make_unique<Bishop>(textures.at((color == Piece::Color::White) ? "white-bishop" : "black-bishop"), col, row, color, *this);
    }

    if (newPiece)
    {
        sf::FloatRect bounds = newPiece->getSprite().getGlobalBounds();
        float scaleFactor = (newHeight / 8.0f) / bounds.size.x;
        newPiece->getSprite().setScale({scaleFactor, scaleFactor});
        board[row][col] = std::move(newPiece);
    }

    promotionActive = false;
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
            tempPiece = std::move(board[newRow][newCol]);
            board[newRow][newCol] = std::move(selectedPiece);

            auto kingsPositions = getKingsPositions();
            sf::Vector2i whiteKingPos = kingsPositions.first;
        
            sf::Vector2i blackKingPos = kingsPositions.second;

            bool isWhiteKingChecked = isKingInCheck(whiteKingPos.y, whiteKingPos.x, Piece::Color::White);
            bool isBlackKingChecked = isKingInCheck(blackKingPos.y, blackKingPos.x, Piece::Color::Black);

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

    if (board[newRow][newCol] && board[newRow][newCol]->getType() == Piece::Type::Pawn)
    {
        Piece::Color pieceColor = board[newRow][newCol]->getColor();
    
        int promotionRowWhite = 7;
        int promotionRowBlack = 0;
    
        if (isFlipped) 
        {
            std::swap(promotionRowWhite, promotionRowBlack);
        }

        if ((pieceColor == Piece::Color::White && newRow == promotionRowWhite) || 
            (pieceColor == Piece::Color::Black && newRow == promotionRowBlack))
        {
            promotionActive = true;
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

std::pair<sf::Vector2i, sf::Vector2i> Board::getKingsPositions() 
{
    sf::Vector2i whiteKingPos(-1, -1);
    sf::Vector2i blackKingPos(-1, -1);

    for (int row = 0; row < 8; row++) 
    {
        for (int col = 0; col < 8; col++) 
        {
            std::unique_ptr<Piece>& piece = board[row][col];

            if (piece && piece->getType() == Piece::Type::King) 
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

    return {whiteKingPos, blackKingPos};
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
    int flippedTargetCol = isFlipped ? (targetCol == 2 ? 5 : 1) : targetCol;
    int flippedMidCol = isFlipped ? (targetCol == 2 ? 4 : 2) : ((targetCol == 2) ? 3 : 5);
    
    int step = (flippedTargetCol > kingCol) ? 1 : -1;

    for (int col = kingCol + step; col != flippedTargetCol; col += step) 
    {
        if (board[row][col]) 
        {
            return false; 
        }
    }

    if (isKingInCheck(row, kingCol, kingColor)) 
    {
        return false;
    }

    if (isKingInCheck(row, flippedMidCol, kingColor)) 
    {
        return false;
    }

    if (isKingInCheck(row, flippedTargetCol, kingColor)) 
    {
        return false;
    }

    return true;
}






