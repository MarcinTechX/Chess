#include "parser.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <filesystem>

void Parser::addBoardPosition(  int rounds, int rowFrom, int colFrom, int rowTo, int colTo, char pieceType,
                                bool isKingChecked, bool isDrawOrStalemate, bool isCheckmate, char promotionPiece,
                                bool canPieceAttackFromRow, bool canPieceAttackFromCol, bool canAttack, bool isPieceCaptured)
{
    BoardPosition newPosition = 
    { rounds, rowFrom, colFrom, rowTo, colTo, pieceType, 
        isKingChecked, isDrawOrStalemate, isCheckmate, promotionPiece,
        canPieceAttackFromRow, canPieceAttackFromCol, canAttack, isPieceCaptured
    };

    boardPositionHistory.push_back(newPosition);
}

void Parser::saveToFile() 
{
    generatePGN();

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time);

    std::ostringstream filenameStream;
    filenameStream << "PGNs/" << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S") << ".txt";
    std::string filename = filenameStream.str();

    std::filesystem::create_directories("PGNs");

    std::ofstream outFile(filename);
    if (outFile.is_open()) 
    {
        outFile << PGNString;
        outFile.close();
    }
}

void Parser::generatePGN()
{
    std::stringstream pgnStream;

    for (const auto& pos : boardPositionHistory)
    {
       if (pos.rounds % 2 == 1)
       {
            pgnStream << pos.rounds / 2 + 1;
            pgnStream << ".";
            pgnStream << " ";
       }

       if (pos.pieceType == 'K' && pos.colFrom == 4 && (pos.colTo == 2 || pos.colTo == 6))
       {
            if (pos.colTo == 2)
            {
                pgnStream << "O-O-O";
            }
            if (pos.colTo == 6)
            {
                pgnStream << "O-O";
            }
        }
        else 
        {
            if (!(pos.pieceType == 'P' || pos.pieceType == ' '))
            {
                pgnStream << pos.pieceType;
            }

            if ((pos.pieceType == 'P' && pos.isPieceCaptured) || pos.promotionPiece != ' ')
            {
                pgnStream << char('a' + pos.colFrom);
            }

            if (pos.canAttack && !pos.canPieceAttackFromRow && !pos.canPieceAttackFromCol)
            {
                pgnStream << char('a' + pos.colFrom); 
            }
            else if (pos.canPieceAttackFromCol)
            {
                pgnStream << std::to_string(pos.rowFrom + 1);
            }
            else if (pos.canPieceAttackFromRow)
            {
                pgnStream << char('a' + pos.colFrom); 
            }

            if (pos.isPieceCaptured)
            {
                pgnStream << "x";
            }

            pgnStream << char('a' + pos.colTo); 
            pgnStream << std::to_string(pos.rowTo + 1);
        }

        if (pos.promotionPiece != ' ')
        {
            pgnStream << "=";

            pgnStream << pos.promotionPiece;
        }

        if (pos.isCheckmate)
        {
            pgnStream << "#";
        }
        else if (pos.isKingChecked)
        {
            pgnStream << "+";
        }
  
        if (pos.isDrawOrStalemate || pos.isCheckmate)
        {   
           pgnStream << " ";

           if (pos.isDrawOrStalemate)
           {
            pgnStream << "1/2 - 1/2";
           }
           else if (pos.isCheckmate)
           {
            if (pos.rounds % 2 == 1)
            {
                pgnStream << "1-0";
            }
            else if (pos.rounds % 2 == 0)
            {
                pgnStream << "0-1";
            }
           }
        }

        if (pos.rounds % 2 == 0)
        {
            pgnStream << "\n";
        }
        else
        {
            pgnStream << " "; 
        }    
    }

    PGNString = pgnStream.str();
}

void Parser::resetBoardPostionHistory()
{
    boardPositionHistory.clear();
}
