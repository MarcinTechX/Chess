#ifndef KNIGHT_HPP
#define KNIGHT_HPP

#include "piece.hpp"

class Knight : public Piece 
{
public:
    Knight(const sf::Texture& texture, float x, float y, Color color);

    bool canMove(int startRow, int startCol, int endRow, int endCol, std::array<std::array<std::unique_ptr<Piece>, 8>, 8>& board) override;
};

#endif 