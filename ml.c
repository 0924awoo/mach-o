#include "ml.h"

struct mach_header_64 mach_header;
struct load_command *load_commands;

struct symtab_command symtab_command;
struct nlist_64 *nlists;
char *string_table;

struct segment_command_64 *segment_commands;
struct section_64 *sections;

struct relocation_info *rel_infos;

int main(int argc, char** argv) {
	int retval, magic;
	FILE *f = 0;

	retval = 0;

	if (argc != 2) {
		printf("usage: ml1 <macro file>\n");
		retval = 1;
		goto end;
	}

	if ((f = fopen(argv[1], "r")) == NULL) {
		printf("fileopen error: %s\n", argv[1]);
		retval = 1;
		goto end;
	}

	// load magic
	fread(&magic, sizeof(magic), 1, f);

	if (magic == MH_MAGIC) {
		printf("i386 mach-o not supported.\n");
		retval = 1;
		goto end;
	} else if (magic == MH_MAGIC_64) {
		mach_header.magic = magic;
		load_macho_header_64(f, &mach_header);
	} else {
		printf("unknown magic: 0x%08x\n", magic);
		retval = 1;
		goto end;
	}
	
	load_commands = (struct load_command *) malloc(sizeof(struct load_command) * mach_header.ncmds);
	segment_commands = (struct segment_command_64 *) malloc(sizeof(struct segment_command_64) * mach_header.ncmds);

	int i,j,k;
	int next_pos;

	// read symtab_commad
	for (i=0; i < mach_header.ncmds; i++) {
		fread(&load_commands[i].cmd, sizeof(uint32_t), 1, f);
		fread(&load_commands[i].cmdsize, sizeof(uint32_t), 1, f);
		next_pos = ftell(f) + load_commands[i].cmdsize - (sizeof(uint32_t) * 2);

		if (load_commands[i].cmd == LC_SYMTAB) {
			// symtab_command
			printf("=== LC_SYMTAB ===\n");
			symtab_command.cmd = load_commands[i].cmd;
			symtab_command.cmdsize = load_commands[i].cmdsize;
			load_symtab_command(f, &symtab_command);

			// load string_table
			string_table = (char *) malloc(symtab_command.strsize);

			fseek(f, symtab_command.stroff, SEEK_SET);
			fread(string_table, symtab_command.strsize, 1, f);

			// dump
			printf("=== string table ===\n");
			char *p = string_table;
			for(j=0; j <= symtab_command.nsyms; j++) {
				printf("offset = 0x%08x, string_table[%d] = %s\n", 
				(unsigned) (p - string_table), j, p);
				p = strchr(p, 0) + 1;
			}

			// load nlist_64
			nlists = (struct nlist_64 *)malloc(sizeof(struct nlist_64) * symtab_command.nsyms);
			fseek(f, symtab_command.symoff, SEEK_SET);
			for (j = 0; j < symtab_command.nsyms; j++) {
				printf("=== nlist_64[%d] ===\n", j);
				load_nlist_64(f, &nlists[j]);
				printf("(string = %s)\n", string_table + nlists[j].n_un.n_strx);
			}

			break;
		}
		fseek(f, next_pos, SEEK_SET);
	}


	// read other load commands
	fseek(f, 32, SEEK_SET);
	for (i=0; i < mach_header.ncmds; i++) {
		fread(&load_commands[i].cmd, sizeof(uint32_t), 1, f);
		fread(&load_commands[i].cmdsize, sizeof(uint32_t), 1, f);
		printf("=== cmd = 0x%08x, cmdsize = 0x%08x ===\n",
			load_commands[i].cmd, load_commands[i].cmdsize);

		next_pos = ftell(f) + load_commands[i].cmdsize - (sizeof(uint32_t) * 2);

		if (load_commands[i].cmd == LC_SEGMENT_64) {
			// segment_command
			printf("=== LC_SEGMENT[%d] ===\n", i);
			segment_commands[i].cmd = load_commands[i].cmd;
			segment_commands[i].cmdsize = load_commands[i].cmdsize;
			load_segment_command_64(f, &segment_commands[i]);

			// sections
			sections = (struct section_64 *)malloc(sizeof(struct section_64) * segment_commands[i].nsects );
			for (j=0; j < segment_commands[i].nsects; j++) {
				printf("=== section[%d] ===\n", j);
				load_section_64(f, &sections[j]);

				// relocation info
				rel_infos = (struct relocation_info *)malloc(sizeof(struct relocation_info) * sections[j].nreloc);
				struct relocation_info *p_reloc = rel_infos;
				fseek(f, sections[j].reloff, SEEK_SET);

				for (k=0; k < sections[j].nreloc; k++) {
					printf("=== section[%d], relocation_info[%d] ===\n", j, k);
					load_relocation_info(f, p_reloc);
					if (p_reloc->r_extern) {
						printf("	symbol_value - 0x%016llx\n", nlists[p_reloc->r_symbolnum].n_value);
						printf("	string_table_index = 0x%08x\n", nlists[p_reloc->r_symbolnum].n_un.n_strx);
						printf("	string = %s\n", string_table + nlists[p_reloc->r_symbolnum].n_un.n_strx);
					}
					p_reloc++;
				}
			}
			free(sections);

			}

		fseek(f, next_pos, SEEK_SET);
	}

	free(segment_commands);
	free(nlists);
	free(load_commands);
end:
	if (f != 0) {
		fclose(f);
	}
	
	return retval;
}
