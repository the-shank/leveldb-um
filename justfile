default:
  just --list

interact:
  ./build.sh
  ./wrapper_build.sh
  rm -r -f ../test
  ./wrapper ../test

tmptest:
  just interact < input
