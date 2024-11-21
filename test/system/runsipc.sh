#!/bin/bash

declare -r ROOT_DIR=${TRAVIS_BUILD_DIR:-$(git rev-parse --show-toplevel)}
declare -r TIPC=${ROOT_DIR}/build/src/tipc
declare -r RTLIB=${ROOT_DIR}/rtlib
declare -r SCRATCH_DIR=$(mktemp -d)

if [ -z "${TIPCLANG}" ]; then
  echo "Error: TIPCLANG environment variable must be set."
  exit 1
fi

curdir="$(basename $(pwd))"
if [ "${curdir}" != "system" ]; then
  echo "Test runner must be executed in .../tipc/test/system"
  exit 1
fi

numtests=0
numfailures=0

initialize_test() {
  rm -f ${SCRATCH_DIR}/*
  ((numtests++))
}

echo "Running tests..."

for i in sipc/*.tip; do
  base="$(basename $i .tip)"
  
  initialize_test
  echo
  echo "Starting test: $i"

  ${TIPC} -do $i
  ${TIPCLANG} -w $i.bc ${RTLIB}/tip_rtlib.bc -o $base

  ./${base} &>/dev/null
  exit_code=${?}
  
  if [ ${exit_code} -ne 0 ]; then
    echo "FAIL: $i"
    ./${base}  # Run again to show output
    ((numfailures++))
  else
    echo "PASS: $i"
    rm ${base}
  fi
  
  rm $i.bc
done

if [ ${numfailures} -eq 0 ]; then
  echo "All ${numtests} tests passed!"
else
  echo "${numfailures}/${numtests} tests failed."
fi

rm -r ${SCRATCH_DIR}
