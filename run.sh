cmake -DCMAKE_BUILD_TYPE=Debug -B build &> /dev/null
cmake --build build -j8
valgrind ./build/main
