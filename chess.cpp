#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window.hpp>

#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <iomanip>
#include "board.hpp"
#include "piece.hpp"
#include "pawn.hpp"
#include "king.hpp"
#include "queen.hpp"
#include "bishop.hpp"
#include "knight.hpp"
#include "rook.hpp"
#include "soundmanager.hpp"

void loadBoard(sf::Texture& boardTexture)
{
    if (!boardTexture.loadFromFile("./Images/board/board.png")) 
    {
        std::cerr << "Failed to load board!" << std::endl;
    }
}

void loadPieces(std::map<std::string, sf::Texture>& textures)
{
    sf::Image image;
    if (!image.loadFromFile("./Images/pieces/pieces.png")) 
    {
        std::cerr << "Failed to load image!" << std::endl;
        return;
    }

    const int imageWidth = image.getSize().x;
    const int imageHeight = image.getSize().y;

    const int pieceWidth = imageWidth / 6;
    const int pieceHeight = imageHeight / 2;

    std::map<std::string, sf::IntRect> pieceRects;
    std::vector<std::string> whitePieces = {"pawn", "king", "queen", "rook", "bishop", "knight"};
    std::vector<std::string> blackPieces = {"pawn", "king", "queen", "rook", "bishop", "knight"};

    int baseYWhite = 0;
    int baseYBlack = pieceHeight;

    for (int i = 0; i < whitePieces.size(); i++) 
    {
        pieceRects["white-" + whitePieces[i]] = sf::IntRect({i * pieceWidth, baseYWhite}, {pieceWidth, pieceHeight});
        pieceRects["black-" + blackPieces[i]] = sf::IntRect({i * pieceWidth, baseYBlack}, {pieceWidth, pieceHeight});
    }

    for (const auto& [key, rect] : pieceRects) 
    {
        sf::Texture texture;
        if (texture.loadFromImage(image, true, rect)) 
        {
            texture.setSmooth(true);
            textures[key] = std::move(texture); 
        }
        else 
        {
            std::cerr << "Failed to load texture for " << key << std::endl;
        }
    }
}

void loadSounds(SoundManager& soundManager)
{
    if (!soundManager.loadSound("move", "./Sounds/move.ogg") ||
        !soundManager.loadSound("capture", "./Sounds/capture.ogg") ||
        !soundManager.loadSound("incorrect_move", "./Sounds/incorrect_move.ogg")) 
    {
        std::cerr << "Failed to load sound!" << std::endl;
    }
}

int main()
{
    sf::Vector2<unsigned int> desktopSize = sf::VideoMode::getDesktopMode().size;

    sf::RenderWindow window(sf::VideoMode({desktopSize.x, desktopSize.y}), "Chess", sf::Style::Default, sf::State::Fullscreen);
    window.setVerticalSyncEnabled(true);
    //window.setFramerateLimit(165);
    
    sf::Texture boardTexture;

    std::map<std::string, sf::Texture> textures;

    SoundManager soundManager;

    std::thread loadBoardThread(loadBoard, std::ref(boardTexture));
    std::thread loadPiecesThread(loadPieces, std::ref(textures));
    std::thread loadSoundsThread(loadSounds, std::ref(soundManager));    

    loadBoardThread.join();
    loadPiecesThread.join();    
    loadSoundsThread.join();

    Board board(window, desktopSize, boardTexture, textures, soundManager);  
    Board& boardRef = board; 

    boardRef.setupBoard();

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
                    else if (boardRef.isMouseInPromotionWindow(window))
                    {
                        Piece::Type promotionPiece;
                        do 
                        {
                            promotionPiece = boardRef.getPromotionPiece(sf::Mouse::getPosition(window));

                        } while (promotionPiece == Piece::Type::None); 

                        boardRef.promotePawn(promotionPiece);

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
                        boardRef.handleMouseRelease(sf::Mouse::getPosition(window));
                    }
                }
            }
        }

        window.clear(sf::Color(128,128,128));

        boardRef.draw(window);

        if (boardRef.promotionActive)
        {
            boardRef.drawPromotionWindow(window);
        }

        window.display(); 
    }
}
