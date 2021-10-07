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

#ifndef H_ELF_PARSE_
#define H_ELF_PARSE_

#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <elf.h>
#include <fcntl.h>

#define PARSER_VERBOSE 1
#define PARSER_NONVERBOSE 0

namespace elf_parser {


    class Parser {
        public:
            // Function signatures
            void setup(std::string elf_prog_path);
            void cleanup();
            std::shared_ptr<void> load_mmap(std::string file_path);
            static bool check_ELF64_magic(unsigned char p_e_ident[16], bool parser_verbose);
            bool print_elf_header();

            // Getters
            const char* get_e_ident();
            const char* get_e_type();
            const char* get_e_machine();
            const char* get_e_version();
            const char* get_e_entry();
            const char* get_e_phoff();
            const char* get_e_shoff();
            const char* get_e_flags();
            const char* get_e_phentsize();
            const char* get_e_phnum();
            const char* get_total_phsize();
            const char* get_e_shentsize();
            const char* get_e_shnum();
            const char* get_total_shsize();
            const char* get_e_shstrndx();
            const char* get_ei_class();

            // Constructors
            Parser(std::string file_path) {
                setup(file_path);
                parser_verbose = 0;
            }
            Parser(std::string file_path, int verbosity) {
                setup(file_path);
                parser_verbose = verbosity;
            }

            // Class variables
            uint8_t parser_verbose;

        private:
            std::shared_ptr<Elf64_Shdr*> read_section_headers(std::shared_ptr<void> p_prog_mmap);
            std::shared_ptr<Elf64_Ehdr> read_elf_header(std::shared_ptr<void> p_prog_mmap);

            // Private variables
            uint8_t p_ei_class; // ELFCLASS64: 2 - ELFCLASS32: 1
            std::shared_ptr<Elf64_Ehdr> p_elf_header;
            std::shared_ptr<Elf64_Shdr*> p_section_headers;
            std::string p_file_path; 
    };

}

#endif

