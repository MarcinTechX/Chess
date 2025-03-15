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

class Board 
{
public:
    Board ();
    explicit Board(float newHeight);
    ~Board();
    void setupBoard(const std::map<std::string, sf::Texture>& textures);
    void setScaleForAllPieces(float newHeight);
    void draw(sf::RenderWindow& window, float newPosX, float newPosY, float newHeight);
    void handleMouseClick(const sf::Vector2i& mousePos);
    void handleMouseMove(const sf::Vector2i& mousePos);
    void handleMouseRelease(const sf::Vector2i& mousePos, float newPosX, float newPosY);
    bool isKingInCheck(int row, int col, Piece::Color kingColor); 
    bool canCastle(int row, int kingCol, int targetCol, Piece::Color kingColor); 
    std::pair<std::pair<sf::Vector2i, Piece::Color>, std::pair<sf::Vector2i, Piece::Color>> getKingsPositions(); 
    void drawPromotionWindow(sf::RenderWindow& window, float newPosX, float newPosY, float newHeight, unsigned int screenWidth, unsigned int screenHeight, std::map<std::string, sf::Texture>& textures);
    std::string getPromotionPiece(const sf::Vector2i& mousePos, float newPosX, float newPosY, float newHeight);
    void promotePawn(const std::string& promotionPiece, std::map<std::string, sf::Texture>& textures);

    bool isWhiteTurn = true;
    unsigned int rounds = 0;
    unsigned int roundEnPassant = 0;
    bool whiteKingChecked = false;
    bool blackKingChecked = false;
    std::array<std::array<std::unique_ptr<Piece>, 8>, 8> board;
    sf::RectangleShape promotionWindow;
    std::map<std::string, sf::Sprite> promotionSprites;
    bool promotionActive = false;

protected:
    std::vector<std::unique_ptr<Piece>> pieces;

private:
    float newHeight;
    std::unique_ptr<Piece> selectedPiece;
    sf::Vector2i selectedPieceOriginalPos;
    bool isDragging = false;
    bool isMoveCorrect = false;
};

#endif
