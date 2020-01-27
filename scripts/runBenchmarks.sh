#!/usr/bin/env bash

cd ../build/ || exit
realPath="../../Benchmarks/EquivalenceCheckingEvaluation/revlib/"
decomposedPath="../../Benchmarks/EquivalenceCheckingEvaluation/qasm/decomposed/"
transpiledPath="../../Benchmarks/EquivalenceCheckingEvaluation/qasm/transpiled/"

timeout=10m

export LANG="en_GB.UTF-8"
export LC_ALL="en_GB.UTF-8"

for f in ${realPath}*.real
do
  filename=$(basename -- "$f")
  filename="${filename%.*}"
  >&2 echo $filename

  {
    timeout $timeout ./QCEC_app $f ${decomposedPath}${filename}_decomposed.qasm Reference --print_csv
    >&2 echo Reference
    timeout $timeout ./QCEC_app $f ${decomposedPath}${filename}_decomposed.qasm Proportional --print_csv
    >&2 echo Proportional
    timeout $timeout ./QCEC_app $f ${decomposedPath}${filename}_decomposed.qasm Lookahead --print_csv
    >&2 echo Lookahead

    timeout $timeout ./QCEC_app ${decomposedPath}${filename}_decomposed.qasm ${transpiledPath}${filename}_transpiled.qasm Reference --print_csv
    >&2 echo Reference
    timeout $timeout ./QCEC_app ${decomposedPath}${filename}_decomposed.qasm ${transpiledPath}${filename}_transpiled.qasm Proportional --print_csv
    >&2 echo Proportional
    timeout $timeout ./QCEC_app ${decomposedPath}${filename}_decomposed.qasm ${transpiledPath}${filename}_transpiled.qasm Lookahead --print_csv
    >&2 echo Lookahead
  } >> results.csv
done
