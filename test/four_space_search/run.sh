#!/bin/bash
# ./build/の"カレントディレクトリ名_test" 形式のテスト実行ファイルを起動する
dir=`pwd`
prog_name=build/four_space_search_test

if [ ! -e ${prog_name} ]
then
  echo ${prog_name} is not found.　1>&2
  exit 1
fi

./${prog_name}
exit $?
