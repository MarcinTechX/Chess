#ifndef PIECE_HPP
#define PIECE_HPP

#include <SFML/Graphics.hpp>

class Board;

class Piece 
{
public:
    enum class Type { King, Queen, Rook, Bishop, Knight, Pawn, None};
    enum class Color { White, Black, None };

    Piece(const sf::Texture& texture, float x, float y, Type type, Color color, Board& boardGame);
    virtual ~Piece() = default;

    bool canMove(int startRow, int startCol, int endRow, int endCol, bool testMove, bool sameColor);

    virtual bool canMoveImpl(int startRow, int startCol, int endRow, int endCol, bool testedMove, bool sameColor) = 0;

    virtual void draw(sf::RenderWindow& window);

    virtual void setPosition(float x, float y);
    sf::Vector2f getPosition() const;
    bool contains(const sf::Vector2i& mousePosition);

    void setScaleFactor(float scaleFactor);

    Type getType() const; 
    Color getColor() const;
    sf::Sprite& getSprite();

    unsigned int movesCount;

protected:
    sf::Sprite sprite;
    Type pieceType;
    Color pieceColor;
    float scaleFactor;
    Board* boardGame;
    bool testMove;
};

#endif
