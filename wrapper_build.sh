#!/bin/bash

set -eu
set -o pipefail

g++ wrapper.c -L ./build/ -I ./include/ -lleveldb -lsnappy -lpthread -o wrapper
