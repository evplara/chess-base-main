#include "Chess.h"
#include "ChessSquare.h"  
#include <limits>
#include <cmath>
#include <cstdint>


Chess::Chess()
{
    _grid = new Grid(8, 8);
}

Chess::~Chess()
{
    delete _grid;
}

bool Chess::canPawnMove(const Bit& bit, const ChessSquare& from, const ChessSquare& to) const
{
    const bool isWhite = bit.gameTag() < 128;
    const int dir      = isWhite ? 1 : -1;   // white moves "up" (increasing row), black "down"
    const int startRow = isWhite ? 1 : 6;    // initial ranks for pawns in your FEN setup

    const int fromCol = from.getColumn();
    const int fromRow = from.getRow();
    const int toCol   = to.getColumn();
    const int toRow   = to.getRow();

    const int dx = toCol - fromCol;
    const int dy = toRow - fromRow;

    Bit* targetBit = to.bit(); // piece currently on the destination, if any

    // 1) Forward moves (no capture)
    if (dx == 0)
    {
        // Cannot move straight into an occupied square
        if (targetBit != nullptr) {
            return false;
        }

        // Single step
        if (dy == dir) {
            return true;
        }

        // Double step from starting rank must be clear all the way
        if (fromRow == startRow && dy == 2 * dir)
        {
            int midRow = fromRow + dir;
            ChessSquare* midSq = _grid->getSquare(fromCol, midRow);
            if (midSq && midSq->bit() == nullptr) {
                return true;
            }
        }

        // Any other vertical distance is illegal
        return false;
    }

    // 2)captues one square diagonally forward onto an enemy piece
    if (dy == dir && std::abs(dx) == 1)
    {
        // must be capturing something; no "moving" diagonally into empty space
        if (targetBit == nullptr) {
            return false;
        }
        
        return true;
    }

    // Everything else is illegal for pawns 
    return false;
}


bool Chess::canKnightMove(const Bit& bit, const ChessSquare& from, const ChessSquare& to) const
{
    (void)bit; 

    const int fromCol = from.getColumn();
    const int fromRow = from.getRow();

    // All relative knight moves (dx, dy)
    static const int offsets[8][2] = {
        {  1,  2 }, {  2,  1 }, {  2, -1 }, {  1, -2 },
        { -1, -2 }, { -2, -1 }, { -2,  1 }, { -1,  2 }
    };

    uint64_t mask = 0;

    for (auto& off : offsets) {
        int nx = fromCol + off[0];
        int ny = fromRow + off[1];
        if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
            int idx = ny * 8 + nx;
            mask |= (1ULL << idx);
        }
    }

    BitboardElement moves(mask);

    const int targetIndex = to.getSquareIndex();
    bool reachable = false;

    moves.forEachBit([&](int idx) {
        if (idx == targetIndex) {
            reachable = true;
        }
    });

    return reachable;
}

bool Chess::canKingMove(const Bit& bit, const ChessSquare& from, const ChessSquare& to) const
{
    (void)bit; 

    const int fromCol = from.getColumn();
    const int fromRow = from.getRow();

    // All eight adjacent squares (dx, dy)
    static const int offsets[8][2] = {
        {  1,  0 }, {  1,  1 }, {  0,  1 }, { -1,  1 },
        { -1,  0 }, { -1, -1 }, {  0, -1 }, {  1, -1 }
    };

    uint64_t mask = 0;

    for (auto& off : offsets) {
        int nx = fromCol + off[0];
        int ny = fromRow + off[1];
        if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
            int idx = ny * 8 + nx;
            mask |= (1ULL << idx);
        }
    }

    BitboardElement moves(mask);

    const int targetIndex = to.getSquareIndex();
    bool reachable = false;

    moves.forEachBit([&](int idx) {
        if (idx == targetIndex) {
            reachable = true;
        }
    });

    return reachable;
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
    //accept board only or full FEN 
    std::string boardField = fen;
    if (auto sp = boardField.find(' '); sp != std::string::npos) {
        boardField = boardField.substr(0, sp);
    }

    //clear existing pieces
    _grid->forEachSquare([&](ChessSquare* sq, int, int){
        sq->setBit(nullptr);
    });

    int rank = 7;      
    int file = 0;     

    auto drop = [&](int x, int y, char c) {
        const bool white = (c >= 'A' && c <= 'Z');
        char lc = (char)std::tolower((unsigned char)c);

        ChessPiece piece = NoPiece;
        switch (lc) {
            case 'p': piece = Pawn;   break;
            case 'n': piece = Knight; break;
            case 'b': piece = Bishop; break;
            case 'r': piece = Rook;   break;
            case 'q': piece = Queen;  break;
            case 'k': piece = King;   break;
            default: return; // ignore unknown
        }

        Bit* b = PieceForPlayer(white ? 0 : 1, piece);
        ChessSquare* sq = _grid->getSquare(x, y);
        sq->setBit(b);

        ImVec2 p = sq->getPosition();
        b->setCenterPosition(ImVec2(p.x + pieceSize/2, p.y + pieceSize/2));
    };

    for (char c : boardField) {
        if (c == '/') { rank--; file = 0; continue; }
        if (c >= '1' && c <= '8') { file += (c - '0'); continue; }
        if (file > 7 || rank < 0) continue;

        int y = rank; 
        int x = file;
        drop(x, y, c);
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
    // Enforce turn: only current player's color may move
    int currentPlayerColor = getCurrentPlayer()->playerNumber() * 128;
    int pieceColor         = bit.gameTag() & 128;
    if (pieceColor != currentPlayerColor) {
        return false;
    }

    // We expect these holders to actually be ChessSquares
    auto* fromSquare = dynamic_cast<ChessSquare*>(&src);
    auto* toSquare   = dynamic_cast<ChessSquare*>(&dst);

    if (!fromSquare || !toSquare) {
        return false;
    }

    // Decode the piece type from the gameTag: low 7 bits store the enum value
    const int tagValue      = bit.gameTag() & 0x7F;
    ChessPiece pieceType    = static_cast<ChessPiece>(tagValue);

    switch (pieceType)
    {
        case Pawn:
            return canPawnMove(bit, *fromSquare, *toSquare);

        case Knight:
            return canKnightMove(bit, *fromSquare, *toSquare);

        case King:
            return canKingMove(bit, *fromSquare, *toSquare);

        // For this assignment we don't implement bishops/rooks/queens yet
        default:
            return false;
    }
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
