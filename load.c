#include "ml.h"

unsigned getbits(unsigned x, int p, int n) {
	return (x >> (p+1-n)) & ~(~0 << n);
}

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

int load_segment_command_64(FILE *f, struct segment_command_64 *seg_cmd_64) {
	fread(&seg_cmd_64->segname, 16, 1, f);
	fread(&seg_cmd_64->vmaddr, sizeof(uint64_t), 1, f);
	fread(&seg_cmd_64->vmsize, sizeof(uint64_t), 1, f);
	fread(&seg_cmd_64->fileoff, sizeof(uint64_t), 1, f);
	fread(&seg_cmd_64->filesize, sizeof(uint64_t), 1, f);
	fread(&seg_cmd_64->maxprot, sizeof(vm_prot_t), 1, f);
	fread(&seg_cmd_64->initprot, sizeof(vm_prot_t), 1, f);
	fread(&seg_cmd_64->nsects, sizeof(uint32_t), 1, f);
	fread(&seg_cmd_64->flags, sizeof(uint32_t), 1, f);

	printf("cmd = 0x%08x\n", seg_cmd_64->cmd);
	printf("cmdsize = 0x%08x\n", seg_cmd_64->cmdsize);
	printf("segname = %s\n", seg_cmd_64->segname);
	printf("vmaddr = 0x%016llx\n", seg_cmd_64->vmaddr);
	printf("vmsize = 0x%016llx\n", seg_cmd_64->vmsize);
	printf("fileoff = 0x%016llx\n", seg_cmd_64->fileoff);
	printf("filesize = 0x%016llx\n", seg_cmd_64->filesize);
	printf("maxprot = 0x%016x\n", seg_cmd_64->maxprot);
	printf("initprot = 0x%016x\n", seg_cmd_64->initprot);
	printf("nsects = 0x%08x\n", seg_cmd_64->nsects);
	printf("flags = 0x%08x\n", seg_cmd_64->flags);

	return 0;
}

int load_section_64(FILE *f, struct section_64 *sect64) {
	fread(&sect64->sectname, 16, 1, f);
	fread(&sect64->segname, 16, 1, f);

	fread(&sect64->addr, sizeof(uint64_t), 1, f);
	fread(&sect64->size, sizeof(uint64_t), 1, f);
	fread(&sect64->offset, sizeof(uint32_t), 1, f);
	fread(&sect64->align, sizeof(uint32_t), 1, f);
	fread(&sect64->reloff, sizeof(uint32_t), 1, f);
	fread(&sect64->nreloc, sizeof(uint32_t), 1, f);
	fread(&sect64->flags, sizeof(uint32_t), 1, f);
	fread(&sect64->reserved1, sizeof(uint32_t), 1, f);
	fread(&sect64->reserved2, sizeof(uint32_t), 1, f);
	fread(&sect64->reserved3, sizeof(uint32_t), 1, f);

	printf("sectname = %s\n", sect64->sectname);
	printf("segname = %s\n", sect64->segname);
	printf("addr = 0x%016llx\n", sect64->addr);
	printf("size = 0x%016llx\n", sect64->size);
	printf("offset = 0x%08x\n", sect64->offset);
	printf("align = 0x%08x\n", sect64->align);
	printf("reloff = 0x%08x\n", sect64->reloff);
	printf("nreloc = 0x%08x\n", sect64->nreloc);
	printf("flags = 0x%08x\n", sect64->flags);
	printf("reserved1 = 0x%08x\n", sect64->reserved1);
	printf("reserved2 = 0x%08x\n", sect64->reserved2);
	printf("reserved3 = 0x%08x\n", sect64->reserved3);

	return 0;
}

int load_relocation_info(FILE *f, struct relocation_info *reloc_info) {
	uint32_t value;

	fread(&reloc_info->r_address, sizeof(uint32_t), 1, f);
	fread(&value, sizeof(uint32_t), 1, f);
	reloc_info->r_symbolnum = getbits(value, 23, 24);
	reloc_info->r_pcrel = getbits(value, 24, 1);
	reloc_info->r_length = getbits(value, 26, 2);
	reloc_info->r_extern = getbits(value, 27, 1);
	reloc_info->r_type = getbits(value, 31, 4);

	printf("r_address = 0x%08x\n", reloc_info->r_address);
	printf("r_type = 0x%x\n", reloc_info->r_address);
	printf("r_extern = 0x%x\n", reloc_info->r_address);
	printf("r_length = 0x%x\n", reloc_info->r_address);
	printf("r_pcrel = 0x%x\n", reloc_info->r_address);
	printf("r_symbolnum = 0x%06x\n", reloc_info->r_address);

	return 0;
}