#include <stdio.h>
#include <stdlib.h>
#include <mach-o/loader.h>

static struct mach_header_64 mach_header;
static struct load_command *load_commands;

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
	for (i=0; i < mach_header.ncmds; i++) {
		fread(&load_commands[i].cmd, sizeof(uint32_t), 1, f);
		fread(&load_commands[i].cmdsize, sizeof(uint32_t), 1, f);
		printf("=== cmd = 0x%08x, cmdsize = 0x%08x ===\n",
			load_commands[i].cmd, load_commands[i].cmdsize);

		// skip
		fseek(f, load_commands[i].cmdsize - (sizeof(uint32_t) * 2), SEEK_CUR);
	}

	free(load_commands);
end:
	if (f != 0) {
		fclose(f);
	}
	
	return retval;
}
