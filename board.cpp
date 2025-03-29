#define rowsAndColumnsNumber 8

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <memory>
#include <map>
#include <iostream>
#include "piece.hpp"
#include "pawn.hpp"
#include "rook.hpp"
#include "knight.hpp"
#include "bishop.hpp"
#include "queen.hpp"
#include "king.hpp"
#include "board.hpp"
#include "soundmanager.hpp"
#include "colorconverter.hpp"

Board::Board(sf::RenderWindow& window, sf::Vector2<unsigned int> desktopSize, sf::Texture& boardTexture, std::map<std::string, sf::Texture>& textures, 
    SoundManager& soundManager, sf::Font& font)
    : isWhiteTurn(true), boardSprite(boardTexture), desktopSize(desktopSize), soundManager(soundManager), textures(textures), font(font),
    isFlipped(false), lastWindowSize({0,0}), promotionActive(false), rounds(0), roundEnPassant(0), 
    isMoveCorrect(false), hasEnPassantMade(false), showLegalMoves(false)
{
    for (int row = 0; row < 8; row++) 
    {
        for (int col = 0; col < 8; col++) 
        {
            board[row][col] = nullptr;
        }
    }

    this->boardTexture = boardTexture;
    this->initialBoardTexture = boardTexture;
    this->boardSprite.setTexture(this->boardTexture);
    this->boardImage = this->boardTexture.copyToImage();
    this->boardImageOriginal = this->initialBoardTexture.copyToImage();  

    squareWidth = boardImageOriginal.getSize().x / 8;
    squareHeight = boardImageOriginal.getSize().y / 8;
    pixel1 = boardImageOriginal.getPixel({0, 0});  
    pixel2 = boardImageOriginal.getPixel({squareWidth, 0});  

    updateBoard(window);
}

Board::~Board()
{

}

bool Board::loadShader()
{
    static bool shaderLoaded = false;
    if (!shaderLoaded)
    {
        if (!shader.loadFromFile("check.frag", sf::Shader::Type::Fragment)) 
        {
            std::cerr << "Failed to load shader!" << std::endl;
            return false;
        }
        shader.setUniform("maxDistance", newHeight / (256 * 8));
        shaderLoaded = true;
    }
    return true;
}

void Board::updateBoard(sf::RenderWindow& window) 
{   
    sf::Vector2u currentSize = window.getSize();
    if (currentSize == lastWindowSize) return;  
    
    lastWindowSize = currentSize;

    unsigned int screenWidth = desktopSize.x;   
    unsigned int screenHeight = desktopSize.y;

    unsigned int shorterSide = std::min(screenWidth, screenHeight); 

    newHeight = shorterSide * 0.7f;
    float scaleFactorY = newHeight / boardTexture.getSize().y;
    float scaleFactorX = scaleFactorY * (boardTexture.getSize().x / boardTexture.getSize().y);
    boardSprite.setScale({scaleFactorX, scaleFactorY}); 

    float newWidth = boardTexture.getSize().x * scaleFactorX;
    newPosX = (screenWidth - newWidth) / 2.0f;
    newPosY = (screenHeight - newHeight) / 2.0f;

    boardSprite.setPosition({newPosX, newPosY}); 
}

void Board::getKingInCheckImage(sf::RenderWindow& window)
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

void Board::setupBoard() 
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

void Board::changeSquarePixels() 
{      
    boardImage = boardImageOriginal;

    int avgRed = (pixel1.r + pixel2.r) / 2;
    int avgGreen = (pixel1.g + pixel2.g) / 2;
    int avgBlue = (pixel1.b + pixel2.b) / 2;

    sf::Color avgColor(avgRed, avgGreen, avgBlue);

    auto changePixelsInSquare = [&](int row, int col) 
    {
        int startX = col * squareWidth;
        int startY = (7 - row) * squareHeight;
        
        for (int i = 0; i < squareWidth; i++) 
        {
            for (int j = 0; j < squareHeight; j++) 
            {
                unsigned int x = startX + i;
                unsigned int y = startY + j;

                if (x < boardImage.getSize().x && y < boardImage.getSize().y) 
                {
                    boardImage.setPixel({x, y}, avgColor);
                }
            }
        }
    };

    changePixelsInSquare(selectedPieceOriginalPos.y, selectedPieceOriginalPos.x);  
    changePixelsInSquare(newRow, newCol); 
    
    if (!boardTexture.loadFromImage(boardImage))  
    {
        std::cerr << "Failed to load texture from modified image!" << std::endl;
    }
}

void Board::draw(sf::RenderWindow& window) 
{   
    window.draw(boardSprite);

    drawTextOnChessboard(window);

    auto kingsPositions = getKingsPositions();
    sf::Vector2i whiteKingPos = kingsPositions.first;
    sf::Vector2i blackKingPos = kingsPositions.second;

    bool isWhiteKingChecked = isKingInCheck(whiteKingPos.y, whiteKingPos.x, Piece::Color::White);
    bool isBlackKingChecked = isKingInCheck(blackKingPos.y, blackKingPos.x, Piece::Color::Black);

    if ((isWhiteKingChecked || isBlackKingChecked) && !selectedPiece)
    {
        drawKingChecked(window);
    }

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
    
    if (selectedPiece || clickCount % 2 == 1) 
    {   
        if (showLegalMoves)
        {    
            drawPossibleMoves(window);
        }
        if (selectedPiece)
        {
            selectedPiece->draw(window);
        }
    }

    updateBoard(window);
}

void Board::drawKingChecked(sf::RenderWindow& window)
{   
    if (!loadShader())
    {
        return;
    }

    float squareWidth = newHeight / 8.0f;
    sf::Sprite spriteChecked((textures["white-king"]));

    auto kingsPositions = getKingsPositions();
    sf::Vector2i kingPos;
    bool isKingChecked = false;

    if (isKingInCheck(kingsPositions.first.y, kingsPositions.first.x, Piece::Color::White)) 
    {
        kingPos = kingsPositions.first;
        spriteChecked.setTexture(textures["white-king"]);
        isKingChecked = true;
    } 
    else if (isKingInCheck(kingsPositions.second.y, kingsPositions.second.x, Piece::Color::Black)) 
    {
        kingPos = kingsPositions.second;
        spriteChecked.setTexture(textures["black-king"]);
        isKingChecked = true;
    }

    if (isKingChecked)
    {
        sf::FloatRect bounds = spriteChecked.getGlobalBounds();
        float scaleFactor = (newHeight / 8.0f) / bounds.size.x;
        spriteChecked.setScale({scaleFactor, scaleFactor});

        spriteChecked.setPosition({newPosX + kingPos.x * squareWidth, newPosY + (7 - kingPos.y) * squareWidth});
        window.draw(spriteChecked, &shader);
    }
}

void Board::drawTextOnChessboard(sf::RenderWindow& window)
{   
    texts.clear();

    float squareWidth = newHeight / 8.0f;
    float squareHeight = newHeight / 8.0f;
    float fontSize = newHeight / 32.0f;
    float offset = squareWidth * 0.05f;

    sf::Text baseText(font);
    baseText.setCharacterSize(fontSize);
    baseText.setString("0");
    float baseHeight = baseText.getLocalBounds().size.y;

    for (int i = 1; i <= 8; i++)
    {
        sf::Text text(font);
        text.setCharacterSize(fontSize);     
        
        if (isFlipped)
        {
            text.setString(std::to_string(9 - i));
        }
        else
        {
            text.setString(std::to_string(i));
        }

        if (i % 2 == 0)
        {
            text.setFillColor(pixel2);
        }
        else
        {
            text.setFillColor(pixel1);
        }

        float posX = newPosX + offset;
        float posY = newPosY + (8 - i) * squareHeight;
        
        text.setPosition({posX, posY});
        texts.push_back(text); 
    }

    for (int i = 0; i < 8; i++)
    {
        sf::Text text(font);
        text.setCharacterSize(fontSize);    
        
        if (isFlipped)
        {
            text.setString(std::string(1, 'h' - i));
        }
        else
        {
            text.setString(std::string(1, 'a' + i));
        }

        if (i % 2 == 0)
        {
            text.setFillColor(pixel1);
        }
        else
        {
            text.setFillColor(pixel2);
        }

        sf::FloatRect textBounds = text.getLocalBounds();
        float posX = newPosX + i * squareWidth + squareWidth - textBounds.size.x - offset;
        float posY = newPosY + 7 * squareHeight + squareHeight - baseHeight - 3 * offset;

        text.setPosition({posX, posY});
        texts.push_back(text); 
    }

    for (const auto& text : texts)
    {
        window.draw(text);
    }
}

sf::Vector2f Board::calculateBoardPosition(int row, int col) 
{
    float squareSize = newHeight / 8.0f;
    if (isFlipped)  
    {
        return sf::Vector2f
        (
            newPosX + (7 - col) * squareSize,
            newPosY + row * squareSize
        );
    }
    return sf::Vector2f
    (
        newPosX + col * squareSize,
        newPosY + (7 - row) * squareSize
    );
}

void Board::flipBoard()
{       
    isPieceSelected = false;

    if (selectedPiece) 
    {
        float squareSize = newHeight / 8.0f;
        float originalX = newPosX + selectedPieceOriginalPos.x * squareSize;
        float originalY = newPosY + (7 - selectedPieceOriginalPos.y) * squareSize;
        
        selectedPiece->setPosition(originalX, originalY);
        
        board[selectedPieceOriginalPos.y][selectedPieceOriginalPos.x] = std::move(selectedPiece);
        
        selectedPiece = nullptr;
    }

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

    flipBoardTexture();

    isFlipped = !isFlipped;
}

void Board::flipBoardTexture()
{
    int width = boardImage.getSize().x;
    int height = boardImage.getSize().y;

    for (unsigned int row = 0; row < height / 2; row++) 
    {
        for (unsigned int col = 0; col < width; col++) 
        {
            sf::Color temp = boardImage.getPixel({col, row});
            boardImage.setPixel({col, row}, boardImage.getPixel({width - col - 1, height - row - 1}));
            boardImage.setPixel({width - col - 1, height - row - 1}, temp);
        }
    }

    if (!boardTexture.loadFromImage(boardImage)) 
    {
        std::cerr << "Failed to load flipped texture!" << std::endl;
    }
}

void Board::drawPossibleMoves(sf::RenderWindow& window)
{   
    int avgRed = (pixel1.r + pixel2.r) / 2;
    int avgGreen = (pixel1.g + pixel2.g) / 2;
    int avgBlue = (pixel1.b + pixel2.b) / 2;

    sf::Color avgColor(avgRed, avgGreen, avgBlue);

    if (selectedPiece || isPieceSelected) 
    {
        for (const auto& move : possibleMoves) 
        {
            int row = move.first;
            int col = move.second;

            sf::CircleShape circle(newHeight / 32.0f);
            circle.setOrigin({circle.getRadius(), circle.getRadius()});
            circle.setFillColor(sf::Color(255 - avgRed, 255 - avgGreen, 255 - avgBlue, 192));
            circle.setPosition({newPosX + col * newHeight / 8.0f + newHeight / 16.0f, newPosY + (7 - row) * newHeight / 8.0f + newHeight / 16.0f});
            window.draw(circle);
        }
    }
}

std::vector<std::pair<int, int>> Board::getPossibleMovesForPiece(int row, int col)  
{  
    possibleMoves.clear();

    Piece* piecePtr = selectedPiece.get();
    if (!piecePtr) 
    {
        return possibleMoves;
    }

    Piece::Color pieceColor = piecePtr->getColor();
    bool isKing = (piecePtr->getType() == Piece::Type::King);

    int kingRow = -1, kingCol = -1;
    for (int r = 0; r < 8; r++) 
    {
        for (int c = 0; c < 8; c++) 
        {
            if (board[r][c] && board[r][c]->getType() == Piece::Type::King && board[r][c]->getColor() == pieceColor)
            {
                kingRow = r;
                kingCol = c;
                break;
            }
        }
        if (kingRow != -1) break;
    }

    for (int destRow = 0; destRow < 8; destRow++) 
    {
        for (int destCol = 0; destCol < 8; destCol++) 
        {
            if (!piecePtr->canMove(row, col, destRow, destCol, true)) 
            {
                continue;
            }

            Piece* capturedPiece = board[destRow][destCol].release();
            board[destRow][destCol] = std::move(board[row][col]);
            board[row][col].reset();

            if (isKing) 
            {
                kingRow = destRow;
                kingCol = destCol;
            }

            if (!isKingInCheck(kingRow, kingCol, pieceColor)) 
            {
                possibleMoves.push_back({destRow, destCol});
            }

            board[row][col] = std::move(board[destRow][destCol]);
            board[destRow][destCol].reset(capturedPiece);
        }
    }

    return possibleMoves;
}

std::tuple<Piece::Color, int, int> Board::getPromotePawnPos()
{
    Piece::Color color = isWhiteTurn ? Piece::Color::White : Piece::Color::Black;

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

sf::FloatRect Board::drawPromotionWindow(sf::RenderWindow& window) 
{
    std::tuple<Piece::Color, int, int> pawnPos = getPromotePawnPos();

    clickCount = 0;

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

    std::string colorPrefix = isWhiteTurn ? "white-" : "black-";
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

bool Board::isMouseInPromotionWindow(sf::RenderWindow& window)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    promotionWindowPos = drawPromotionWindow(window);

    return promotionWindowPos.contains({(float)mousePos.x, (float)(mousePos.y)});
}

Piece::Type Board::getPromotionPiece(const sf::Vector2i& mousePos)
{   
    std::tuple<Piece::Color, int, int> pawnPos = getPromotePawnPos();

    Piece::Color color = std::get<0>(pawnPos);
    int row = std::get<1>(pawnPos);
    int col = std::get<2>(pawnPos);

    std::vector<Piece::Type> pieceTypes = 
    {
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
            else if (rowMouse >= 0 && rowMouse < 4)
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
            else if (rowMouse <= 7 && rowMouse > 3)
            {
                return pieceTypes[7 - rowMouse];
            }
        }
    }

    return Piece::Type::None; 
}

void Board::promotePawn(Piece::Type promotionPiece)
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

    isWhiteTurn = !isWhiteTurn;
    rounds++;

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
                        if (previousRow == row && previousCol == col) 
                        {
                            clickCount++;
                        }
                        else 
                        {
                            clickCount = 1;
                        }

                        if (clickCount % 2 == 0) 
                        {
                            selectedPiece = std::move(board[row][col]);
                        }
                        else 
                        {
                            selectedPiece = std::move(board[row][col]);
                            isPieceSelected = true;
                            previousRow = row;
                            previousCol = col;
                            selectedPieceOriginalPos = {col, row};
                            getPossibleMovesForPiece(row, col);
                        }
                    }
                }
            }
        }
    }
}

void Board::handleMouseMove(const sf::Vector2i& mousePos) 
{ 
    if (selectedPiece) 
    {
        isDragging = true;
        selectedPiece->setPosition(mousePos.x - (newHeight / 16.0f), mousePos.y - (newHeight/ 16.0f)); 
    }
}

void Board::handleMouseRelease(const sf::Vector2i& mousePos) 
{   
    soundManager.stopAllSounds();

    isMoveCorrect = false;
    hasEnPassantMade = false;

    float tempRow = 8 - (mousePos.y - newPosY) / (newHeight / 8.0f);
    float tempCol = (mousePos.x - newPosX) / (newHeight / 8.0f);

    if (tempRow < 0 || tempRow > 8 || tempCol < 0 || tempCol > 8)
    {
        if (selectedPiece)
        {
            board[selectedPieceOriginalPos.y][selectedPieceOriginalPos.x] = std::move(selectedPiece);
            isPieceSelected = false;
            clickCount = 0;
        }
        return;
    }

    newRow = std::max(0, std::min(7, static_cast<int>(tempRow)));
    newCol = std::max(0, std::min(7, static_cast<int>(tempCol)));

    if (!selectedPiece && clickCount % 2 == 1) 
    {
        if (board[previousRow][previousCol]) 
        {
            selectedPiece = std::move(board[previousRow][previousCol]);
            selectedPieceOriginalPos = {previousCol, previousRow};
        }
    }

    if (!selectedPiece)
        return;

    bool isPieceCaptured;
    std::unique_ptr<Piece> tempPiece;

    if (newCol >= 0 && newCol < 8 && newRow >= 0 && newRow < 8) 
    {   
        if (selectedPiece->canMove(selectedPieceOriginalPos.y, selectedPieceOriginalPos.x, newRow, newCol, false)) 
        {  
            tempPiece = std::move(board[newRow][newCol]);
            board[newRow][newCol] = std::move(selectedPiece);

            auto kingsPositions = getKingsPositions();
            sf::Vector2i whiteKingPos = kingsPositions.first;
            sf::Vector2i blackKingPos = kingsPositions.second;

            bool isWhiteKingChecked = isKingInCheck(whiteKingPos.y, whiteKingPos.x, Piece::Color::White);
            bool isBlackKingChecked = isKingInCheck(blackKingPos.y, blackKingPos.x, Piece::Color::Black);

            isPieceCaptured = (tempPiece != nullptr) || hasEnPassantMade;

            if ((isWhiteTurn && isWhiteKingChecked) || (!isWhiteTurn && isBlackKingChecked)) 
            {
                board[selectedPieceOriginalPos.y][selectedPieceOriginalPos.x] = std::move(board[newRow][newCol]);
                board[newRow][newCol] = std::move(tempPiece);
                soundManager.playSoundOnce("incorrect_move");
            } 
            else 
            {
                isMoveCorrect = true;
                changeSquarePixels();
            }
        }
    }
    
    if (!isMoveCorrect) 
    {   
        if (selectedPiece) 
        {   
            board[selectedPieceOriginalPos.y][selectedPieceOriginalPos.x] = std::move(selectedPiece);
            if (newRow != selectedPieceOriginalPos.y || newCol != selectedPieceOriginalPos.x)
            {
                isPieceSelected = false;
                clickCount = 0;
            }
            if (isDragging && (newRow != selectedPieceOriginalPos.y || newCol != selectedPieceOriginalPos.x))
            {
                isPieceSelected = false;
                clickCount = 0;
            }
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

    if (isMoveCorrect)
    {   
        if (!(board[newRow][newCol]->getType() == Piece::Type::Pawn && (newRow == 0 || newRow == 7)))
        {
            isWhiteTurn = !isWhiteTurn;
            rounds++;
            lastRoundIndex = rounds;
            isPieceSelected = false;
            clickCount = 0;
        }
        if (isPieceCaptured)
        {
            soundManager.playSoundOnce("capture");
        }
        else if (!isPieceCaptured)
        {   
            soundManager.playSoundOnce("move");
        }
    }

    isDragging = false;
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
/*
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
                    if (piece->canMove(r, c, row, col, true)) 
                    {
                        return true; 
                    }
                }
            }
        }
    }
    return false;
}
*/

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

bool Board::isKingInCheck(int kingRow, int kingCol, Piece::Color kingColor) 
{
    Piece::Color opponentColor = (kingColor == Piece::Color::White) ? Piece::Color::Black : Piece::Color::White;

    int pawnDirection = (opponentColor == Piece::Color::White) ? -1 : 1;
    if (isFlipped)
    {
        pawnDirection = -pawnDirection;
    }
    if (kingRow + pawnDirection >= 0 && kingRow + pawnDirection < 8) 
    {
        if ((kingCol - 1 >= 0 && board[kingRow + pawnDirection][kingCol - 1] && 
             board[kingRow + pawnDirection][kingCol - 1]->getType() == Piece::Type::Pawn &&
             board[kingRow + pawnDirection][kingCol - 1]->getColor() == opponentColor) ||
            (kingCol + 1 < 8 && board[kingRow + pawnDirection][kingCol + 1] && 
             board[kingRow + pawnDirection][kingCol + 1]->getType() == Piece::Type::Pawn &&
             board[kingRow + pawnDirection][kingCol + 1]->getColor() == opponentColor)) 
        {
            return true;
        }
    }

    int knightMoves[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
    for (auto& move : knightMoves) 
    {
        int r = kingRow + move[0], c = kingCol + move[1];
        if (r >= 0 && r < 8 && c >= 0 && c < 8 && board[r][c] && 
            board[r][c]->getType() == Piece::Type::Knight && 
            board[r][c]->getColor() == opponentColor) 
            {
                return true;
            }
    }

    int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (auto& dir : directions) 
    {
        int r = kingRow, c = kingCol;
        while (true) {
            r += dir[0];
            c += dir[1];
            if (r < 0 || r >= 8 || c < 0 || c >= 8) break;

            if (board[r][c]) 
            {
                if ((board[r][c]->getColor() == opponentColor) && 
                    (board[r][c]->getType() == Piece::Type::Rook || 
                     board[r][c]->getType() == Piece::Type::Queen)) 
                    {
                        return true;
                    }
                if (board[r][c]->getType() != Piece::Type::None) break;
            }
        }
    }

    directions[0][0] = -1; directions[0][1] = -1;
    directions[1][0] = 1; directions[1][1] = 1;
    directions[2][0] = -1; directions[2][1] = 1;
    directions[3][0] = 1; directions[3][1] = -1;

    for (auto& dir : directions) 
    {
        int r = kingRow, c = kingCol;
        while (true) {
            r += dir[0];
            c += dir[1];
            if (r < 0 || r >= 8 || c < 0 || c >= 8) break;

            if (board[r][c])    
            {
                if ((board[r][c]->getColor() == opponentColor) && 
                    (board[r][c]->getType() == Piece::Type::Bishop || 
                     board[r][c]->getType() == Piece::Type::Queen)) 
                    {
                        return true;
                    }
                if (board[r][c]->getType() != Piece::Type::None) break;
            }
        }
    }

    for (int i = -1; i <= 1; ++i) 
    {
        for (int j = -1; j <= 1; ++j) 
        {
            int r = kingRow + i, c = kingCol + j;
            if (r >= 0 && r < 8 && c >= 0 && c < 8 && 
                board[r][c] && board[r][c]->getType() == Piece::Type::King && 
                board[r][c]->getColor() == opponentColor) 
                {
                    return true;
                }
        }
    }

    return false;
}

/*
sf::Color Board::adjustPixel(const sf::Color& color) 
{
    ColorConverter::RGB rgb = {color.r, color.g, color.b};
    ColorConverter::HSV hsv = ColorConverter::rgbToHsv(rgb);
    ColorConverter::HSV adjustPixel = ColorConverter::adjustValue(hsv);
    ColorConverter::HSV adjustPixel2 = ColorConverter::adjustSaturation(adjustPixel);
    ColorConverter::RGB rgb2 = ColorConverter::hsvToRgb(adjustPixel2);

    int newR = rgb2.r;
    int newG = rgb2.g;
    int newB = rgb2.b;

    return sf::Color(newR, newG, newB, color.a);
}
*/