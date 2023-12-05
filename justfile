default:
  just --list

tmptest:
  ./build.sh
  ./wrapper_build.sh
  rm -r -f ../test
  ./wrapper ../test < input
