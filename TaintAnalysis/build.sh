SCRIPT_PATH="${BASH_SOURCE:-$0}"
ABS_DIRECTORY="$(dirname "${SCRIPT_PATH}")"

cmake --build build
clang++-13 -flegacy-pass-manager -Xclang -load -Xclang ${ABS_DIRECTORY}/build/libTaintPass.so $1