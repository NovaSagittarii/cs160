#include "player_board.hh"

#include <vector>

PlayerBoard::PlayerBoard() {
  // populate the first 7
  piece_queue_index_ = 0;
  piece_queue_[0] = &pieces::I;
  piece_queue_[1] = &pieces::J;
  piece_queue_[2] = &pieces::L;
  piece_queue_[3] = &pieces::O;
  piece_queue_[4] = &pieces::S;
  piece_queue_[5] = &pieces::T;
  piece_queue_[6] = &pieces::Z;
  queue_curr_ = piece_queue_;
  queue_next_ = piece_queue_ + 7;
  // Note: next set will be generated on pop

  // initialize `current_piece_`
  current_piece_ = piece_queue_[0];
  ResetPosition();
}

PlayerBoard::PlayerBoard(const PlayerBoard& other) {
  board_ = other.board_;

  current_piece_ = other.current_piece_;
  px_ = other.px_;
  py_ = other.py_;
  pd_ = other.pd_;
  spin_ = other.spin_;

  for (int i = 0; i < 14; ++i) piece_queue_[i] = other.piece_queue_[i];
  piece_queue_index_ = other.piece_queue_index_;
  queue_curr_ = other.queue_curr_;
  queue_next_ = other.queue_next_;

  garbage_rng_ = other.garbage_rng_;
  queue_rng_ = other.queue_rng_;
}

void PlayerBoard::LoadBoard(const std::string& s) {
  // std::istringstream in(s);
  // std::vector<std::string> g;
  // std::string line;
  // while (in >> line) g.push_back(line);
  board_.Clear();
  Piece p("", s, "#", "#", "#");
  p.Place(board_, 0, p.height(), 0);
}

const Piece* PlayerBoard::QueuePop() {
  if (piece_queue_index_ == 0 || piece_queue_index_ == 7) RefreshQueue();
  if (piece_queue_index_ == 14) piece_queue_index_ = 0;
  return piece_queue_[piece_queue_index_++];
}

void PlayerBoard::RefreshQueue() {
  for (size_t i = 0; i < 7; ++i) {
    queue_next_[i] = queue_curr_[i];
  }
  // knuth shuffle using queue_rng
  for (size_t i = 7 - 1; i >= 1; --i) {
    size_t j = queue_rng_ % i;
    std::swap(queue_next_[i], queue_next_[j]);
    queue_rng_ = hash(queue_rng_);
  }
  std::swap(queue_next_, queue_curr_);
}

void PlayerBoard::ResetPosition() {
  px_ = (Board::width - current_piece_->width() - 2) / 2;
  py_ = 20;
  pd_ = 0;
}

void PlayerBoard::Softdrop() {
  while (py_ - 1 >= -10 && IsValidPosition(px_, py_ - 1, pd_)) --py_;
}

void PlayerBoard::Harddrop() {
  Softdrop();
  // std::cout << board_ << "\n";
  int mobility = 0;
  if (IsValidPosition(px_ - 1, py_, pd_)) mobility |= 1;
  if (IsValidPosition(px_ + 1, py_, pd_)) mobility |= 2;
  if (IsValidPosition(px_, py_ + 1, pd_)) mobility |= 3;

  spin_ = mobility == 0;
  current_piece_->Place(board_, px_, py_, pd_);
  // std::cout << board_ << "\n";
  current_piece_ = QueuePop();
  ResetPosition();
}

void PlayerBoard::RotateCW() {
  /**
   * 0->1	( 0, 0)	(-1, 0)	(-1,+1)	( 0,-2)	(-1,-2)
   * 1->2	( 0, 0)	(+1, 0)	(+1,-1)	( 0,+2)	(+1,+2)
   * 2->3	( 0, 0)	(+1, 0)	(+1,+1)	( 0,-2)	(+1,-2)
   * 3->0	( 0, 0)	(-1, 0)	(-1,-1)	( 0,+2)	(-1,+2)
   */
  const static std::vector<std::array<int, 2>> offsets = {
      {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2},
  };
  for (auto [dx, dy] : offsets) {
    // std::cout << "attempt dxy=" << dx << "," << dy << "\n";
    if (pd_ == 1 || pd_ == 2) dx *= -1;
    if (pd_ == 1 || pd_ == 3) dy *= -1;
    if (IsValidPosition(px_ + dx, py_ + dy, (pd_ + 1) % 4)) {
      px_ += dx;
      py_ += dy;
      pd_ = (pd_ + 1) % 4;
      break;
    }
  }
}
void PlayerBoard::RotateCCW() {
  /*
   * 0->3	( 0, 0)	(+1, 0)	(+1,+1)	( 0,-2)	(+1,-2)
   * 1->0	( 0, 0)	(+1, 0)	(+1,-1)	( 0,+2)	(+1,+2)
   * 2->1	( 0, 0)	(-1, 0)	(-1,+1)	( 0,-2)	(-1,-2)
   * 3->2	( 0, 0)	(-1, 0)	(-1,-1)	( 0,+2)	(-1,+2)
   */
  const static std::vector<std::array<int, 2>> offsets = {
      {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2},
  };
  for (auto [dx, dy] : offsets) {
    // std::cout << "attempt dxy=" << dx << "," << dy << "\n";
    if (pd_ == 1 || pd_ == 2) dx *= -1;
    if (pd_ == 1 || pd_ == 3) dy *= -1;
    if (IsValidPosition(px_ + dx, py_ + dy, (pd_ - 1 + 4) % 4)) {
      px_ += dx;
      py_ += dy;
      pd_ = (pd_ - 1 + 4) % 4;
      break;
    }
  }
}
void PlayerBoard::Rotate180() {
  /// TODO: 180 kick table
  if (IsValidPosition(px_, py_, pd_ ^ 2)) {
    pd_ ^= 2;
  }
}
void PlayerBoard::MoveLeft() {
  if (IsValidPosition(px_ - 1, py_, pd_)) {
    px_ -= 1;
  }
}
void PlayerBoard::MoveRight() {
  if (IsValidPosition(px_ + 1, py_, pd_)) {
    px_ += 1;
  }
}

bool PlayerBoard::IsValidPosition(int x, int y, int d) const {
  bool ok = !current_piece_->Intersects(board_, x, y, d);
  // std::cout << "testing " << x << " " << y << " " << d << " ";
  // std::cout << (ok ? "OK" : "FAIL");
  // std::cout << "\n";
  return ok;
}

std::ostream& operator<<(std::ostream& out, const PlayerBoard& rhs) {
  out << "curr: " << rhs.current_piece_->name() << "\n";
  out << rhs.board_;
  return out;
}
