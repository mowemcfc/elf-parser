// MIT License

// Copyright (c) 2018 finixbit

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#include "elf_parser.hpp"
using namespace elf_parser;


void Elf::setup(std::string prog_path) {
    elf_prog_path = prog_path;
    return;
}

int8_t Elf::load_mmap() {
    int fd, i;
    struct stat st;

    if ((fd = open(elf_prog_path.c_str(), O_RDONLY, 0) < 0)) {
        printf("ERROR: Could not open file %s\n", elf_prog_path.c_str());
        return -1;
    }

    if (fstat(fd, &st) < 0) {
        printf("ERROR: Could not fstat file %s\n", elf_prog_path.c_str());
        return -1;
    }

    return 0;
}