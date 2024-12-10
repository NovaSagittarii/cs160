#ifndef SRC_BOARD_HH_
#define SRC_BOARD_HH_

#include <bitset>
#include <iostream>

class Board {
 public:
  const static int width = 10;
  const static int height = 20;
  typedef std::bitset<width * height> Grid;

  Grid grid;  // contents of the board

  friend std::ostream& operator<<(std::ostream& out, const Board& rhs);
};

#endif  // SRC_BOARD_HH_