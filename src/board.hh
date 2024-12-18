#ifndef SRC_BOARD_HH_
#define SRC_BOARD_HH_

#include <bitset>
#include <iostream>

class Board {
 public:
  void Clear();
  friend std::ostream& operator<<(std::ostream& out, const Board& rhs);

  const static int width = 10;
  const static int height = 25;
  typedef std::bitset<width * height> Grid;

  Grid grid;  // contents of the board
};

#endif  // SRC_BOARD_HH_