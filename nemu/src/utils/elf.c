#include <common.h>
#include <stdio.h>
#include <fcntl.h>
#include <gelf.h>
#include <libelf.h>
#include <unistd.h>
#ifdef CONFIG_FTRACE

// 帮助函数：符号类型
const char* elf_sym_type(unsigned char st_info) {
    switch (ELF64_ST_TYPE(st_info)) {
        case STT_NOTYPE:  return "NOTYPE";
        case STT_OBJECT:  return "OBJECT";
        case STT_FUNC:    return "FUNC";
        case STT_SECTION: return "SECTION";
        case STT_FILE:    return "FILE";
        default:          return "UNKNOWN";
    }
}

// 帮助函数：符号绑定
const char* elf_sym_bind(unsigned char st_info) {
    switch (ELF64_ST_BIND(st_info)) {
        case STB_LOCAL:  return "LOCAL";
        case STB_GLOBAL: return "GLOBAL";
        case STB_WEAK:   return "WEAK";
        default:         return "UNKNOWN";
    }
}
void init_elf(const char *elf_file) { 
    int fd = open(elf_file, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "cannot open %s", elf_file);
        return;
    }
    elf_version(EV_CURRENT);
    Elf *e = elf_begin(fd, ELF_C_READ, NULL);
    if (!e) {
        fprintf(stderr, "elf_begin failed: %s", elf_errmsg(-1));
        close(fd);
        return;
    }

    Elf_Scn *scn = NULL;
    GElf_Shdr shdr;
    while ((scn = elf_nextscn(e, scn)) != NULL) {
        gelf_getshdr(scn, &shdr);
        if (shdr.sh_type == SHT_SYMTAB) {
            Elf_Data *data = elf_getdata(scn, NULL);
            int count = shdr.sh_size / shdr.sh_entsize;

            Elf_Scn *strscn = elf_getscn(e, shdr.sh_link);
            Elf_Data *strdata = elf_getdata(strscn, NULL);
                        for (int i = 0; i < count; i++) {
                GElf_Sym sym;
                gelf_getsym(data, i, &sym);

                const char *name = (const char *)((char *)strdata->d_buf + sym.st_name);

                printf("%3d: %08lx %5ld %-6s %-6s %s\n",
                    i,
                    (long)sym.st_value,
                    (long)sym.st_size,
                    elf_sym_type(sym.st_info),
                    elf_sym_bind(sym.st_info),
                    name);
            }
        }
    }
}
#endif
