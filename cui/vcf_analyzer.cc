#include <iostream>
#include <chrono>

#include <boost/program_options.hpp>

#include "VCFAnalyzer.h"
#include "SGFParser.h"

using namespace std;
using namespace boost::program_options;

using namespace realcore;

string VCFResultString(const variables_map &arg_map, const MoveList &board_move_list, const VCFAnalyzer &vcf_analyzer, const VCFSearch &vcf_search, const VCFResult &vcf_result);
string GetTreeSGFText(const variables_map &arg_map, const MoveList &board_move_list, const MoveTree &tree);

int main(int argc, char* argv[])
{
  // オプション設定
  options_description option;

  option.add_options()
    ("string,s", value<string>(), "開始局面: [a-o]形式のテキスト")
    ("sgf-input", value<string>(), "開始局面: SGF形式のテキスト")
    ("pos", value<string>(), "POS形式ファイル名")
    ("depth,d", value<VCFSearchDepth>()->default_value(225), "探索深さ(default:225)")
    ("dual", "余詰/最善応手を探索する")
    ("sgf-output", "探索結果をSGF形式で出力する")
    ("help,h", "ヘルプを表示");
  
  variables_map arg_map;
  store(parse_command_line(argc, argv, option), arg_map);

  size_t input_count = (arg_map.count("string") ? 1 : 0) + (arg_map.count("pos") ? 1 : 0) + (arg_map.count("sgf-input") ? 1 : 0);
  bool is_help = arg_map.count("help") || input_count != 1;

  if(is_help){
    cout << "Usage: " << argv[0] << " [options]" << endl;
    cout << option;
    cout << endl;
    return 0;
  }

  MoveList board_sequence;

  if(arg_map.count("string")){
    const auto board_string = arg_map["string"].as<string>();
    board_sequence = MoveList(board_string);
  }else if(arg_map.count("sgf-input")){
    const auto sgf_data = arg_map["sgf-input"].as<string>();
    GetMoveListFromSGFData(kSGFCheckNone, sgf_data, &board_sequence);
  }else{
    const auto pos_file = arg_map["pos"].as<string>();
    ReadPOSFile(pos_file, &board_sequence);
  }

  if(!IsNonTerminateNormalSequence(board_sequence)){
    cerr << "The move sequence is not a non-terminal normal sequence: " << board_sequence.str() << endl;
  }

  const auto search_depth = arg_map["depth"].as<VCFSearchDepth>();

  VCFAnalyzer vcf_analyzer(board_sequence);
  
  cerr << vcf_analyzer.GetSettingInfo() << endl;

  VCFSearch vcf_search;
  vcf_search.remain_depth = search_depth;
  vcf_search.detect_dual_solution = arg_map.count("dual");

  VCFResult vcf_result;

  vcf_analyzer.Solve(vcf_search, &vcf_result);

  cout << VCFResultString(arg_map, board_sequence, vcf_analyzer, vcf_search, vcf_result);

  return 0;
}

string VCFResultString(const variables_map &arg_map, const MoveList &board_move_list, const VCFAnalyzer &vcf_analyzer, const VCFSearch &vcf_search, const VCFResult &vcf_result)
{
  stringstream ss;
  
  // 探索結果
  ss << "Result: ";

  if(vcf_result.solved){
    ss << "Solved" << endl;

    ss << "First Move: " << MoveString(vcf_result.best_response[0]) << endl;

    const bool is_sgf = arg_map.count("sgf-output");
    
    if(is_sgf){
      ss << "Best response: " << vcf_result.best_response.GetSGFPositionText() << endl;
    }else{
      ss << "Best response: " << vcf_result.best_response.str() << endl;
    }
    
    ss << "Proof Tree: " << vcf_result.proof_tree.str() << endl;

    if(vcf_search.detect_dual_solution){
      if(vcf_result.detect_dual_solution){
        ss << "Dual Solution: " << vcf_result.dual_solution_tree.str();
      }else{
        ss << "Dual Solution: -";
      }
    }else{
        ss << "Dual Solution: Unknown";
    }
  }else if(vcf_result.disproved){
    ss << "Disproved";
  }else{
    ss << "Terminated";
  }

  ss << endl;

  // 探索深さ
  ss << "Depth: " << vcf_result.search_depth << endl;

  const auto& search_manager = vcf_analyzer.GetSearchManager();

  // 探索ノード数
  const auto node_count = search_manager.GetNode();
  ss << "Nodes: " << node_count << endl;

  // 探索時間
  const auto search_time = search_manager.GetSearchTime();
  ss << "Time(s): " << search_time / 1000.0 << endl;

  // 探索速度
  if(search_time > 0){
    ss << "NPS: " << 1000 * node_count / search_time << endl;
  }else{
    ss << "NPS: INF" << endl;
  }

  return ss.str();
}

string GetTreeSGFText(const variables_map &arg_map, const MoveList &board_move_list, const MoveTree &tree)
{
  stringstream ss;

  ss << "(;GM[4]FF[4]SZ[15]";
  ss << board_move_list.GetSGFPositionText();
  ss << (board_move_list.IsBlackTurn() ? "PL[B]" : "PL[W]");
  ss << tree.GetSGFLabeledText(board_move_list.IsBlackTurn());
  ss << ")";
  
  return ss.str();
}
