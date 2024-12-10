#include "player_board.hh"

#include <gtest/gtest.h>

#include <iostream>

TEST(PlayerBoard, CopyConstructor) {
  PlayerBoard pb;
  const Piece* I = &pieces::I;
  const Piece* O = &pieces::O;
  pb.set_current_piece(I);
  EXPECT_EQ(pb.current_piece(), I) << "previously set";
  EXPECT_EQ(PlayerBoard(pb).current_piece(), I)
      << "copied board should inherit current piece";
  pb.set_current_piece(O);
  EXPECT_EQ(pb.current_piece(), O) << "previously set";
  EXPECT_EQ(PlayerBoard(pb).current_piece(), O) << "copied board should inherit current piece";
}

TEST(PlayerBoard, QueuePop) {
  PlayerBoard pb;
  std::vector<std::string> bags;
  for (int i = 0; i < 3; ++i) {
    std::string bag;
    for (int j = 0; j < 7; ++j) {
      bag += pb.QueuePop()->name();
    }
    bags.push_back(bag);
    std::cout << bag << "\n";
  }
  EXPECT_NE(bags[0], bags[1]) << "Bags *should* be different.";
  EXPECT_NE(bags[0], bags[2]) << "Bags *should* be different.";
  EXPECT_NE(bags[1], bags[2]) << "Bags *should* be different.";
}

TEST(PlayerBoard, BagContents) {
  PlayerBoard pb;
  for (int i = 0; i < 5000; ++i) {
    std::set<std::string> bag;
    for (int j = 0; j < 7; ++j) {
      std::string name = pb.QueuePop()->name();
      bag.insert(name);
      // std::cout << name;
      EXPECT_LE(bag.size(), 7) << "Shouldn't have more than 7 unique items.";
    }
    // std::cout << "\n";
    EXPECT_EQ(bag.size(), 7) << "There should be 7 unique items per bag.";
  }
}

TEST(PlayerBoard, Harddrop_I) {
  PlayerBoard pb;
  const Piece* I = &pieces::I;
  for (int i = 0; i < 10; ++i) {
    pb.set_current_piece(I);
    pb.ResetPosition();
    EXPECT_EQ(pb.py(), 23) << "Should spawn at the top";
    pb.Softdrop();
    ASSERT_EQ(pb.py(), i) << "Should slowly be building up";
    pb.Harddrop();
  }
}

TEST(PlayerBoard, Harddrop_Z) {
  PlayerBoard pb;
  const Piece* I = &pieces::Z;
  for (int i = 0; i < 10; ++i) {
    pb.set_current_piece(I);
    pb.ResetPosition();
    EXPECT_EQ(pb.py(), 23) << "Should spawn at the top";
    pb.Softdrop();
    ASSERT_EQ(pb.py(), i*2) << "Should slowly be building up";
    pb.Harddrop();
  }
}
