#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <elf.h>
#include <iostream>
#include <string>
#include <fstream>
#include "stub/stub.h"

#define ALIGN(x, a) (((x) + ((a)-1)) & ~((a)-1))

#define YELLOW "\033[1;33m"
#define GREEN  "\033[1;32m"
#define RESET  "\033[0m"
#define RED "\033[1;31m"
#define CYAN "\033[1;36m"

void banner(){
    printf("\n\n");
    printf("\033[1;31m");  // Bold Cyan

    printf("███████╗███╗   ██╗████████╗██████╗ ██╗   ██╗    ███╗   ███╗ █████╗ ███████╗██╗  ██╗\n");
    printf("██╔════╝████╗  ██║╚══██╔══╝██╔══██╗╚██╗ ██╔╝    ████╗ ████║██╔══██╗██╔════╝██║ ██╔╝\n");
    printf("█████╗  ██╔██╗ ██║   ██║   ██████╔╝ ╚████╔╝     ██╔████╔██║███████║███████╗█████╔╝ \n");
    printf("██╔══╝  ██║╚██╗██║   ██║   ██╔══██╗  ╚██╔╝      ██║╚██╔╝██║██╔══██║╚════██║██╔═██╗ \n");
    printf("███████╗██║ ╚████║   ██║   ██║  ██║   ██║       ██║ ╚═╝ ██║██║  ██║███████║██║  ██╗\n");
    printf("╚══════╝╚═╝  ╚═══╝   ╚═╝   ╚═╝  ╚═╝   ╚═╝       ╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝\n");

    printf("\033[0m");  // Reset
    
    printf("\033[1;35m");
    printf("\n\t=========> Simple ELF Packer based on XOR encoding <=========\n\t\t==> Github https://github.com/0x3xploit <== \n\n");
    printf("\033[0m");
}


int main(int argc, char** argv) {


    banner();

    if (argc < 6 || strcmp(argv[2], "-o") != 0 || strcmp(argv[4], "-k") != 0) {
        fprintf(stderr, "Usage: %s <input> -o <output> -k <xor_key>\n", argv[0]);
        return 1;
    }


    const char* input = argv[1];
    const char* output = argv[3];
    const int KEY = atoi(argv[5]);
    
    FILE* f = fopen(input, "rb");
    if (!f) { perror("fopen input"); return 1; }

    fseek(f, 0, SEEK_END);
    size_t elf_size = ftell(f);
    rewind(f);

    uint8_t* elf_data = (uint8_t*)malloc(elf_size);
    fread(elf_data, 1, elf_size, f);
    fclose(f);

    Elf64_Ehdr* ehdr = (Elf64_Ehdr*)elf_data;
    Elf64_Phdr* phdrs = (Elf64_Phdr*)(elf_data + ehdr->e_phoff);

    if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0) {
        fprintf(stderr, RED "[!] Not an ELF file\n" RESET);
        return 1;
    }

    
    // Get original entry
    Elf64_Addr original_entry = ehdr->e_entry;
    
    
    //find PT_LOAD segment
    Elf64_Phdr* last_load = nullptr;
    Elf64_Off inject_offset = 0;
    Elf64_Addr inject_vaddr = 0;

    for (int i = 0; i < ehdr->e_phnum; ++i) {
        if (phdrs[i].p_type == PT_LOAD) {
            Elf64_Off end_off = phdrs[i].p_offset + phdrs[i].p_filesz;
            Elf64_Addr end_va = phdrs[i].p_vaddr + phdrs[i].p_filesz;
            inject_offset = ALIGN(end_off, 0x1000);
            inject_vaddr = ALIGN(end_va, 0x1000);
            last_load = &phdrs[i];
        }
    }

    if (!last_load) {
        fprintf(stderr, RED "[-] No PT_LOAD segment found\n" RESET);
        return 1;
    }

    // Find .text section
    Elf64_Shdr* shdrs = (Elf64_Shdr*)(elf_data + ehdr->e_shoff);
    const char* shstrtab = (const char*)(elf_data + shdrs[ehdr->e_shstrndx].sh_offset);

    Elf64_Addr text_vaddr = 0;
    Elf64_Xword text_size = 0;
    Elf64_Off text_offset = 0;

    for (int i = 0; i < ehdr->e_shnum; ++i) {
        const char* name = shstrtab + shdrs[i].sh_name;
        if (strcmp(name, ".text") == 0) {
            text_vaddr = shdrs[i].sh_addr;
            text_size = shdrs[i].sh_size;
            text_offset = shdrs[i].sh_offset;
            break;
        }
    }

    if (text_vaddr == 0 || text_size == 0) {
        fprintf(stderr, RED "[-] Text section not found\n" RESET);
        return 1;
    }



    printf(YELLOW"[+] OEP (Original Entry Point) => " GREEN "%p\n" RESET,original_entry);
    printf(YELLOW"[+] Text vaddr => " GREEN "%p\n" RESET,text_vaddr);
    printf(YELLOW"[+] Text size => " GREEN "%p\n" RESET,text_size);
    printf(YELLOW"[+] Text offset => " GREEN "%p\n" RESET,text_offset);
    


    memcpy(&stub_bin[1],  &original_entry, 4);    // .text_start
    memcpy(&stub_bin[7], &text_size,  4);    // .text_size
    memcpy(&stub_bin[13], &text_vaddr,4);    // original_entry
    stub_bin[22] = KEY;                  // XOR key
    

    // Mark segment containing .text as writable (fix segfault)
    for (int i = 0; i < ehdr->e_phnum; ++i) {
        if (phdrs[i].p_type == PT_LOAD) {
            Elf64_Addr seg_start = phdrs[i].p_vaddr;
            Elf64_Addr seg_end   = seg_start + phdrs[i].p_memsz;
            if (text_vaddr >= seg_start && (text_vaddr + text_size) <= seg_end) {
                phdrs[i].p_flags |= PF_W;
                break;
            }
        }
    }

    // XOR .text section
    for (size_t i = 0; i < text_size; ++i) {
        elf_data[text_offset + i] ^= KEY;
    }
    printf(YELLOW"[+] Text section encoded with XOR key => " GREEN "0x%x\n" RESET,KEY);

    // Expand segment if needed
    Elf64_Xword new_end = inject_offset + stub_bin_len;
    Elf64_Xword old_end = last_load->p_offset + last_load->p_filesz;

    if (new_end > old_end) {
        last_load->p_filesz = new_end - last_load->p_offset;
        last_load->p_memsz = last_load->p_filesz;
        last_load->p_flags |= PF_X;
    }

    ehdr->e_entry = inject_vaddr;
    printf(YELLOW"[+] Entry point set to stub: => " GREEN "0x%lx\n" RESET, inject_vaddr);

    FILE* out = fopen(output, "wb");
    if (!out) { perror("fopen output"); return 1; }

    fwrite(elf_data, 1, elf_size, out);

    if (inject_offset > elf_size) {
        size_t pad = inject_offset - elf_size;
        uint8_t* zero = (uint8_t*)calloc(1, pad);
        fwrite(zero, 1, pad, out);
        free(zero);
    }

    fwrite(stub_bin, 1, stub_bin_len, out);

    fseek(out, 0, SEEK_SET);
    fwrite(elf_data, 1, ehdr->e_ehsize + ehdr->e_phnum * ehdr->e_phentsize, out);

    fclose(out);
    free(elf_data);

    printf(YELLOW"[+] Packed ELF written to => " CYAN " %s\n", output);
    printf(YELLOW "\n[✓] Packing completed successfully!\n" RESET);
    return 0;
}