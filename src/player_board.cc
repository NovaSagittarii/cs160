#include "player_board.hh"

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
  px_ = (Board::width - current_piece_->width()) / 2;
  py_ = 23;
  pd_ = 0;
}

void PlayerBoard::Softdrop() {
  while (py_ - 1 >= 0 && IsValidPosition(px_, py_ - 1, pd_)) --py_;
}

void PlayerBoard::Harddrop() {
  Softdrop();
  // std::cout << board_ << "\n";
  current_piece_->Place(board_, px_, py_, pd_);
  // std::cout << board_ << "\n";
  current_piece_ = QueuePop();
  ResetPosition();
}

bool PlayerBoard::IsValidPosition(int x, int y, int d) const {
  bool ok = !current_piece_->Intersects(board_, x, y, d);
  // std::cout << "testing " << x << " " << y << " " << d << " ";
  // std::cout << (ok ? "OK" : "FAIL");
  // std::cout << "\n";
  return ok;
}
