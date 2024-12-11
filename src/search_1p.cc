/**
 * game tree search for 1 player
 */

#include <parlay/parallel.h>

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

  int total_nodes = 0;
  for (int i = 0; i < 100; ++i) {
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
    const int max_depth = 10;
    for (int curr_level = 2; curr_level <= max_depth; ++curr_level) {
      std::sort(curr_id.rbegin(), curr_id.rend());

      // sequential search
      // for (size_t j = 0; j < std::min<size_t>(1000, curr_id.size()); ++j) {
      //   ++nodes_explored;
      //   auto [eval, id, pb_idx] = curr_id[j];
      //   auto& pb = curr_pb[pb_idx];
      //   if (curr_level == max_depth) {
      //     if (eval > best_eval) {
      //       best_eval = eval;
      //       best_placement_id = id;
      //     }
      //   }
      //   for (auto [x, y, d] : pb.GeneratePlacements()) {
      //     PlayerBoard pb_(pb);
      //     pb_.SimulatePlacement(x, y, d);
      //     double eval = pb_.Evaluate();
      //     next_id.push_back({eval, id, next_id.size()});
      //     next_pb.push_back(pb_);
      //   }
      // };

      // multithreaded search
      std::vector<std::vector<std::tuple<double, int, int>>> next_ids(1000);
      std::vector<std::vector<PlayerBoard>> next_pbs(1000);

      auto ExploreNodeIndex = [&](size_t j) -> void {
        auto [eval, id, pb_idx] = curr_id[j];
        auto& pb = curr_pb[pb_idx];
        auto placements = pb.GeneratePlacements();
        for (size_t k = 0; k < std::min<size_t>(100, placements.size()); ++k) {
          auto [x, y, d] = placements[k];
          PlayerBoard pb_(pb);
          pb_.SimulatePlacement(x, y, d);
          double eval = pb_.Evaluate();
          next_ids[j].push_back({eval, id, 0});
          next_pbs[j].push_back(pb_);
        }
      };
      const size_t max_j = std::min<size_t>(1000, curr_id.size());
      // multithreaded mode (of parallel code)
      parlay::parallel_for(0, std::min<size_t>(1000, curr_id.size()),
                           ExploreNodeIndex);

      // sequential mode (of parallel code)
      // for (size_t j = 0; j < max_j; ++j) {
      //   ExploreNodeIndex(j);
      // }

      // collecting results
      for (size_t j = 0; j < max_j; ++j) {
        for (size_t k = 0; k < next_ids[j].size(); ++k) {
          auto [eval, id, pb_idx] = next_ids[j][k];
          auto pb_ = next_pbs[j][k];
          next_id.push_back({eval, id, next_id.size()});
          next_pb.push_back(std::move(pb_));

          if (curr_level == max_depth) {
            if (eval > best_eval) {
              best_eval = eval;
              best_placement_id = id;
            }
          }
          ++nodes_explored;
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
    std::cout << "eval=" << std::setw(6) << best_eval << "; ";
    std::cout << "nodes explored=" << std::setw(7) << nodes_explored << "; ";
    total_nodes += nodes_explored;
    std::cout << "total explored=" << std::setw(7) << total_nodes << "; ";
    std::cout << std::endl;
  }
}