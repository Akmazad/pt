#ifndef PT_WANDERER_HPP_
#define PT_WANDERER_HPP_

#include <memory>
#include <queue>
#include <stack>
#include <string>
#include <vector>

#include <libpll/pll.h>
#include <model.hpp>
#include <pll_partition.hpp>

#include "authority.hpp"
#include "common.hpp"
#include "options.hpp"
#include "position.hpp"

namespace pt {

class Wanderer {
 private:
  const Options options_;
  Authority& authority_;
  pll::Partition partition_;

  // a data structure that makes more sense for this might be a stack
  // of pairs like {tree, move_queue}. MoveBack() could pop the top of
  // the stack and reset things for the new tree on the top of the
  // stack and continue with the moves in the queue. life would be
  // easier if there were a Tree class that took care of tree resource
  // management, copying etc.

  // the primary benefit of keeping the tree history around is so we
  // don't have to worry about restoring branch lengths upon moving
  // back after a full optimization is performed. I don't think
  // storing all the branch lengths would use much less memory than
  // storing the whole tree.

  std::stack<Position> path_;
  std::stack<std::queue<TreeMove>> move_queues_;

 public:
  Wanderer(const Options& options,
           Authority& authority,
           const Position& starting_position,
           const std::vector<std::string>& labels,
           const std::vector<std::string>& sequences);

  ~Wanderer();

  void Teleport(const Position& position);
  void Start();

 private:
  void QueueMoves();

  bool TestMove(pll_utree_t* tree, pll_unode_t* node, MoveType type);

  void MoveForward();
  void MoveBack();
};

} // namespace pt

#endif /* PT_WANDERER_HPP_ */
