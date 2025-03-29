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
    explicit Board( sf::RenderWindow& window, sf::Vector2<unsigned int> desktopSize, sf::Texture& boardTexture, std::map<std::string, 
                    sf::Texture>& textures, SoundManager& soundManager, sf::Font& font);
    ~Board();
    void getKingInCheckImage(sf::RenderWindow& window);
    bool loadShader();
    void updateBoard(sf::RenderWindow& window);
    void setupBoard();
    void drawKingChecked(sf::RenderWindow& window);
    void drawTextOnChessboard(sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    void flipBoard(); 
    void drawPossibleMoves(sf::RenderWindow& window);
    std::vector<std::pair<int, int>> getPossibleMovesForPiece(int row, int col);
    sf::FloatRect drawPromotionWindow(sf::RenderWindow& window);
    bool isMouseInPromotionWindow(sf::RenderWindow& window);
    Piece::Type getPromotionPiece(const sf::Vector2i& mousePos);
    void promotePawn(Piece::Type promotionPiece);
    void handleMouseClick(const sf::Vector2i& mousePos);
    void handleMouseMove(const sf::Vector2i& mousePos);
    void handleMouseRelease(const sf::Vector2i& mousePos);
    bool canCastle(int row, int kingCol, int targetCol, Piece::Color kingColor); 

    unsigned int rounds;
    unsigned int roundEnPassant;
    std::array<std::array<std::unique_ptr<Piece>, 8>, 8> board;
    sf::RectangleShape promotionWindow;
    bool promotionActive;
    bool isFlipped;
    bool hasEnPassantMade;
    bool showLegalMoves;

private:
    void setScaleForAllPieces();
    void changeSquarePixels(); 
    sf::Vector2f calculateBoardPosition(int row, int col); 
    void flipBoardTexture();    
    std::tuple<Piece::Color, int, int> getPromotePawnPos();
    std::pair<sf::Vector2i, sf::Vector2i> getKingsPositions(); 
    //bool isKingInCheck(int row, int col, Piece::Color kingColor); 
    bool isKingInCheck(int kingRow, int kingCol, Piece::Color kingColor);
    //sf::Color adjustPixel(const sf::Color& color);

    std::vector<std::unique_ptr<Piece>> pieces;
    sf::Vector2<unsigned int> desktopSize;
    sf::Vector2u lastWindowSize;
    float newHeight;
    float newPosX, newPosY;
    unsigned int screenWidth, screenHeight;
    unsigned int squareWidth, squareHeight;
    sf::Color pixel1, pixel2;
    sf::Image boardImage;
    sf::Image boardImageOriginal;
    sf::Texture initialBoardTexture; 
    sf::Texture boardTexture;
    sf::Sprite boardSprite;
    std::map<std::string, sf::Texture> textures;
    sf::Font font;
    sf::Shader shader;
    std::vector<sf::Text> texts;
    bool isMoveCorrect;
    bool isWhiteTurn;
    sf::FloatRect promotionWindowPos;
    std::unique_ptr<Piece> selectedPiece;
    sf::Vector2i selectedPieceOriginalPos;
    SoundManager& soundManager;
    int lastRoundIndex;
    int newRow, newCol;
    bool isPieceSelected = false;
    int previousRow, previousCol;
    int nextRow = -1, nextCol = -1;
    int clickCount = -1;
    bool isDragging = false;
    std::vector<std::pair<int, int>> possibleMoves;
};

#endif
