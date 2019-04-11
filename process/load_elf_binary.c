/*
 * linux/fs/binfmt_elf.c
 *
 * These are the functions used to load ELF format executables as used
 * on SVr4 machines.  Information on the format may be found in the book
 * "UNIX SYSTEM V RELEASE 4 Programmers Guide: Ansi C and Programming Support
 * Tools".
 *
 * Copyright 1993, 1994: Eric Youngdale (ericy@cais.com).
 */
/*
 * Modify by Shuyong Chen < shuyong.chen@gmail.com >
 */

#define _GNU_SOURCE
#include <features.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>

#include <stdint.h>
#include <elf.h>
#include <link.h>
#include <sys/auxv.h>
#include <sys/mman.h>

#include <sys/user.h>	// for PAGE_SIZE
#ifndef PAGE_SIZE
#define PAGE_SHIFT		12
#define PAGE_SIZE		(1UL << PAGE_SHIFT)
#define PAGE_MASK		(~(PAGE_SIZE-1))
#endif

#ifndef O_LARGEFILE
#define O_LARGEFILE	00100000
#endif

/* Used to tag non-static symbols that are private and never exposed by the shared library. */
#define __LIBC_HIDDEN__ __attribute__((visibility("hidden")))

__LIBC_HIDDEN__ int load_elf_binary(char* buf, int file, int argc, char *const argv[], int envc, char *const envp[]);
static unsigned long elf_map(int , unsigned long, ElfW(Phdr) *,
				int, int, unsigned long);

#if ELF_EXEC_PAGESIZE > PAGE_SIZE
#define ELF_MIN_ALIGN	ELF_EXEC_PAGESIZE
#else
#define ELF_MIN_ALIGN	PAGE_SIZE
#endif

#ifndef ELF_CORE_EFLAGS
#define ELF_CORE_EFLAGS	0
#endif

#define ELF_PAGESTART(_v) ((_v) & ~(unsigned long)(ELF_MIN_ALIGN-1))
#define ELF_PAGEOFFSET(_v) ((_v) & (ELF_MIN_ALIGN-1))
#define ELF_PAGEALIGN(_v) (((_v) + ELF_MIN_ALIGN - 1) & ~(ELF_MIN_ALIGN - 1))

// FIXME:
#if defined(__LP64__)
#define TASK_SIZE	(0x800000000000UL - 4096)
#define ELF_ET_DYN_BASE	0x100000000UL
#else
#define TASK_SIZE	(0xC0000000UL)
#define ELF_ET_DYN_BASE	0x000400000UL
#endif
//#define ELF_ET_DYN_BASE (TASK_SIZE / 3 * 2)

#define BAD_ADDR(x) ((unsigned long)(x) >= TASK_SIZE)

typedef unsigned bool;
#define false	0
#define true	!(false)

static int set_brk(unsigned long start, unsigned long end, int prot)
{
	start = ELF_PAGEALIGN(start);
	end = ELF_PAGEALIGN(end);
	if (end > start) {
		/*
		 * Map the last of the bss segment.
		 * If the header is requesting these pages to be
		 * executable, honour that (ppc32 needs this).
		 */
		void *addr = mmap((void *)start, end - start,
				prot, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS/* | MAP_DENYWRITE*/,
				0, 0);
		int error = addr == (void *) -1 ? -ENOMEM : 0;
		if (error)
			return error;
	}
	return 0;
}

/*
 * Zero Userspace
 */
static inline unsigned long
clear_user(void *to, unsigned long n)
{
	memset(to, 0, n);
	return 0;
}

/* We need to explicitly zero any fractional pages
   after the data section (i.e. bss).  This would
   contain the junk from the file that should not
   be in memory
 */
static int padzero(unsigned long elf_bss)
{
	unsigned long nbyte;

	nbyte = ELF_PAGEOFFSET(elf_bss);
	if (nbyte) {
		nbyte = ELF_MIN_ALIGN - nbyte;
		if (clear_user((void *) elf_bss, nbyte))
			return -EFAULT;
	}
	return 0;
}

#define ARCH_DLINFO						\
do {								\
	OLD_AUX_ENT(AT_SYSINFO_EHDR);				\
} while (0)

static int
create_elf_tables(ElfW(Addr) *auxv, ElfW(Ehdr) *exec,
		unsigned long load_addr, unsigned long interp_load_addr)
{
	ElfW(Addr) *elf_info;
	int ei_index = 0;

	/* Create the ELF interpreter info */
	elf_info = (ElfW(Addr) *)auxv;
	/* update AT_VECTOR_SIZE_BASE if the number of NEW_AUX_ENT() changes */
#define OLD_AUX_ENT(type)	\
        do { \
                unsigned long val = getauxval(type); \
                if (val != 0 || type == AT_FLAGS || type == AT_SECURE) { \
                        elf_info[ei_index++] = (ElfW(Addr))type; \
                        elf_info[ei_index++] = (ElfW(Addr))val; \
                } \
        } while(0)

#define NEW_AUX_ENT(id, val) \
	do { \
		elf_info[ei_index++] = id; \
		elf_info[ei_index++] = val; \
	} while (0)

#ifdef ARCH_DLINFO
	/* 
	 * ARCH_DLINFO must come first so PPC can do its special alignment of
	 * AUXV.
	 * update AT_VECTOR_SIZE_ARCH if the number of NEW_AUX_ENT() in
	 * ARCH_DLINFO changes
	 */
	ARCH_DLINFO;
#endif
	OLD_AUX_ENT(AT_HWCAP);
	OLD_AUX_ENT(AT_PAGESZ);
	OLD_AUX_ENT(AT_CLKTCK);
	NEW_AUX_ENT(AT_PHDR, load_addr + exec->e_phoff);
	NEW_AUX_ENT(AT_PHENT, sizeof(ElfW(Phdr)));
	NEW_AUX_ENT(AT_PHNUM, exec->e_phnum);
	NEW_AUX_ENT(AT_BASE, interp_load_addr);
	NEW_AUX_ENT(AT_FLAGS, 0);
	NEW_AUX_ENT(AT_ENTRY, exec->e_entry);
	OLD_AUX_ENT(AT_UID);
	OLD_AUX_ENT(AT_EUID);
	OLD_AUX_ENT(AT_GID);
	OLD_AUX_ENT(AT_EGID);
	OLD_AUX_ENT(AT_SECURE);
	OLD_AUX_ENT(AT_RANDOM);
	OLD_AUX_ENT(AT_HWCAP2);
	OLD_AUX_ENT(AT_EXECFN);
	OLD_AUX_ENT(AT_PLATFORM);
	OLD_AUX_ENT(AT_BASE_PLATFORM);
	OLD_AUX_ENT(AT_EXECFD);

#undef OLD_AUX_ENT
#undef NEW_AUX_ENT

	return 0;
}

static unsigned long elf_map(int filep, unsigned long addr,
		ElfW(Phdr) *eppnt, int prot, int type,
		unsigned long total_size)
{
	unsigned long map_addr;
	unsigned long size = eppnt->p_filesz + ELF_PAGEOFFSET(eppnt->p_vaddr);
	unsigned long off = eppnt->p_offset - ELF_PAGEOFFSET(eppnt->p_vaddr);
	addr = ELF_PAGESTART(addr);
	size = ELF_PAGEALIGN(size);

	/* mmap() will return -EINVAL if given a zero size, but a
	 * segment with zero filesize is perfectly valid */
	if (!size)
		return addr;

	/*
	* total_size is the size of the ELF (interpreter) image.
	* The _first_ mmap needs to know the full size, otherwise
	* randomization might put this image into an overlapping
	* position with the ELF binary image. (since size < total_size)
	* So we first map the 'big' image - and unmap the remainder at
	* the end. (which unmap is needed for ELF images with holes.)
	*/
	if (total_size) {
		total_size = ELF_PAGEALIGN(total_size);
		map_addr = (unsigned long)mmap((void*)addr, total_size, prot, type, filep, off);
		if (!BAD_ADDR(map_addr))
			munmap((void*)(map_addr+size), total_size-size);
	} else
		map_addr = (unsigned long)mmap((void*)addr, size, prot, type, filep, off);

	return(map_addr);
}

static unsigned long total_mapping_size(ElfW(Phdr) *cmds, int nr)
{
	int i, first_idx = -1, last_idx = -1;

	for (i = 0; i < nr; i++) {
		if (cmds[i].p_type == PT_LOAD) {
			last_idx = i;
			if (first_idx == -1)
				first_idx = i;
		}
	}
	if (first_idx == -1)
		return 0;

	return cmds[last_idx].p_vaddr + cmds[last_idx].p_memsz -
				ELF_PAGESTART(cmds[first_idx].p_vaddr);
}

/**
 * load_elf_phdrs() - load ELF program headers
 * @elf_ex:   ELF header of the binary whose program headers should be loaded
 * @elf_file: the opened ELF binary file
 *
 * Loads ELF program headers from the binary file elf_file, which has the ELF
 * header pointed to by elf_ex, into a newly allocated array. The caller is
 * responsible for freeing the allocated data. Returns an ERR_PTR upon failure.
 */
static ElfW(Phdr) *load_elf_phdrs(ElfW(Ehdr) *elf_ex,
				       int elf_file)
{
	ElfW(Phdr) *elf_phdata = NULL;
	int retval, size, err = -1;
	off_t pos = elf_ex->e_phoff;

	/*
	 * If the size of this structure has changed, then punt, since
	 * we will be doing the wrong thing.
	 */
	if (elf_ex->e_phentsize != sizeof(ElfW(Phdr)))
		goto out;

	/* Sanity check the number of program headers... */
	if (elf_ex->e_phnum < 1 ||
		elf_ex->e_phnum > 65536U / sizeof(ElfW(Phdr)))
		goto out;

	/* ...and their total size. */
	size = sizeof(ElfW(Phdr)) * elf_ex->e_phnum;
	if ((unsigned long)size > ELF_MIN_ALIGN)
		goto out;

	elf_phdata = malloc(size);
	if (!elf_phdata)
		goto out;

	/* Read in the program headers */
	retval = lseek(elf_file, pos, SEEK_SET);
	if (retval != pos) {
		err = (retval < 0) ? retval : -EIO;
		goto out;
	}

	retval = read(elf_file, elf_phdata, size);
	if (retval != size) {
		err = (retval < 0) ? retval : -EIO;
		goto out;
	}

	/* Success! */
	err = 0;
out:
	if (err) {
		free(elf_phdata);
		elf_phdata = NULL;
	}
	return elf_phdata;
}

#ifndef CONFIG_ARCH_BINFMT_ELF_STATE

/**
 * struct arch_elf_state - arch-specific ELF loading state
 *
 * This structure is used to preserve architecture specific data during
 * the loading of an ELF file, throughout the checking of architecture
 * specific ELF headers & through to the point where the ELF load is
 * known to be proceeding (ie. SET_PERSONALITY).
 *
 * This implementation is a dummy for architectures which require no
 * specific state.
 */
struct arch_elf_state {
};

#define INIT_ARCH_ELF_STATE {}

/**
 * arch_elf_pt_proc() - check a PT_LOPROC..PT_HIPROC ELF program header
 * @ehdr:	The main ELF header
 * @phdr:	The program header to check
 * @elf:	The open ELF file
 * @is_interp:	True if the phdr is from the interpreter of the ELF being
 *		loaded, else false.
 * @state:	Architecture-specific state preserved throughout the process
 *		of loading the ELF.
 *
 * Inspects the program header phdr to validate its correctness and/or
 * suitability for the system. Called once per ELF program header in the
 * range PT_LOPROC to PT_HIPROC, for both the ELF being loaded and its
 * interpreter.
 *
 * Return: Zero to proceed with the ELF load, non-zero to fail the ELF load
 *         with that return code.
 */
static inline int arch_elf_pt_proc(ElfW(Ehdr) *ehdr,
				   ElfW(Phdr) *phdr,
				   int elf, bool is_interp,
				   struct arch_elf_state *state)
{
	/* Dummy implementation, always proceed */
	(void)ehdr;
	(void)phdr;
	(void)elf;
	(void)is_interp;
	(void)state;
	return 0;
}

/**
 * arch_check_elf() - check an ELF executable
 * @ehdr:	The main ELF header
 * @has_interp:	True if the ELF has an interpreter, else false.
 * @interp_ehdr: The interpreter's ELF header
 * @state:	Architecture-specific state preserved throughout the process
 *		of loading the ELF.
 *
 * Provides a final opportunity for architecture code to reject the loading
 * of the ELF & cause an exec syscall to return an error. This is called after
 * all program headers to be checked by arch_elf_pt_proc have been.
 *
 * Return: Zero to proceed with the ELF load, non-zero to fail the ELF load
 *         with that return code.
 */
static inline int arch_check_elf(ElfW(Ehdr) *ehdr, bool has_interp,
				 ElfW(Ehdr) *interp_ehdr,
				 struct arch_elf_state *state)
{
	/* Dummy implementation, always proceed */
	(void)ehdr;
	(void)has_interp;
	(void)interp_ehdr;
	(void)state;
	return 0;
}

#endif /* !CONFIG_ARCH_BINFMT_ELF_STATE */

static int GetTargetElfMachine() {
#if defined(__arm__)
  return EM_ARM;
#elif defined(__aarch64__)
  return EM_AARCH64;
#elif defined(__i386__)
  return EM_386;
#elif defined(__mips__)
  return EM_MIPS;
#elif defined(__x86_64__)
  return EM_X86_64;
#endif
}

#define elf_check_arch(x)	((x)->e_machine == GetTargetElfMachine())

/* This is much more generalized than the library routine read function,
   so we keep this separate.  Technically the library read function
   is only provided so that we can read a.out libraries that have
   an ELF header */

static unsigned long load_elf_interp(ElfW(Ehdr) *interp_elf_ex,
		int interpreter, unsigned long *interp_map_addr,
		unsigned long no_base, ElfW(Phdr) *interp_elf_phdata)
{
	ElfW(Phdr) *eppnt;
	unsigned long load_addr = 0;
	int load_addr_set = 0;
	unsigned long last_bss = 0, elf_bss = 0;
	int bss_prot = 0;
	unsigned long error = ~0UL;
	unsigned long total_size;
	int i;

	/* First of all, some simple consistency checks */
	if (interp_elf_ex->e_type != ET_EXEC &&
	    interp_elf_ex->e_type != ET_DYN)
		goto out;
	if (!elf_check_arch(interp_elf_ex))
		goto out;

	total_size = total_mapping_size(interp_elf_phdata,
					interp_elf_ex->e_phnum);
	if (!total_size) {
		error = -EINVAL;
		goto out;
	}

	eppnt = interp_elf_phdata;
	for (i = 0; i < interp_elf_ex->e_phnum; i++, eppnt++) {
		if (eppnt->p_type == PT_LOAD) {
			int elf_type = MAP_PRIVATE | MAP_DENYWRITE;
			int elf_prot = 0;
			unsigned long vaddr = 0;
			unsigned long k, map_addr;

			if (eppnt->p_flags & PF_R)
		    		elf_prot = PROT_READ;
			if (eppnt->p_flags & PF_W)
				elf_prot |= PROT_WRITE;
			if (eppnt->p_flags & PF_X)
				elf_prot |= PROT_EXEC;
			vaddr = eppnt->p_vaddr;
			if (interp_elf_ex->e_type == ET_EXEC || load_addr_set)
				elf_type |= MAP_FIXED;
			else if (no_base && interp_elf_ex->e_type == ET_DYN)
				load_addr = -vaddr;

			map_addr = elf_map(interpreter, load_addr + vaddr,
					eppnt, elf_prot, elf_type, total_size);
			total_size = 0;
			if (!*interp_map_addr)
				*interp_map_addr = map_addr;
			error = map_addr;
			if (BAD_ADDR(map_addr))
				goto out;

			if (!load_addr_set &&
			    interp_elf_ex->e_type == ET_DYN) {
				load_addr = map_addr - ELF_PAGESTART(vaddr);
				load_addr_set = 1;
			}

			/*
			 * Check to see if the section's size will overflow the
			 * allowed task size. Note that p_filesz must always be
			 * <= p_memsize so it's only necessary to check p_memsz.
			 */
			k = load_addr + eppnt->p_vaddr;
			if (BAD_ADDR(k) ||
			    eppnt->p_filesz > eppnt->p_memsz ||
			    eppnt->p_memsz > TASK_SIZE ||
			    TASK_SIZE - eppnt->p_memsz < k) {
				error = -ENOMEM;
				goto out;
			}

			/*
			 * Find the end of the file mapping for this phdr, and
			 * keep track of the largest address we see for this.
			 */
			k = load_addr + eppnt->p_vaddr + eppnt->p_filesz;
			if (k > elf_bss)
				elf_bss = k;

			/*
			 * Do the same thing for the memory mapping - between
			 * elf_bss and last_bss is the bss section.
			 */
			k = load_addr + eppnt->p_vaddr + eppnt->p_memsz;
			if (k > last_bss) {
				last_bss = k;
				bss_prot = elf_prot;
			}
		}
	}

	/*
	 * Now fill out the bss section: first pad the last page from
	 * the file up to the page boundary, and zero it from elf_bss
	 * up to the end of the page.
	 */
	if (padzero(elf_bss)) {
		error = -EFAULT;
		goto out;
	}
	/*
	 * Next, align both the file and mem bss up to the page size,
	 * since this is where elf_bss was just zeroed up to, and where
	 * last_bss will end after the mmap() below.
	 */
	elf_bss = ELF_PAGEALIGN(elf_bss);
	last_bss = ELF_PAGEALIGN(last_bss);
	/* Finally, if there is still more bss to allocate, do it. */
	if (last_bss > elf_bss) {
		error = (unsigned long)mmap((void *)elf_bss, last_bss - elf_bss,
				bss_prot, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS/* | MAP_DENYWRITE*/,
				0, 0);
		error = error == (unsigned long)-1 ? -ENOMEM : 0;
		if (error)
			goto out;
	}

	error = load_addr;
out:
	return error;
}

#define __ALIGN_KERNEL(x, a)		__ALIGN_KERNEL_MASK(x, (typeof(x))(a) - 1)
#define __ALIGN_KERNEL_MASK(x, mask)	(((x) + (mask)) & ~(mask))
#define ALIGN(x, a)		__ALIGN_KERNEL((x), (a))

/* to align the pointer to the (next) page boundary */
#define PAGE_ALIGN(addr) ALIGN(addr, PAGE_SIZE)

/* Stack area protections */
#define EXSTACK_DEFAULT   0	/* Whatever the arch defaults to */
#define EXSTACK_DISABLE_X 1	/* Disable executable stacks */
#define EXSTACK_ENABLE_X  2	/* Enable executable stacks */

#define MAX_ERRNO	4095

#define IS_ERR_VALUE(x) ((x) >= (unsigned long)-MAX_ERRNO)

static inline long PTR_ERR(const void *ptr)
{
	return (long) ptr;
}

static inline long IS_ERR(const void *ptr)
{
	return IS_ERR_VALUE((unsigned long)ptr);
}

__LIBC_HIDDEN__ int load_elf_binary(char* buf, int file, int argc, char *const argv[], int envc, char *const envp[])
{
	int interpreter = -1;
 	unsigned long load_addr = 0, load_bias = 0;
	int load_addr_set = 0;
	char * elf_interpreter = NULL;
	unsigned long error;
	ElfW(Phdr) *elf_ppnt, *elf_phdata, *interp_elf_phdata = NULL;
	unsigned long elf_bss, elf_brk;
	int bss_prot = 0;
	int retval, i;
	unsigned long elf_entry;
	unsigned long interp_load_addr = 0;
	unsigned long start_code, end_code, start_data, end_data;
	unsigned long reloc_func_desc = 0;
	int executable_stack = EXSTACK_DEFAULT;
	struct {
		ElfW(Ehdr) elf_ex;
		ElfW(Ehdr) interp_elf_ex;
	} *loc;
	struct arch_elf_state arch_state = INIT_ARCH_ELF_STATE;
	off_t pos;

	loc = malloc(sizeof(*loc));
	if (!loc) {
		retval = -ENOMEM;
		goto out_ret;
	}
	
	/* Get the exec-header */
	loc->elf_ex = *((ElfW(Ehdr) *)buf);

	retval = -ENOEXEC;
	/* First of all, some simple consistency checks */
	if (memcmp(loc->elf_ex.e_ident, ELFMAG, SELFMAG) != 0)
		goto out;

	if (loc->elf_ex.e_type != ET_EXEC && loc->elf_ex.e_type != ET_DYN)
		goto out;
	if (!elf_check_arch(&loc->elf_ex))
		goto out;

	elf_phdata = load_elf_phdrs(&loc->elf_ex, file);
	if (!elf_phdata)
		goto out;

	elf_ppnt = elf_phdata;
	elf_bss = 0;
	elf_brk = 0;

	start_code = ~0UL;
	end_code = 0;
	start_data = 0;
	end_data = 0;

	for (i = 0; i < loc->elf_ex.e_phnum; i++) {
		if (elf_ppnt->p_type == PT_INTERP) {
			/* This is the program interpreter used for
			 * shared libraries - for now assume that this
			 * is an a.out format binary
			 */
			retval = -ENOEXEC;
			if (elf_ppnt->p_filesz > PATH_MAX || 
			    elf_ppnt->p_filesz < 2)
				goto out_free_ph;

			retval = -ENOMEM;
			elf_interpreter = malloc(elf_ppnt->p_filesz);
			if (!elf_interpreter)
				goto out_free_ph;

			pos = elf_ppnt->p_offset;
			retval = lseek(file, pos, SEEK_SET);
			if (retval != pos) {
				if (retval >= 0)
					retval = -EIO;
				goto out_free_interp;
			}
			retval = read(file, elf_interpreter,
					     elf_ppnt->p_filesz);
			if (retval != (int)elf_ppnt->p_filesz) {
				if (retval >= 0)
					retval = -EIO;
				goto out_free_interp;
			}
			/* make sure path is NULL terminated */
			retval = -ENOEXEC;
			if (elf_interpreter[elf_ppnt->p_filesz - 1] != '\0')
				goto out_free_interp;

			interpreter = open(elf_interpreter, O_LARGEFILE | O_RDONLY);
			retval = interpreter;
			if (interpreter < 0)
				goto out_free_interp;

			/* Get the exec headers */
			pos = 0;
			retval = lseek(interpreter, pos, SEEK_SET);
			if (retval != pos) {
				if (retval >= 0)
					retval = -EIO;
				goto out_free_dentry;
			}

			retval = read(interpreter, &loc->interp_elf_ex,
					     sizeof(loc->interp_elf_ex));
			if (retval != sizeof(loc->interp_elf_ex)) {
				if (retval >= 0)
					retval = -EIO;
				goto out_free_dentry;
			}

			break;
		}
		elf_ppnt++;
	}

	elf_ppnt = elf_phdata;
	for (i = 0; i < loc->elf_ex.e_phnum; i++, elf_ppnt++)
		switch (elf_ppnt->p_type) {
		case PT_GNU_STACK:
			if (elf_ppnt->p_flags & PF_X)
				executable_stack = EXSTACK_ENABLE_X;
			else
				executable_stack = EXSTACK_DISABLE_X;
			break;

		case PT_LOPROC ... PT_HIPROC:
			retval = arch_elf_pt_proc(&loc->elf_ex, elf_ppnt,
						  file, false,
						  &arch_state);
			if (retval)
				goto out_free_dentry;
			break;
		}

	/* Some simple consistency checks for the interpreter */
	if (elf_interpreter) {
		retval = -ELIBBAD;
		/* Not an ELF interpreter */
		if (memcmp(loc->interp_elf_ex.e_ident, ELFMAG, SELFMAG) != 0)
			goto out_free_dentry;
		/* Verify the interpreter has a valid arch */
		if (!elf_check_arch(&loc->interp_elf_ex))
			goto out_free_dentry;

		/* Load the interpreter program headers */
		interp_elf_phdata = load_elf_phdrs(&loc->interp_elf_ex,
						   interpreter);
		if (!interp_elf_phdata)
			goto out_free_dentry;

		/* Pass PT_LOPROC..PT_HIPROC headers to arch code */
		elf_ppnt = interp_elf_phdata;
		for (i = 0; i < loc->interp_elf_ex.e_phnum; i++, elf_ppnt++)
			switch (elf_ppnt->p_type) {
			case PT_LOPROC ... PT_HIPROC:
				retval = arch_elf_pt_proc(&loc->interp_elf_ex,
							  elf_ppnt, interpreter,
							  true, &arch_state);
				if (retval)
					goto out_free_dentry;
				break;
			}
	}

	/*
	 * Allow arch code to reject the ELF at this point, whilst it's
	 * still possible to return an error to the code that invoked
	 * the exec syscall.
	 */
	retval = arch_check_elf(&loc->elf_ex,
				!!interpreter, &loc->interp_elf_ex,
				&arch_state);
	if (retval)
		goto out_free_dentry;

	/* Now we do a little grungy work by mmapping the ELF image into
	   the correct location in memory. */
	for(i = 0, elf_ppnt = elf_phdata;
	    i < loc->elf_ex.e_phnum; i++, elf_ppnt++) {
		int elf_prot = 0, elf_flags;
		unsigned long k, vaddr;
		unsigned long total_size = 0;

		if (elf_ppnt->p_type != PT_LOAD)
			continue;

		if (elf_brk > elf_bss) {
			unsigned long nbyte;
	            
			/* There was a PT_LOAD segment with p_memsz > p_filesz
			   before this one. Map anonymous pages, if needed,
			   and clear the area.  */
			retval = set_brk(elf_bss + load_bias,
					 elf_brk + load_bias,
					 bss_prot);
			if (retval)
				goto out_free_dentry;
			nbyte = ELF_PAGEOFFSET(elf_bss);
			if (nbyte) {
				nbyte = ELF_MIN_ALIGN - nbyte;
				if (nbyte > elf_brk - elf_bss)
					nbyte = elf_brk - elf_bss;
				if (clear_user((void *)(elf_bss + load_bias), nbyte)) {
					/*
					 * This bss-zeroing can fail if the ELF
					 * file specifies odd protections. So
					 * we don't check the return value
					 */
				}
			}
		}

		if (elf_ppnt->p_flags & PF_R)
			elf_prot |= PROT_READ;
		if (elf_ppnt->p_flags & PF_W)
			elf_prot |= PROT_WRITE;
		if (elf_ppnt->p_flags & PF_X)
			elf_prot |= PROT_EXEC;

		elf_flags = MAP_PRIVATE | MAP_DENYWRITE | MAP_EXECUTABLE;

		vaddr = elf_ppnt->p_vaddr;
		/*
		 * If we are loading ET_EXEC or we have already performed
		 * the ET_DYN load_addr calculations, proceed normally.
		 */
		if (loc->elf_ex.e_type == ET_EXEC || load_addr_set) {
			elf_flags |= MAP_FIXED;
		} else if (loc->elf_ex.e_type == ET_DYN) {
			/*
			 * This logic is run once for the first LOAD Program
			 * Header for ET_DYN binaries to calculate the
			 * randomization (load_bias) for all the LOAD
			 * Program Headers, and to calculate the entire
			 * size of the ELF mapping (total_size). (Note that
			 * load_addr_set is set to true later once the
			 * initial mapping is performed.)
			 *
			 * There are effectively two types of ET_DYN
			 * binaries: programs (i.e. PIE: ET_DYN with INTERP)
			 * and loaders (ET_DYN without INTERP, since they
			 * _are_ the ELF interpreter). The loaders must
			 * be loaded away from programs since the program
			 * may otherwise collide with the loader (especially
			 * for ET_EXEC which does not have a randomized
			 * position). For example to handle invocations of
			 * "./ld.so someprog" to test out a new version of
			 * the loader, the subsequent program that the
			 * loader loads must avoid the loader itself, so
			 * they cannot share the same load range. Sufficient
			 * room for the brk must be allocated with the
			 * loader as well, since brk must be available with
			 * the loader.
			 *
			 * Therefore, programs are loaded offset from
			 * ELF_ET_DYN_BASE and loaders are loaded into the
			 * independently randomized mmap region (0 load_bias
			 * without MAP_FIXED).
			 */
			if (elf_interpreter) {
				load_bias = ELF_ET_DYN_BASE;
				elf_flags |= MAP_FIXED;
			} else
				load_bias = 0;

			/*
			 * Since load_bias is used for all subsequent loading
			 * calculations, we must lower it by the first vaddr
			 * so that the remaining calculations based on the
			 * ELF vaddrs will be correctly offset. The result
			 * is then page aligned.
			 */
			load_bias = ELF_PAGESTART(load_bias - vaddr);

			total_size = total_mapping_size(elf_phdata,
							loc->elf_ex.e_phnum);
			if (!total_size) {
				retval = -EINVAL;
				goto out_free_dentry;
			}
		}

		error = elf_map(file, load_bias + vaddr, elf_ppnt,
				elf_prot, elf_flags, total_size);
		if (BAD_ADDR(error)) {
			retval = IS_ERR((void *)error) ?
				PTR_ERR((void*)error) : -EINVAL;
			goto out_free_dentry;
		}

		if (!load_addr_set) {
			load_addr_set = 1;
			load_addr = (elf_ppnt->p_vaddr - elf_ppnt->p_offset);
			if (loc->elf_ex.e_type == ET_DYN) {
				load_bias += error -
				             ELF_PAGESTART(load_bias + vaddr);
				load_addr += load_bias;
				reloc_func_desc = load_bias;
			}
		}
		k = elf_ppnt->p_vaddr;
		if (k < start_code)
			start_code = k;
		if (start_data < k)
			start_data = k;

		/*
		 * Check to see if the section's size will overflow the
		 * allowed task size. Note that p_filesz must always be
		 * <= p_memsz so it is only necessary to check p_memsz.
		 */
		if (BAD_ADDR(k) || elf_ppnt->p_filesz > elf_ppnt->p_memsz ||
		    elf_ppnt->p_memsz > TASK_SIZE ||
		    TASK_SIZE - elf_ppnt->p_memsz < k) {
			/* set_brk can never work. Avoid overflows. */
			retval = -EINVAL;
			goto out_free_dentry;
		}

		k = elf_ppnt->p_vaddr + elf_ppnt->p_filesz;

		if (k > elf_bss)
			elf_bss = k;
		if ((elf_ppnt->p_flags & PF_X) && end_code < k)
			end_code = k;
		if (end_data < k)
			end_data = k;
		k = elf_ppnt->p_vaddr + elf_ppnt->p_memsz;
		if (k > elf_brk) {
			bss_prot = elf_prot;
			elf_brk = k;
		}
	}

	loc->elf_ex.e_entry += load_bias;
	elf_bss += load_bias;
	elf_brk += load_bias;
	start_code += load_bias;
	end_code += load_bias;
	start_data += load_bias;
	end_data += load_bias;

	/* Calling set_brk effectively mmaps the pages that we need
	 * for the bss and break sections.  We must do this before
	 * mapping in the interpreter, to make sure it doesn't wind
	 * up getting placed where the bss needs to go.
	 */
	retval = set_brk(elf_bss, elf_brk, bss_prot);
	if (retval)
		goto out_free_dentry;
	if ((elf_bss != elf_brk) && (padzero(elf_bss))) {
		retval = -EFAULT; /* Nobody gets to see this, but.. */
		goto out_free_dentry;
	}

	if (elf_interpreter) {
		unsigned long interp_map_addr = 0;

		elf_entry = load_elf_interp(&loc->interp_elf_ex,
					    interpreter,
					    &interp_map_addr,
					    load_bias, interp_elf_phdata);
		if (!IS_ERR((void *)elf_entry)) {
			/*
			 * load_elf_interp() returns relocation
			 * adjustment
			 */
			interp_load_addr = elf_entry;
			elf_entry += loc->interp_elf_ex.e_entry;
		}
		if (BAD_ADDR(elf_entry)) {
			retval = IS_ERR((void *)elf_entry) ?
					(int)elf_entry : -EINVAL;
			goto out_free_dentry;
		}
		reloc_func_desc = interp_load_addr;

	} else {
		elf_entry = loc->elf_ex.e_entry;
		if (BAD_ADDR(elf_entry)) {
			retval = -EINVAL;
			goto out_free_dentry;
		}
	}

	free(interp_elf_phdata);
	free(elf_phdata);

	int stack_size = PAGE_SIZE;
#if 0
	// FIXME: Very Very Dangerous !
	// Can not come back !
	struct stack_frame {
		struct stack_frame* previous;	// pointer to previous frame
		void* caller;			// caller address
		void* first;			// first argument, maybe.
	};

	struct stack_frame* frame = (struct stack_frame*)__builtin_frame_address(0);
	while(frame && (unsigned long)frame < TASK_SIZE) {
		if (frame >= frame->previous) break;
		frame = frame->previous;
	}
	char* user_stack = (char *)(frame - stack_size);
#else
	char* user_stack = (char *)__builtin_alloca_with_align (PAGE_SIZE, 16);
#endif

	// clear user stack
	memset(user_stack, 0, stack_size);

	// aligned to 16
	char *stack = (void *)(((unsigned long)user_stack + 15) / 16 * 16);
	ElfW(Addr)* args = (ElfW(Addr)*)(stack + stack_size / 2);
	int args_index = 0;

	// save argc
	args[args_index++] = (ElfW(Addr))argc;

	// save argv[]
	for (i = 0; i < argc; i++) {
		args[args_index++] = (ElfW(Addr))argv[i];
	}
	args[args_index++] = 0;

	// save environment[]
	for (i = 0; i < envc; i++) {
		args[args_index++] = (ElfW(Addr))envp[i];
	}
	args[args_index++] = 0;

	ElfW(Addr)* auxv = &args[args_index];
	retval = create_elf_tables(auxv, &loc->elf_ex,
			  load_addr, interp_load_addr);
	if (retval < 0)
		goto out;

	if (0/* MMAP_PAGE_ZERO */) {
		/* Why this, you ask???  Well SVr4 maps page 0 as read-only,
		   and some applications "depend" upon this behavior.
		   Since we do not have the power to recompile these, we
		   emulate the SVr4 behavior. Sigh. */
		error = (unsigned long)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_EXEC,
				MAP_FIXED | MAP_PRIVATE, 0, 0);
	}

	// copy from musl-libc
#if defined(__arm__)

#define CRTJMP(pc,sp) __asm__ __volatile__( \
	"mov sp,%1 ; bx %0" : : "r"(pc), "r"(sp) : "memory" )

#elif defined(__aarch64__)

#define CRTJMP(pc,sp) __asm__ __volatile__( \
	"mov sp,%1 ; br %0" : : "r"(pc), "r"(sp) : "memory" )

#elif defined(__i386__)

#define CRTJMP(pc,sp) __asm__ __volatile__( \
	"mov %1,%%esp ; jmp *%0" : : "r"(pc), "r"(sp) : "memory" )

#elif defined(__mips__)

#define CRTJMP(pc,sp) __asm__ __volatile__( \
	"move $sp,%1 ; jr %0" : : "r"(pc), "r"(sp) : "memory" )

#elif defined(__x86_64__)

#define CRTJMP(pc,sp) __asm__ __volatile__( \
	"mov %1,%%rsp ; jmp *%0" : : "r"(pc), "r"(sp) : "memory" )

#endif

	// execute programme in current thread
	//printf("Ready to call: 0x%08lx\n", elf_entry);
	CRTJMP((void *)elf_entry, args);

	retval = 0;
out:
	free(loc);
out_ret:
	return retval;

	/* error cleanup */
out_free_dentry:
	free(interp_elf_phdata);
out_free_interp:
	free(elf_interpreter);
out_free_ph:
	free(elf_phdata);
	goto out;
}

