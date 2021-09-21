all:harness

harness:harness.cpp
	g++ -o harness elf_parser.cpp harness.cpp  -std=gnu++11