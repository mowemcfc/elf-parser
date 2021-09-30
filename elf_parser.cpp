// MIT License

// Copyright (c) 2021 mowemcfc

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

// TODO: implement verbosity options in constructor

#include "elf_parser.hpp"
using namespace elf_parser;
using namespace std;
using elf_parser::Parser;

void Parser::setup(std::string prog_path) {
    Parser::p_file_path = prog_path;
    return;
}


void Parser::cleanup() {
    munmap(p_prog_mmap, p_mmap_size);
    return;
}


Elf64_Ehdr* Parser::read_elf_header() {
    p_elf_header = (Elf64_Ehdr*) p_prog_mmap;    
    return p_elf_header;
}



bool Parser::check_ELF64_magic(unsigned char p_e_ident[16], bool parser_verbose) {
    std::string e_ident(reinterpret_cast<char const*>(p_e_ident), 16);

    if ( e_ident.compare(0, 4, ELFMAG) != 0 ) {
        if(parser_verbose) {
            cout << "file does not contain a valid ELF header" << endl;
        }
        return false;
    }

    return true;
}

// TODO: use boost formatting instead of printf for magic hex
bool Parser::print_elf_header() {
    if ( !Parser::check_ELF64_magic(p_elf_header->e_ident, parser_verbose) ) {
        cout << "file is not a valid ELF (or magic is malformed)" << endl;
    }

    cout << "ELF Magic: ";
    for (int i = 0; i < EI_NIDENT; i++) {
        printf("%02x ", p_elf_header->e_ident[i]);
    }
    cout << endl;

    return true;
}


uint8_t Parser::get_ei_class() {
    if ( p_elf_header->e_ident[EI_CLASS] == ELFCLASS64 ) {
        p_ei_class = ELFCLASS64;
        return ELFCLASS64;
    } else if ( p_elf_header->e_ident[EI_CLASS] == ELFCLASS32 ) {
        p_ei_class = ELFCLASS32;
        return ELFCLASS32;
    } else {
        p_ei_class = ELFCLASSNONE;
        if (parser_verbose) {
            cout << "e_ident ei_class is of invalid type" << endl;
        }
    }

    return p_ei_class;
}


int8_t Parser::load_mmap(std::string file_path) {
    int fd, i;
    struct stat st;

    if ((fd = open(file_path.c_str(), O_RDONLY)) < 0) {
        cout << "ERROR: Could not open file " << file_path << endl;
        close(fd);
        return -1;
    }

    if (fstat(fd, &st) < 0) {
        cout << "ERROR: Could not fstat file " << file_path << endl;
        close(fd);
        return -1;
    }

    p_mmap_size = (size_t) st.st_size;
    p_prog_mmap = static_cast<uint8_t*>(mmap(NULL, p_mmap_size, PROT_READ, MAP_PRIVATE, fd, 0));

    if ((unsigned char*) p_prog_mmap == MAP_FAILED) {
        cout << "ERROR: Failed to initialize memory map for " << file_path << endl;
        close(fd);
        return -1;
    }


    return 0;
}
