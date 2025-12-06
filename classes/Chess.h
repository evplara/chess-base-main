#pragma once

#include <vector>
#include <map>
#include <string>
#include "Game.h"
#include "Grid.h"
#include "Bitboard.h"    

constexpr int pieceSize = 80;
    


// enum ChessPiece
// {
//     NoPiece,
//     Pawn,
//     Knight,
//     Bishop,
//     Rook,
//     Queen,
//     King
// };

class ChessSquare;      

class Chess : public Game
{
public:
    Chess();
    ~Chess();

    void setUpBoard() override;

    bool canBitMoveFrom(Bit &bit, BitHolder &src) override;
    bool canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
    bool actionForEmptyHolder(BitHolder &holder) override;

    void stopGame() override;

    Player *checkForWinner() override;
    bool checkForDraw() override;

    std::string initialStateString() override;
    std::string stateString() override;

    void setStateString(const std::string &s) override;

    Grid* getGrid() override { return _grid; }

    bool gameHasAI() override { return true; }

    void generateMoves(std::vector<BitMove>& outMoves);

    void updateAI() override;



private:
    Bit* PieceForPlayer(const int playerNumber, ChessPiece piece);
    Player* ownerAt(int x, int y) const;
    void FENtoBoard(const std::string& fen);
    char pieceNotation(int x, int y) const;

    bool canPawnMove(const Bit& bit, const ChessSquare& from, const ChessSquare& to) const;
    bool canBishopMove(const Bit& bit, const ChessSquare& from, const ChessSquare& to) const;
    bool canKnightMove(const Bit& bit, const ChessSquare& from, const ChessSquare& to) const;
    bool canKingMove(const Bit& bit, const ChessSquare& from, const ChessSquare& to) const;
    bool canRookMove  (const Bit& bit, const ChessSquare& from, const ChessSquare& to) const;
    bool canQueenMove (const Bit& bit, const ChessSquare& from, const ChessSquare& to) const;

    int evaluateBoard(const std::string& state);

    std::vector<BitMove> generateAllMoves(const std::string& state, int playerColor);
    int negamax(std::string& state, int depth, int alpha, int beta, int playerColor);

    int _countMoves = 0;
    Grid* _grid;

    static constexpr int WHITE = 1;
    static constexpr int BLACK = -1;
    static constexpr int negInfinite = -10000000;
    static constexpr int posInfinite =  10000000;

    static std::map<char, int> evaluateScores;

    std::vector<BitMove> _debugMoves;
};
