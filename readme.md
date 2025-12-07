Image Here
![alt text](https://github.com/evplara/chess-base-main/blob/main/Screenshot%202025-11-10%20215921.png "ChessBoard")

Part 2 Implemntation: 
## Pawns:
Move forward 1 square if empty.
Move 2 squares from starting rank if both squares are empty.
Capture diagonally forward by 1 square.
No en passant or promotion (not required).

## Knights (Bitboard-based)
Can move in all 8 L-shaped positions.
Knights jump over pieces.
Movement mask generated using a bitboard and the provided iterator.

## King (Bitboard-based)
Moves 1 square in any direction (8 possibilities).
Also generated with a bitboard mask.
Castling is not implemented yet.

## Captures:
A piece can move onto a square with an enemy piece.
Same-color captures are blocked by canDropBitAtPoint.
When a capture happens, the enemy piece is removed from the board.

## Turn Handling:
Each piece has a color encoded in its gameTag.
Only the current player's pieces can be moved.
The turn changes after a successful move.

## Challenges:
### Legal vs pseudo-legal moves (king safety, check, checkmate)

Originally, my engine only generated pseudo-legal moves they matched the geometry of each piece but there was no concept of:

Check, Illegal king exposure, or checkmate/stalemate.

This caused two problems:

The AI could leave its own king in check and still think the move was valid.

The human player could make moves that didn’t respond to check and even move pieces while their king stayed in check.

So I added isKingInCheck(state, color), which finds the king for a given color and asks “can any enemy pseudo-legal move capture this square?” and generateLegalMoves(state, color), which calls generateAllMoves, then filters out any move that leaves that side’s king in check by simulating it on a copy of the state.

Then I changed
negamax and updateAI to use generateLegalMoves instead of generateAllMoves.
canBitMoveFromTo to first check the piece’s basic movement.
Then simulate the move on the stateString() and call isKingInCheck to reject any move that would leave the player’s own king in check.
Finally, I updated
checkForWinner() to detect checkmate (no legal moves and king in check).
checkForDraw() to detect stalemate (no legal moves and king not in check).

Another challenge was making the AI code work cleanly inside an existing engine that already supported multiple games. I had to
Make Chess::gameHasAI() return true so the framework knew Chess has an AI mode.

Make updateAI() color-agnostic by inferring the side to move from getCurrentPlayer()->playerNumber() and translating that to WHITE or BLACK.

## Depth Achieved and Performance
The code is written so I can easily increase this to 4 or 5 plies in Release builds if I want a stronger AI later.

Because I generate legal moves, not just pseudo-legal ones, and use alpha–beta, the search is still fast enough to be playable while giving reasonable chess strength for a project of this size.

I also track _countMoves inside negamax, so I can print how many positions were evaluated per move for debugging and performance tuning.

### How Well the AI Plays

Given its design, the AI plays roughly at a basic beginner to casual intermediate level:

The evaluation function is purely material-based:

Pawns: 100

Knights: 200

Bishops: 230

Rooks: 400

Queens: 900

Kings: 2000 (just a big number so losing the king is very bad)

White pieces have positive values, black pieces have negative values; so the evaluation returns a material advantage from White’s perspective.

Because of this
The AI will happily capture free pieces and avoid blatant blunders like hanging its queen.
It tends to simplify when winning, since trading down material preserves a lead.
It sometimes makes non-positional moves (for example, it doesn’t “know” about king safety beyond basic checks, pawn structure, or piece activity) because there are no piece-square tables or deeper positional heuristics yet.
At depth 3, it can see simple tactics (like a one-move capture or short se

Initial Board after adding pawn, knight, king movement
<img width="1569" height="714" alt="image" src="https://github.com/user-attachments/assets/6f40a236-8886-482c-9a5d-d13af42ea208" />

Debug values after running chess with only pawn, knight, king movement
<img width="1866" height="970" alt="image" src="https://github.com/user-attachments/assets/a48b8b8d-692e-43a0-b330-1cb02303310b" />
