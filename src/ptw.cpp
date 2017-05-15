#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

// pll.h is missing a header guard
#ifndef LIBPLL_PLL_H_
#define LIBPLL_PLL_H_
#include <libpll/pll.h>
#endif

#include <model_parameters.hpp>
#include <pll_partition.hpp>
#include <pll_util.hpp>

#include "authority.hpp"
#include "wanderer.hpp"

void WriteTreeTable(pt::TreeTable& trees, const std::string& path)
{
  std::ofstream ofs(path);

  auto table = trees.lock_table();
  for (auto& item : table) {
    ofs << item.second << "\t" << item.first << "\n";
  }
}

int main(int argc, const char* argv[])
{
  if (argc != 7) {
    std::cerr << "usage:\n\n"
              << "ptw\n"
              << "\t<input_tree>\n"
              << "\t<input_alignment>\n"
              << "\t<input_raxml_info>\n"
              << "\t<lnl_offset>\n"
              << "\t<output_good_trees>\n"
              << "\t<output_visited_trees>\n";
    return 1;
  }

  //
  // parse arguments
  //

  std::string newick_path(argv[1]);
  std::string fasta_path(argv[2]);
  std::string raxml_path(argv[3]);

  double lnl_offset = std::stod(argv[4]);

  std::string good_trees_path(argv[5]);
  std::string visited_trees_path(argv[6]);

  if (lnl_offset > 0.0) {
    std::cerr << "error: expected an lnl_offset < 0.\n";
    return 1;
  }

  //
  // load tree, sequences, and model info
  //

  unsigned int tip_node_count;
  pll_utree_t* tree = pll_utree_parse_newick(newick_path.c_str(),
                                             &tip_node_count);

  std::vector<std::string> labels;
  std::vector<std::string> sequences;
  pt::pll::ParseFasta(fasta_path, tip_node_count, labels, sequences);

  pt::pll::ModelParameters parameters = pt::pll::ParseRaxmlInfo(raxml_path);

  //
  // initialize the partition and create a wanderer
  //

  pt::pll::Partition partition(tree, tip_node_count, parameters, labels, sequences);
  partition.TraversalUpdate(tree, pt::pll::TraversalType::FULL);
  double ml_lnl = partition.LogLikelihood(tree);

  pt::Authority authority(ml_lnl, lnl_offset);
  pt::Wanderer wanderer(authority, std::move(partition), tree);

  //
  // go!
  //

  wanderer.Start();

  //
  // write output
  //

  // ensure that the good tree table is filtered based on the final threshold
  authority.FilterGoodTreeTable();

  WriteTreeTable(authority.GetGoodTreeTable(), good_trees_path);
  WriteTreeTable(authority.GetVisitedTreeTable(), visited_trees_path);

  //
  // clean up and return
  //

  pll_utree_every(tree, pt::pll::cb_erase_data);
  pll_utree_destroy(tree);

  return 0;
}
