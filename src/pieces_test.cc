#include "pieces.hh"

#include <gtest/gtest.h>

#include <iostream>

TEST(Pieces, PrintTest) {
  Board b;
  std::ostringstream o1, o2;
  o1 << b;
  std::cout << b << std::endl;
  pieces::Z.Place(b, 0, 0, 0);
  o2 << b;
  std::cout << b << std::endl;
  EXPECT_NE(o1.str(), o2.str()) << "Placement should've changed the board.";
}

TEST(Pieces, Intersect) {
  for (int d = 0; d < 3; ++d) {  // try various orientations of O
    Board b;
    EXPECT_FALSE(pieces::O.Intersects(b, 1, 0, d)) << "Nothing's on the board";
    pieces::O.Place(b, 0, 0, d);
    EXPECT_TRUE(pieces::O.Intersects(b, 0, 0, (d + 1) % 3))
        << "Should overlap";
    EXPECT_TRUE(pieces::O.Intersects(b, 2, 1, (d + 2) % 3))
        << "Should overlap";
    EXPECT_TRUE(pieces::O.Intersects(b, 1, 0, (d + 3) % 3))
        << "Should overlap";
  }
}