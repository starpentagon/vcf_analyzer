#ifndef PARALELLE_VCF_ANALYZER_H
#define PARALELLE_VCF_ANALYZER_H

#include <deque>
#include <boost/program_options.hpp>
#include <boost/thread.hpp>

#include "VCFAnalyzer.h"
#include "VCFTranspositionTable.h"
#include "CSVReader.h"

class ParallelVCFAnalyzer
{
public:
  ParallelVCFAnalyzer(const std::string &problem_db_path);

  //! @brief 解図を並行実行する
  //! @param thread_num 同時実行数
  //! @param vcf_search 解図設定
  void Run(const size_t thread_num, const boost::program_options::variables_map &arg_map);

  //! @brief 解図結果のHeaderを出力する
  static void OutputHeader();
private:
  //! @param thread_id thread id
  //! @param vcf_search 解図設定
  void VCFAnalyze(const size_t thread_id, const boost::program_options::variables_map &arg_map);

  //! @brief 問題indexを取得する
  //! @retval true 問題indexを取得, false 問題indexのリストが空
  const bool GetProblemIndex(size_t * const problem_index);

  //! @brief 解図結果を出力する
  void Output(const size_t problem_id, const realcore::VCFAnalyzer &vcf_analyzer, const realcore::VCFSearch &vcf_search, const realcore::VCFResult &vcf_result) const;
  
  std::deque<size_t> problem_index_list_;    //!< 問題indexのリスト
  std::map<std::string, realcore::StringVector> problem_db_;   //!< VCF問題DB
  std::vector< std::shared_ptr<realcore::VCFTable> > vcf_table_list_;   //!< 置換表(thread分確保する)

  mutable boost::mutex mutex_cout_;   //!< 標準出力のmutex
  mutable boost::mutex mutex_cerr_;   //!< 標準エラー出力のmutex
  mutable boost::mutex mutex_problem_index_list_;   //!< problem_index_list_のmutex
};

#endif    // PARALELLE_VCF_ANALYZER_H
