#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window.hpp>

#include <iostream>
#include <map>
#include <string>
#include "board.hpp"

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
        !textures["black-knight"].loadFromFile("Images/black-knight.png") 
    ) {
        return -1;
    }

    Board board(newHeight);
    board.setupBoard(textures);

    while (window.isOpen()) 
    {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
            }
            else if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePressed->button == sf::Mouse::Button::Left)
                    board.handleMouseClick(sf::Mouse::getPosition(window));
            }
            else if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
                board.handleMouseMove(sf::Mouse::getPosition(window));
            }
            else if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouseReleased->button == sf::Mouse::Button::Left)
                    board.handleMouseRelease(sf::Mouse::getPosition(window), newPosX, newPosY);
            }
        }
        
        sf::Vector2i whiteKingPos = board.findWhiteKingPosition(board.board);
        sf::Vector2i blackKingPos = board.findBlackKingPosition(board.board);

        window.clear(sf::Color(128,128,128));

        window.draw(boardSprite);

        board.draw(window, newPosX, newPosY, newHeight);

        board.isWhiteKingInCheck(board.board);

        board.isBlackKingInCheck(board.board);

        window.display();
    }
}
