cmake -DCMAKE_BUILD_TYPE=Debug -B build &> /dev/null
cmake --build build -j8
if [ -z "$1" ]
then
	./build/main
else
	if [ $1 == 'valgrind' ]
	then
		valgrind --leak-check=full --show-leak-kinds=all ./build/main
	else 
		./build/main
	fi
fi



