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


// TODO: use boost?? formatting instead of printf for magic hex
bool Parser::print_elf_header() {
    if ( !Parser::check_ELF64_magic(p_elf_header->e_ident, parser_verbose) ) {
        cout << "WARN: file is not a valid ELF (magic is malformed)" << endl;
    }

    cout << "ELF Magic: " << get_e_ident() << endl;
    cout << "ELF Type: " << get_e_type() << endl;
    cout << "Machine: " << get_e_machine() << endl;
    cout << "Version: " << get_e_version() << endl;
    cout << "Entry point: " << get_e_entry() << endl; //TODO: check for PIE binary and notify of offset/fixed address. c.f DT_FLAGS_1 and e_type==ET_DYN
    cout << "Offset to program headers: " << get_e_phoff() << endl;
    cout << "Offset to section headers: " << get_e_shoff() << endl;
    cout << "Processor-specific Flags: " << get_e_flags() << endl;

    return true;
}


const char* Parser::get_e_flags() {
    static char ret_string[16];
    snprintf(ret_string, 16, "0x%x", p_elf_header->e_flags);

    return ret_string;
}


const char* Parser::get_e_shoff() {
    static char ret_string[16];
    snprintf(ret_string, 16, "%lu bytes", p_elf_header->e_shoff);

    return ret_string;
}


const char* Parser::get_e_phoff() {
    static char ret_string[16];
    snprintf(ret_string, 16, "%lu bytes", p_elf_header->e_phoff);

    return ret_string;
}


const char* Parser::get_e_entry() {
    static char ret_string[24];
    snprintf(ret_string, 24, "0x%2.2lx", p_elf_header->e_entry);
    
    return ret_string;
}


const char* Parser::get_e_version() {
    if ( p_elf_header->e_version == EV_NONE ) {
        return "Invalid version";
    }

    static char ret_string[8];
    snprintf(ret_string, 8, "0x%u", p_elf_header->e_version);

    return ret_string;
}


const char* Parser::get_e_ident() {
    static char ret_string[64];

    uint8_t cur = 0;
    for (int i = 0; i < EI_NIDENT; i++)
        cur = snprintf(ret_string + (cur*i), 64-cur, "%02x ", p_elf_header->e_ident[i]);

    return ret_string;
}


// TODO: There are a lot of machine types missing here - update w/ more complete list at later date
const char* Parser::get_e_machine() {
    switch (p_elf_header->e_machine) {
        case EM_NONE:		return "None";
        case EM_AARCH64:	return "AArch64";
        case EM_M32:		return "WE32100";
        case EM_SPARC:		return "Sparc";
        case EM_SPU:		return "SPU";
        case EM_386:		return "Intel 80386";
        case EM_68K:		return "MC68000";
        case EM_88K:		return "MC88000";
        case EM_IAMCU:		return "Intel MCU";
        case EM_860:		return "Intel 80860";
        case EM_MIPS:		return "MIPS R3000";
        case EM_S370:		return "IBM System/370";
        case EM_MIPS_RS3_LE:	return "MIPS R4000 big-endian";
        case EM_PARISC:		return "HPPA";
        case EM_SPARC32PLUS:	return "Sparc v8+" ;
        case EM_960:		return "Intel 90860";
        case EM_PPC:		return "PowerPC";
        case EM_PPC64:		return "PowerPC64";
        case EM_FR20:		return "Fujitsu FR20";
        case EM_FT32:		return "FTDI FT32";
        case EM_RH32:		return "TRW RH32";
        case EM_ARM:		return "ARM";
        case EM_SH:			return "Renesas / SuperH SH";
        case EM_SPARCV9:	return "Sparc v9";
        case EM_TRICORE:	return "Siemens Tricore";
        case EM_ARC:		return "ARC";
        case EM_ARC_COMPACT:	return "ARCompact";
        case EM_H8_300:		return "Renesas H8/300";
        case EM_H8_300H:	return "Renesas H8/300H";
        case EM_H8S:		return "Renesas H8S";
        case EM_H8_500:		return "Renesas H8/500";
        case EM_IA_64:		return "Intel IA-64";
        case EM_MIPS_X:		return "Stanford MIPS-X";
        case EM_COLDFIRE:	return "Motorola Coldfire";
        case EM_ALPHA:		return "Alpha";
        case EM_D10V:		return "d10v";
        case EM_D30V:		return "d30v";
        case EM_M32R:		return "Renesas M32R (formerly Mitsubishi M32r)";
        case EM_V800:		return "Renesas V850 (using RH850 ABI)";
        case EM_V850:		return "Renesas V850";
        case EM_MN10300:	return "mn10300";
        case EM_MN10200:	return "mn10200";
        case EM_MOXIE:		return "Moxie";
        case EM_FR30:		return "Fujitsu FR30";
        case EM_PJ:			return "picoJava";
        case EM_MMA:		return "Fujitsu Multimedia Accelerator";
        case EM_PCP:		return "Siemens PCP";
        case EM_NCPU:		return "Sony nCPU embedded RISC processor";
        case EM_NDR1:		return "Denso NDR1 microprocesspr";
        case EM_STARCORE:	return "Motorola Star*Core processor";
        case EM_ME16:		return "Toyota ME16 processor";
        case EM_ST100:		return "STMicroelectronics ST100 processor";
        case EM_TINYJ:		return "Advanced Logic Corp. TinyJ embedded processor";
        case EM_PDSP:		return "Sony DSP processor";
        case EM_PDP10:		return "Digital Equipment Corp. PDP-10";
        case EM_PDP11:		return "Digital Equipment Corp. PDP-11";
        case EM_FX66:		return "Siemens FX66 microcontroller";
        case EM_ST9PLUS:	return "STMicroelectronics ST9+ 8/16 bit microcontroller";
        case EM_ST7:		return "STMicroelectronics ST7 8-bit microcontroller";
        case EM_68HC16:		return "Motorola MC68HC16 Microcontroller";
        case EM_68HC12:		return "Motorola MC68HC12 Microcontroller";
        case EM_68HC11:		return "Motorola MC68HC11 Microcontroller";
        case EM_68HC08:		return "Motorola MC68HC08 Microcontroller";
        case EM_68HC05:		return "Motorola MC68HC05 Microcontroller";
        case EM_SVX:		return "Silicon Graphics SVx";
        case EM_ST19:		return "STMicroelectronics ST19 8-bit microcontroller";
        case EM_VAX:		return "Digital VAX";
        case EM_VISIUM:		return "CDS VISIUMcore processor";
        case EM_AVR:		return "Atmel AVR 8-bit microcontroller";
        case EM_CRIS:		return "Axis Communications 32-bit embedded processor";
        case EM_JAVELIN:	return "Infineon Technologies 32-bit embedded cpu";
        case EM_FIREPATH:	return "Element 14 64-bit DSP processor";
        case EM_ZSP:		return "LSI Logic's 16-bit DSP processor";
        case EM_MMIX:		return "Donald Knuth's educational 64-bit processor";
        case EM_HUANY:		return "Harvard Universitys's machine-independent object format";
        case EM_PRISM:		return "Vitesse Prism";
        case EM_X86_64:		return "Advanced Micro Devices X86-64";
        case EM_S390:		return "IBM S/390";
        case EM_CRX:		return "National Semiconductor CRX microprocessor";
        case EM_IP2K:		return "Ubicom IP2xxx 8-bit microcontrollers";
        case EM_XTENSA:		return "Tensilica Xtensa Processor";
        case EM_VIDEOCORE:	return "Alphamosaic VideoCore processor";
        case EM_TMM_GPP:	return "Thompson Multimedia General Purpose Processor";
        case EM_NS32K:		return "National Semiconductor 32000 series";
        case EM_TPC:		return "Tenor Network TPC processor";
        case EM_ST200:		return "STMicroelectronics ST200 microcontroller";
        case EM_MAX:		return "MAX Processor";
        case EM_CR:			return "National Semiconductor CompactRISC";
        case EM_F2MC16:		return "Fujitsu F2MC16";
        case EM_MSP430:		return "Texas Instruments msp430 microcontroller";
        case EM_LATTICEMICO32:	return "Lattice Mico32";
        case EM_M32C:       return "Renesas M32c";
        case EM_BLACKFIN:	return "Analog Devices Blackfin";
        case EM_SE_C33:		return "S1C33 Family of Seiko Epson processors";
        case EM_SEP:		return "Sharp embedded microprocessor";
        case EM_ARCA:		return "Arca RISC microprocessor";
        case EM_UNICORE:	return "Unicore";
        case EM_EXCESS:		return "eXcess 16/32/64-bit configurable embedded CPU";
        case EM_DXP:		return "Icera Semiconductor Inc. Deep Execution Processor";
        case EM_ALTERA_NIOS2:	return "Altera Nios II";
        case EM_M16C:		return "Renesas M16C series microprocessors";
        case EM_DSPIC30F:	return "Microchip Technology dsPIC30F Digital Signal Controller";
        case EM_CE:			return "Freescale Communication Engine RISC core";
        case EM_TSK3000:	return "Altium TSK3000 core";
        case EM_RS08:		return "Freescale RS08 embedded processor";
        case EM_ECOG2:		return "Cyan Technology eCOG2 microprocessor";
        case EM_DSP24:		return "New Japan Radio (NJR) 24-bit DSP Processor";
        case EM_VIDEOCORE3:	return "Broadcom VideoCore III processor";
        case EM_SE_C17:		return "Seiko Epson C17 family";
        case EM_TI_C6000:	return "Texas Instruments TMS320C6000 DSP family";
        case EM_TI_C2000:	return "Texas Instruments TMS320C2000 DSP family";
        case EM_TI_C5500:	return "Texas Instruments TMS320C55x DSP family";
        case EM_MMDSP_PLUS:	return "STMicroelectronics 64bit VLIW Data Signal Processor";
        case EM_CYPRESS_M8C:    return "Cypress M8C microprocessor";
        case EM_R32C:		return "Renesas R32C series microprocessors";
        case EM_TRIMEDIA:	return "NXP Semiconductors TriMedia architecture family";
        case EM_QDSP6:		return "QUALCOMM DSP6 Processor";
        case EM_8051:		return "Intel 8051 and variants";
        case EM_STXP7X:		return "STMicroelectronics STxP7x family";
        case EM_NDS32:		return "Andes Technology compact code size embedded RISC processor family";
        case EM_ECOG1X:		return "Cyan Technology eCOG1X family";
        case EM_MAXQ30:		return "Dallas Semiconductor MAXQ30 Core microcontrollers";
        case EM_XIMO16:		return "New Japan Radio (NJR) 16-bit DSP Processor";
        case EM_MANIK:		return "M2000 Reconfigurable RISC Microprocessor";
        case EM_CRAYNV2:	return "Cray Inc. NV2 vector architecture";
        case EM_RL78:		return "Renesas RL78";
        case EM_RX:			return "Renesas RX";
        case EM_METAG:		return "Imagination Technologies Meta processor architecture";
        case EM_MCST_ELBRUS:	return "MCST Elbrus general purpose hardware architecture";
        case EM_ECOG16:		return "Cyan Technology eCOG16 family";
        case EM_ETPU:		return "Freescale Extended Time Processing Unit";
        case EM_SLE9X:		return "Infineon Technologies SLE9X core";
        case EM_AVR32:		return "Atmel Corporation 32-bit microprocessor family";
        case EM_STM8:		return "STMicroeletronics STM8 8-bit microcontroller";
        case EM_TILE64:		return "Tilera TILE64 multicore architecture family";
        case EM_TILEPRO:	return "Tilera TILEPro multicore architecture family";
        case EM_TILEGX:		return "Tilera TILE-Gx multicore architecture family";
        case EM_CUDA:		return "NVIDIA CUDA architecture";
        case EM_XGATE:		return "Motorola XGATE embedded processor";

        default:
            static char ret_string[48];
            snprintf(ret_string, 48, "Unknown machine type: %u", p_elf_header->e_machine);
            return ret_string;
    }
}


const char* Parser::get_e_type() {
    switch (p_elf_header->e_type) {
        case ET_NONE: return  "No file type";
        case ET_REL: return "Relocatable file";
        case ET_EXEC: return "Executable file";
        case ET_DYN: return "Shared object file";
        case ET_CORE: return "Core file";

        default:
            static char ret_string[48];

            if ( (p_elf_header->e_type >= ET_LOPROC) && (p_elf_header->e_type <= ET_HIPROC) ) {
                snprintf(ret_string, 48, "Processor specific: %02x", p_elf_header->e_type);        
            } else if ( (p_elf_header->e_type >= ET_LOOS) && (p_elf_header->e_type <= ET_HIOS) ) {
                snprintf(ret_string, 48, "Operating System specific: %02x", p_elf_header->e_type);
            } else {
                snprintf(ret_string, 48, "Invalid e_type: %u", p_elf_header->e_type);
                if(parser_verbose) {
                    cout << "WARN: ELF has invalid header E_TYPE value" << endl;
                }
            }
            return ret_string;
    }
}


const char* Parser::get_ei_class() {
    if ( p_elf_header->e_ident[EI_CLASS] == ELFCLASS64 ) {
        p_ei_class = ELFCLASS64;
        return "64 Bit ELF";
    } else if ( p_elf_header->e_ident[EI_CLASS] == ELFCLASS32 ) {
        p_ei_class = ELFCLASS32;
        return "32 Bit ELF";
    } else {
        p_ei_class = ELFCLASSNONE;
        if (parser_verbose) {
            cout << "WARN: e_ident ei_class is of invalid type" << endl;
        }
        return "Invalid type";
    }
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
