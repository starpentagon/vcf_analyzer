#include <iostream>
#include <chrono>

#include <boost/program_options.hpp>

#include "ParallelVCFAnalyzer.h"

using namespace std;
using namespace boost::program_options;
using namespace realcore;

string VCFResultString(const VCFAnalyzer &vcf_analyzer, const VCFResult &vcf_result);

int main(int argc, char* argv[])
{
  // オプション設定
  options_description option;

  option.add_options()
    ("problem-db,p", value<string>(), "問題DBのファイルパス")
    ("time,t", value<SearchCounter>()->default_value(0), "探索時間(秒)(default: 無制限)")
    ("depth,d", value<VCFSearchDepth>()->default_value(225), "探索深さ(default: 無制限)")
    ("dual", "余詰を探索する")
    ("thread", value<size_t>()->default_value(1), "同時実行数(default: 1)")
    ("header", "ヘッダを表示")
    ("help,h", "ヘルプを表示");
  
  variables_map arg_map;
  store(parse_command_line(argc, argv, option), arg_map);

  if(arg_map.count("header")){
    ParallelVCFAnalyzer::OutputHeader();
    return 0;
  }

  bool is_help = arg_map.count("help") || !arg_map.count("problem-db");

  if(is_help){
    cout << "Usage: " << argv[0] << " [options]" << endl;
    cout << option;
    cout << "Note: problem-db must have ID, Name, Board column" << endl;
    cout << endl;
    return 0;
  }

  const auto problem_db_path = arg_map["problem-db"].as<string>();
  ParallelVCFAnalyzer parallel_vcf_analyzer(problem_db_path);

  const auto thread_num = arg_map["thread"].as<size_t>();
  
  parallel_vcf_analyzer.Run(thread_num, arg_map);

  return 0;
}
