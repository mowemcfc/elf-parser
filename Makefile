all:parser

parser: elf_parser.cpp
	g++ -g -o parser elf_parser.cpp -lboost_program_options -std=gnu++14 

clean:
	rm -f parser