#ifndef SRC_GAME_BOARD_HH_
#define SRC_GAME_BOARD_HH_

#include <array>

#include "board.hh"
#include "hash.h"
#include "pieces.hh"

/**
 * Represents a game state for one player
 */
class PlayerBoard {
 public:
  PlayerBoard();
  PlayerBoard(const PlayerBoard&);

  int px() const { return px_; }
  int py() const { return py_; }
  int pd() const { return pd_; }
  const Piece* current_piece() const { return current_piece_; }
  void set_current_piece(const Piece* piece) { current_piece_ = piece; }

  /**
   * @brief gets the next piece, and updates internal queue
   *
   * @return `const Piece*` of the next piece
   */
  const Piece* QueuePop();

  /**
   * Resets the position of the current piece.
   */
  void ResetPosition();

  void Harddrop();
  void Softdrop();
  void RotateCW();
  void RotateCCW();
  void Rotate180();
  void MoveLeft();
  void MoveRight();

  /**
   * @brief Checks if you can put the piece there without intersecting existing
   * blocks.
   *
   * @param x horizontal offset
   * @param y vertical offset
   * @return `true` if `current_piece` can be placed with offsets `x`, `y`.
   */
  bool IsValidPosition(int x, int y, int d) const;

  friend std::ostream& operator<<(std::ostream& out, const PlayerBoard& rhs);

 private:
  Board board_;

  // used to allow overriding the current piece in testing
  const Piece* current_piece_;
  // piece offsets
  int px_, py_;
  // piece orientation direction
  int pd_;

  // circular queue indexed by `piece_queue_index`
  const Piece* piece_queue_[14];
  const Piece** queue_curr_;  // current set of 7
  const Piece** queue_next_;  // next set of 7
  // indexes into piece_queue
  uint8_t piece_queue_index_;
  // rng state
  uint64_t queue_rng_, garbage_rng_;

  /**
   * Determines the next 7 unknown pieces of the queue.
   */
  void RefreshQueue();
};

#endif  // SRC_GAME_BOARD_HH_