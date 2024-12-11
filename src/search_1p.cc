/**
 * game tree search for 1 player
 */

#include <algorithm>
#include <iomanip>
#include <tuple>
#include <utility>

#include "player_board.hh"

int32_t main() {
  PlayerBoard pb;
  pb.LoadBoard(
      "#####...##\n"
      "####...###\n"
      "#####.####");
  pb.set_current_piece(&pieces::T);
  std::cout << pb << "\n";

  for (int i = 0; i < 10000; ++i) {
    int best_placement_id = -1;
    double best_eval = -1e9;
    auto placements = pb.GeneratePlacements();
    // <eval, id, pb_idx>
    std::vector<std::tuple<double, int, int>> curr_id, next_id;
    std::vector<PlayerBoard> curr_pb, next_pb;
    for (int id = 0; id < (int)placements.size(); ++id) {
      auto [x, y, d] = placements[id];
      PlayerBoard pb_(pb);
      pb_.SimulatePlacement(x, y, d);
      double eval = pb_.Evaluate();
      curr_id.push_back({eval, id, curr_id.size()});
      curr_pb.push_back(std::move(pb_));
    }

    int nodes_explored = 0;
    const int max_depth = 6;
    for (int curr_level = 2; curr_level <= max_depth; ++curr_level) {
      std::sort(curr_id.rbegin(), curr_id.rend());
      for (size_t j = 0; j < std::min<size_t>(1000, curr_id.size()); ++j) {
        ++nodes_explored;
        auto [eval, id, pb_idx] = curr_id[j];
        auto& pb = curr_pb[pb_idx];
        if (curr_level == max_depth) {
          if (eval > best_eval) {
            best_eval = eval;
            best_placement_id = id;
          }
        }
        for (auto [x, y, d] : pb.GeneratePlacements()) {
          PlayerBoard pb_(pb);
          pb_.SimulatePlacement(x, y, d);
          double eval = pb_.Evaluate();
          next_id.push_back({eval, id, next_id.size()});
          next_pb.push_back(pb_);
        }
      }
      curr_pb.clear();
      curr_id.clear();
      std::swap(curr_pb, next_pb);
      std::swap(curr_id, next_id);
    }

    assert(best_placement_id >= 0 && "should have at least ONE placement");
    auto [x, y, d] = placements[best_placement_id];
    pb.SimulatePlacement(x, y, d);
    std::cout << std::string(10, '\n');
    std::cout << pb;
    std::cout << "eval= " << best_eval << "; ";
    std::cout << "nodes explored= " << nodes_explored << std::endl;
  }
}