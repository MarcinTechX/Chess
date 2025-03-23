#ifndef BOARD_HPP
#define BOARD_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <map>
#include <array>
#include "piece.hpp"
#include "pawn.hpp"
#include "rook.hpp"
#include "knight.hpp"
#include "bishop.hpp"
#include "queen.hpp"
#include "king.hpp"
#include "soundmanager.hpp"

class Board 
{
public:
    Board();
    explicit Board(float newHeight, const sf::Texture& boardTexture, sf::RenderWindow& window);
    ~Board();
    void updateBoard(sf::RenderWindow& window); 
    void setupBoard(const std::map<std::string, sf::Texture>& textures);
    void setScaleForAllPieces();
    void changeSquarePixels(int oldRow, int oldCol, int newRow, int newCol, sf::Texture& boardTexture); 
    void draw(sf::RenderWindow& window, float newPosX, float newPosY);
    void flipBoard();
    void flipBoardTexture();
    std::tuple<Piece::Color, int, int> getPromotePawnPos();
    sf::FloatRect drawPromotionWindow(sf::RenderWindow& window, float newPosX, float newPosY, unsigned int screenWidth, unsigned int screenHeight, std::map<std::string, sf::Texture>& textures);
    bool isMouseInPromotionWindow(sf::RenderWindow& window, sf::FloatRect promotionWindowPos);
    Piece::Type getPromotionPiece(const sf::Vector2i& mousePos, float newPosX, float newPosY);
    void promotePawn(Piece::Type promotionPiece, std::map<std::string, sf::Texture>& textures);
    void handleMouseClick(const sf::Vector2i& mousePos);
    void handleMouseMove(const sf::Vector2i& mousePos);
    void handleMouseRelease(const sf::Vector2i& mousePos, float newPosX, float newPosY);
    std::pair<sf::Vector2i, sf::Vector2i> getKingsPositions(); 
    bool isKingInCheck(int row, int col, Piece::Color kingColor); 
    bool canCastle(int row, int kingCol, int targetCol, Piece::Color kingColor); 
    sf::Color adjustPixel(const sf::Color& color);

    bool isWhiteTurn = true;
    unsigned int rounds = 0;
    unsigned int roundEnPassant = 0;
    bool whiteKingChecked = false;
    bool blackKingChecked = false;
    std::array<std::array<std::unique_ptr<Piece>, 8>, 8> board;
    std::array<std::array<std::unique_ptr<Piece>, 8>, 8> boardCopy;
    std::array<std::array<std::unique_ptr<Piece>, 8>, 8> flippedBoard;
    sf::RectangleShape promotionWindow;
    std::map<std::string, sf::Sprite> promotionSprites;
    bool promotionActive = false;
    bool isFlipped = false;
    bool isMoveCorrect = false;
    bool hasEnPassantMade = false;

protected:
    std::vector<std::unique_ptr<Piece>> pieces;

private:
    float newHeight;
    sf::Texture initialBoardTexture; 
    bool isInitialBoardTextureSet = false;
    sf::Texture boardTexture;
    sf::Sprite boardSprite;
    std::unique_ptr<Piece> selectedPiece;
    sf::Vector2i selectedPieceOriginalPos;
    bool isDragging = false;
    SoundManager soundManager;
    int lastRoundIndex;
};

#endif
