#include "board.hh"

#include <array>

std::ostream& operator<<(std::ostream& out, const Board& rhs) {
  const std::string empty = ".";
  const std::string upper_half = "\u2580";
  const std::string lower_half = "\u2584";
  const std::string full = "\u2588";
  std::array<std::string, 4> chars = {empty, upper_half, lower_half, full};
  for (int i = 0; i < Board::height; i += 2) {
    for (int j = 0; j < Board::width; ++j) {
      int id = 0;
      if (rhs.grid[i * Board::width + j]) id |= 1;
      if (i + 1 < Board::height && rhs.grid[(i + 1) * Board::width + j])
        id |= 2;
      out << chars[id];
    }
    out << "\n";
  }
  return out;
}