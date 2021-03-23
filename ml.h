#ifndef ML_HEADER_INCLUDED
#define ML_HEADER_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach-o/reloc.h>

extern struct mach_header_64 mach_header;
extern struct load_command *load_commands;

extern struct symtab_command symtab_command;
extern struct nlist_64 *nlists;
extern char *string_table;

extern struct segment_command_64 *segment_commands;
extern struct section_64 *sections;

extern struct relocation_info *rel_infos;


unsigned getbits(unsigned x, int p, int n) ;

int load_macho_header_64(FILE *f, struct mach_header_64 *h);

int load_symtab_command(FILE *f, struct symtab_command *symtab_cmd);

int load_nlist_64(FILE *f, struct nlist_64 *nl_64);

int load_segment_command_64(FILE *f, struct segment_command_64 *seg_cmd_64);

int load_section_64(FILE *f, struct section_64 *sect64);

int load_relocation_info(FILE *f, struct relocation_info *reloc_info);

#endif