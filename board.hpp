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
#include "parser.hpp"

class Board 
{
public:
    explicit Board( sf::RenderWindow& window, sf::Vector2<unsigned int> desktopSize, sf::Texture& boardTexture, std::map<std::string, 
                    sf::Texture>& textures, SoundManager& soundManager, sf::Font& font, Parser& parser);
    ~Board();
    bool loadShader();
    void updateBoard(sf::RenderWindow& window);
    void setupBoard();
    void resetGame();
    sf::Vector2i getBoardPositionFromMouse(int mouseX, int mouseY);
    void draw(sf::RenderWindow& window);
    void flipBoard(); 
    void drawPossibleMoves(sf::RenderWindow& window);
    std::vector<std::pair<int, int>> getPossibleMovesForPiece(int row, int col);
    std::vector<std::pair<int, int>> getPossibleMovesForPieceCopy(int row, int col);
    std::pair<bool, bool> areAnyMovesAvailable();
    sf::FloatRect drawPromotionWindow(sf::RenderWindow& window);
    bool isMouseInPromotionWindow(sf::RenderWindow& window);
    Piece::Type getPromotionPiece(sf::Vector2i mousePos);
    void promotePawn(Piece::Type promotionPiece);
    void clickOnPiece(sf::Vector2i mousePos);
    void handleMouseClick(sf::RenderWindow& window);
    void handleMouseMove(sf::Vector2i mousePos);
    void handleMouseRelease(sf::Vector2i mousePos);
    std::pair<float, float> getTempPos(const sf::Vector2i& mousePos);
    bool isPosOnCheckboard(float tempRow, float tempCol);
    bool checkIsMoveCorrect(int newRow, int newCol);
    bool canCastle(int row, int kingCol, int targetCol, Piece::Color kingColor); 
    void playGameSound();

    unsigned int rounds;
    unsigned int roundEnPassant;
    std::array<std::array<std::unique_ptr<Piece>, 8>, 8> board;
    sf::RectangleShape promotionWindow;
    bool promotionActive;
    bool isFlipped;
    bool hasEnPassantMade;
    bool showLegalMoves;
    bool soundPlayed = false;
    bool isPawnGetPromotion = false;

private:
    void resetBoard();
    void setScaleForAllPieces();
    void drawKingChecked(sf::RenderWindow& window,  std::pair<sf::Vector2i, sf::Vector2i>& kingsPositions);
    void drawTextOnChessboard(sf::RenderWindow& window);
    void drawStoreMovingPositions(sf::RenderWindow& window);
    void drawSelectedPiecePlace(sf::RenderWindow& window);
    void changeSquarePixels(); 
    std::tuple<Piece::Color, int, int> getPromotePawnPos();
    std::pair<sf::Vector2i, sf::Vector2i> getKingsPositions(); 
    bool isKingInCheck(int kingRow, int kingCol, Piece::Color kingColor);
    void addGamePosition();
    bool isBoardRepeatedThreeTimes();
    bool piecesEnoughToCheckmate();
    std::tuple<bool, bool, bool> attackTheSameColor(); 
    //sf::Color adjustPixel(const sf::Color& color);

    std::vector<std::unique_ptr<Piece>> pieces;
    sf::Vector2<unsigned int> desktopSize;
    sf::Vector2u lastWindowSize;
    float newHeight;
    float newPosX, newPosY;
    unsigned int screenWidth, screenHeight;
    unsigned int squareWidth, squareHeight;
    float scaleFactorY, scaleFactorX;
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
    Parser& parser;
    int lastRoundIndex;
    int newRow, newCol;
    bool isPieceSelected = false;
    int previousRow, previousCol;
    int nextRow = -1, nextCol = -1;
    int clickCount = -1;
    std::vector<std::pair<int, int>> possibleMoves;
    std::vector<std::pair<int, int>> possibleMovesCopy;
    std::vector<std::pair<int, int>> whiteMoves;
    std::vector<std::pair<int, int>> blackMoves;
    bool whiteHasMoves = true;
    bool blackHasMoves = true;
    bool isPieceCaptured = false;
    bool isWhiteKingChecked = false;
    bool isBlackKingChecked = false;
    std::unique_ptr<Piece> tempPiece;
    bool isCheckMate = false;
    bool isStaleMate = false;
    int checkRound = 0;
    std::pair<sf::Vector2i, sf::Vector2i> kingsPositions;
    std::vector<sf::RectangleShape> movingPositions;
    sf::RectangleShape r1, r2;
    int previousRowForMovingPos, previousColForMovingPos;
    int newRowForMovingPos, newColForMovingPos;
    sf::RectangleShape selectedPlace;
    int lastRoundDraw = 0;
    bool isDraw = false;
    std::vector<std::string> gamePositions;
    bool isBoardPosThreeTime = false;
    bool isPiecesEnoughToCheckmate = false;
    Piece::Type promotionPiece;
};

#endif