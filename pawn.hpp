#ifndef PAWN_HPP
#define PAWN_HPP

#include "piece.hpp"

class Pawn : public Piece 
{
public:
    Pawn(const sf::Texture& texture, float x, float y, Color color);  

    bool canMove(int startRow, int startCol, int endRow, int endCol, std::array<std::array<std::unique_ptr<Piece>, 8>, 8>& board);

    void setHasMoved(bool moved); 

private:
    bool hasMoved;
    unsigned int movesCount;
};

#endif
