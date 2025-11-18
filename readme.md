Image Here
![alt text](https://github.com/evplara/chess-base-main/blob/main/Screenshot%202025-11-10%20215921.png "ChessBoard")

Part 2 Implemntation: 
Pawns:
Move forward 1 square if empty.
Move 2 squares from starting rank if both squares are empty.
Capture diagonally forward by 1 square.
No en passant or promotion (not required).

Knights (Bitboard-based)
Can move in all 8 L-shaped positions.
Knights jump over pieces.
Movement mask generated using a bitboard and the provided iterator.

King (Bitboard-based)
Moves 1 square in any direction (8 possibilities).
Also generated with a bitboard mask.
Castling is not implemented yet.

Captures:
A piece can move onto a square with an enemy piece.
Same-color captures are blocked by canDropBitAtPoint.
When a capture happens, the enemy piece is removed from the board.

Turn Handling:
Each piece has a color encoded in its gameTag.
Only the current player's pieces can be moved.
The turn changes after a successful move.

Initial Board after adding pawn, knight, king movement
<img width="1569" height="714" alt="image" src="https://github.com/user-attachments/assets/6f40a236-8886-482c-9a5d-d13af42ea208" />

Debug values after running chess with only pawn, knight, king movement
<img width="1866" height="970" alt="image" src="https://github.com/user-attachments/assets/a48b8b8d-692e-43a0-b330-1cb02303310b" />
