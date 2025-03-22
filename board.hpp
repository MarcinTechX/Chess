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
    explicit Board(float newHeight);
    ~Board();
    void setupBoard(const std::map<std::string, sf::Texture>& textures);
    void setScaleForAllPieces();
    void draw(sf::RenderWindow& window, float newPosX, float newPosY);
    std::array<std::array<std::unique_ptr<Piece>, 8>, 8> copyBoard(const std::array<std::array<std::unique_ptr<Piece>, 8>, 8>& board);
    void flipBoard();
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
    std::unique_ptr<Piece> selectedPiece;
    sf::Vector2i selectedPieceOriginalPos;
    bool isDragging = false;
    SoundManager soundManager;
};

#endif
