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
  PlayerBoard(uint64_t seed);
  PlayerBoard(const PlayerBoard&);

  /**
   * @brief loads the string onto the board, useful for testing
   */
  void LoadBoard(const std::string&);

  int px() const { return px_; }
  int py() const { return py_; }
  int pd() const { return pd_; }
  void set_px(int px) { px_ = px; }
  void set_py(int py) { py_ = py; }
  void set_pd(int pd) { pd_ = pd; }
  bool spin() const { return spin_; }
  int attack() const { return attack_; }
  const auto& board() const { return board_; }
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
   * Clears any lines that are filled, applies combo and spin bonus.
   *
   * @return the number of lines cleared
   */
  int ClearLines();

  /**
   * @brief Checks if you can put the piece there without intersecting existing
   * blocks.
   *
   * @param x horizontal offset
   * @param y vertical offset
   * @return `true` if `current_piece` can be placed with offsets `x`, `y`.
   */
  bool IsValidPosition(int x, int y, int d) const;

  std::vector<std::array<int, 3>> GeneratePlacements() const;

  friend std::ostream& operator<<(std::ostream& out, const PlayerBoard& rhs);

 private:
  Board board_;

  // used to allow overriding the current piece in testing
  const Piece* current_piece_;
  // piece offsets
  int px_, py_;
  // piece orientation direction
  int pd_;
  // whether spin bonus is active, determined at harddrop time, before line
  // updates (double attack)
  bool spin_;
  int b2b_ = 0;     // length of back to back
  int combo_ = 0;   // length of ongoing combo
  int attack_ = 0;  // pending attack

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