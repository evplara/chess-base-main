#include "Chess.h"
#include <limits>
#include <cmath>

Chess::Chess()
{
    _grid = new Grid(8, 8);
}

Chess::~Chess()
{
    delete _grid;
}

char Chess::pieceNotation(int x, int y) const
{
    const char *wpieces = { "0PNBRQK" };
    const char *bpieces = { "0pnbrqk" };
    Bit *bit = _grid->getSquare(x, y)->bit();
    char notation = '0';
    if (bit) {
        notation = bit->gameTag() < 128 ? wpieces[bit->gameTag()] : bpieces[bit->gameTag()-128];
    }
    return notation;
}

Bit* Chess::PieceForPlayer(const int playerNumber, ChessPiece piece)
{
    const char* pieces[] = { "pawn.png", "knight.png", "bishop.png", "rook.png", "queen.png", "king.png" };

    Bit* bit = new Bit();
    // should possibly be cached from player class?
    const char* pieceName = pieces[piece - 1];
    std::string spritePath = std::string("") + (playerNumber == 0 ? "w_" : "b_") + pieceName;
    bit->LoadTextureFromFile(spritePath.c_str());
    bit->setOwner(getPlayerAt(playerNumber));
    bit->setSize(pieceSize, pieceSize);

        bit->setGameTag((playerNumber == 0 ? 0 : 128) + static_cast<int>(piece));

    return bit;
}

void Chess::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = 8;
    _gameOptions.rowY = 8;

    _grid->initializeChessSquares(pieceSize, "boardsquare.png");
    FENtoBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

    startGame();
}

void Chess::FENtoBoard(const std::string& fen) {
    // 1) Take only field #1 (piece placement). If a full FEN is passed, we ignore the rest for now.
    std::string boardField = fen;
    if (auto sp = boardField.find(' '); sp != std::string::npos) {
        boardField = boardField.substr(0, sp);
    }

    // 2) Clear the board first
    _grid->forEachSquare([&](ChessSquare* square, int, int) {
        square->setBit(nullptr);
    });

    // 3) Parse ranks from 8 -> 1 (FEN order). Our grid uses y=0 at the top, so:
    //    rank 8 -> y=0, rank 7 -> y=1, ..., rank 1 -> y=7
    int rank = 7;   // 7..0
    int file = 0;   // 0..7

    auto pushPiece = [&](int x, int y, char c) {
        const bool white = (c >= 'A' && c <= 'Z');
        char lc = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

        ChessPiece piece = NoPiece;
        switch (lc) {
            case 'p': piece = Pawn;   break;
            case 'n': piece = Knight; break;
            case 'b': piece = Bishop; break;
            case 'r': piece = Rook;   break;
            case 'q': piece = Queen;  break;
            case 'k': piece = King;   break;
            default:  return; // ignore unknown
        }

        Bit* b = PieceForPlayer(white ? 0 : 1, piece);
        if (b) {
            // gameTag set in PieceForPlayer; just drop it on the board
            _grid->getSquare(x, y)->setBit(b);
        }
    };

    for (char c : boardField) {
        if (c == '/') {
            // next rank (downwards on screen)
            rank--;
            file = 0;
            continue;
        }
        if (c >= '1' && c <= '8') {
            file += (c - '0'); // skip that many empty files
            continue;
        }
        if (file < 0 || file > 7 || rank < 0 || rank > 7) {
            continue; // guard against malformed FEN
        }
        int y = 7 - rank;    // rank 7->y=0 ... rank 0->y=7
        int x = file;
        pushPiece(x, y, c);
        file++;
    }

    
}


bool Chess::actionForEmptyHolder(BitHolder &holder)
{
    return false;
}

bool Chess::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    // need to implement friendly/unfriendly in bit so for now this hack
    int currentPlayer = getCurrentPlayer()->playerNumber() * 128;
    int pieceColor = bit.gameTag() & 128;
    if (pieceColor == currentPlayer) return true;
    return false;
}

bool Chess::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    return true;
}

void Chess::stopGame()
{
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

Player* Chess::ownerAt(int x, int y) const
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8) {
        return nullptr;
    }

    auto square = _grid->getSquare(x, y);
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

Player* Chess::checkForWinner()
{
    return nullptr;
}

bool Chess::checkForDraw()
{
    return false;
}

std::string Chess::initialStateString()
{
    return stateString();
}

std::string Chess::stateString()
{
    std::string s;
    s.reserve(64);
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
            s += pieceNotation( x, y );
        }
    );
    return s;}

void Chess::setStateString(const std::string &s)
{
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y * 8 + x;
        char playerNumber = s[index] - '0';
        if (playerNumber) {
            square->setBit(PieceForPlayer(playerNumber - 1, Pawn));
        } else {
            square->setBit(nullptr);
        }
    });
}
