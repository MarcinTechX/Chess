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

bool Piece::canMove(int startRow, int startCol, int endRow, int endCol) 
{
    if (canMoveImpl(startRow, startCol, endRow, endCol)) {
        movesCount++;  
        return true;
    }
    return false;
}
