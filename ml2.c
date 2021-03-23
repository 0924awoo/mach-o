#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>

static struct mach_header_64 mach_header;
static struct load_command *load_commands;

static struct symtab_command symtab_command;
static struct nlist_64 *nlists;
static char *string_table;


int load_macho_header_64(FILE *f, struct mach_header_64 *h) {
	printf("== load 64bit mach-o header == \n");
	fread(&h->cputype, sizeof(cpu_type_t), 1, f);
	fread(&h->cpusubtype, sizeof(cpu_subtype_t), 1, f);
	fread(&h->filetype, sizeof(uint32_t), 1, f);
	fread(&h->ncmds, sizeof(uint32_t), 1, f);
	fread(&h->sizeofcmds, sizeof(uint32_t), 1, f);
	fread(&h->flags, sizeof(uint32_t), 1, f);
	fread(&h->reserved, sizeof(uint32_t), 1, f);

	printf("magic = 0x%08x\n", h->magic);
	printf("cputype = 0x%08x\n", h->cputype);
	printf("cpusubtype = 0x%08x\n", h->cpusubtype);
	printf("filetype = 0x%08x\n", h->filetype);
	printf("ncmds = 0x%08x\n", h->ncmds);
	printf("sizeofcmds = 0x%08x\n", h->sizeofcmds);
	printf("flags = 0x%08x\n", h->flags);
	printf("reserved = 0x%08x\n", h->reserved);

	return 0;
}

int load_symtab_command(FILE *f, struct symtab_command *symtab_cmd) {
	fread(&symtab_cmd->symoff, sizeof(uint32_t), 1, f);
	fread(&symtab_cmd->nsyms, sizeof(uint32_t), 1, f);
	fread(&symtab_cmd->stroff, sizeof(uint32_t), 1, f);
	fread(&symtab_cmd->strsize, sizeof(uint32_t), 1, f);

	printf("symoff = 0x%08x\n", symtab_cmd->symoff);
	printf("nsyms = 0x%08x\n", symtab_cmd->nsyms);
	printf("stroff = 0x%08x\n", symtab_cmd->stroff);
	printf("strsize = 0x%08x\n", symtab_cmd->strsize);

	return 0;
}

int load_nlist_64(FILE *f, struct nlist_64 *nl_64) {
	fread(&nl_64->n_un.n_strx, sizeof(uint32_t), 1, f);
	fread(&nl_64->n_type, sizeof(uint8_t), 1, f);
	fread(&nl_64->n_sect, sizeof(uint8_t), 1, f);
	fread(&nl_64->n_desc, sizeof(uint16_t), 1, f);
	fread(&nl_64->n_value, sizeof(uint64_t), 1, f);

	printf("n_un.n_strx = 0x%08x\n", nl_64->n_un.n_strx);
	printf("n_type = 0x%02x\n", nl_64->n_type);
	printf("n_sect = 0x%02x\n", nl_64->n_sect);
	printf("n_desc = 0x%04x\n", nl_64->n_desc);
	printf("n_value = 0x%016llx\n", nl_64->n_value);

	return 0;
}




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

	int i;
	int next_pos;
	for (i=0; i < mach_header.ncmds; i++) {
		fread(&load_commands[i].cmd, sizeof(uint32_t), 1, f);
		fread(&load_commands[i].cmdsize, sizeof(uint32_t), 1, f);
		printf("=== cmd = 0x%08x, cmdsize = 0x%08x ===\n",
			load_commands[i].cmd, load_commands[i].cmdsize);


		next_pos = ftell(f) + load_commands[i].cmdsize - (sizeof(uint32_t) * 2);

		if (load_commands[i].cmd == LC_SYMTAB) {
			// symtab_command
			printf("=== LC_SYMTAB ===\n");
			symtab_command.cmd = load_commands[i].cmd;
			symtab_command.cmdsize = load_commands[i].cmdsize;
			load_symtab_command(f, &symtab_command);
		}

		fseek(f, next_pos, SEEK_SET);
	}

	// load string_table
	string_table = (char *) malloc(symtab_command.strsize);

	fseek(f, symtab_command.stroff, SEEK_SET);
	fread(string_table, symtab_command.strsize, 1, f);

	// dump
	printf("=== string table ===\n");
	char *p = string_table;
	for(i=0; i <= symtab_command.nsyms; i++) {
		printf("offset = 0x%08x, string_table[%d] = %s\n", 
		(unsigned) (p - string_table), i, p);
		p = strchr(p, 0) + 1;
	}

	// load nlist_64
	nlists = (struct nlist_64 *)malloc(sizeof(struct nlist_64) * symtab_command.nsyms);
	fseek(f, symtab_command.symoff, SEEK_SET);
	for (i = 0; i < symtab_command.nsyms; i++) {
		printf("=== nlist_64[%d] ===\n", i);
		load_nlist_64(f, &nlists[i]);
		printf("(string = %s)\n", string_table + nlists[i].n_un.n_strx);
	}


	free(load_commands);
end:
	if (f != 0) {
		fclose(f);
	}
	
	return retval;
}
