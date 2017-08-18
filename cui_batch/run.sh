#!/bin/bash

./build.sh

result_file=result.csv

./build/vcf_analyzer_batch --problem-db VCF_u.csv --time 14400 --thread 4 > ${result_file}
./sort_and_add_header.sh ${result_file}

./summrize_result.py ${result_file}

