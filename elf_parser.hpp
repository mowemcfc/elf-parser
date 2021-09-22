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

#ifndef H_ELF_PARSE_
#define H_ELF_PARSE_

#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <elf.h>
#include <fcntl.h>

namespace elf_parser {


    struct e_ident {
        unsigned char ei_mag[4];
        unsigned char ei_class;
        unsigned char ei_data;
        unsigned char ei_version;
        unsigned char ei_pad;
        unsigned char ei_nident;
    };


    class Elf {
        public:
            int8_t load_mmap();
            void setup(std::string elf_prog_path);
        private:
            std::string prog_path; 
            uint8_t* prog_mmap;
    };

}

#endif

