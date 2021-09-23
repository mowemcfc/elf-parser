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


void Parser::setup(std::string prog_path) {
    Parser::p_file_path = prog_path;
    return;
}


void Parser::cleanup() {
    munmap(p_prog_mmap, p_mmap_size);
    return;
}

void Parser::read_eident() {

}


Elf64_Ehdr* Parser::read_elf_header() {
    p_elf_header = (Elf64_Ehdr*) p_prog_mmap;    
    return p_elf_header;
}


uint8_t Parser::check_ELF64_magic() {
    std::string p_elf_magic;
    p_elf_magic.append((const char*)p_elf_header->e_ident, 4);

    if (p_elf_magic.compare(0, 4, ELFMAG) == 0) {
        if(parser_verbose) {
            cout << "file contains valid ELF header" << endl;
        }
        return 0;
    }

    return -1;

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
