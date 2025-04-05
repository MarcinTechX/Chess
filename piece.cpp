#include "piece.hpp"

#include <iostream>

Piece::Piece(const sf::Texture& texture, float x, float y, Type type, Color color, Board& boardGame)
    : sprite(texture), pieceType(type), pieceColor(color), scaleFactor(1.0f), movesCount(0) 
{
    sprite.setPosition({x, y});
    this->boardGame = &boardGame;
}

void Piece::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}

void Piece::setPosition(float x, float y) 
{
    sprite.setPosition({x, y});
}

sf::Vector2f Piece::getPosition() const 
{
    return sprite.getPosition();
}

bool Piece::contains(const sf::Vector2i& mousePosition) 
{
    sf::FloatRect bounds = sprite.getGlobalBounds();
    return bounds.contains(static_cast<sf::Vector2f>(mousePosition));
}

Piece::Type Piece::getType() const 
{
    return pieceType; 
}

Piece::Color Piece::getColor() const 
{
    return pieceColor;
}

sf::Sprite& Piece::getSprite() 
{
    return sprite;
}

void Piece::setScaleFactor(float scaleFactor) 
{
    this->scaleFactor = scaleFactor;
    sprite.setScale({scaleFactor, scaleFactor}); 
}

bool Piece::canMove(int startRow, int startCol, int endRow, int endCol, bool testMove, bool sameColor) 
{   
    if (endRow < 0 || endRow >= 8 || endCol < 0 || endCol >= 8) 
    {
        return false;
    }

    if (canMoveImpl(startRow, startCol, endRow, endCol, testMove, sameColor)) 
    {   
        if (testMove)
        {   
            return true;
        }
        else
        {
            movesCount++;  
            return true;
        }
    }
    return false;
}



