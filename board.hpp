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
    explicit Board(sf::RenderWindow& window, sf::Vector2<unsigned int> desktopSize, sf::Texture& boardTexture, std::map<std::string, sf::Texture>& textures, SoundManager& soundManager);
    ~Board();
    void updateBoard(sf::RenderWindow& window); 
    void setupBoard();
    void setScaleForAllPieces();
    void changeSquarePixels(); 
    void draw(sf::RenderWindow& window);
    void flipBoard();
    void flipBoardTexture();
    std::tuple<Piece::Color, int, int> getPromotePawnPos();
    sf::FloatRect drawPromotionWindow(sf::RenderWindow& window);
    bool isMouseInPromotionWindow(sf::RenderWindow& window);
    Piece::Type getPromotionPiece(const sf::Vector2i& mousePos);
    void promotePawn(Piece::Type promotionPiece);
    void handleMouseClick(const sf::Vector2i& mousePos);
    void handleMouseMove(const sf::Vector2i& mousePos);
    void handleMouseRelease(const sf::Vector2i& mousePos);
    std::pair<sf::Vector2i, sf::Vector2i> getKingsPositions(); 
    bool isKingInCheck(int row, int col, Piece::Color kingColor); 
    bool canCastle(int row, int kingCol, int targetCol, Piece::Color kingColor); 
    sf::Color adjustPixel(const sf::Color& color);

    bool isWhiteTurn;
    unsigned int rounds;
    unsigned int roundEnPassant;
    bool whiteKingChecked;
    bool blackKingChecked;
    std::array<std::array<std::unique_ptr<Piece>, 8>, 8> board;
    std::array<std::array<std::unique_ptr<Piece>, 8>, 8> boardCopy;
    std::array<std::array<std::unique_ptr<Piece>, 8>, 8> flippedBoard;
    sf::RectangleShape promotionWindow;
    std::map<std::string, sf::Sprite> promotionSprites;
    bool promotionActive;
    bool isFlipped;
    bool isMoveCorrect;
    bool hasEnPassantMade;

protected:
    std::vector<std::unique_ptr<Piece>> pieces;

private:
    sf::Vector2<unsigned int> desktopSize;
    float newHeight;
    float newPosX, newPosY;
    unsigned intscreenWidth, screenHeight;
    sf::Image boardImage;
    sf::Image boardImageOriginal;
    sf::Texture initialBoardTexture; 
    bool isInitialBoardTextureSet = false;
    sf::Texture boardTexture;
    sf::Sprite boardSprite;
    std::map<std::string, sf::Texture> textures;
    sf::RectangleShape highlightSquare1, highlightSquare2;
    bool showHighlights = false;
    sf::FloatRect promotionWindowPos;
    std::unique_ptr<Piece> selectedPiece;
    sf::Vector2i selectedPieceOriginalPos;
    bool isDragging = false;
    SoundManager& soundManager;
    int lastRoundIndex;
    int newRow, newCol, oldRow, oldCol;
};

#endif
