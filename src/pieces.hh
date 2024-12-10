#ifndef SRC_PIECES_HH_
#define SRC_PIECES_HH_

#include <array>
#include <cassert>
#include <sstream>
#include <vector>

#include "board.hh"

class Piece {
 public:
  /**
   * @brief Construct a new Piece object.
   *
   * @param name
   * @param sn string for north
   * @param se string for east
   * @param ss string for south
   * @param sw string for west
   */
  Piece(const std::string& name, const std::string& sn, const std::string& se,
        const std::string& ss, const std::string& sw) {
    name_ = name;
    LoadString(nesw_[0], sn);
    LoadString(nesw_[1], se);
    LoadString(nesw_[2], ss);
    LoadString(nesw_[3], sw);
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
    if (dx < 0) return true;
    if (dy + y_offset_ < 0) return true;
    // std::cout << "dy=" << dy << "; h=" << height_ << "; bh=" << Board::height
    // << "\n";
    if (dx + width_ > Board::width || dy + height_ > Board::height) {
      return true;
    }
    auto mask = nesw_[direction] << (dx + Board::width * dy);
    // Board b;
    // b.grid = board.grid; std::cout << b << "\n";
    // b.grid = mask; std::cout << b << "\n";
    return (board.grid & mask).any();
  }

  void Place(Board& board, int dx, int dy, int direction) const {
    assert(dx >= 0 && "Invalid offset");
    assert(dy + y_offset_ >= 0 && "Invalid offset");
    assert(dx + width_ <= Board::width && "Invalid offset");
    assert(dy + height_ <= Board::height && "Invalid offset");
    auto mask = nesw_[direction] << (dx + Board::width * dy);
    board.grid |= mask;
  }
  size_t width() const { return width_; }
  size_t height() const { return height_; }
  const std::string& name() const { return name_; }

 private:
  size_t y_offset_;
  size_t width_, height_;
  std::array<Board::Grid, 4> nesw_;
  std::string name_;

  void LoadString(Board::Grid& grid, const std::string& s) {
    std::vector<std::string> g;
    std::istringstream in(s);
    std::string line;
    while (in >> line) g.push_back(line);
    grid ^= grid;  // clear
    size_t firsti = Board::height;
    int first_width = -1;
    width_ = 0;
    height_ = 0;
    for (size_t i = 0; i < g.size(); ++i) {
      for (size_t j = 0; j < g[i].size(); ++j) {
        assert(i * Board::width + j < grid.size() &&
               "Input shouldn't go out of bounds.");
        if (g[g.size() - 1 - i][j] == '#') {  // flip vertically (storage)
          grid[i * Board::width + j] = 1;
          height_ = std::max(height_, i);
          width_ = std::max(width_, j);
          firsti = std::min(firsti, i);
        }
      }
      if (first_width == -1) {
        first_width = line.size();
      } else {
        assert(first_width == (int)line.size() &&
               "Input should be a n x m rectangle.");
      }
    }
    // alignment
    // int dx = 0;
    // int dy = firsti;
    y_offset_ = firsti;
    // Board b;
    // std::cout << "dy=" << dy << "\n";
    // b.grid = grid; std::cout << b << "\n";
    // grid >>= (dx + Board::width * dy);
    // b.grid = grid; std::cout << b << "\n";
  }
};

namespace pieces {

const static Piece J("J",
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
const static Piece L("L",
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
const static Piece S("S",
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
const static Piece Z("Z",
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
const static Piece O("O",
                     "##\n"
                     "##",
                     "##\n"
                     "##",
                     "##\n"
                     "##",
                     "##\n"
                     "##");
const static Piece I("I",
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
const static Piece T("T",
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
const static Piece Line10("10", "##########", "##########", "##########",
                          "##########");

}  // namespace pieces

#endif  // SRC_PIECES_HH_