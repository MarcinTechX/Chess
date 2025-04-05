#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <string>

class Parser
{
public:
    void addBoardPosition(  int rounds, int rowFrom, int colFrom, int rowTo, int colTo, char pieceType,
                            bool isKingChecked, bool isDrawOrStalemate, bool isCheckmate, char promotionPiece,
                            bool canPieceAttackFromRow, bool canPieceAttackFromCol, bool canAttack, bool isPieceCaptured);

    void saveToFile();
    void resetBoardPostionHistory();

    struct BoardPosition
    {
        int rounds;
        int rowFrom, colFrom;
        int rowTo, colTo;
        char pieceType;
        bool isKingChecked;
        bool isDrawOrStalemate;
        bool isCheckmate;
        char promotionPiece;
        bool canPieceAttackFromRow;
        bool canPieceAttackFromCol;
        bool canAttack;
        bool isPieceCaptured;
    };

    void generatePGN();

    std::vector<BoardPosition> boardPositionHistory;

    std::string PGNString = "";

private:
};

#endif 