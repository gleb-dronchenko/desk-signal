#!/bin/sh
set -eu

root=$(CDPATH= cd -- "$(dirname "$0")/../.." && pwd)

compile() {
  name=$1
  shift
  clang++ -Wall -Wextra -Werror -I "$root/esp32/main" -o "/tmp/$name" \
    "$root/esp32/host/${name}.cpp" \
    "$@"
}

compile reading_test "$root/esp32/main/reading.cpp"
compile state_body_test "$root/esp32/main/state_body.cpp"

/tmp/reading_test
/tmp/state_body_test
