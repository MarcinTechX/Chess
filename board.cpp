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
    scaleFactorY = newHeight / boardTexture.getSize().y;
    scaleFactorX = scaleFactorY * (boardTexture.getSize().x / boardTexture.getSize().y);
    boardSprite.setScale({scaleFactorX, scaleFactorY}); 

    float newWidth = boardTexture.getSize().x * scaleFactorX;
    newPosX = (screenWidth - newWidth) / 2.0f;
    newPosY = (screenHeight - newHeight) / 2.0f;

    boardSprite.setPosition({newPosX, newPosY}); 
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

sf::Vector2i Board::getBoardPositionFromMouse(int mouseX, int mouseY)
{
    int offsetX = newPosX;
    int offsetY = newPosY;

    if (isFlipped)
    {
        mouseY = newPosY + newHeight - (mouseY - newPosY);
        mouseX = newPosX + newHeight - (mouseX - newPosX);
    }

    return sf::Vector2i(mouseX, mouseY);
}

void Board::changeSquarePixels() 
{      
    int avgRed = (pixel1.r + pixel2.r) / 2;
    int avgGreen = (pixel1.g + pixel2.g) / 2;
    int avgBlue = (pixel1.b + pixel2.b) / 2;

    sf::Color avgColor(avgRed, avgGreen, avgBlue);

    movingPositions.clear();

    float squareWidth = newHeight / 8.0;
    float squareHeight = newHeight / 8.0;

    previousRowForMovingPos = previousRow;
    previousColForMovingPos = previousCol;

    newRowForMovingPos = newRow;
    newColForMovingPos = newCol;

    r1.setSize(sf::Vector2f({squareWidth, squareHeight}));
    r1.setPosition({newPosX + previousColForMovingPos * squareWidth, newPosY + (7 - previousRowForMovingPos) * squareHeight});
    r1.setFillColor(avgColor);

    r2.setSize(sf::Vector2f({squareWidth, squareHeight}));
    r2.setPosition({newPosX + newColForMovingPos * squareWidth, newPosY + (7 - newRowForMovingPos) * squareHeight});
    r2.setFillColor(avgColor);

    movingPositions.push_back(r1);
    movingPositions.push_back(r2);
}

void Board::drawStoreMovingPositions(sf::RenderWindow& window)
{   
    if (isFlipped)
    {   
        float squareWidth = newHeight / 8.0;
        float squareHeight = newHeight / 8.0;
        
        r1.setPosition({newPosX + (7 - previousColForMovingPos) * squareWidth, newPosY + previousRowForMovingPos * squareHeight});
        r2.setPosition({newPosX + (7 - newColForMovingPos) * squareWidth, newPosY + newRowForMovingPos * squareHeight});
    }
    else
    {
        float squareWidth = newHeight / 8.0;
        float squareHeight = newHeight / 8.0;
        
        r1.setPosition({newPosX + previousColForMovingPos * squareWidth, newPosY + (7 - previousRowForMovingPos) * squareHeight});
        r2.setPosition({newPosX + newColForMovingPos * squareWidth, newPosY + (7 - newRowForMovingPos) * squareHeight});
    }

    window.draw(r1);
    window.draw(r2);
}

void Board::drawSelectedPiecePlace(sf::RenderWindow& window)
{    
    int avgRed = (pixel1.r + pixel2.r) / 2;
    int avgGreen = (pixel1.g + pixel2.g) / 2;
    int avgBlue = (pixel1.b + pixel2.b) / 2;

    sf::Color avgColor(avgRed, avgGreen, avgBlue);

    float squareWidth = newHeight / 8.0;
    float squareHeight = newHeight / 8.0;
    
    selectedPlace.setSize(sf::Vector2f({squareWidth, squareHeight}));
    
    if (isFlipped)
    {
        selectedPlace.setPosition({newPosX + (7 - previousCol) * squareWidth, newPosY + previousRow * squareHeight});
    }
    else
    {
        selectedPlace.setPosition({newPosX + previousCol * squareWidth, newPosY + (7 - previousRow) * squareHeight});
    }

    if (isPieceSelected)
    {
        selectedPlace.setFillColor(avgColor);
    }
    else
    {
        selectedPlace.setFillColor(sf::Color::Transparent);
    }

    window.draw(selectedPlace);
}

void Board::draw(sf::RenderWindow& window) 
{   
    window.draw(boardSprite);

    drawTextOnChessboard(window);

    if (selectedPiece || clickCount % 2 == 1) 
    {
        drawSelectedPiecePlace(window);
    }

    drawStoreMovingPositions(window);

    if (isWhiteKingChecked || isBlackKingChecked || isCheckMate)
    {
        drawKingChecked(window, kingsPositions);
    }

    float offset = newHeight / 8.0f;
    float posX, posY;

    for (int row = 0; row < 8; row++) 
    {
        for (int col = 0; col < 8; col++) 
        {
            if (board[row][col] && board[row][col] != selectedPiece) 
            {   
                int displayRow = isFlipped ? row : 7 - row;
                int displayCol = isFlipped ? 7 - col : col;

                posX = newPosX + displayCol * offset;
                posY = newPosY + displayRow * offset;

                board[row][col]->setPosition(posX, posY);
                board[row][col]->draw(window);
            }
        }
    }
    
    if (selectedPiece || clickCount % 2 == 1) 
    {   
        if (showLegalMoves && !isCheckMate)
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

void Board::drawKingChecked(sf::RenderWindow& window, std::pair<sf::Vector2i, sf::Vector2i>& kingsPositions)
{   
    if (!loadShader())
    {
        return;
    }

    sf::Vector2i kingPos;
    const sf::Texture* kingTexture = nullptr;

    if (isWhiteKingChecked) 
    {
        kingPos = kingsPositions.first;
        kingTexture = &textures["white-king"];
    } 
    else if (isBlackKingChecked) 
    {
        kingPos = kingsPositions.second;
        kingTexture = &textures["black-king"];
    }

    if (!kingTexture) 
    {
        return; 
    }

    sf::Sprite spriteChecked(*kingTexture);
    
    float squareWidth = newHeight / 8.0f;
    float scaleFactor = squareWidth / std::max(spriteChecked.getGlobalBounds().size.x, 1.0f);
    spriteChecked.setScale({scaleFactor, scaleFactor});
    if (isFlipped)
    {
        spriteChecked.setPosition({newPosX + (7 - kingPos.x) * squareWidth, newPosY + kingPos.y * squareWidth});
    }
    else
    {
        spriteChecked.setPosition({newPosX + kingPos.x * squareWidth, newPosY + (7 - kingPos.y) * squareWidth});
    }
    
    window.draw(spriteChecked, &shader);
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
    
    clickCount = 0;
   
    isPieceSelected = false;

    isFlipped = !isFlipped;
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
            int row;
            int col;

            if (isFlipped)
            {
                row = 7 - move.first;
                col = 7 - move.second;
            }
            else
            {
                row = move.first;
                col = move.second;
            }

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
    
    if (!board[row][col]) 
    {
        return possibleMoves; 
    }

    Piece* originalPiece = board[row][col].get(); 

    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            if (originalPiece->canMove(row, col, r, c, true)) 
            {  
                std::unique_ptr<Piece> tempPiece = std::move(board[r][c]);
                board[r][c] = std::move(board[row][col]);
                board[row][col] = nullptr;

                auto kingsPositions = getKingsPositions();
                sf::Vector2i whiteKingPos = kingsPositions.first;
                sf::Vector2i blackKingPos = kingsPositions.second;

                bool isWhiteKingChecked = isKingInCheck(whiteKingPos.y, whiteKingPos.x, Piece::Color::White);
                bool isBlackKingChecked = isKingInCheck(blackKingPos.y, blackKingPos.x, Piece::Color::Black);

                if (!((isWhiteTurn && isWhiteKingChecked) || (!isWhiteTurn && isBlackKingChecked))) 
                {
                    possibleMoves.push_back({r, c});
                } 

                board[row][col] = std::move(board[r][c]);
                board[r][c] = std::move(tempPiece); 
            }
        }
    }

    return possibleMoves; 
}

std::vector<std::pair<int, int>> Board::getPossibleMovesForPieceCopy(int row, int col) 
{    
    possibleMovesCopy.clear();

    if (!board[row][col]) 
    {
        return possibleMovesCopy; 
    }

    Piece* originalPiece = board[row][col].get(); 

    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            if (originalPiece->canMove(row, col, r, c, true)) 
            {  
                std::unique_ptr<Piece> tempPiece = std::move(board[r][c]);
                board[r][c] = std::move(board[row][col]);
                board[row][col] = nullptr;

                auto kingsPositions = getKingsPositions();
                sf::Vector2i whiteKingPos = kingsPositions.first;
                sf::Vector2i blackKingPos = kingsPositions.second;

                bool isWhiteKingChecked = isKingInCheck(whiteKingPos.y, whiteKingPos.x, Piece::Color::White);
                bool isBlackKingChecked = isKingInCheck(blackKingPos.y, blackKingPos.x, Piece::Color::Black);

                if (!((isWhiteTurn && isWhiteKingChecked) || (!isWhiteTurn && isBlackKingChecked))) 
                {
                    possibleMovesCopy.push_back({r, c});
                } 

                board[row][col] = std::move(board[r][c]);
                board[r][c] = std::move(tempPiece); 
            }
        }
    }

    return possibleMovesCopy; 
}

std::pair<bool, bool> Board::areAnyMovesAvailable()
{   
    whiteHasMoves = true;
    blackHasMoves = true;

    possibleMovesCopy.clear(); 
    whiteMoves.clear();
    blackMoves.clear();

    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            if (board[row][col])
            {
                std::vector<std::pair<int, int>> moves = getPossibleMovesForPieceCopy(row, col);

                if (board[row][col]->getColor() == Piece::Color::White)
                {
                    whiteMoves.insert(whiteMoves.end(), moves.begin(), moves.end());
                }
                else
                {
                    blackMoves.insert(blackMoves.end(), moves.begin(), moves.end());
                }
            }
        }
    }

    whiteHasMoves = !whiteMoves.empty();
    blackHasMoves = !blackMoves.empty();

    return {whiteHasMoves, blackHasMoves};
}

std::tuple<Piece::Color, int, int> Board::getPromotePawnPos()
{
    Piece::Color color = isWhiteTurn ? Piece::Color::White : Piece::Color::Black;

    int row = (color == Piece::Color::White) ? 7 : 0;

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
            promotionWindow.setPosition({newPosX + (7 - col) * newHeight / 8.0f, newPosY + (row - 3) * newHeight / 8.0f});
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
            promotionWindow.setPosition({newPosX + (7 - col) * newHeight / 8.0f, newPosY + row * newHeight / 8.0f});
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

Piece::Type Board::getPromotionPiece(sf::Vector2i mousePos)
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

    mousePos = getBoardPositionFromMouse(mousePos.x, mousePos.y);

    float iconSize = newHeight / 8.0f; 
    int rowMouse = (mousePos.y - newPosY) / iconSize;   
    int colMouse = (mousePos.x - newPosX) / iconSize;
    
    if (colMouse == col)
    {   
        if (color == Piece::Color::White)
        {
            return pieceTypes[rowMouse];
        }
        else 
        {   
            return pieceTypes[7 - rowMouse];
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
}

void Board::clickOnPiece(sf::Vector2i mousePos) 
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
                            getPossibleMovesForPiece(row, col);
                            selectedPiece = std::move(board[row][col]);
                            isPieceSelected = true;
                            previousRow = row;
                            previousCol = col;
                            selectedPieceOriginalPos = {col, row};
                        }
                        return;
                    }
                }
            }
        }
    }
}

void Board::handleMouseClick(sf::RenderWindow& window)
{
    if (isCheckMate || isStaleMate)
    {
        return;
    }

    sf::Vector2i mousePos = sf::Mouse::getPosition(window); 
    sf::Vector2i mousePosInv = getBoardPositionFromMouse(mousePos.x, mousePos.y);

    if (promotionActive)
    {
        if (isMouseInPromotionWindow(window))
        {
            Piece::Type promotionPiece;
            do 
            {
                promotionPiece = getPromotionPiece(sf::Mouse::getPosition(window));

            } while (promotionPiece == Piece::Type::None); 

            if (promotionPiece != Piece::Type::None)
            {    
                isPawnGetPromotion = true;
                rounds++;
            }

            promotePawn(promotionPiece);
            promotionActive = false;
        }
        return;
    }

    std::pair<float, float> tempPos = getTempPos(mousePosInv);
    int clickedRow = static_cast<int>(tempPos.first);
    int clickedCol = static_cast<int>(tempPos.second);

    if (clickedRow < 0 || clickedRow >= 8 || clickedCol < 0 || clickedCol >= 8)
    {
        return; 
    }

    if (board[clickedRow][clickedCol] && board[clickedRow][clickedCol]->getColor() == (isWhiteTurn ? Piece::Color::White : Piece::Color::Black))
    {
        clickOnPiece(mousePos); 
    }
    else if (isPieceSelected) 
    {
        handleMouseRelease(mousePos);
    }
}

void Board::handleMouseMove(sf::Vector2i mousePos) 
{   
    if (selectedPiece && !isCheckMate) 
    {
        isDragging = true;
        selectedPiece->setPosition(mousePos.x - (newHeight / 16.0f), mousePos.y - (newHeight/ 16.0f)); 
    }
}

void Board::handleMouseRelease(sf::Vector2i mousePos)  
{   
    mousePos = getBoardPositionFromMouse(mousePos.x, mousePos.y);
    std::pair<float, float> tempPos = getTempPos(mousePos);
    float tempRow = tempPos.first;
    float tempCol = tempPos.second;

    if (!isPosOnCheckboard(tempRow, tempCol) && !isCheckMate)
    {   
        return;
    }

    newRow = std::max(0, std::min(7, static_cast<int>(tempRow)));
    newCol = std::max(0, std::min(7, static_cast<int>(tempCol)));

    isMoveCorrect = false;
    hasEnPassantMade = false;
    isPieceCaptured = false;
    isWhiteKingChecked = false;
    isBlackKingChecked = false;
    soundPlayed = false;

    if (!selectedPiece && clickCount % 2 == 1) 
    {
        if (board[previousRow][previousCol]) 
        {
            selectedPiece = std::move(board[previousRow][previousCol]);
            selectedPieceOriginalPos = {previousCol, previousRow};
        }
    }

    if (selectedPiece) 
    {
        checkIsMoveCorrect(newRow, newCol);
    }

    isPieceCaptured = (tempPiece != nullptr) || hasEnPassantMade;
    
    if (!isMoveCorrect && selectedPiece) 
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

    if (board[newRow][newCol] && board[newRow][newCol]->getType() == Piece::Type::Pawn)
    {
        Piece::Color pieceColor = board[newRow][newCol]->getColor();
    
        int promotionRowWhite = 7;
        int promotionRowBlack = 0;

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
            isPieceSelected = false;
            clickCount = 0;
            rounds++;
        }
    }

    kingsPositions = getKingsPositions();
    sf::Vector2i whiteKingPos = kingsPositions.first;
    sf::Vector2i blackKingPos = kingsPositions.second;

    isWhiteKingChecked = isKingInCheck(whiteKingPos.y, whiteKingPos.x, Piece::Color::White);
    isBlackKingChecked = isKingInCheck(blackKingPos.y, blackKingPos.x, Piece::Color::Black);

    areAnyMovesAvailable();

    if ((isPawnGetPromotion || isMoveCorrect) && !isCheckMate && !promotionActive)
    {  
        soundManager.stopAllSounds();
        playGameSound();
    }

    isPawnGetPromotion = false;
    
    isDragging = false;
}

std::pair<float, float> Board::getTempPos(const sf::Vector2i& mousePos)
{
    float tempRow = 8 - (mousePos.y - newPosY) / (newHeight / 8.0f);
    float tempCol = (mousePos.x - newPosX) / (newHeight / 8.0f);

    return {tempRow, tempCol};
}

bool Board::isPosOnCheckboard(float tempRow, float tempCol)
{
    if (tempRow < 0 || tempRow > 8 || tempCol < 0 || tempCol > 8)
    {
        if (selectedPiece)
        {
            board[selectedPieceOriginalPos.y][selectedPieceOriginalPos.x] = std::move(selectedPiece);
            isPieceSelected = false;
            clickCount = 0;
        }
        return false;
    }
    return true;
}

bool Board::checkIsMoveCorrect(int newRow, int newCol)
{
    tempPiece;

    if (newRow == previousRow && newCol == previousCol)
    {
        isMoveCorrect = false;
    }
    else if (newCol >= 0 && newCol < 8 && newRow >= 0 && newRow < 8) 
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

            if ((isWhiteTurn && isWhiteKingChecked) || (!isWhiteTurn && isBlackKingChecked)) 
            {
                board[selectedPieceOriginalPos.y][selectedPieceOriginalPos.x] = std::move(board[newRow][newCol]);
                board[newRow][newCol] = std::move(tempPiece);
                if (!isCheckMate)
                {
                    clickCount = 0;
                    soundManager.playSoundOnce("incorrect_move");
                }
            } 
            else 
            {
                isMoveCorrect = true;
                changeSquarePixels();
            }
        }
    }
    return isMoveCorrect;
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

bool Board::canCastle(int row, int kingCol, int targetCol, Piece::Color kingColor) 
{
    int midCol = (targetCol == 2) ? 3 : 5;  
    int step = (targetCol > kingCol) ? 1 : -1;  

    for (int col = kingCol + step; col != targetCol; col += step) 
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

void Board::playGameSound() 
{  
    if (rounds > 0)
    {   
        if ((!isWhiteTurn && !blackHasMoves && isBlackKingChecked) || (isWhiteTurn && !whiteHasMoves && isWhiteKingChecked))
        {
            soundManager.playSoundOnce("checkmate");
            isCheckMate = true;
            soundPlayed = true; 
        }
        else if (isWhiteKingChecked || isBlackKingChecked)
        {
            soundManager.playSoundOnce("check");
            soundPlayed = true;
        }
        else if (isPieceCaptured)
        {
            soundManager.playSoundOnce("capture");
            soundPlayed = true;
        }
        else if ((isWhiteTurn && !whiteHasMoves) || (!isWhiteTurn && !blackHasMoves))
        {
            soundManager.playSoundOnce("stalemate");
            isStaleMate = true;
            soundPlayed = true;
        }
        else if (!isPieceCaptured)
        {   
            soundManager.playSoundOnce("move");
            soundPlayed = true;
        }
    }
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