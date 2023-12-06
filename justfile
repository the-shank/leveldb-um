default:
  just --list

build:
  ./build.sh
  ./wrapper_build.sh

bench:
  just build
  ./build/db_bench

interact:
  just build
  rm -r -f ../test
  ./wrapper ../test

tmptest:
  just interact < input
