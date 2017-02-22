/* Copyright (C) 2011
 * Corscience GmbH & Co. KG - Simon Schwarz <schwarz@corscience.de>
 *  - Added prep subcommand support
 *  - Reorganized source - modeled after powerpc version
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <image.h>
#include <u-boot/zlib.h>
#include <asm/byteorder.h>
#include <libfdt.h>
#include <fdt_support.h>
#if defined (MT7623_ASIC_BOARD)
#include <configs/mt7623_evb.h>
#elif defined (MT7622_ASIC_BOARD) || defined (MT7622_FPGA_BOARD)
#include <configs/mt7622_evb.h>
#endif
#include <asm/bootm.h>
#include <linux/compiler.h>

#if defined(CONFIG_ARMV7_NONSEC) || defined(CONFIG_ARMV7_VIRT)
#include <asm/armv7.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

static struct tag *params;

static ulong get_sp(void)
{
	ulong ret;

	asm("mov %0, sp" : "=r"(ret) : );
	return ret;
}

void arch_lmb_reserve(struct lmb *lmb)
{
	ulong sp;

	/*
	 * Booting a (Linux) kernel image
	 *
	 * Allocate space for command line and board info - the
	 * address should be as high as possible within the reach of
	 * the kernel (see CONFIG_SYS_BOOTMAPSZ settings), but in unused
	 * memory, which means far enough below the current stack
	 * pointer.
	 */
	sp = get_sp();
	debug("## Current stack ends at 0x%08lx ", sp);

	/* adjust sp by 4K to be safe */
	sp -= 4096;
	lmb_reserve(lmb, sp,
		    gd->bd->bi_dram[0].start + gd->bd->bi_dram[0].size - sp);
}

/**
 * announce_and_cleanup() - Print message and prepare for kernel boot
 *
 * @fake: non-zero to do everything except actually boot
 */
static void announce_and_cleanup(int fake)
{
	printf("\nStarting kernel ...%s\n\n", fake ?
		"(fake run for tracing)" : "");
	bootstage_mark_name(BOOTSTAGE_ID_BOOTM_HANDOFF, "start_kernel");
#ifdef CONFIG_BOOTSTAGE_FDT
	if (flag == BOOTM_STATE_OS_FAKE_GO)
		bootstage_fdt_add_report();
#endif
#ifdef CONFIG_BOOTSTAGE_REPORT
	bootstage_report();
#endif

#ifdef CONFIG_USB_DEVICE
	udc_disconnect();
#endif
	cleanup_before_linux();
}

static void setup_start_tag (bd_t *bd)
{
	params = (struct tag *)bd->bi_boot_params;

	params->hdr.tag = ATAG_CORE;
	params->hdr.size = tag_size (tag_core);

	params->u.core.flags = 0;
	params->u.core.pagesize = 0;
	params->u.core.rootdev = 0;

	params = tag_next (params);
}

static void setup_memory_tags(bd_t *bd)
{
	int i;

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		params->hdr.tag = ATAG_MEM;
		params->hdr.size = tag_size (tag_mem32);

		params->u.mem.start = bd->bi_dram[i].start;
		params->u.mem.size = bd->bi_dram[i].size;

		params = tag_next (params);
	}
}

static void setup_commandline_tag(bd_t *bd, char *commandline)
{
	char *p;

	if (!commandline)
		return;

	/* eat leading white space */
	for (p = commandline; *p == ' '; p++);

	/* skip non-existent command lines so the kernel will still
	 * use its default command line.
	 */
	if (*p == '\0')
		return;

	params->hdr.tag = ATAG_CMDLINE;
	params->hdr.size =
		(sizeof (struct tag_header) + strlen (p) + 1 + 4) >> 2;

	strcpy (params->u.cmdline.cmdline, p);

	params = tag_next (params);
}

static void setup_initrd_tag(bd_t *bd, ulong initrd_start, ulong initrd_end)
{
	/* an ATAG_INITRD node tells the kernel where the compressed
	 * ramdisk can be found. ATAG_RDIMG is a better name, actually.
	 */
	params->hdr.tag = ATAG_INITRD2;
	params->hdr.size = tag_size (tag_initrd);

	params->u.initrd.start = initrd_start;
	params->u.initrd.size = initrd_end - initrd_start;

	params = tag_next (params);
}

static void setup_serial_tag(struct tag **tmp)
{
	struct tag *params = *tmp;
	struct tag_serialnr serialnr;

	get_board_serial(&serialnr);
	params->hdr.tag = ATAG_SERIAL;
	params->hdr.size = tag_size (tag_serialnr);
	params->u.serialnr.low = serialnr.low;
	params->u.serialnr.high= serialnr.high;
	params = tag_next (params);
	*tmp = params;
}

static void setup_revision_tag(struct tag **in_params)
{
	u32 rev = 0;

	rev = get_board_rev();
	params->hdr.tag = ATAG_REVISION;
	params->hdr.size = tag_size (tag_revision);
	params->u.revision.rev = rev;
	params = tag_next (params);
}
#if defined (NAND_ABTC_ATAG)
static void setup_nand_tag(struct tag **in_params)
{
	int i,j;
	extern unsigned int flash_number;
	params->hdr.tag = ATAG_FLASH_NUMBER_INFO;	
	params->hdr.size = tag_size (tag_nand_number);
		
	params->u.tag_nand_number.number = flash_number;
	params = tag_next (params);

	for(i = 0; i < flash_number; i++)
	{
		//printf("FLASH_INFO[%d] name=%s\n",i,gen_FlashTable[i].devciename);
		params->hdr.tag = ATAG_FLASH_INFO;
		params->hdr.size = ((sizeof(struct tag_header) + sizeof(flashdev_info)) >> 2);	
		memset(&params->u.gen_FlashTable_p, 0, sizeof(flashdev_info));
		memcpy(&params->u.gen_FlashTable_p, &gen_FlashTable[i],  sizeof(flashdev_info)); 
        //printf("ATAG_FLASH_INFO[%d] addr=%x name=%s size=%d\n",i,&params->u.gen_FlashTable_p,params->u.gen_FlashTable_p.devciename,params->hdr.size);
		params = tag_next (params);
	}

	return ;
}
#endif
static void setup_end_tag(bd_t *bd)
{
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;
}

__weak void setup_board_tags(struct tag **in_params) {}

static void do_nonsec_virt_switch(void)
{
#if defined(CONFIG_ARMV7_NONSEC) || defined(CONFIG_ARMV7_VIRT)
	if (armv7_switch_nonsec() == 0)
#ifdef CONFIG_ARMV7_VIRT
		if (armv7_switch_hyp() == 0)
			debug("entered HYP mode\n");
#else
		debug("entered non-secure state\n");
#endif
#endif

#ifdef CONFIG_ARM64
	smp_kick_all_cpus();
	armv8_switch_to_el2();
#ifdef CONFIG_ARMV8_SWITCH_TO_EL1
	armv8_switch_to_el1();
#endif
#endif
}

/* Subcommand: PREP */
static void boot_prep_linux(bootm_headers_t *images)
{
	char *commandline = getenv("bootargs");

	if (IMAGE_ENABLE_OF_LIBFDT && images->ft_len) {
#ifdef CONFIG_OF_LIBFDT
		debug("using: FDT\n");
		if (image_setup_linux(images)) {
			printf("FDT creation failed! hanging...");
			hang();
		}
#endif
	} else if (BOOTM_ENABLE_TAGS) {
		debug("using: ATAGS\n");
#if 1   /* Nelson: workaround to ignore atags for kernel */
		setup_start_tag(gd->bd);
		if (BOOTM_ENABLE_SERIAL_TAG)
			setup_serial_tag(&params);
		if (BOOTM_ENABLE_CMDLINE_TAG)
			setup_commandline_tag(gd->bd, commandline);
		if (BOOTM_ENABLE_REVISION_TAG)
			setup_revision_tag(&params);
		if (BOOTM_ENABLE_MEMORY_TAGS)
			setup_memory_tags(gd->bd);
		if (BOOTM_ENABLE_INITRD_TAG) {
			if (images->rd_start && images->rd_end) {
				setup_initrd_tag(gd->bd, images->rd_start,
						 images->rd_end);
			}
		}
#if defined (NAND_ABTC_ATAG)		
		if (BOOTM_ENABLE_NAND_TAG)
			setup_nand_tag(&params);
#endif
		setup_board_tags(&params);
		setup_end_tag(gd->bd);
#endif
	} else {
		printf("FDT and ATAGS support not compiled in - hanging\n");
		hang();
	}
	do_nonsec_virt_switch();
}

/* Subcommand: GO */
static void boot_jump_linux(bootm_headers_t *images, int flag)
{
#ifdef CONFIG_ARM64
	void (*kernel_entry)(void *fdt_addr);
	int fake = (flag & BOOTM_STATE_OS_FAKE_GO);

	kernel_entry = (void (*)(void *fdt_addr))images->ep;

	debug("## Transferring control to Linux (at address %lx)...\n",
		(ulong) kernel_entry);
	bootstage_mark(BOOTSTAGE_ID_RUN_OS);

	announce_and_cleanup(fake);

	if (!fake)
		kernel_entry(images->ft_addr);
#elif defined(CONFIG_MTK_ATF)
    extern void jumparch64_smc(ulong addr, ulong arg1, ulong arg2);

    /* 
     * Iverson 20150528 - show message for boot.
     */
	debug("images->ep = %lx, images->ft_addr = %lx\n", images->ep, (unsigned long)images->ft_addr);
    
	announce_and_cleanup(0);
    jumparch64_smc(images->ep, (unsigned long)images->ft_addr, 0);
#else
	unsigned long machid = gd->bd->bi_arch_number;
	char *s;
	void (*kernel_entry)(int zero, int arch, uint params);
	unsigned long r2;
	int fake = (flag & BOOTM_STATE_OS_FAKE_GO);

	kernel_entry = (void (*)(int, int, uint))images->ep;

	s = getenv("machid");
	if (s) {
		strict_strtoul(s, 16, &machid);
		printf("Using machid 0x%lx from environment\n", machid);
	}

	debug("## Transferring control to Linux (at address %08lx)" \
		"...\n", (ulong) kernel_entry);
	bootstage_mark(BOOTSTAGE_ID_RUN_OS);
	announce_and_cleanup(fake);

	if (IMAGE_ENABLE_OF_LIBFDT && images->ft_len)
		r2 = (unsigned long)images->ft_addr;
	else
		r2 = gd->bd->bi_boot_params;

	if (!fake){
		kernel_entry(0, machid, r2);
   }
#endif
}

/* Main Entry point for arm bootm implementation
 *
 * Modeled after the powerpc implementation
 * DIFFERENCE: Instead of calling prep and go at the end
 * they are called if subcommand is equal 0.
 */
int do_bootm_linux(int flag, int argc, char *argv[], bootm_headers_t *images)
{
	/* No need for those on ARM */
	if (flag & BOOTM_STATE_OS_BD_T || flag & BOOTM_STATE_OS_CMDLINE)
		return -1;

	if (flag & BOOTM_STATE_OS_PREP) {
		boot_prep_linux(images);
		return 0;
	}

	if (flag & (BOOTM_STATE_OS_GO | BOOTM_STATE_OS_FAKE_GO)) {
		boot_jump_linux(images, flag);
		return 0;
	}

	boot_prep_linux(images);
	boot_jump_linux(images, flag);
	return 0;
}

#ifdef CONFIG_CMD_BOOTZ

struct zimage_header {
	uint32_t	code[9];
	uint32_t	zi_magic;
	uint32_t	zi_start;
	uint32_t	zi_end;
};

#define	LINUX_ARM_ZIMAGE_MAGIC	0x016f2818

int bootz_setup(ulong image, ulong *start, ulong *end)
{
	struct zimage_header *zi;

	zi = (struct zimage_header *)map_sysmem(image, 0);
	if (zi->zi_magic != LINUX_ARM_ZIMAGE_MAGIC) {
		puts("Bad Linux ARM zImage magic!\n");
		return 1;
	}

	*start = zi->zi_start;
	*end = zi->zi_end;

	printf("Kernel image @ %#08lx [ %#08lx - %#08lx ]\n", image, *start,
	      *end);

	return 0;
}

#endif	/* CONFIG_CMD_BOOTZ */

#if defined(CONFIG_BOOTM_VXWORKS)
void boot_prep_vxworks(bootm_headers_t *images)
{
#if defined(CONFIG_OF_LIBFDT)
	int off;

	if (images->ft_addr) {
		off = fdt_path_offset(images->ft_addr, "/memory");
		if (off < 0) {
			if (arch_fixup_memory_node(images->ft_addr))
				puts("## WARNING: fixup memory failed!\n");
		}
	}
#endif
	cleanup_before_linux();
}
void boot_jump_vxworks(bootm_headers_t *images)
{
	/* ARM VxWorks requires device tree physical address to be passed */
	((void (*)(void *))images->ep)(images->ft_addr);
}
#endif
