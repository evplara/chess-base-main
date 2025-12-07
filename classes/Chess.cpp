#include "Chess.h"
#include "ChessSquare.h"  
#include <limits>
#include <cmath>
#include <cstdint>
#include <vector>

std::map<char, int> Chess::evaluateScores = {
    // white pieces
    {'P', 100}, {'N', 200}, {'B', 230},
    {'R', 400}, {'Q', 900}, {'K', 2000},
    //black pieces
    {'p', -100}, {'n', -200}, {'b', -230},
    {'r', -400}, {'q', -900}, {'k', -2000},
    {'0', 0}
};

std::vector<BitMove> Chess::generateAllMoves(const std::string& state, int playerColor)
{
    std::vector<BitMove> moves;
    moves.reserve(64);

    auto isWhitePiece = [](char c) { return (c >= 'A' && c <= 'Z'); };
    auto isBlackPiece = [](char c) { return (c >= 'a' && c <= 'z'); };

    auto isMyPiece = [&](char c) {
        if (c == '0') return false;
        if (playerColor == WHITE) return isWhitePiece(c);
        return isBlackPiece(c);
    };

    auto isEnemyPiece = [&](char c) {
        if (c == '0') return false;
        if (playerColor == WHITE) return isBlackPiece(c);
        return isWhitePiece(c);
    };

    for (int idx = 0; idx < 64; ++idx)
    {
        char c = state[idx];
        if (!isMyPiece(c)) continue;

        int row = idx / 8;
        int col = idx % 8;
        char lc = (char)std::tolower((unsigned char)c);

        auto addMove = [&](int toIdx) {
            BitMove m;
            m.from = idx;
            m.to   = toIdx;
            moves.push_back(m);
        };

        if (lc == 'p')
        {
            int dir      = (playerColor == WHITE ? 1 : -1);
            int startRow = (playerColor == WHITE ? 1 : 6);

            int oneStepRow = row + dir;
            if (oneStepRow >= 0 && oneStepRow < 8)
            {
                int oneStepIdx = oneStepRow * 8 + col;
                if (state[oneStepIdx] == '0') {
                    addMove(oneStepIdx);

                    // double push from starting rank
                    if (row == startRow) {
                        int twoStepRow = row + 2 * dir;
                        if (twoStepRow >= 0 && twoStepRow < 8) {
                            int twoStepIdx = twoStepRow * 8 + col;
                            if (state[twoStepIdx] == '0') {
                                addMove(twoStepIdx);
                            }
                        }
                    }
                }
            }

            int capRow = row + dir;
            if (capRow >= 0 && capRow < 8)
            {
                int capCols[2] = { col - 1, col + 1 };
                for (int k = 0; k < 2; ++k) {
                    int cc = capCols[k];
                    if (cc < 0 || cc > 7) continue;
                    int capIdx = capRow * 8 + cc;
                    if (isEnemyPiece(state[capIdx])) {
                        addMove(capIdx);
                    }
                }
            }
        }

        else if (lc == 'n')
        {
            static const int offsets[8][2] = {
                {  1,  2 }, {  2,  1 }, {  2, -1 }, {  1, -2 },
                { -1, -2 }, { -2, -1 }, { -2,  1 }, { -1,  2 }
            };

            for (auto& off : offsets) {
                int nx = col + off[0];
                int ny = row + off[1];
                if (nx < 0 || nx > 7 || ny < 0 || ny > 7) continue;

                int toIdx = ny * 8 + nx;
                char dest = state[toIdx];

                if (!isMyPiece(dest)) { // empty or enemy
                    addMove(toIdx);
                }
            }
        }

        //King
        else if (lc == 'k')
        {
            static const int offsets[8][2] = {
                {  1,  0 }, {  1,  1 }, {  0,  1 }, { -1,  1 },
                { -1,  0 }, { -1, -1 }, {  0, -1 }, {  1, -1 }
            };

            for (auto& off : offsets) {
                int nx = col + off[0];
                int ny = row + off[1];
                if (nx < 0 || nx > 7 || ny < 0 || ny > 7) continue;

                int toIdx = ny * 8 + nx;
                char dest = state[toIdx];

                if (!isMyPiece(dest)) {
                    addMove(toIdx);
                }
            }
        }

        // Bishop
        else if (lc == 'b')
        {
            static const int dirs[4][2] = {
                {  1,  1 }, {  1, -1 }, { -1,  1 }, { -1, -1 }
            };

            for (auto& d : dirs) {
                int nx = col;
                int ny = row;
                while (true) {
                    nx += d[0];
                    ny += d[1];
                    if (nx < 0 || nx > 7 || ny < 0 || ny > 7) break;

                    int toIdx = ny * 8 + nx;
                    char dest = state[toIdx];

                    if (dest == '0') {
                        addMove(toIdx);
                    } else {
                        if (isEnemyPiece(dest)) {
                            addMove(toIdx);
                        }
                        break; // blocked
                    }
                }
            }
        }

        // Rook
        else if (lc == 'r')
        {
            static const int dirs[4][2] = {
                {  1,  0 }, { -1,  0 }, {  0,  1 }, {  0, -1 }
            };

            for (auto& d : dirs) {
                int nx = col;
                int ny = row;
                while (true) {
                    nx += d[0];
                    ny += d[1];
                    if (nx < 0 || nx > 7 || ny < 0 || ny > 7) break;

                    int toIdx = ny * 8 + nx;
                    char dest = state[toIdx];

                    if (dest == '0') {
                        addMove(toIdx);
                    } else {
                        if (isEnemyPiece(dest)) {
                            addMove(toIdx);
                        }
                        break;
                    }
                }
            }
        }

        // queen
        else if (lc == 'q')
        {
            static const int dirs[8][2] = {
                {  1,  0 }, { -1,  0 }, {  0,  1 }, {  0, -1 },
                {  1,  1 }, {  1, -1 }, { -1,  1 }, { -1, -1 }
            };

            for (auto& d : dirs) {
                int nx = col;
                int ny = row;
                while (true) {
                    nx += d[0];
                    ny += d[1];
                    if (nx < 0 || nx > 7 || ny < 0 || ny > 7) break;

                    int toIdx = ny * 8 + nx;
                    char dest = state[toIdx];

                    if (dest == '0') {
                        addMove(toIdx);
                    } else {
                        if (isEnemyPiece(dest)) {
                            addMove(toIdx);
                        }
                        break;
                    }
                }
            }
        }
    }

    return moves;
}


Chess::Chess()
{
    _grid = new Grid(8, 8);
}

Chess::~Chess()
{
    delete _grid;
}

int Chess::negamax(std::string& state, int depth, int alpha, int beta, int playerColor)
{
    _countMoves++;

    // Leaf node evaluate board from current player's perspective
    if (depth == 0) {
        return evaluateBoard(state) * playerColor;
    }

    auto newMoves = generateLegalMoves(state, playerColor);

    if (newMoves.empty()) {
        if (isKingInCheck(state, playerColor)) {
            // checkmated – large negative score
            return -100000 * playerColor; 
        } else {
            // stalemate – neutral
            return 0;
        }
    }

    int bestVal = negInfinite;

    for (auto move : newMoves)
    {
        char boardSave   = state[move.to];
        char pieceMoving = state[move.from];

        // Make move
        state[move.to]   = pieceMoving;
        state[move.from] = '0';

        // Recurse with swapped color, negated score and swapped alpha/beta
        int val = -negamax(state,
                           depth - 1,
                           -beta,
                           -alpha,
                           -playerColor);

        // Undo move
        state[move.from] = pieceMoving;
        state[move.to]   = boardSave;

        if (val > bestVal) {
            bestVal = val;
        }

        if (bestVal > alpha) {
            alpha = bestVal;
        }

        if (alpha >= beta) {
            break;  // beta cutoff
        }
    }

    return bestVal;
}

void Chess::updateAI()
{
    // Determine which color the AI is playing based on current player number
    int currentPlayerNum = getCurrentPlayer()->playerNumber();
    int aiColor = (currentPlayerNum == 0 ? WHITE : BLACK); // 0 = white, 1 = black

    std::string state = stateString();
    _countMoves = 0;

    int bestVal = negInfinite;
    BitMove bestMove;

    auto rootMoves = generateLegalMoves(state, aiColor);

    int searchDepth = 3;

    for (auto move : rootMoves)
    {
        char boardSave   = state[move.to];
        char pieceMoving = state[move.from];

        // Make move on state copy
        state[move.to]   = pieceMoving;
        state[move.from] = '0';

        // Evaluate this move using negamax
        int moveVal = -negamax(state, searchDepth - 1,
                               negInfinite, posInfinite,
                               -aiColor);

        // Undo move
        state[move.from] = pieceMoving;
        state[move.to]   = boardSave;

        if (moveVal > bestVal) {
            bestVal  = moveVal;
            bestMove = move;
        }
    }

    if (bestVal == negInfinite || rootMoves.empty()) {
        //checkmate or stalemate do nothing
        return;
    }


    int srcSquare = bestMove.from;
    int dstSquare = bestMove.to;

    BitHolder& src = getHolderAt(srcSquare & 7, srcSquare / 8);
    BitHolder& dst = getHolderAt(dstSquare & 7, dstSquare / 8);

    Bit* bit = src.bit();
    if (!bit) return; // safety

    dst.dropBitAtPoint(bit, ImVec2(0, 0)); 
    src.setBit(nullptr);

    bitMovedFromTo(*bit, src, dst);

    
}


bool Chess::canPawnMove(const Bit& bit, const ChessSquare& from, const ChessSquare& to) const
{
    const bool isWhite = bit.gameTag() < 128;
    const int dir      = isWhite ? 1 : -1;   
    const int startRow = isWhite ? 1 : 6;  

    const int fromCol = from.getColumn();
    const int fromRow = from.getRow();
    const int toCol   = to.getColumn();
    const int toRow   = to.getRow();

    const int dx = toCol - fromCol;
    const int dy = toRow - fromRow;

    Bit* targetBit = to.bit(); 

    if (dx == 0)
    {
        if (targetBit != nullptr) {
            return false;
        }

        if (dy == dir) {
            return true;
        }

        if (fromRow == startRow && dy == 2 * dir)
        {
            int midRow = fromRow + dir;
            ChessSquare* midSq = _grid->getSquare(fromCol, midRow);
            if (midSq && midSq->bit() == nullptr) {
                return true;
            }
        }

        return false;
    }

    if (dy == dir && std::abs(dx) == 1)
    {
        if (targetBit == nullptr) {
            return false;
        }
        
        return true;
    }

    return false;
}

bool Chess::canBishopMove(const Bit& bit, const ChessSquare& from, const ChessSquare& to) const
{
    const int fromCol = from.getColumn();
    const int fromRow = from.getRow();
    const int toCol   = to.getColumn();
    const int toRow   = to.getRow();

    const int dx = toCol - fromCol;
    const int dy = toRow - fromRow;

    // Must move diagonally: |dx| == |dy|, and not staying in place
    if (dx == 0 || std::abs(dx) != std::abs(dy)) {
        return false;
    }

    // Cannot capture own piece
    if (Bit* destBit = to.bit()) {
        if (destBit->getOwner() == bit.getOwner()) {
            return false;
        }
    }

    int stepX = (dx > 0) ? 1 : -1;
    int stepY = (dy > 0) ? 1 : -1;

    int x = fromCol + stepX;
    int y = fromRow + stepY;

    // Squares between from and to must be empty
    while (x != toCol || y != toRow) {
        ChessSquare* sq = _grid->getSquare(x, y);
        if (sq && sq->bit() != nullptr) {
            return false; // blocked
        }
        x += stepX;
        y += stepY;
    }

    return true;
}

bool Chess::canRookMove(const Bit& bit, const ChessSquare& from, const ChessSquare& to) const
{
    const int fromCol = from.getColumn();
    const int fromRow = from.getRow();
    const int toCol   = to.getColumn();
    const int toRow   = to.getRow();

    const int dx = toCol - fromCol;
    const int dy = toRow - fromRow;

    if (dx != 0 && dy != 0) {
        return false;
    }
    if (dx == 0 && dy == 0) {
        return false; // no move
    }

    if (Bit* destBit = to.bit()) {
        if (destBit->getOwner() == bit.getOwner()) {
            return false;
        }
    }

    int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
    int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);

    int x = fromCol + stepX;
    int y = fromRow + stepY;

    // Path must be empty
    while (x != toCol || y != toRow) {
        ChessSquare* sq = _grid->getSquare(x, y);
        if (sq && sq->bit() != nullptr) {
            return false; 
        }
        x += stepX;
        y += stepY;
    }

    return true;
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

bool Chess::canQueenMove(const Bit& bit, const ChessSquare& from, const ChessSquare& to) const
{
    if (canBishopMove(bit, from, to)) return true;
    if (canRookMove(bit, from, to))   return true;
    return false;
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

bool Chess::isKingInCheck(const std::string& state, int color) const
{
    // Find our king
    char kingChar = (color == WHITE ? 'K' : 'k');
    int kingIndex = -1;

    for (int i = 0; i < 64; ++i) {
        if (state[i] == kingChar) {
            kingIndex = i;
            break;
        }
    }

    if (kingIndex == -1) {
        return true;
    }

    int enemyColor = -color;

    // Generate all enemy moves and see if any hit king square
    auto enemyMoves = const_cast<Chess*>(this)->generateAllMoves(state, enemyColor);

    for (const auto& mv : enemyMoves) {
        if (mv.to == kingIndex) {
            return true;
        }
    }

    return false;
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

    if (gameHasAI()) {
        setAIPlayer(1);  
    }

    _debugMoves.clear();
    generateMoves(_debugMoves);
    
    startGame();
}

int Chess::evaluateBoard(const std::string& state)
{
    int value = 0;
    for (char ch : state) {
        auto it = evaluateScores.find(ch);
        if (it != evaluateScores.end()) {
            value += it->second;
        }
    }
    return value;
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


void Chess::generateMoves(std::vector<BitMove>& outMoves)
{
    outMoves.clear();

    int currentColorTag = getCurrentPlayer()->playerNumber() * 128;

    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            ChessSquare* fromSq = _grid->getSquare(x, y);
            Bit* pieceBit = fromSq->bit();
            if (!pieceBit) continue;

            // Skip enemy pieces
            if ((pieceBit->gameTag() & 128) != currentColorTag)
                continue;

            int tagValue = pieceBit->gameTag() & 0x7F;
            ChessPiece pieceType = static_cast<ChessPiece>(tagValue);

            if (pieceType != Pawn && pieceType != Knight && pieceType != King)
                continue;

            int fromIndex = y * 8 + x;

            for (int ty = 0; ty < 8; ++ty)
            {
                for (int tx = 0; tx < 8; ++tx)
                {
                    if (tx == x && ty == y) continue; 

                    ChessSquare* toSq = _grid->getSquare(tx, ty);

                    if (!toSq->canDropBitAtPoint(pieceBit, pieceBit->getPosition()))
                        continue;

                    if (!canBitMoveFromTo(*pieceBit, *fromSq, *toSq))
                        continue;

                    int toIndex = ty * 8 + tx;

                    outMoves.emplace_back(fromIndex, toIndex, pieceType);
                }
            }
        }
    }
}

std::vector<BitMove> Chess::generateLegalMoves(const std::string& state, int color) const
{
    std::vector<BitMove> legal;
    auto pseudo = const_cast<Chess*>(this)->generateAllMoves(state, color);

    for (auto mv : pseudo) {
        std::string next = state;

        char moving = next[mv.from];
        char captured = next[mv.to];

        // make the move
        next[mv.to]   = moving;
        next[mv.from] = '0';

        // if our king is not in check after this move, it's legal
        if (!isKingInCheck(next, color)) {
            legal.push_back(mv);
        }
    }

    return legal;
}


bool Chess::actionForEmptyHolder(BitHolder &holder)
{
    return false;
}

bool Chess::canBitMoveFrom(Bit &bit, BitHolder &src)
{
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

    auto* fromSquare = dynamic_cast<ChessSquare*>(&src);
    auto* toSquare   = dynamic_cast<ChessSquare*>(&dst);

    if (!fromSquare || !toSquare) {
        return false;
    }

    const int tagValue      = bit.gameTag() & 0x7F;
    ChessPiece pieceType    = static_cast<ChessPiece>(tagValue);

    bool basicLegal = false;
    switch (pieceType)
    {
        case Pawn:
            basicLegal = canPawnMove(bit, *fromSquare, *toSquare);
            break;

        case Knight:
            basicLegal = canKnightMove(bit, *fromSquare, *toSquare);
            break;

        case Bishop:
            basicLegal = canBishopMove(bit, *fromSquare, *toSquare);
            break;

        case Rook:
            basicLegal = canRookMove(bit, *fromSquare, *toSquare);
            break;

        case Queen:
            basicLegal = canQueenMove(bit, *fromSquare, *toSquare);
            break;

        case King:
            basicLegal = canKingMove(bit, *fromSquare, *toSquare);
            break;

        default:
            basicLegal = false;
            break;
    }

    if (!basicLegal) {
        return false;
    }

    std::string state = stateString();

    int fromIdx = fromSquare->getSquareIndex();
    int toIdx   = toSquare->getSquareIndex();

    char moving   = state[fromIdx];
    char captured = state[toIdx];

    state[toIdx]   = moving;
    state[fromIdx] = '0';

    // Determine color of moving side
    Player* owner = bit.getOwner();
    int color     = (owner->playerNumber() == 0 ? WHITE : BLACK);

    // If this move leaves king in check, it's illegal
    if (isKingInCheck(state, color)) {
        return false;
    }

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
    std::string state = stateString();
    int currentPlayerNum = getCurrentPlayer()->playerNumber();
    int toMoveColor = (currentPlayerNum == 0 ? WHITE : BLACK);

    auto legal = generateLegalMoves(state, toMoveColor);

    if (!legal.empty()) {
        return nullptr; // game continues
    }

    if (isKingInCheck(state, toMoveColor)) {
        // checkmate: the side to move is mated, so the other player wins
        int winnerIndex = (currentPlayerNum == 0 ? 1 : 0);
        return getPlayerAt(winnerIndex); // adapt to your API
    }

    return nullptr;
}

bool Chess::checkForDraw()
{
    std::string state = stateString();
    int currentPlayerNum = getCurrentPlayer()->playerNumber();
    int toMoveColor = (currentPlayerNum == 0 ? WHITE : BLACK);

    auto legal = generateLegalMoves(state, toMoveColor);

    if (!legal.empty()) return false;
    if (isKingInCheck(state, toMoveColor)) return false; // that's checkmate, not draw

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
