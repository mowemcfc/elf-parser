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

#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include "elf_parser.hpp"

using namespace elf_parser;
using namespace std;
using elf_parser::Parser;
using boost::format;
namespace po = boost::program_options;


void* Elf_Mmap::get_mmap() {
    return prog_mmap;
}


void Parser::setup(std::string prog_path) {
    p_prog_mmap = make_unique<Elf_Mmap>(prog_path);
    p_prog_mmap->set_elf_header(p_prog_mmap->get_mmap());
    p_prog_mmap->set_section_headers(p_prog_mmap->get_mmap());
    return;
}


Elf64_Shdr* Elf_Mmap::get_section_headers(void) {
    Elf64_Shdr* sectheader = (Elf64_Shdr*) ((char*) prog_mmap + p_elf_header->e_shoff);
    return sectheader;
}


Elf64_Ehdr* Elf_Mmap::get_elf_header(void) {
    Elf64_Ehdr* elfheader = (Elf64_Ehdr*) prog_mmap;
    return elfheader;
}


void Elf_Mmap::set_section_headers(void* p_prog_mmap) {
    p_section_headers = (Elf64_Shdr*) ((char*) p_prog_mmap + p_elf_header->e_shoff);
}


void Elf_Mmap::set_elf_header(void* p_prog_mmap) {
    p_elf_header = (Elf64_Ehdr*) p_prog_mmap;
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


bool Parser::print_elf_header() {

    Elf64_Ehdr* p_elf_header = p_prog_mmap->get_elf_header();

    cout << format("ELF Magic:                          %s") % get_e_ident() << endl;
    cout << format("ELF Type:                           %s") % get_e_type() << endl;
    cout << format("Machine:                            %s") % get_e_machine() << endl;
    cout << format("Version:                            %s") % get_e_version() << endl;
    //TODO: check for PIE binary and notify of offset/fixed address. c.f DT_FLAGS_1 and e_type==ET_DYN
    cout << format("Entry point:                        %s") % get_e_entry() << endl; 
    cout << format("Offset to program headers:          %s") % get_e_phoff() << endl;
    cout << format("Offset to section headers:          %s") % get_e_shoff() << endl;
    cout << format("Processor-specific Flags:           %s") % get_e_flags() << endl;
    cout << format("Program header Size:                %s") % get_e_phentsize() << endl;
    cout << format("Number of program headers:          %s") % get_e_phnum() << endl;
    if ( parser_verbose ) {
        cout << format("Space (total) of program headers:   %s") % get_total_phsize() << endl;
    }
    cout << format("Section header size:                %s") % get_e_shentsize() << endl;
    cout << format("Number of section headers:          %s") % get_e_shnum() << endl;
    if ( parser_verbose ) {
        cout << format("Space (total) of section headers:   %s") % get_total_shsize() << endl;
    }
    cout << format("Section name string table index:    %s") % get_e_shstrndx() << endl;
    cout << "\n";

    return true;
}


bool Parser::print_section_headers() {
    Elf64_Ehdr* p_elf_header = p_prog_mmap->get_elf_header();
    for ( int i = 0; i < p_elf_header->e_shnum; i++ ) {
        cout << format("\nSection Header %s") % i << endl;
        cout << format("    Name:               %s") % get_sh_name(i) << endl; 
        cout << format("    Type:               %s") % get_sh_type(i) << endl;
        //cout << format("    Flags:              %s") % get_sh_flags(i) << endl;
        cout << format("    First Byte Address: %s") % get_sh_addr(i) << endl;
        cout << format("    Section Entry Size: %s bytes") % get_sh_entsize(i) << endl;
        cout << format("    Section Offset:     %s") % get_sh_offset(i) << endl;
        cout << format("    Size:               %s bytes") % get_sh_size(i) << endl;
    }
    return true;
}


const char* Parser::get_sh_size(int sh_idx) {
    Elf64_Shdr* p_section_headers = p_prog_mmap->get_section_headers();
    static char ret_string[32];
    snprintf(ret_string, 32, "%ld", p_section_headers[sh_idx].sh_size);

    return ret_string;
}


const char* Parser::get_sh_offset(int sh_idx) {
    Elf64_Shdr* p_section_headers = p_prog_mmap->get_section_headers();
    static char ret_string[32];
    snprintf(ret_string, 32, "0x%lx", p_section_headers[sh_idx].sh_offset);

    return ret_string;
}


const char* Parser::get_sh_addr(int sh_idx) {
    Elf64_Shdr* p_section_headers = p_prog_mmap->get_section_headers();
    static char ret_string[32];
    snprintf(ret_string, 32, "0x%lx", p_section_headers[sh_idx].sh_addr);

    return ret_string;
}

const char* Parser::get_sh_type(int sh_idx) {
    Elf64_Shdr* p_section_headers = p_prog_mmap->get_section_headers();
    static char ret_string[32];
    snprintf(ret_string, 32, "%d", p_section_headers[sh_idx].sh_type);

    switch (p_section_headers[sh_idx].sh_type) {
        case SHT_NULL:           return "Null Section";
        case SHT_PROGBITS:       return "Application Specific";
        case SHT_SYMTAB:         return "Symbol Table";
        case SHT_STRTAB:         return "String Table";
        case SHT_RELA:           return "Relocation Table w/ Addends";
        case SHT_HASH:           return "Symbol Hash Table";
        case SHT_DYNAMIC:        return "Dynamic Section";
        case SHT_NOTE:           return "Note Section";
        case SHT_NOBITS:         return "NOBITS (Empty Section)";
        case SHT_REL:            return "Relocation Table (no Addends)";
        case SHT_SHLIB:          return "Reserved Section (no semantics)";
        case SHT_DYNSYM:         return "Dynamic Symbol Table";
        case SHT_INIT_ARRAY:     return "Init Function Table";
        case SHT_FINI_ARRAY:     return "Fini Function Table";
        case SHT_PREINIT_ARRAY:  return "Pre-Init Function Table";
        case SHT_GROUP:          return "Section Group";
        case SHT_SYMTAB_SHNDX:   return "XINDEX Symbol Table";
        default:                 break;
    }

    if ((p_section_headers[sh_idx].sh_type >= SHT_LOOS) && (p_section_headers[sh_idx].sh_type <= SHT_HIOS)) {
        snprintf(ret_string, 64, "OS specific: 0x%x", p_section_headers[sh_idx].sh_type);        
    } else if ((p_section_headers[sh_idx].sh_type >= SHT_LOPROC) && (p_section_headers[sh_idx].sh_type <= SHT_HIPROC)) {
        snprintf(ret_string, 64, "Processor specific: 0x%x", p_section_headers[sh_idx].sh_type);        
    } else if ((p_section_headers[sh_idx].sh_type >= SHT_LOUSER) && (p_section_headers[sh_idx].sh_type <= SHT_HIUSER)) {
        snprintf(ret_string, 64, "Application specific: 0x%x", p_section_headers[sh_idx].sh_type);        
    } else {
        snprintf(ret_string, 64, "Invalid Type: 0x%x", p_section_headers[sh_idx].sh_type);        
    }

    return ret_string;
}


const char* Parser::get_sh_entsize(int sh_idx) {
    Elf64_Shdr* p_section_headers = p_prog_mmap->get_section_headers();
    static char ret_string[32];
    snprintf(ret_string, 32, "%ld", p_section_headers[sh_idx].sh_entsize);

    return ret_string;
}


const char* Parser::get_sh_name(int sh_idx) {
    Elf64_Shdr* p_section_headers = p_prog_mmap->get_section_headers();
    Elf64_Ehdr* p_elf_header = p_prog_mmap->get_elf_header();
    Elf64_Shdr* strtab = &p_section_headers[p_elf_header->e_shstrndx];
    static char ret_string[32];

    snprintf(ret_string, 32, "%s", (char*) p_prog_mmap->get_mmap() + strtab->sh_offset + p_section_headers[sh_idx].sh_name);

    return ret_string;
}


// This member holds the section header table index of the entry associated with the section name string table. 
const char* Parser::get_e_shstrndx() {
    Elf64_Ehdr* p_elf_header = p_prog_mmap->get_elf_header();
    Elf64_Shdr* p_section_headers = p_prog_mmap->get_section_headers();
    static char ret_string[32];

    if ( p_elf_header->e_shstrndx == SHN_UNDEF ) {
        return "Undefined";
    } else if ( p_elf_header->e_shstrndx == SHN_XINDEX ) {
        // TODO: get index from section 0 sh_link field
        snprintf(ret_string, 32, "%d", p_section_headers[0].sh_link);

        return ret_string;
    } else {
        snprintf(ret_string, 32, "%d", p_elf_header->e_shstrndx );

        return ret_string;
    }
}


const char* Parser::get_total_shsize() {
    Elf64_Ehdr* p_elf_header = p_prog_mmap->get_elf_header();
    // Total occupied space by program headers = e_phentsize * e_shnum
    static char ret_string[32];
    snprintf(ret_string, 32, "%d (bytes)", p_elf_header->e_shnum * p_elf_header->e_shentsize);

    return ret_string;
} 


const char* Parser::get_e_shnum() {
    Elf64_Ehdr* p_elf_header = p_prog_mmap->get_elf_header();
    Elf64_Shdr* p_section_headers = p_prog_mmap->get_section_headers();
    static char ret_string[16];

    if ( p_elf_header->e_shnum == 0 ) {
        return "0 (No headers)";
    } else if ( p_elf_header-> e_shnum == SHN_UNDEF ) {
        snprintf(ret_string, 16, "%lu", p_section_headers[0].sh_size);
    } else {
        snprintf(ret_string, 16, "%u", p_elf_header->e_shnum);
    }

    return ret_string;
}


const char* Parser::get_e_shentsize() {
    Elf64_Ehdr* p_elf_header = p_prog_mmap->get_elf_header();
    static char ret_string[16];
    snprintf(ret_string, 16, "%u (bytes)", p_elf_header->e_shentsize);

    return ret_string;
}


const char* Parser::get_total_phsize() {
    Elf64_Ehdr* p_elf_header = p_prog_mmap->get_elf_header();
    // Total occupied space by program headers = e_phentsize * e_phnum    
    static char ret_string[32];
    snprintf(ret_string, 32, "%d (bytes)", p_elf_header->e_phnum * p_elf_header->e_phentsize);

    return ret_string;
}


const char* Parser::get_e_phnum() {
    Elf64_Ehdr* p_elf_header = p_prog_mmap->get_elf_header();
    static char ret_string[16];

    if ( p_elf_header->e_phnum == 0 ) {
        return "0 (No headers)";
    } else {
        snprintf(ret_string, 16, "%u", p_elf_header->e_phnum);
    }

    return ret_string;
}


const char* Parser::get_e_phentsize() {
    Elf64_Ehdr* p_elf_header = p_prog_mmap->get_elf_header();
    static char ret_string[32];
    snprintf(ret_string, 36, "%u (bytes per header)", p_elf_header->e_phentsize);

    return ret_string;
}


const char* Parser::get_e_flags() {
    Elf64_Ehdr* p_elf_header = p_prog_mmap->get_elf_header();
    static char ret_string[16];
    snprintf(ret_string, 16, "0x%x", p_elf_header->e_flags);

    return ret_string;
}


const char* Parser::get_e_shoff() {
    Elf64_Ehdr* p_elf_header = p_prog_mmap->get_elf_header();
    static char ret_string[16];
    snprintf(ret_string, 16, "%lu bytes", p_elf_header->e_shoff);

    return ret_string;
}


const char* Parser::get_e_phoff() {
    Elf64_Ehdr* p_elf_header = p_prog_mmap->get_elf_header();
    static char ret_string[16];
    snprintf(ret_string, 16, "%lu bytes", p_elf_header->e_phoff);

    return ret_string;
}


const char* Parser::get_e_entry() {
    Elf64_Ehdr* p_elf_header = p_prog_mmap->get_elf_header();
    static char ret_string[24];
    snprintf(ret_string, 24, "0x%2.2lx", p_elf_header->e_entry);
    
    return ret_string;
}


const char* Parser::get_e_version() {
    Elf64_Ehdr* p_elf_header = p_prog_mmap->get_elf_header();
    if ( p_elf_header->e_version == EV_NONE ) {
        return "Invalid version";
    }

    static char ret_string[8];
    snprintf(ret_string, 8, "0x%u", p_elf_header->e_version);

    return ret_string;
}


const char* Parser::get_e_ident() {
    Elf64_Ehdr* p_elf_header = p_prog_mmap->get_elf_header();
    static char ret_string[64];

    if ( parser_verbose && !Parser::check_ELF64_magic(p_elf_header->e_ident, parser_verbose) ) {
        cout << "WARN: file is not a valid ELF (magic is malformed)" << endl;
    }

    uint8_t cur = 0;
    for (int i = 0; i < EI_NIDENT; i++)
        cur = snprintf(ret_string + (cur*i), 64-cur, "%02x ", p_elf_header->e_ident[i]);

    return ret_string;
}


// TODO: There are a lot of machine types missing here - update w/ more complete list at later date
const char* Parser::get_e_machine() {
    Elf64_Ehdr* p_elf_header = p_prog_mmap->get_elf_header();
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
        case EM_MIPS_RS3_LE:    return "MIPS R4000 big-endian";
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
    Elf64_Ehdr* p_elf_header = p_prog_mmap->get_elf_header();
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


const uint8_t Parser::get_ei_class() {
    Elf64_Ehdr* p_elf_header = p_prog_mmap->get_elf_header();
    if ( p_elf_header->e_ident[EI_CLASS] == ELFCLASS64 ) {
        p_ei_class = ELFCLASS64;
        return ELFCLASS64;
    } else if ( p_elf_header->e_ident[EI_CLASS] == ELFCLASS32 ) {
        p_ei_class = ELFCLASS32;
        return ELFCLASS32;
    } else {
        p_ei_class = ELFCLASSNONE;
        if (parser_verbose) {
            cout << "WARN: e_ident ei_class is of invalid type" << endl;
        }
        return ELFCLASSNONE;
    }
}


int main(int argc, char* argv[]) {
    po::options_description desc(
    "ELF Parser 1.0.0\n"
    "Written by mowemcfc (jcartermcfc@gmail.com)\n"
    "Allowed options"
    );

    desc.add_options()
        ("help", "produce help message")
        ("headers", "print program headers")
        ("sections", "prints section headers");


    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    

    std::string prog_path = "test";
    Parser parser = Parser(prog_path, 1);

    if ( parser.get_ei_class() != ELFCLASS64 ) {
        cout << "ERROR: Only 64 bit ELF executables are supported, exiting" << endl;
        return 1;
    }

    if ( vm.count("help") ) {
        cout << desc << "\n";
        return 0;
    }

    if ( vm.count("headers") ) {
        parser.print_elf_header();
    }

    if ( vm.count("sections") ) {
        parser.print_section_headers();
    }

    return 0;
}