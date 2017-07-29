#include "ParallelVCFAnalyzer.h"

using namespace std;
using namespace boost;
using namespace boost::program_options;
using namespace realcore;

ParallelVCFAnalyzer::ParallelVCFAnalyzer(const std::string &problem_db_path)
{
  const auto is_read = ReadCSV(problem_db_path, &problem_db_);

  if(!is_read){
    cerr << "Failed to read the problem db: " << problem_db_path << endl;
    return;
  }

  const auto problem_count = problem_db_["Board"].size();
  cerr << "Read: " << problem_count << " problems" << endl;

  for(size_t i=0; i<problem_count; i++){
    problem_index_list_.emplace_back(i);
  }
}

void ParallelVCFAnalyzer::Run(const size_t thread_num, const variables_map &arg_map)
{
  assert(thread_num >= 1);
  thread_group thread_group;

  // 置換表を確保する
  vcf_table_list_.reserve(thread_num);
  
  for(size_t i=0; i<thread_num; i++){
    vcf_table_list_.emplace_back(std::make_shared<VCFTable>(kDefaultVCFTableSpace, kLockFree));
  }

  // VCF Analyzerの設定を出力
  const auto &vcf_table = vcf_table_list_[0];
  VCFAnalyzer vcf_analyzer(MoveList(), vcf_table);
  cerr << vcf_analyzer.GetSettingInfo() << endl;

  for(size_t i=0; i<thread_num; i++){
    thread_group.create_thread(bind(&ParallelVCFAnalyzer::VCFAnalyze, this, i, arg_map));
  }

  thread_group.join_all();
}

const bool ParallelVCFAnalyzer::GetProblemIndex(size_t * const problem_index)
{
  boost::mutex::scoped_lock lock(mutex_problem_index_list_);
  assert(problem_index != nullptr);

  if(problem_index_list_.empty()){
    return false;
  }

  *problem_index = problem_index_list_.front();
  problem_index_list_.pop_front();

  return true;
}

void ParallelVCFAnalyzer::VCFAnalyze(const size_t thread_id, const variables_map &arg_map)
{
  const StringVector &board_list = problem_db_.at("Board");
  const StringVector &id_list = problem_db_.at("ID");
  const StringVector &name_list = problem_db_.at("Name");

  VCFSearch vcf_search;

  vcf_search.remain_depth = arg_map["depth"].as<VCFSearchDepth>();
  vcf_search.detect_dual_solution = arg_map.count("dual");

  size_t problem_id = 0;
  bool exist_problem = GetProblemIndex(&problem_id);

  while(exist_problem){
    {
      const auto problem_info = id_list[problem_id] + "_" + name_list[problem_id];

      boost::mutex::scoped_lock lock(mutex_cerr_);
      cerr << problem_info << endl;
    }

    const auto board_string = board_list[problem_id];
    MoveList board_sequence(board_string);
    const auto &vcf_table = vcf_table_list_[thread_id];

    if(!IsNonTerminateNormalSequence(board_sequence)){
      boost::mutex::scoped_lock lock(mutex_cerr_);
      cerr << "The move sequence is not a non-terminal normal sequence. The result may not be accurate. : " << board_sequence.str() << endl;

#ifndef NDEBUG
      VCFAnalyzer vcf_analyzer(MoveList(), vcf_table);
      VCFResult vcf_result;
      Output(problem_id, vcf_analyzer, vcf_search, vcf_result);

      exist_problem = GetProblemIndex(&problem_id);
      continue;
#endif
    }

    vcf_table->Initialize();

    VCFAnalyzer vcf_analyzer(board_sequence, vcf_table);
    SearchManager &search_manager = vcf_analyzer.GetSearchManager();
    const auto search_time = 1000 * arg_map["time"].as<SearchCounter>();
    search_manager.SetSearchTimeLimit(search_time);

    VCFResult vcf_result;

    vcf_analyzer.Solve(vcf_search, &vcf_result);
    Output(problem_id, vcf_analyzer, vcf_search, vcf_result);

    exist_problem = GetProblemIndex(&problem_id);
  }
}

void ParallelVCFAnalyzer::OutputHeader()
{
  cout << "ID,";
  cout << "Name,";
  cout << "Board,";
  cout << "Result,";
  cout << "FirstMove,";
  cout << "BestResponse,";
  cout << "ProofTree,";
  cout << "SearchedDepth,";
  cout << "DualSolution,";
  cout << "GetProofTreeCount,";
  cout << "GetProofTreeSuccessCount,";
  cout << "SimulationCount,";
  cout << "SimulationSuccessCount,";
  cout << "Time(sec),";
  cout << "Nodes,";
  cout << "NPS";
  cout << endl;
}

void ParallelVCFAnalyzer::Output(const size_t problem_id, const realcore::VCFAnalyzer &vcf_analyzer, const realcore::VCFSearch &vcf_search, const realcore::VCFResult &vcf_result) const
{
  const StringVector &id_list = problem_db_.at("ID");
  const StringVector &name_list = problem_db_.at("Name");
  const StringVector &board_list = problem_db_.at("Board");

  stringstream ss;

  // ID, Name, Board
  ss << id_list[problem_id] << ",";
  ss << name_list[problem_id] << ",";
  ss << board_list[problem_id] << ",";

  // Result
  if(vcf_result.solved){
    ss << "Solved";
  }else if(vcf_result.disproved){
    ss << "Disproved";
  }else{
    ss << "Terminated";
  }

  ss << ",";

  // FirstMove
  const string first_move_str = vcf_result.solved ? MoveString(vcf_result.proof_tree.GetTopNodeMove()) : "";
  ss << first_move_str << ",";

  // BestResponse
  if(vcf_search.detect_dual_solution){
    ss << vcf_result.best_response.str() << ",";
  }else{
    ss << "Unknown,";
  }

  // ProofTree
  const string proof_tree_str = vcf_result.solved ? vcf_result.proof_tree.str() : "";
  ss << proof_tree_str << ",";

  // SearchedDepth
  ss << vcf_result.search_depth << ",";
  
  // DualSolution
  if(vcf_search.detect_dual_solution){
    if(vcf_result.detect_dual_solution){
      ss << vcf_result.dual_solution_tree.str() << ",";
    }else{
      ss << "-,";
    }
  }else{
    ss << "Unknown,";
  }

  const auto& search_manager = vcf_analyzer.GetSearchManager();

  // GetProofTree
  const auto proof_tree_count = search_manager.GetProofTreeCount();
  const auto proof_tree_success_count = search_manager.GetProofTreeSuccessCount();

  ss << proof_tree_count << ",";
  ss << proof_tree_success_count << ",";
  
  // Simulation
  const auto simulation_count = search_manager.GetSimulationCount();
  const auto simulation_success_count = search_manager.GetSimulationSuccessCount();

  ss << simulation_count << ",";
  ss << simulation_success_count << ",";

  // Time(sec)
  const auto search_time = search_manager.GetSearchTime() / 1000.0;
  ss << search_time << ",";

  // 探索ノード数
  const auto node_count = search_manager.GetNode();
  ss << node_count << ",";

  // 探索速度(NPS)
  if(search_time > 0){
    ss << node_count / search_time;
  }else{
    ss << "INF";
  }

  boost::mutex::scoped_lock lock(mutex_cout_);
  cout << ss.str() << endl;
}
