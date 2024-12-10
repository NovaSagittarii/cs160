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
   * placed at the specific position.
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
    if (dx + width_ >= Board::width) return true;
    if (dy + height_ >= Board::height) return true;
    int offset = (dx + Board::width * dy);
    auto mask = nesw_[direction];
    if (offset > 0) {
      mask <<= offset;
    } else {
      mask >>= -offset;
    }
    /// TODO: fix vertical out-of-bounds (not this scuffed implementation)
    auto mmask = mask;
    if (offset > 0) {
      mmask >>= offset;
    } else {
      mmask <<= -offset;
    }
    if (mmask != nesw_[direction]) return true;  // its off the board

    // std::cout << "yoffset" << y_offset_ << " ? " << (dy + (int)y_offset_)
    //           << "\n";
    // std::cout << "<< " << (dx + Board::width * dy) << "\n";
    // Board b;
    // b.grid = board.grid; std::cout << "board" << b << "\n";
    // b.grid = mask; std::cout << "mask" << b << "\n";
    return (board.grid & mask).any();
  }

  void Place(Board& board, int dx, int dy, int direction) const {
    assert(dx >= 0 && "Invalid offset");
    // assert(dy + y_offset_ >= 0 && "Invalid offset"); // This is broken?
    assert(dx + width_ < Board::width && "Invalid offset");
    assert(dy + height_ <= Board::height && "Invalid offset");
    int offset = (dx + Board::width * dy);
    auto mask = nesw_[direction];
    if (offset > 0) {
      mask <<= offset;
    } else {
      mask >>= -offset;
    }
    board.grid |= mask;
    // Board b;
    // std::cout << "dxy= " << dx << "," << dy << "\n";
    // std::cout << "yo= " << y_offset_ << "\n";
    // std::cout << "wh= " << width_ << "," << height_ << "\n";
    // b.grid = nesw_[direction]; std::cout << "pre" << b << "\n";
    // b.grid = mask; std::cout << "post" << b << "\n";
  }
  size_t width() const { return width_; }
  size_t height() const { return height_; }
  const auto& nesw() const { return nesw_; }
  const std::string& name() const { return name_; }

 private:
  int y_offset_;
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
          height_ = std::max(height_, g.size() - 1 - i);
          width_ = std::max(width_, j);
          firsti = std::min(firsti, g.size() - i);
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
    y_offset_ = height_ - firsti;
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
const static std::vector<const Piece*> kAll7 = {&J, &T, &L, &S, &Z, &I, &O};

}  // namespace pieces

#endif  // SRC_PIECES_HH_