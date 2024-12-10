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
  EXPECT_EQ(PlayerBoard(pb).current_piece(), O)
      << "copied board should inherit current piece";
}

TEST(PlayerBoard, QueuePop) {
  PlayerBoard pb(0);
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
  PlayerBoard pb(0);
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
  int expect_y = 0;
  pb.set_current_piece(I);
  pb.ResetPosition();
  pb.Softdrop();
  expect_y = pb.py();
  for (int i = 0; i < 10; ++i) {
    pb.set_current_piece(I);
    pb.ResetPosition();
    EXPECT_EQ(pb.py(), 20) << "Should spawn at the top";
    pb.Softdrop();
    EXPECT_EQ(pb.py(), expect_y) << "Should slowly be building up";
    pb.Harddrop();
    ++expect_y;
  }
}

TEST(PlayerBoard, Harddrop_Z) {
  PlayerBoard pb;
  const Piece* piece = &pieces::Z;
  pb.set_current_piece(piece);
  pb.ResetPosition();
  pb.Softdrop();
  int low_y = pb.py();
  for (int i = 0; i < 10; ++i) {
    pb.set_current_piece(piece);
    pb.ResetPosition();
    pb.Softdrop();
    ASSERT_EQ(pb.py(), low_y + i * 2) << "Should slowly be building up";
    pb.Harddrop();
  }
}

TEST(PlayerBoard, LeftRight) {
  PlayerBoard pb;
  for (auto piece : pieces::kAll7) {
    pb.set_current_piece(piece);
    pb.ResetPosition();
    std::cout << "== currently testing " << piece->name() << "\n";
    int initx = pb.px();
    pb.MoveLeft();
    ASSERT_EQ(pb.px(), initx - 1) << "dx=-1 for " << pb.current_piece()->name();
    pb.MoveLeft();
    ASSERT_EQ(pb.px(), initx - 2) << "dx=-2 for " << pb.current_piece()->name();
    pb.MoveRight();
    ASSERT_EQ(pb.px(), initx - 1) << "dx=-1 for " << pb.current_piece()->name();
    pb.MoveRight();
    ASSERT_EQ(pb.px(), initx) << "dx=0 for " << pb.current_piece()->name();
    pb.MoveRight();
    ASSERT_EQ(pb.px(), initx + 1) << "dx=1 for " << pb.current_piece()->name();
    pb.MoveRight();
    ASSERT_EQ(pb.px(), initx + 2) << "dx=2 for " << pb.current_piece()->name();
  }
}

TEST(PlayerBoard, Rotation) {
  PlayerBoard pb;
  EXPECT_EQ(pb.pd(), 0) << "should spawn facing north";
  for (int d = 0; d < 4; ++d) {
    EXPECT_EQ(pb.pd(), d) << "rotating clockwise";
    pb.RotateCW();
  }
  for (int d = 3; d >= 0; --d) {
    pb.RotateCCW();
    EXPECT_EQ(pb.pd(), d) << "rotating counter-clockwise";
  }
  pb.Rotate180();
  EXPECT_EQ(pb.pd(), 2) << "rotate 180";
}

TEST(PlayerBoard, Kick_SSD) {
  PlayerBoard pb;
  pb.LoadBoard(
      "####..##.#\n"
      "###..####.");
  pb.set_current_piece(&pieces::S);
  pb.ResetPosition();
  pb.RotateCW();
  EXPECT_GE(pb.py(), 2) << "it should be somewhere pretty high";
  pb.MoveLeft();
  EXPECT_GE(pb.py(), 2) << "it should be somewhere pretty high";
  pb.Softdrop();
  EXPECT_EQ(pb.py(), 1) << "soft dropped";
  pb.RotateCW();
  EXPECT_EQ(pb.py(), 0) << "kick should work";
  pb.Harddrop();
  EXPECT_TRUE(pb.spin()) << "this should count as a spin";
  pb.Softdrop();
  pb.RotateCW();
  pb.Harddrop();
  EXPECT_FALSE(pb.spin()) << "that is not a spin";
}

TEST(PlayerBoard, Kick_TST_CW) {
  PlayerBoard pb;
  pb.LoadBoard(
      "##........\n"
      "#.........\n"
      "#.########\n"
      "#..#######\n"
      "#.########");
  pb.set_current_piece(&pieces::T);
  pb.ResetPosition();
  pb.Softdrop();
  EXPECT_GT(pb.py(), 0) << "should be resting on the lines";
  int initx = pb.px();
  int expect_x = initx;
  while (pb.px() > 1) {
    pb.MoveLeft();
    --expect_x;
    EXPECT_EQ(pb.px(), expect_x) << "should be able to move to the left";
  }
  pb.RotateCW();
  EXPECT_EQ(pb.py(), 0) << "kick should work";
  pb.Harddrop();
  EXPECT_TRUE(pb.spin()) << "this should count as a spin";
  EXPECT_EQ(pb.attack(), 6) << "6 attack from TST";
  pb.Softdrop();
  pb.RotateCW();
  pb.Harddrop();
  EXPECT_FALSE(pb.spin()) << "that is not a spin";
}

TEST(PlayerBoard, Kick_TST_CCW) {
  PlayerBoard pb;
  pb.LoadBoard(
      "........##\n"
      ".........#\n"
      "#.######.#\n"
      "##.####..#\n"
      "#.######.#");
  pb.set_current_piece(&pieces::T);
  pb.ResetPosition();
  pb.Softdrop();
  EXPECT_GT(pb.py(), 0) << "should be resting on the lines";
  pb.MoveRight();
  pb.MoveRight();
  pb.MoveRight();
  pb.MoveRight();
  pb.RotateCCW();
  EXPECT_EQ(pb.py(), 0) << "kick should work";
  pb.Harddrop();
  EXPECT_TRUE(pb.spin()) << "this should count as a spin";
  pb.Softdrop();
  pb.RotateCW();
  pb.Harddrop();
  EXPECT_FALSE(pb.spin()) << "that is not a spin";
}

TEST(PlayerBoard, GeneratePlacements_O) {
  PlayerBoard pb;
  pb.set_current_piece(&pieces::O);
  EXPECT_EQ(pb.GeneratePlacements().size(), 36)
      << "There are 36 possible locations to put an O on an empty board. 9 "
         "horizontal offsets, and 4 orientations for each ";
}

TEST(PlayerBoard, GeneratePlacements_TST) {
  PlayerBoard pb;
  pb.LoadBoard(
      "##........\n"
      "#.........\n"
      "#.#####.##\n"
      "#..#####.#\n"
      "#.#######.");
  pb.set_current_piece(&pieces::T);
  pb.ResetPosition();
  bool ok = false;
  for (auto [x, y, d] : pb.GeneratePlacements()) {
    if (y == 0) ok = true;
  }
  EXPECT_TRUE(ok) << "Should have found one valid TST kick.";
}


TEST(PlayerBoard, ClearLines) {
  PlayerBoard pb;
  pb.LoadBoard(
      "##########\n"
      "#.........\n"
      "##########");
  EXPECT_EQ(pb.ClearLines(), 2) << "there are two filled in lines";
  EXPECT_EQ(pb.board().grid[0], 1) << "that one block should've dropped";
}
