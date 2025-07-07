# chess

Chess implementation, written in C

---

## to-do list

### Game Setup

- [x] Implement 8x8 board grid.
- [x] Place pieces in their starting positions.
- [x] Represent pieces (King, Queen, Bishop, Knight, Rook, Pawn).

### Piece Movement

- [x] Implement legal moves for all pieces (King, Queen, Bishop, Knight, Rook, Pawn).
- [x] Implement Pawn's two-square opening move.
- [x] Implement Pawn promotion when reaching the last rank.
- [x] Implement Castling.
- [x] Implement En Passant capture rule.

### Check & Checkmate

- [x] Implement "Check" state when a King is under threat.
- [x] Implement "Checkmate" detection when the King is under threat and no legal move can avoid it.
- [x] Implement "Stalemate" detection (draw scenario).
- [x] Implement King safety checks to prevent illegal moves that place your own King in check.

### Game Turn & Validity

- [x] Implement turn-based system (White goes first, alternating turns).
- [x] Ensure only legal moves are allowed for each piece.
- [x] Implement the undo move functionality

### Game End Conditions

- [x] Implement Checkmate (Game over when the opponent's King is checkmated).
- [x] Implement Stalemate (Draw if no valid moves and the King is not in check).
- [x] Implement draw conditions (insufficient material, 50-move rule, etc.).

### Basic User Interface

- [x] Display an 8x8 grid visually (graphical or text-based).
- [x] Allow players to select pieces and make moves.
- [x] Highlight valid moves for selected pieces.
- [ ] Indicate check/checkmate status.

### ⏱️ Time Control

- [ ] Implement a basic timer (e.g., each player gets a set amount of time).
- [ ] Add countdown clock for each player’s turn.

### 💾 Save & Resume (Optional)

- [ ] Implement saving game state and resuming from the saved state.

### AI

- [ ] Implement basic AI opponent (easy difficulty).
- [ ] Allow players to play against the AI.
