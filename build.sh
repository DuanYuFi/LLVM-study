cmake --build build
clang++-13 -flegacy-pass-manager -Xclang -load -Xclang ~/Compilers/Study/build/libMyPass.so $1