#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window.hpp>

#include <iostream>
#include <map>
#include <string>
#include "board.hpp"
#include "piece.hpp"
#include "pawn.hpp"
#include "king.hpp"
#include "queen.hpp"
#include "bishop.hpp"
#include "knight.hpp"
#include "rook.hpp"

int main()
{
    sf::Vector2<unsigned int> desktopSize = sf::VideoMode::getDesktopMode().size;

    unsigned int screenWidth = desktopSize.x;
    unsigned int screenHeight = desktopSize.y;

    sf::RenderWindow window(sf::VideoMode({screenWidth, screenHeight}), "Chess", sf::Style::Default, sf::State::Fullscreen);

    sf::Texture boardTexture;
    
    if (!boardTexture.loadFromFile("Images/board.png")) 
    {
        return -1;
    }

    sf::Sprite boardSprite(boardTexture);

    float newHeight = screenHeight * 0.8f;

    float scaleFactorY = newHeight / boardTexture.getSize().y;

    float scaleFactorX = scaleFactorY * (boardTexture.getSize().x / boardTexture.getSize().y);

    boardSprite.setScale({scaleFactorX, scaleFactorY});

    float newWidth = boardTexture.getSize().x * scaleFactorX;
    float newPosX = (screenWidth - newWidth) / 2.0f;
    float newPosY = (screenHeight - newHeight) / 2.0f;

    boardSprite.setPosition({newPosX, newPosY});

    std::map<std::string, sf::Texture> textures;

    if (!textures["white-king"].loadFromFile("Images/white-king.png") ||
        !textures["white-pawn"].loadFromFile("Images/white-pawn.png") ||
        !textures["white-queen"].loadFromFile("Images/white-queen.png") ||
        !textures["white-rook"].loadFromFile("Images/white-rook.png") ||
        !textures["white-bishop"].loadFromFile("Images/white-bishop.png") ||
        !textures["white-knight"].loadFromFile("Images/white-knight.png") ||
        !textures["black-king"].loadFromFile("Images/black-king.png") ||
        !textures["black-pawn"].loadFromFile("Images/black-pawn.png") ||
        !textures["black-queen"].loadFromFile("Images/black-queen.png") ||
        !textures["black-rook"].loadFromFile("Images/black-rook.png") ||
        !textures["black-bishop"].loadFromFile("Images/black-bishop.png") ||
        !textures["black-knight"].loadFromFile("Images/black-knight.png")) 
    {
        return -1;
    }

    sf::RectangleShape promotionWindow({2*newHeight/8.0f, 2*newHeight/8.0f});
    float promotionWindowHeight = promotionWindow.getSize().y;
    float promotionWindowWidth = promotionWindow.getSize().x;
    float newPosPromotionWindowX = (screenWidth - promotionWindowWidth) / 2.0f;
    float newPosPromotionWindowY = (screenHeight - promotionWindowHeight) / 2.0f;
    promotionWindow.setPosition({newPosPromotionWindowX, newPosPromotionWindowY});

    Board board(newHeight);  
    Board& boardRef = board;  

    boardRef.setupBoard(textures);

    Piece::Color promotionColor = boardRef.isWhiteTurn ? Piece::Color::White : Piece::Color::Black;

    sf::Sprite whiteQueenSprite(textures["white-queen"]);

    float iconSize = promotionWindow.getSize().x /2.0f; 
    whiteQueenSprite.setScale({iconSize / whiteQueenSprite.getLocalBounds().size.x, iconSize / whiteQueenSprite.getLocalBounds().size.y});

    whiteQueenSprite.setPosition({newPosPromotionWindowX, newPosPromotionWindowY});

    std::map<std::string, sf::Sprite> promotionSprites;

    while (window.isOpen()) 
    { 
        while (const std::optional event = window.pollEvent()) 
        {
            if (event->is<sf::Event::Closed>()) 
            {
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) 
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                {
                    window.close();
                }
            }
            else if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) 
            {
                if (mousePressed->button == sf::Mouse::Button::Left)
                {
                    if (!boardRef.promotionActive)
                    {
                        boardRef.handleMouseClick(sf::Mouse::getPosition(window));
                    }
                    else
                    {
                        std::string promotionPiece;
                        do 
                        {
                            promotionPiece = boardRef.getPromotionPiece(sf::Mouse::getPosition(window), newPosX, newPosY, newHeight);
                            sf::sleep(sf::milliseconds(10));

                        } while (promotionPiece.empty()); 

                        boardRef.promotePawn(promotionPiece, textures);

                        boardRef.promotionActive = false;
                    }
                }
            }
            else if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) 
            {
                boardRef.handleMouseMove(sf::Mouse::getPosition(window));
            }
            else if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) 
            {
                if (mouseReleased->button == sf::Mouse::Button::Left)
                {   
                    if(!boardRef.promotionActive)
                    {
                        boardRef.handleMouseRelease(sf::Mouse::getPosition(window), newPosX, newPosY);
                    }
                }
            }
        }

        window.clear(sf::Color(128,128,128));

        window.draw(boardSprite);

        boardRef.draw(window, newPosX, newPosY, newHeight);

        if(boardRef.promotionActive)
        {
            boardRef.drawPromotionWindow(window, newPosX, newPosY, newHeight, screenWidth, screenHeight, textures);
        }

        window.display(); 
    }
}
