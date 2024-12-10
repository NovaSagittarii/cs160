#include "player_board.hh"

#include <array>
#include <set>
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

PlayerBoard::PlayerBoard(uint64_t seed) : PlayerBoard() {
  garbage_rng_ = queue_rng_ = seed;
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
  ClearLines();

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

int PlayerBoard::ClearLines() {
  int lines = 0;
  const static Board::Grid line = pieces::Line10.nesw()[0];
  int j = 0;
  for (int i = 0; i < Board::height; ++i) {  // scan from bottom to top
    auto mask = (line << (Board::width * i));
    /// TODO: maybe vector<int32_t> would make more sense since you
    /// have these row-wise operations?
    auto line_mask = board_.grid & mask;
    if (line_mask == mask) {
      ++lines;
      board_.grid ^= mask;
    } else {
      if (i != j) {  // move row i to row j
        line_mask |= (line_mask >> (Board::width * (i - j)));
        board_.grid ^= line_mask;
        // std::cout << "move " << i << " to " << j << "\n";
        // Board b; b.grid = line_mask; std::cout << b << "\n";
      }
      ++j;
    }
    // std::cout << mask << "\n";
    // Board b; b.grid = mask; std::cout << b << "\n";
    // board_.grid
  }
  int base = lines;
  if (!spin_ && (base == 1 || base == 2 || base == 3)) base -= 1;
  // std::cout << "cleared " << lines << (spin_ ? "+spin" : "") << "\n";
  if (spin_ || lines >= 4) {
    if (b2b_) ++attack_;
    ++b2b_;
  } else {
    b2b_ = 0;
  }
  attack_ += base * (spin_ ? 2 : 1);
  return lines;
}

bool PlayerBoard::IsValidPosition(int x, int y, int d) const {
  bool ok = !current_piece_->Intersects(board_, x, y, d);
  // std::cout << "testing " << x << " " << y << " " << d << " ";
  // std::cout << (ok ? "OK" : "FAIL");
  // std::cout << "\n";
  return ok;
}

std::vector<std::array<int, 3>> PlayerBoard::GeneratePlacements() const {
  PlayerBoard pb(*this);  // make a copy to do the simulations
  /// TODO: replace with static array with vis counter
  std::set<std::array<int, 3>> vis, vis_stable;
  pb.ResetPosition();
  auto dfs = [&](auto&& dfs) -> void {
    int px = pb.px();
    int py = pb.py();
    int pd = pb.pd();
    std::array<int, 3> k = {px, py, pd};
    if (vis.count(k)) return;
    vis.insert(k);

    pb.MoveLeft();
    dfs(dfs);
    pb.set_px(px);
    pb.set_py(py);
    pb.set_pd(pd);

    pb.MoveRight();
    dfs(dfs);
    pb.set_px(px);
    pb.set_py(py);
    pb.set_pd(pd);

    pb.RotateCW();
    dfs(dfs);
    pb.set_px(px);
    pb.set_py(py);
    pb.set_pd(pd);

    pb.RotateCCW();
    dfs(dfs);
    pb.set_px(px);
    pb.set_py(py);
    pb.set_pd(pd);

    pb.Softdrop();
    vis_stable.insert({pb.px(), pb.py(), pb.pd()});
    dfs(dfs);
    pb.set_px(px);
    pb.set_py(py);
    pb.set_pd(pd);
  };
  dfs(dfs);
  return {vis_stable.begin(), vis_stable.end()};
};

std::ostream& operator<<(std::ostream& out, const PlayerBoard& rhs) {
  out << "curr: " << rhs.current_piece_->name() << "\n";
  out << rhs.board_;
  return out;
}
