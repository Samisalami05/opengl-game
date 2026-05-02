cmake -DCMAKE_BUILD_TYPE=Debug -B build &> /dev/null
cmake --build build -j8
valgrind --leak-check=full --show-leak-kinds=all ./build/main
