output_file = flipper 

all: format check
	clang++ main.cpp -o $(output_file) -std=c++17 -Wall -Wextra -Wshadow
format:
	clang-format -i main.cpp
check:
	cppcheck main.cpp
clean:
	rm -vf stripper
