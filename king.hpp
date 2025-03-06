#ifndef KING_HPP
#define KING_HPP

#include "piece.hpp"

class King : public Piece 
{
public:
    King(const sf::Texture& texture, float x, float y, Color color);

    bool canMove(int startRow, int startCol, int endRow, int endCol, std::array<std::array<std::unique_ptr<Piece>, 8>, 8>& board) override;
};

#endif 