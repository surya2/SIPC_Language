#!/bin/sh
set -e

# Set ROOT_DIR to GITHUB_WORKSPACE or the top-level Git directory if GITHUB_WORKSPACE is not set
ROOT_DIR=${GITHUB_WORKSPACE:-$(git rev-parse --show-toplevel)}
RTLIB_DIR="${ROOT_DIR}/rtlib"
UNIT_TEST_DIR="${ROOT_DIR}/build/test/unit"
SYSTEM_TEST_DIR="${ROOT_DIR}/test/system"

usage() {
  echo "usage: $0 [-h] [-s] [-u] [-n]" 1>&2
  echo "run the complete unit and system test suite"
  echo
  echo "-h  display help"
  echo "-s  runs system tests only"
  echo "-u  runs unit tests only"
  echo "-n runs new sipc tests only"
}

run_unit_tests() {
  find "${ROOT_DIR}" -name '*gcda' -delete
  echo "running the unit test suite"
  find "${UNIT_TEST_DIR}" -name '*_unit_tests' | xargs -n1 sh -c
  echo "unit test run complete"
}

assert_unit_test_dir() {
  if [ ! -d "${UNIT_TEST_DIR}" ]; then
    echo "${UNIT_TEST_DIR} was not found. Please make sure you build the project before running tests."
    exit 1
  fi
}

run_system_tests() {
  # Change to RTLIB_DIR directory
  cd "${RTLIB_DIR}" || exit 1
  if ! ./build.sh; then
    echo "error: could not build the runtime library"
    exit 1
  fi

  echo "running the system test suite"
  # Change to SYSTEM_TEST_DIR directory
  cd "${SYSTEM_TEST_DIR}" || exit 1
  if ! ./run.sh; then
    echo "error while running system tests"
    exit 1
  fi
  echo "system test suite complete"
}


run_newsipc_tests() {
  # Change to RTLIB_DIR directory
  cd "${RTLIB_DIR}" || exit 1
  if ! ./build.sh; then
    echo "error: could not build the runtime library"
    exit 1
  fi

  echo "running the sipc test suite"
  # Change to SYSTEM_TEST_DIR directory
  cd "${SYSTEM_TEST_DIR}" || exit 1
  if ! ./runsipc.sh; then
    echo "error while running sipc tests"
    exit 1
  fi
  echo "sipc test suite complete"
}

run_system_tests="true"
run_unit_tests="true"
run_newsipc_tests=""
while getopts ":hsun" opt; do
  case "${opt}" in
    h)
      usage
      exit 0
      ;;
    s)
      echo "Preparing to run only the system tests suite"
      run_unit_tests=""
      ;;
    u)
      echo "Preparing to run only the unit tests suite"
      run_system_tests=""
      ;;
    n)
      echo "Preparing to run only the new sipc test suite"
      run_unit_tests=""
      run_system_tests=""
      run_newsipc_tests="true"
      ;;
    *)
      echo "$0 illegal option"
      usage
      exit 1
      ;;
  esac
done
shift $((OPTIND - 1))

if [ -n "${run_unit_tests}" ]; then
  assert_unit_test_dir
  run_unit_tests
fi

if [ -n "${run_system_tests}" ]; then
  run_system_tests
fi

if [ -n "${run_newsipc_tests}" ]; then
  run_newsipc_tests
fi
