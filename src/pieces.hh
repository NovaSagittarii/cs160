#ifndef SRC_PIECES_HH_
#define SRC_PIECES_HH_

#include <array>
#include <cassert>
#include <sstream>

#include "board.hh"

class Piece {
 public:
  /**
   * @brief Construct a new Piece object.
   *
   * @param sn string for north
   * @param se string for east
   * @param ss string for south
   * @param sw string for west
   */
  Piece(const std::string& sn, const std::string& se, const std::string& ss,
        const std::string& sw) {
    LoadString(nesw[0], sn);
    LoadString(nesw[1], se);
    LoadString(nesw[2], ss);
    LoadString(nesw[3], sw);
  }

  /**
   * @brief Piece-board intersection routine for checking if a piece can be
   * placed at the specific position. Returns true
   *
   * @param grid to check against
   * @param dx horizontal offset of the top-left corner
   * @param dy vertical offset of the top-left corner
   * @param direction that the piece faces (orientation)
   * @return `true` if you cannot place it, either because there is an
   * intersection, OR the placement is off the board.
   */
  bool Intersects(const Board& board, int dx, int dy, int direction) const {
    assert(dx >= 0 && "Never should use negative offset.");
    assert(dy >= 0 && "Never should use negative offset.");
    if (dx + width > Board::width || dy + height > Board::height) return false;
    auto mask = nesw[direction] << (dx + Board::width * dy);
    return (board.grid & mask).any();
  }

  void Place(Board& board, int dx, int dy, int direction) const {
    assert(dx >= 0 && "Invalid offset");
    assert(dy >= 0 && "Invalid offset");
    assert(dx + width <= Board::width && "Invalid offset");
    assert(dy + height <= Board::height && "Invalid offset");
    auto mask = nesw[direction] << (dx + Board::width * dy);
    board.grid |= mask;
  }

 private:
  int width, height;
  std::array<Board::Grid, 4> nesw;

  void LoadString(Board::Grid& grid, const std::string& s) {
    std::istringstream in(s);
    std::string line;
    grid ^= grid;  // clear
    int i = 0;
    width = -1;
    while (std::getline(in, line)) {
      for (int j = 0; j < line.size(); ++j) {
        assert(i * Board::width + j < grid.size() &&
               "Input shouldn't go out of bounds.");
        if (line[j] == '#') {
          grid[i * Board::width + j] = 1;
        }
      }
      ++i;
      if (width == -1) {
        width = line.size();
      } else {
        assert(width == line.size() && "Input should be a n x m rectangle.");
      }
    }
    height = i;
  }
};

namespace pieces {

const static Piece J(
    "#..\n"
    "###\n"
    "...",
    "...\n"
    "###\n"
    "#..",
    "##.\n"
    ".#.\n"
    ".#.",
    ".#.\n"
    ".#.\n"
    "##.");
const static Piece L(
    "..#\n"
    "###\n"
    "...",
    ".#.\n"
    ".#.\n"
    ".##",
    "...\n"
    "###\n"
    "#..",
    "##.\n"
    ".#.\n"
    ".#.");
const static Piece S(
    ".##\n"
    "##.\n"
    "...",
    ".#.\n"
    ".##\n"
    "..#",
    "...\n"
    ".##\n"
    "##.",
    "#..\n"
    "##.\n"
    ".#.");
const static Piece Z(
    "##.\n"
    ".##\n"
    "...",
    "..#\n"
    ".##\n"
    ".#.",
    "...\n"
    "##.\n"
    ".##",
    ".#.\n"
    "##.\n"
    "#..");
const static Piece O(
    "##\n"
    "##",
    "##\n"
    "##",
    "##\n"
    "##",
    "##\n"
    "##");
const static Piece I(
    "....\n"
    "####\n"
    "....\n"
    "....",
    "..#.\n"
    "..#.\n"
    "..#.\n"
    "..#.",
    "....\n"
    "....\n"
    "####\n"
    "....",
    ".#..\n"
    ".#..\n"
    ".#..\n"
    ".#..");
const static Piece T(
    ".#.\n"
    "###\n"
    "...",
    ".#.\n"
    ".##\n"
    ".#.",
    "...\n"
    "###\n"
    ".#.",
    ".#.\n"
    "##.\n"
    ".#.");

}  // namespace pieces

#endif  // SRC_PIECES_HH_