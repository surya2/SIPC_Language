#!/bin/bash
declare -r ROOT_DIR=${TRAVIS_BUILD_DIR:-$(git rev-parse --show-toplevel)}
declare -r TIPC=${ROOT_DIR}/build/src/tipc
declare -r RTLIB=${ROOT_DIR}/rtlib
declare -r SCRATCH_DIR=$(mktemp -d)

if [ -z "${TIPCLANG}" ]; then
  echo error: TIPCLANG env var must be set
  exit 1
fi

curdir="$(basename `pwd`)"
if [ "${curdir}" != "system" ]; then
  echo "Test runner must be executed in .../tipc/test/system"
  exit 1
fi

numtests=0
numfailures=0

initialize_test() {
  echo -n "."
  rm -f ${SCRATCH_DIR}/*
  ((numtests++))
}


#self contained test cases for SIPC
#note: should change file extension to .sip?
# Self contained test cases
for i in sipc/*.tip
do
  base="$(basename $i .tip)"

  # test optimized program
  initialize_test
  ${TIPC} $i
  ${TIPCLANG} -w $i.bc ${RTLIB}/tip_rtlib.bc -o $base

  ./${base} &>/dev/null
  exit_code=${?}
  if [ ${exit_code} -ne 0 ]; then
    echo -n "Test failure for : " 
    echo $i
    ./${base}
    ((numfailures++))
  else 
    echo -n "optimized test passed for : "
    echo $i
  fi 
  rm $i.bc

  # test unoptimized program
  initialize_test
  ${TIPC} -do $i
  ${TIPCLANG} -w $i.bc ${RTLIB}/tip_rtlib.bc -o $base

  ./${base} &>/dev/null
  exit_code=${?}
  if [ ${exit_code} -ne 0 ]; then
    echo -n "Test failure for : " 
    echo $i
    ./${base}
    ((numfailures++))
  else 
    echo -n "unoptimized test passed for : "
    echo $i
    rm ${base}
  fi 
  rm $i.bc
done


# #type checking and pretty printing of SIPC
# for i in sipc/*.tip
# do
#   initialize_test
#   base="$(basename $i .tip)"
#   expected="$i.pppt"  # Expected pretty print output file
#   generated="${SCRATCH_DIR}/$base.pppt"  # Generated output for this test run

#   # Run the type-checking and pretty-printing
#   ${TIPC} -pp -pt $i > "$generated"

#   # TEMP CODE: if expected pretty print output does not exist, saves generated as expected
#   if [[ ! -f "$expected" ]]; then
#     echo "No expected output found for $i. Saving generated output as expected."
#     cp "$generated" "$expected"  # Save generated output as expected for next run
#   else
#     # If the expected file exists, compare it with the generated output
#     diff "$expected" "$generated" > "${SCRATCH_DIR}/$base.diff"
#     if [[ -s ${SCRATCH_DIR}/$base.diff ]]; then
#       echo -n "Test differences for : " 
#       echo $i
#       cat "${SCRATCH_DIR}/$base.diff"
#       ((numfailures++))
#     fi 
#   fi
# done


# Print out the test results
if [ ${numfailures} -eq "0" ]; then
  echo -n " all " 
  echo -n ${numtests}
  echo " tests passed"
else
  echo -n " " 
  echo -n ${numfailures}/${numtests}
  echo " tests failed"
fi

rm -r ${SCRATCH_DIR}
