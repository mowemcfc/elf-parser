all:parser

parser: elf_parser.cpp
	g++ -o parser elf_parser.cpp -lboost_program_options -std=gnu++11

clean:
	rm -f parser