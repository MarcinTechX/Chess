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

    unsigned int shorterSide = std::min(screenWidth, screenHeight);

    sf::RenderWindow window(sf::VideoMode({screenWidth, screenHeight}), "Chess", sf::Style::Default, sf::State::Fullscreen);
    sf::Texture boardTexture;
    
    if (!boardTexture.loadFromFile("Images/board/board.png")) 
    {
        return -1;
    }

    sf::Sprite boardSprite(boardTexture);

    float newHeight = shorterSide * 0.7f;

    float scaleFactorY = newHeight / boardTexture.getSize().y;

    float scaleFactorX = scaleFactorY * (boardTexture.getSize().x / boardTexture.getSize().y);

    boardSprite.setScale({scaleFactorX, scaleFactorY});

    float newWidth = boardTexture.getSize().x * scaleFactorX;
    float newPosX = (screenWidth - newWidth) / 2.0f;
    float newPosY = (screenHeight - newHeight) / 2.0f;

    boardSprite.setPosition({newPosX, newPosY});

    std::map<std::string, sf::Texture> textures;

    if (!textures["white-king"].loadFromFile("Images/pieces/white-king.png") ||
        !textures["white-pawn"].loadFromFile("Images/pieces/white-pawn.png") ||
        !textures["white-queen"].loadFromFile("Images/pieces/white-queen.png") ||
        !textures["white-rook"].loadFromFile("Images/pieces/white-rook.png") ||
        !textures["white-bishop"].loadFromFile("Images/pieces/white-bishop.png") ||
        !textures["white-knight"].loadFromFile("Images/pieces/white-knight.png") ||
        !textures["black-king"].loadFromFile("Images/pieces/black-king.png") ||
        !textures["black-pawn"].loadFromFile("Images/pieces/black-pawn.png") ||
        !textures["black-queen"].loadFromFile("Images/pieces/black-queen.png") ||
        !textures["black-rook"].loadFromFile("Images/pieces/black-rook.png") ||
        !textures["black-bishop"].loadFromFile("Images/pieces/black-bishop.png") ||
        !textures["black-knight"].loadFromFile("Images/pieces/black-knight.png")) 
    {
        return -1;
    }

    for (auto& texture : textures) 
    {
        texture.second.setSmooth(true);
    }

    Board board(newHeight, boardTexture, window);  
    Board& boardRef = board;  

    boardRef.setupBoard(textures);

    sf::FloatRect promotionWindowPos;
/*
    sf::Image boardImage = boardTexture.copyToImage();

    int numberOfPixelsToChange = 10000;
    sf::Color newColor(255, 0, 0);

    for (int i = 0; i < numberOfPixelsToChange; ++i)
    {
        unsigned int x = std::rand() % boardImage.getSize().x; 
        unsigned int y = std::rand() % boardImage.getSize().y; 

        if (x < boardImage.getSize().x && y < boardImage.getSize().y)
        {
            boardImage.setPixel({x, y}, newColor); 
        }
    }

    if (!boardTexture.loadFromImage(boardImage)) {
        std::cerr << "Failed to load texture from modified image!" << std::endl;
        return -1;
    }
*/

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
                if (keyPressed->scancode == sf::Keyboard::Scancode::F)
                {   
                    boardRef.flipBoard();
                    boardRef.getPromotePawnPos();
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
                    else if (boardRef.isMouseInPromotionWindow(window, promotionWindowPos))
                    {
                        Piece::Type promotionPiece;
                        do 
                        {
                            promotionPiece = boardRef.getPromotionPiece(sf::Mouse::getPosition(window), newPosX, newPosY);
                            sf::sleep(sf::milliseconds(10));

                        } while (promotionPiece == Piece::Type::None); 

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

        boardRef.draw(window, newPosX, newPosY);

        if (boardRef.promotionActive)
        {
            promotionWindowPos = boardRef.drawPromotionWindow(window, newPosX, newPosY, screenWidth, screenHeight, textures);
        }

        window.display(); 
    }
}
