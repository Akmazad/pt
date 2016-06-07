#ifndef PT_PARTITION_
#define PT_PARTITION_

#include <iostream>
#include <memory>
#include <string>

#include "pll-utils.hpp"

/// @file partition.hpp
/// @brief Headers for the Partition class.

namespace pt {




/// @brief The representation of a tree, alignment, and all associated data.
///
class Partition {
  private:
    unsigned int tip_nodes_count_;
    pll_utree_t * tree_;

  public:
    Partition(
      std::string newick_path,
      std::string fasta_path);
   virtual ~Partition();

   unsigned int tip_nodes_count() { return tip_nodes_count_; };
   unsigned int inner_nodes_count() { return (tip_nodes_count() - 2); };
   unsigned int nodes_count() { return (tip_nodes_count() + inner_nodes_count()); };
   unsigned int branch_count() { return (nodes_count() - 1); };

   std::string ToNewick();

};

}

#endif  // PT_PARTITION_

