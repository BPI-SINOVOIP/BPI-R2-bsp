/*
 * (C) Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */


/*
 * Boot support
 */
#include <common.h>
#include <watchdog.h>
#include <command.h>
#include <image.h>
#include <malloc.h>
#include <u-boot/zlib.h>
#include <bzlib.h>
#include <environment.h>
#include <lmb.h>
#include <linux/ctype.h>
#include <asm/byteorder.h>
#include <asm/io.h>
#include <linux/compiler.h>

#if defined(CONFIG_BOOTM_VXWORKS) && \
	(defined(CONFIG_PPC) || defined(CONFIG_ARM))
#include <vxworks.h>
#endif

#if defined(CONFIG_CMD_USB)
#include <usb.h>
#endif

#ifdef CONFIG_SYS_HUSH_PARSER
#include <hush.h>
#endif

#if defined(CONFIG_OF_LIBFDT)
#include <libfdt.h>
#include <fdt_support.h>
#endif

#ifdef CONFIG_LZMA
#include <lzma/LzmaTypes.h>
#include <lzma/LzmaDec.h>
#include <lzma/LzmaTools.h>
#endif /* CONFIG_LZMA */

#ifdef CONFIG_LZO
#include <linux/lzo.h>
#endif /* CONFIG_LZO */

DECLARE_GLOBAL_DATA_PTR;

#ifndef CONFIG_SYS_BOOTM_LEN
#define CONFIG_SYS_BOOTM_LEN	0x800000	/* use 8MByte as default max gunzip size */
#endif

#ifdef CONFIG_BZIP2
extern void bz_internal_error(int);
#endif

#if defined(CONFIG_CMD_IMI)
static int image_info(unsigned long addr);
#endif

#if defined(CONFIG_CMD_IMLS)
#include <flash.h>
#include <mtd/cfi_flash.h>
extern flash_info_t flash_info[]; /* info for FLASH chips */
#endif

#if defined(CONFIG_CMD_IMLS) || defined(CONFIG_CMD_IMLS_NAND)
static int do_imls(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
#endif

#include <linux/err.h>
#include <nand.h>

#if defined(CONFIG_SILENT_CONSOLE) && !defined(CONFIG_SILENT_U_BOOT_ONLY)
static void fixup_silent_linux(void);
#endif

static int do_bootm_standalone(int flag, int argc, char * const argv[],
			       bootm_headers_t *images);

static const void *boot_get_kernel(cmd_tbl_t *cmdtp, int flag, int argc,
				char * const argv[], bootm_headers_t *images,
				ulong *os_data, ulong *os_len);

/*
 *  Continue booting an OS image; caller already has:
 *  - copied image header to global variable `header'
 *  - checked header magic number, checksums (both header & image),
 *  - verified image architecture (PPC) and type (KERNEL or MULTI),
 *  - loaded (first part of) image to header load address,
 *  - disabled interrupts.
 *
 * @flag: Flags indicating what to do (BOOTM_STATE_...)
 * @argc: Number of arguments. Note that the arguments are shifted down
 *	 so that 0 is the first argument not processed by U-Boot, and
 *	 argc is adjusted accordingly. This avoids confusion as to how
 *	 many arguments are available for the OS.
 * @images: Pointers to os/initrd/fdt
 * @return 1 on error. On success the OS boots so this function does
 * not return.
 */
typedef int boot_os_fn(int flag, int argc, char * const argv[],
			bootm_headers_t *images);

#ifdef CONFIG_BOOTM_LINUX
extern boot_os_fn do_bootm_linux;
#endif
#ifdef CONFIG_BOOTM_NETBSD
static boot_os_fn do_bootm_netbsd;
#endif
#if defined(CONFIG_LYNXKDI)
static boot_os_fn do_bootm_lynxkdi;
extern void lynxkdi_boot(image_header_t *);
#endif
#ifdef CONFIG_BOOTM_RTEMS
static boot_os_fn do_bootm_rtems;
#endif
#if defined(CONFIG_BOOTM_OSE)
static boot_os_fn do_bootm_ose;
#endif
#if defined(CONFIG_BOOTM_PLAN9)
static boot_os_fn do_bootm_plan9;
#endif
#if defined(CONFIG_BOOTM_VXWORKS) && \
	(defined(CONFIG_PPC) || defined(CONFIG_ARM))
static boot_os_fn do_bootm_vxworks;
#endif
#if defined(CONFIG_CMD_ELF)
static boot_os_fn do_bootm_qnxelf;
int do_bootvx(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int do_bootelf(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
#endif
#if defined(CONFIG_INTEGRITY)
static boot_os_fn do_bootm_integrity;
#endif

static boot_os_fn *boot_os[] = {
	[IH_OS_U_BOOT] = do_bootm_standalone,
#ifdef CONFIG_BOOTM_LINUX
	[IH_OS_LINUX] = do_bootm_linux,
#endif
#ifdef CONFIG_BOOTM_NETBSD
	[IH_OS_NETBSD] = do_bootm_netbsd,
#endif
#ifdef CONFIG_LYNXKDI
	[IH_OS_LYNXOS] = do_bootm_lynxkdi,
#endif
#ifdef CONFIG_BOOTM_RTEMS
	[IH_OS_RTEMS] = do_bootm_rtems,
#endif
#if defined(CONFIG_BOOTM_OSE)
	[IH_OS_OSE] = do_bootm_ose,
#endif
#if defined(CONFIG_BOOTM_PLAN9)
	[IH_OS_PLAN9] = do_bootm_plan9,
#endif
#if defined(CONFIG_BOOTM_VXWORKS) && \
	(defined(CONFIG_PPC) || defined(CONFIG_ARM))
	[IH_OS_VXWORKS] = do_bootm_vxworks,
#endif
#if defined(CONFIG_CMD_ELF)
	[IH_OS_QNX] = do_bootm_qnxelf,
#endif
#ifdef CONFIG_INTEGRITY
	[IH_OS_INTEGRITY] = do_bootm_integrity,
#endif
};

bootm_headers_t images;		/* pointers to os/initrd/fdt images */

/* Allow for arch specific config before we boot */
static void __arch_preboot_os(void)
{
	/* please define platform specific arch_preboot_os() */
}
void arch_preboot_os(void) __attribute__((weak, alias("__arch_preboot_os")));

#define IH_INITRD_ARCH IH_ARCH_DEFAULT

#ifdef CONFIG_LMB
static void boot_start_lmb(bootm_headers_t *images)
{
	ulong		mem_start;
	phys_size_t	mem_size;

	lmb_init(&images->lmb);

	mem_start = getenv_bootm_low();
	mem_size = getenv_bootm_size();

	lmb_add(&images->lmb, (phys_addr_t)mem_start, mem_size);

	arch_lmb_reserve(&images->lmb);
	board_lmb_reserve(&images->lmb);
}
#else
#define lmb_reserve(lmb, base, size)
static inline void boot_start_lmb(bootm_headers_t *images) { }
#endif

static int bootm_start(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	memset((void *)&images, 0, sizeof(images));
	images.verify = getenv_yesno("verify");

	boot_start_lmb(&images);

	bootstage_mark_name(BOOTSTAGE_ID_BOOTM_START, "bootm_start");
	images.state = BOOTM_STATE_START;

	return 0;
}

static int bootm_find_os(cmd_tbl_t *cmdtp, int flag, int argc,
			 char * const argv[])
{
	const void *os_hdr;

	/* get kernel image header, start address and length */
	os_hdr = boot_get_kernel(cmdtp, flag, argc, argv,
			&images, &images.os.image_start, &images.os.image_len);
	if (images.os.image_len == 0) {
		puts("ERROR: can't get kernel image!\n");
		return 1;
	}

	/* get image parameters */
	switch (genimg_get_format(os_hdr)) {
	case IMAGE_FORMAT_LEGACY:
		images.os.type = image_get_type(os_hdr);
		images.os.comp = image_get_comp(os_hdr);
		images.os.os = image_get_os(os_hdr);

		images.os.end = image_get_image_end(os_hdr);
		images.os.load = image_get_load(os_hdr);
		break;
#if defined(CONFIG_FIT)
	case IMAGE_FORMAT_FIT:
		if (fit_image_get_type(images.fit_hdr_os,
					images.fit_noffset_os, &images.os.type)) {
			puts("Can't get image type!\n");
			bootstage_error(BOOTSTAGE_ID_FIT_TYPE);
			return 1;
		}

		if (fit_image_get_comp(images.fit_hdr_os,
					images.fit_noffset_os, &images.os.comp)) {
			puts("Can't get image compression!\n");
			bootstage_error(BOOTSTAGE_ID_FIT_COMPRESSION);
			return 1;
		}

		if (fit_image_get_os(images.fit_hdr_os,
					images.fit_noffset_os, &images.os.os)) {
			puts("Can't get image OS!\n");
			bootstage_error(BOOTSTAGE_ID_FIT_OS);
			return 1;
		}

		images.os.end = fit_get_end(images.fit_hdr_os);

		if (fit_image_get_load(images.fit_hdr_os, images.fit_noffset_os,
					&images.os.load)) {
			puts("Can't get image load address!\n");
			bootstage_error(BOOTSTAGE_ID_FIT_LOADADDR);
			return 1;
		}
		break;
#endif
	default:
		puts("ERROR: unknown image format type!\n");
		return 1;
	}

	/* find kernel entry point */
	if (images.legacy_hdr_valid) {
		images.ep = image_get_ep(&images.legacy_hdr_os_copy);
#if defined(CONFIG_FIT)
	} else if (images.fit_uname_os) {
		int ret;

		ret = fit_image_get_entry(images.fit_hdr_os,
					  images.fit_noffset_os, &images.ep);
		if (ret) {
			puts("Can't get entry point property!\n");
			return 1;
		}
#endif
	} else {
		puts("Could not find kernel entry point!\n");
		return 1;
	}

	if (images.os.type == IH_TYPE_KERNEL_NOLOAD) {
		images.os.load = images.os.image_start;
		images.ep += images.os.load;
	}

	images.os.start = (ulong)os_hdr;

	return 0;
}

static int bootm_find_ramdisk(int flag, int argc, char * const argv[])
{
	int ret;

	/* find ramdisk */
	ret = boot_get_ramdisk(argc, argv, &images, IH_INITRD_ARCH,
			       &images.rd_start, &images.rd_end);
	if (ret) {
		puts("Ramdisk image is corrupt or invalid\n");
		return 1;
	}

	return 0;
}

#if defined(CONFIG_OF_LIBFDT)
static int bootm_find_fdt(int flag, int argc, char * const argv[])
{
	int ret;

	/* find flattened device tree */
	ret = boot_get_fdt(flag, argc, argv, IH_ARCH_DEFAULT, &images,
			   &images.ft_addr, &images.ft_len);
	if (ret) {
		puts("Could not find a valid device tree\n");
		return 1;
	}

	set_working_fdt_addr(images.ft_addr);

	return 0;
}
#endif

static int bootm_find_other(cmd_tbl_t *cmdtp, int flag, int argc,
			    char * const argv[])
{
	if (((images.os.type == IH_TYPE_KERNEL) ||
	     (images.os.type == IH_TYPE_KERNEL_NOLOAD) ||
	     (images.os.type == IH_TYPE_MULTI)) &&
	    (images.os.os == IH_OS_LINUX ||
		 images.os.os == IH_OS_VXWORKS)) {
		if (bootm_find_ramdisk(flag, argc, argv))
			return 1;

#if defined(CONFIG_OF_LIBFDT)
		if (bootm_find_fdt(flag, argc, argv))
			return 1;
#endif
	}

	return 0;
}

#define BOOTM_ERR_RESET		-1
#define BOOTM_ERR_OVERLAP	-2
#define BOOTM_ERR_UNIMPLEMENTED	-3
static int bootm_load_os(bootm_headers_t *images, unsigned long *load_end,
		int boot_progress)
{
	image_info_t os = images->os;
	uint8_t comp = os.comp;
	ulong load = os.load;
	ulong blob_start = os.start;
	ulong blob_end = os.end;
	ulong image_start = os.image_start;
	ulong image_len = os.image_len;
	__maybe_unused uint unc_len = CONFIG_SYS_BOOTM_LEN;
	int no_overlap = 0;
	void *load_buf, *image_buf;
#if defined(CONFIG_LZMA) || defined(CONFIG_LZO)
	int ret;
#endif /* defined(CONFIG_LZMA) || defined(CONFIG_LZO) */

	const char *type_name = genimg_get_type_name(os.type);

	load_buf = map_sysmem(load, unc_len);
	image_buf = map_sysmem(image_start, image_len);
	switch (comp) {
	case IH_COMP_NONE:
		if (load == blob_start || load == image_start) {
			printf("   XIP %s ... ", type_name);
			no_overlap = 1;
		} else {
			printf("   Loading %s ... ", type_name);
			memmove_wd(load_buf, image_buf, image_len, CHUNKSZ);
		}
		*load_end = load + image_len;
		break;
#ifdef CONFIG_GZIP
	case IH_COMP_GZIP:
		printf("   Uncompressing %s ... ", type_name);
		if (gunzip(load_buf, unc_len, image_buf, &image_len) != 0) {
			puts("GUNZIP: uncompress, out-of-mem or overwrite "
				"error - must RESET board to recover\n");
			if (boot_progress)
				bootstage_error(BOOTSTAGE_ID_DECOMP_IMAGE);
			return BOOTM_ERR_RESET;
		}

		*load_end = load + image_len;
		break;
#endif /* CONFIG_GZIP */
#ifdef CONFIG_BZIP2
	case IH_COMP_BZIP2:
		printf("   Uncompressing %s ... ", type_name);
		/*
		 * If we've got less than 4 MB of malloc() space,
		 * use slower decompression algorithm which requires
		 * at most 2300 KB of memory.
		 */
		int i = BZ2_bzBuffToBuffDecompress(load_buf, &unc_len,
			image_buf, image_len,
			CONFIG_SYS_MALLOC_LEN < (4096 * 1024), 0);
		if (i != BZ_OK) {
			printf("BUNZIP2: uncompress or overwrite error %d "
				"- must RESET board to recover\n", i);
			if (boot_progress)
				bootstage_error(BOOTSTAGE_ID_DECOMP_IMAGE);
			return BOOTM_ERR_RESET;
		}

		*load_end = load + unc_len;
		break;
#endif /* CONFIG_BZIP2 */
#ifdef CONFIG_LZMA
	case IH_COMP_LZMA: {
		SizeT lzma_len = unc_len;
		printf("   Uncompressing %s ... ", type_name);

		ret = lzmaBuffToBuffDecompress(load_buf, &lzma_len,
					       image_buf, image_len);
		unc_len = lzma_len;
		if (ret != SZ_OK) {
			printf("LZMA: uncompress or overwrite error %d "
				"- must RESET board to recover\n", ret);
			bootstage_error(BOOTSTAGE_ID_DECOMP_IMAGE);
			return BOOTM_ERR_RESET;
		}
		*load_end = load + unc_len;
		break;
	}
#endif /* CONFIG_LZMA */
#ifdef CONFIG_LZO
	case IH_COMP_LZO: {
		size_t size;

		printf("   Uncompressing %s ... ", type_name);

		ret = lzop_decompress(image_buf, image_len, load_buf, &size);
		if (ret != LZO_E_OK) {
			printf("LZO: uncompress or overwrite error %d "
			      "- must RESET board to recover\n", ret);
			if (boot_progress)
				bootstage_error(BOOTSTAGE_ID_DECOMP_IMAGE);
			return BOOTM_ERR_RESET;
		}

		*load_end = load + size;
		break;
	}
#endif /* CONFIG_LZO */
	default:
		printf("Unimplemented compression type %d\n", comp);
		return BOOTM_ERR_UNIMPLEMENTED;
	}

	flush_cache(load, (*load_end - load) * sizeof(ulong));

	puts("OK\n");
	debug("   kernel loaded at 0x%08lx, end = 0x%08lx\n", load, *load_end);
	bootstage_mark(BOOTSTAGE_ID_KERNEL_LOADED);

	if (!no_overlap && (load < blob_end) && (*load_end > blob_start)) {
		debug("images.os.start = 0x%lX, images.os.end = 0x%lx\n",
			blob_start, blob_end);
		debug("images.os.load = 0x%lx, load_end = 0x%lx\n", load,
			*load_end);

		/* Check what type of image this is. */
		if (images->legacy_hdr_valid) {
			if (image_get_type(&images->legacy_hdr_os_copy)
					== IH_TYPE_MULTI)
				puts("WARNING: legacy format multi component image overwritten\n");
			return BOOTM_ERR_OVERLAP;
		} else {
			puts("ERROR: new format image overwritten - must RESET the board to recover\n");
			bootstage_error(BOOTSTAGE_ID_OVERWRITTEN);
			return BOOTM_ERR_RESET;
		}
	}

	return 0;
}

static int do_bootm_standalone(int flag, int argc, char * const argv[],
			       bootm_headers_t *images)
{
	char  *s;
	int   (*appl)(int, char * const []);

	/* Don't start if "autostart" is set to "no" */
	if (((s = getenv("autostart")) != NULL) && (strcmp(s, "no") == 0)) {
		setenv_hex("filesize", images->os.image_len);
		return 0;
	}
	appl = (int (*)(int, char * const []))images->ep;
	appl(argc, argv);
	return 0;
}

/* we overload the cmd field with our state machine info instead of a
 * function pointer */
static cmd_tbl_t cmd_bootm_sub[] = {
	U_BOOT_CMD_MKENT(start, 0, 1, (void *)BOOTM_STATE_START, "", ""),
	U_BOOT_CMD_MKENT(loados, 0, 1, (void *)BOOTM_STATE_LOADOS, "", ""),
#ifdef CONFIG_SYS_BOOT_RAMDISK_HIGH
	U_BOOT_CMD_MKENT(ramdisk, 0, 1, (void *)BOOTM_STATE_RAMDISK, "", ""),
#endif
#ifdef CONFIG_OF_LIBFDT
	U_BOOT_CMD_MKENT(fdt, 0, 1, (void *)BOOTM_STATE_FDT, "", ""),
#endif
	U_BOOT_CMD_MKENT(cmdline, 0, 1, (void *)BOOTM_STATE_OS_CMDLINE, "", ""),
	U_BOOT_CMD_MKENT(bdt, 0, 1, (void *)BOOTM_STATE_OS_BD_T, "", ""),
	U_BOOT_CMD_MKENT(prep, 0, 1, (void *)BOOTM_STATE_OS_PREP, "", ""),
	U_BOOT_CMD_MKENT(fake, 0, 1, (void *)BOOTM_STATE_OS_FAKE_GO, "", ""),
	U_BOOT_CMD_MKENT(go, 0, 1, (void *)BOOTM_STATE_OS_GO, "", ""),
};

static int boot_selected_os(int argc, char * const argv[], int state,
		bootm_headers_t *images, boot_os_fn *boot_fn)
{
	arch_preboot_os();
	boot_fn(state, argc, argv, images);

	/* Stand-alone may return when 'autostart' is 'no' */
	if (images->os.type == IH_TYPE_STANDALONE ||
	    state == BOOTM_STATE_OS_FAKE_GO) /* We expect to return */
		return 0;
	bootstage_error(BOOTSTAGE_ID_BOOT_OS_RETURNED);
#ifdef DEBUG
	puts("\n## Control returned to monitor - resetting...\n");
#endif
	return BOOTM_ERR_RESET;
}

/**
 * bootm_disable_interrupts() - Disable interrupts in preparation for load/boot
 *
 * @return interrupt flag (0 if interrupts were disabled, non-zero if they were
 *	enabled)
 */
static ulong bootm_disable_interrupts(void)
{
	ulong iflag;

	/*
	 * We have reached the point of no return: we are going to
	 * overwrite all exception vector code, so we cannot easily
	 * recover from any failures any more...
	 */
	iflag = disable_interrupts();
#ifdef CONFIG_NETCONSOLE
	/* Stop the ethernet stack if NetConsole could have left it up */
	eth_halt();
	eth_unregister(eth_get_dev());
#endif

#if defined(CONFIG_CMD_USB)
	/*
	 * turn off USB to prevent the host controller from writing to the
	 * SDRAM while Linux is booting. This could happen (at least for OHCI
	 * controller), because the HCCA (Host Controller Communication Area)
	 * lies within the SDRAM and the host controller writes continously to
	 * this area (as busmaster!). The HccaFrameNumber is for example
	 * updated every 1 ms within the HCCA structure in SDRAM! For more
	 * details see the OpenHCI specification.
	 */
	usb_stop();
#endif
	return iflag;
}

/**
 * Execute selected states of the bootm command.
 *
 * Note the arguments to this state must be the first argument, Any 'bootm'
 * or sub-command arguments must have already been taken.
 *
 * Note that if states contains more than one flag it MUST contain
 * BOOTM_STATE_START, since this handles and consumes the command line args.
 *
 * Also note that aside from boot_os_fn functions and bootm_load_os no other
 * functions we store the return value of in 'ret' may use a negative return
 * value, without special handling.
 *
 * @param cmdtp		Pointer to bootm command table entry
 * @param flag		Command flags (CMD_FLAG_...)
 * @param argc		Number of subcommand arguments (0 = no arguments)
 * @param argv		Arguments
 * @param states	Mask containing states to run (BOOTM_STATE_...)
 * @param images	Image header information
 * @param boot_progress 1 to show boot progress, 0 to not do this
 * @return 0 if ok, something else on error. Some errors will cause this
 *	function to perform a reboot! If states contains BOOTM_STATE_OS_GO
 *	then the intent is to boot an OS, so this function will not return
 *	unless the image type is standalone.
 */
static int do_bootm_states(cmd_tbl_t *cmdtp, int flag, int argc,
		char * const argv[], int states, bootm_headers_t *images,
		int boot_progress)
{
	boot_os_fn *boot_fn;
	ulong iflag = 0;
	int ret = 0, need_boot_fn;

    // Iverson debug
    printf("bootm flag=%x, states=%x\n", flag, states);

	images->state |= states;

	/*
	 * Work through the states and see how far we get. We stop on
	 * any error.
	 */
	if (states & BOOTM_STATE_START)
		ret = bootm_start(cmdtp, flag, argc, argv);

	if (!ret && (states & BOOTM_STATE_FINDOS))
		ret = bootm_find_os(cmdtp, flag, argc, argv);

	if (!ret && (states & BOOTM_STATE_FINDOTHER)) {
		ret = bootm_find_other(cmdtp, flag, argc, argv);
		argc = 0;	/* consume the args */
	}

	/* Load the OS */
	if (!ret && (states & BOOTM_STATE_LOADOS)) {
		ulong load_end;

		iflag = bootm_disable_interrupts();
		ret = bootm_load_os(images, &load_end, 0);
		if (ret == 0)
			lmb_reserve(&images->lmb, images->os.load,
				    (load_end - images->os.load));
		else if (ret && ret != BOOTM_ERR_OVERLAP)
			goto err;
		else if (ret == BOOTM_ERR_OVERLAP)
			ret = 0;
#if defined(CONFIG_SILENT_CONSOLE) && !defined(CONFIG_SILENT_U_BOOT_ONLY)
		if (images->os.os == IH_OS_LINUX)
			fixup_silent_linux();
#endif
	}

	/* Relocate the ramdisk */
#ifdef CONFIG_SYS_BOOT_RAMDISK_HIGH
	if (!ret && (states & BOOTM_STATE_RAMDISK)) {
		ulong rd_len = images->rd_end - images->rd_start;

		ret = boot_ramdisk_high(&images->lmb, images->rd_start,
			rd_len, &images->initrd_start, &images->initrd_end);
		if (!ret) {
			setenv_hex("initrd_start", images->initrd_start);
			setenv_hex("initrd_end", images->initrd_end);
		}
	}
#endif
#if defined(CONFIG_OF_LIBFDT) && defined(CONFIG_LMB)
	if (!ret && (states & BOOTM_STATE_FDT)) {
		boot_fdt_add_mem_rsv_regions(&images->lmb, images->ft_addr);
		ret = boot_relocate_fdt(&images->lmb, &images->ft_addr,
					&images->ft_len);
	}
#endif

	/* From now on, we need the OS boot function */
	if (ret)
		return ret;
	boot_fn = boot_os[images->os.os];
	need_boot_fn = states & (BOOTM_STATE_OS_CMDLINE |
			BOOTM_STATE_OS_BD_T | BOOTM_STATE_OS_PREP |
			BOOTM_STATE_OS_FAKE_GO | BOOTM_STATE_OS_GO);
	if (boot_fn == NULL && need_boot_fn) {
		if (iflag)
			enable_interrupts();
		printf("ERROR: booting os '%s' (%d) is not supported\n",
		       genimg_get_os_name(images->os.os), images->os.os);
		bootstage_error(BOOTSTAGE_ID_CHECK_BOOT_OS);
		return 1;
	}

	/* Call various other states that are not generally used */
	if (!ret && (states & BOOTM_STATE_OS_CMDLINE))
		ret = boot_fn(BOOTM_STATE_OS_CMDLINE, argc, argv, images);
	if (!ret && (states & BOOTM_STATE_OS_BD_T))
		ret = boot_fn(BOOTM_STATE_OS_BD_T, argc, argv, images);
	if (!ret && (states & BOOTM_STATE_OS_PREP))
		ret = boot_fn(BOOTM_STATE_OS_PREP, argc, argv, images);

#ifdef CONFIG_TRACE
	/* Pretend to run the OS, then run a user command */
	if (!ret && (states & BOOTM_STATE_OS_FAKE_GO)) {
		char *cmd_list = getenv("fakegocmd");

		ret = boot_selected_os(argc, argv, BOOTM_STATE_OS_FAKE_GO,
				images, boot_fn);
		if (!ret && cmd_list)
			ret = run_command_list(cmd_list, -1, flag);
	}
#endif

	/* Check for unsupported subcommand. */
	if (ret) {
		puts("subcommand not supported\n");
		return ret;
	}

	/* Now run the OS! We hope this doesn't return */
	if (!ret && (states & BOOTM_STATE_OS_GO))
		ret = boot_selected_os(argc, argv, BOOTM_STATE_OS_GO,
				images, boot_fn);

	/* Deal with any fallout */
err:
	if (iflag)
		enable_interrupts();

	if (ret == BOOTM_ERR_UNIMPLEMENTED)
		bootstage_error(BOOTSTAGE_ID_DECOMP_UNIMPL);
	else if (ret == BOOTM_ERR_RESET)
		do_reset(cmdtp, flag, argc, argv);

	return ret;
}

static int do_bootm_subcommand(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	int ret = 0;
	long state;
	cmd_tbl_t *c;

	c = find_cmd_tbl(argv[0], &cmd_bootm_sub[0], ARRAY_SIZE(cmd_bootm_sub));
	argc--; argv++;

	if (c) {
		state = (long)c->cmd;
		if (state == BOOTM_STATE_START)
			state |= BOOTM_STATE_FINDOS | BOOTM_STATE_FINDOTHER;
	} else {
		/* Unrecognized command */
		return CMD_RET_USAGE;
	}

	if (state != BOOTM_STATE_START && images.state >= state) {
		printf("Trying to execute a command out of order\n");
		return CMD_RET_USAGE;
	}

	ret = do_bootm_states(cmdtp, flag, argc, argv, state, &images, 0);

	return ret;
}

/*******************************************************************/
/* bootm - boot application image from image in memory */
/*******************************************************************/

int do_bootm(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
#ifdef CONFIG_NEEDS_MANUAL_RELOC
	static int relocated = 0;

	if (!relocated) {
		int i;

		/* relocate boot function table */
		for (i = 0; i < ARRAY_SIZE(boot_os); i++)
			if (boot_os[i] != NULL)
				boot_os[i] += gd->reloc_off;

		/* relocate names of sub-command table */
		for (i = 0; i < ARRAY_SIZE(cmd_bootm_sub); i++)
			cmd_bootm_sub[i].name += gd->reloc_off;

		relocated = 1;
	}
#endif

	/* determine if we have a sub command */
	argc--; argv++;
	if (argc > 0) {
		char *endp;

		simple_strtoul(argv[0], &endp, 16);
		/* endp pointing to NULL means that argv[0] was just a
		 * valid number, pass it along to the normal bootm processing
		 *
		 * If endp is ':' or '#' assume a FIT identifier so pass
		 * along for normal processing.
		 *
		 * Right now we assume the first arg should never be '-'
		 */
		if ((*endp != 0) && (*endp != ':') && (*endp != '#'))
			return do_bootm_subcommand(cmdtp, flag, argc, argv);
	}

	return do_bootm_states(cmdtp, flag, argc, argv, BOOTM_STATE_START |
		BOOTM_STATE_FINDOS | BOOTM_STATE_FINDOTHER |
		BOOTM_STATE_LOADOS |
#if defined(CONFIG_PPC) || defined(CONFIG_MIPS)
		BOOTM_STATE_OS_CMDLINE |
#endif
		BOOTM_STATE_OS_PREP | BOOTM_STATE_OS_FAKE_GO |
		BOOTM_STATE_OS_GO, &images, 1);
}

int bootm_maybe_autostart(cmd_tbl_t *cmdtp, const char *cmd)
{
	const char *ep = getenv("autostart");

	if (ep && !strcmp(ep, "yes")) {
		char *local_args[2];
		local_args[0] = (char *)cmd;
		local_args[1] = NULL;
		printf("Automatic boot of image at addr 0x%08lX ...\n", load_addr);
		return do_bootm(cmdtp, 0, 1, local_args);
	}

	return 0;
}

/**
 * image_get_kernel - verify legacy format kernel image
 * @img_addr: in RAM address of the legacy format image to be verified
 * @verify: data CRC verification flag
 *
 * image_get_kernel() verifies legacy image integrity and returns pointer to
 * legacy image header if image verification was completed successfully.
 *
 * returns:
 *     pointer to a legacy image header if valid image was found
 *     otherwise return NULL
 */
static image_header_t *image_get_kernel(ulong img_addr, int verify)
{
	image_header_t *hdr = (image_header_t *)img_addr;

	if (!image_check_magic(hdr)) {
		puts("Bad Magic Number\n");
		bootstage_error(BOOTSTAGE_ID_CHECK_MAGIC);
		return NULL;
	}
	bootstage_mark(BOOTSTAGE_ID_CHECK_HEADER);

	if (!image_check_hcrc(hdr)) {
		puts("Bad Header Checksum\n");
		bootstage_error(BOOTSTAGE_ID_CHECK_HEADER);
		return NULL;
	}

	bootstage_mark(BOOTSTAGE_ID_CHECK_CHECKSUM);
	image_print_contents(hdr);

	if (verify) {
		puts("   Verifying Checksum ... ");
		if (!image_check_dcrc(hdr)) {
			printf("Bad Data CRC\n");
			bootstage_error(BOOTSTAGE_ID_CHECK_CHECKSUM);
			return NULL;
		}
		puts("OK\n");
	}
	bootstage_mark(BOOTSTAGE_ID_CHECK_ARCH);

	if (!image_check_target_arch(hdr)) {
		printf("Unsupported Architecture 0x%x\n", image_get_arch(hdr));
		bootstage_error(BOOTSTAGE_ID_CHECK_ARCH);
		return NULL;
	}
	return hdr;
}

/**
 * boot_get_kernel - find kernel image
 * @os_data: pointer to a ulong variable, will hold os data start address
 * @os_len: pointer to a ulong variable, will hold os data length
 *
 * boot_get_kernel() tries to find a kernel image, verifies its integrity
 * and locates kernel data.
 *
 * returns:
 *     pointer to image header if valid image was found, plus kernel start
 *     address and length, otherwise NULL
 */
static const void *boot_get_kernel(cmd_tbl_t *cmdtp, int flag, int argc,
		char * const argv[], bootm_headers_t *images, ulong *os_data,
		ulong *os_len)
{
	image_header_t	*hdr;
	ulong		img_addr;
	const void *buf;
#if defined(CONFIG_FIT)
	const char	*fit_uname_config = NULL;
	const char	*fit_uname_kernel = NULL;
	int		os_noffset;
#endif

	/* find out kernel image address */
	if (argc < 1) {
		img_addr = load_addr;
		debug("*  kernel: default image load address = 0x%08lx\n",
				load_addr);
#if defined(CONFIG_FIT)
	} else if (fit_parse_conf(argv[0], load_addr, &img_addr,
							&fit_uname_config)) {
		debug("*  kernel: config '%s' from image at 0x%08lx\n",
				fit_uname_config, img_addr);
	} else if (fit_parse_subimage(argv[0], load_addr, &img_addr,
							&fit_uname_kernel)) {
		debug("*  kernel: subimage '%s' from image at 0x%08lx\n",
				fit_uname_kernel, img_addr);
#endif
	} else {
		img_addr = simple_strtoul(argv[0], NULL, 16);
		debug("*  kernel: cmdline image address = 0x%08lx\n", img_addr);
	}

	bootstage_mark(BOOTSTAGE_ID_CHECK_MAGIC);

	/* copy from dataflash if needed */
	img_addr = genimg_get_image(img_addr);

	/* check image type, for FIT images get FIT kernel node */
	*os_data = *os_len = 0;
	buf = map_sysmem(img_addr, 0);
	switch (genimg_get_format(buf)) {
	case IMAGE_FORMAT_LEGACY:
		printf("## Booting kernel from Legacy Image at %08lx ...\n",
				img_addr);
		hdr = image_get_kernel(img_addr, images->verify);
		if (!hdr)
			return NULL;
		bootstage_mark(BOOTSTAGE_ID_CHECK_IMAGETYPE);

		/* get os_data and os_len */
		switch (image_get_type(hdr)) {
		case IH_TYPE_KERNEL:
		case IH_TYPE_KERNEL_NOLOAD:
			*os_data = image_get_data(hdr);
			*os_len = image_get_data_size(hdr);
			break;
		case IH_TYPE_MULTI:
			image_multi_getimg(hdr, 0, os_data, os_len);
			break;
		case IH_TYPE_STANDALONE:
			*os_data = image_get_data(hdr);
			*os_len = image_get_data_size(hdr);
			break;
		default:
			printf("Wrong Image Type for %s command\n",
				cmdtp->name);
			bootstage_error(BOOTSTAGE_ID_CHECK_IMAGETYPE);
			return NULL;
		}

		/*
		 * copy image header to allow for image overwrites during
		 * kernel decompression.
		 */
		memmove(&images->legacy_hdr_os_copy, hdr,
			sizeof(image_header_t));

		/* save pointer to image header */
		images->legacy_hdr_os = hdr;

		images->legacy_hdr_valid = 1;
		bootstage_mark(BOOTSTAGE_ID_DECOMP_IMAGE);
		break;
#if defined(CONFIG_FIT)
	case IMAGE_FORMAT_FIT:
		os_noffset = fit_image_load(images, FIT_KERNEL_PROP,
				img_addr,
				&fit_uname_kernel, &fit_uname_config,
				IH_ARCH_DEFAULT, IH_TYPE_KERNEL,
				BOOTSTAGE_ID_FIT_KERNEL_START,
				FIT_LOAD_IGNORED, os_data, os_len);
		if (os_noffset < 0)
			return NULL;

		images->fit_hdr_os = map_sysmem(img_addr, 0);
		images->fit_uname_os = fit_uname_kernel;
		images->fit_uname_cfg = fit_uname_config;
		images->fit_noffset_os = os_noffset;
		break;
#endif
	default:
		printf("Wrong Image Format for %s command\n", cmdtp->name);
		bootstage_error(BOOTSTAGE_ID_FIT_KERNEL_INFO);
		return NULL;
	}

	debug("   kernel data at 0x%08lx, len = 0x%08lx (%ld)\n",
			*os_data, *os_len, *os_len);

	return buf;
}

#ifdef CONFIG_SYS_LONGHELP
static char bootm_help_text[] =
	"[addr [arg ...]]\n    - boot application image stored in memory\n"
	"\tpassing arguments 'arg ...'; when booting a Linux kernel,\n"
	"\t'arg' can be the address of an initrd image\n"
#if defined(CONFIG_OF_LIBFDT)
	"\tWhen booting a Linux kernel which requires a flat device-tree\n"
	"\ta third argument is required which is the address of the\n"
	"\tdevice-tree blob. To boot that kernel without an initrd image,\n"
	"\tuse a '-' for the second argument. If you do not pass a third\n"
	"\ta bd_info struct will be passed instead\n"
#endif
#if defined(CONFIG_FIT)
	"\t\nFor the new multi component uImage format (FIT) addresses\n"
	"\tmust be extened to include component or configuration unit name:\n"
	"\taddr:<subimg_uname> - direct component image specification\n"
	"\taddr#<conf_uname>   - configuration specification\n"
	"\tUse iminfo command to get the list of existing component\n"
	"\timages and configurations.\n"
#endif
	"\nSub-commands to do part of the bootm sequence.  The sub-commands "
	"must be\n"
	"issued in the order below (it's ok to not issue all sub-commands):\n"
	"\tstart [addr [arg ...]]\n"
	"\tloados  - load OS image\n"
#if defined(CONFIG_SYS_BOOT_RAMDISK_HIGH)
	"\tramdisk - relocate initrd, set env initrd_start/initrd_end\n"
#endif
#if defined(CONFIG_OF_LIBFDT)
	"\tfdt     - relocate flat device tree\n"
#endif
	"\tcmdline - OS specific command line processing/setup\n"
	"\tbdt     - OS specific bd_t processing\n"
	"\tprep    - OS specific prep before relocation or go\n"
	"\tgo      - start OS";
#endif

U_BOOT_CMD(
	bootm,	CONFIG_SYS_MAXARGS,	1,	do_bootm,
	"boot application image from memory", bootm_help_text
);

/*******************************************************************/
/* bootd - boot default image */
/*******************************************************************/
#if defined(CONFIG_CMD_BOOTD)
int do_bootd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int rcode = 0;

	if (run_command(getenv("bootcmd"), flag) < 0)
		rcode = 1;
	return rcode;
}

U_BOOT_CMD(
	boot,	1,	1,	do_bootd,
	"boot default, i.e., run 'bootcmd'",
	""
);

/* keep old command name "bootd" for backward compatibility */
U_BOOT_CMD(
	bootd, 1,	1,	do_bootd,
	"boot default, i.e., run 'bootcmd'",
	""
);

#endif


/*******************************************************************/
/* iminfo - print header info for a requested image */
/*******************************************************************/
#if defined(CONFIG_CMD_IMI)
static int do_iminfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int	arg;
	ulong	addr;
	int	rcode = 0;

	if (argc < 2) {
		return image_info(load_addr);
	}

	for (arg = 1; arg < argc; ++arg) {
		addr = simple_strtoul(argv[arg], NULL, 16);
		if (image_info(addr) != 0)
			rcode = 1;
	}
	return rcode;
}

static int image_info(ulong addr)
{
	void *hdr = (void *)addr;

	printf("\n## Checking Image at %08lx ...\n", addr);

	switch (genimg_get_format(hdr)) {
	case IMAGE_FORMAT_LEGACY:
		puts("   Legacy image found\n");
		if (!image_check_magic(hdr)) {
			puts("   Bad Magic Number\n");
			return 1;
		}

		if (!image_check_hcrc(hdr)) {
			puts("   Bad Header Checksum\n");
			return 1;
		}

		image_print_contents(hdr);

		puts("   Verifying Checksum ... ");
		if (!image_check_dcrc(hdr)) {
			puts("   Bad Data CRC\n");
			return 1;
		}
		puts("OK\n");
		return 0;
#if defined(CONFIG_FIT)
	case IMAGE_FORMAT_FIT:
		puts("   FIT image found\n");

		if (!fit_check_format(hdr)) {
			puts("Bad FIT image format!\n");
			return 1;
		}

		fit_print_contents(hdr);

		if (!fit_all_image_verify(hdr)) {
			puts("Bad hash in FIT image!\n");
			return 1;
		}

		return 0;
#endif
	default:
		puts("Unknown image format!\n");
		break;
	}

	return 1;
}

U_BOOT_CMD(
	iminfo,	CONFIG_SYS_MAXARGS,	1,	do_iminfo,
	"print header information for application image",
	"addr [addr ...]\n"
	"    - print header information for application image starting at\n"
	"      address 'addr' in memory; this includes verification of the\n"
	"      image contents (magic number, header and payload checksums)"
);
#endif


/*******************************************************************/
/* imls - list all images found in flash */
/*******************************************************************/
#if defined(CONFIG_CMD_IMLS)
static int do_imls_nor(void)
{
	flash_info_t *info;
	int i, j;
	void *hdr;

	for (i = 0, info = &flash_info[0];
		i < CONFIG_SYS_MAX_FLASH_BANKS; ++i, ++info) {

		if (info->flash_id == FLASH_UNKNOWN)
			goto next_bank;
		for (j = 0; j < info->sector_count; ++j) {

			hdr = (void *)info->start[j];
			if (!hdr)
				goto next_sector;

			switch (genimg_get_format(hdr)) {
			case IMAGE_FORMAT_LEGACY:
				if (!image_check_hcrc(hdr))
					goto next_sector;

				printf("Legacy Image at %08lX:\n", (ulong)hdr);
				image_print_contents(hdr);

				puts("   Verifying Checksum ... ");
				if (!image_check_dcrc(hdr)) {
					puts("Bad Data CRC\n");
				} else {
					puts("OK\n");
				}
				break;
#if defined(CONFIG_FIT)
			case IMAGE_FORMAT_FIT:
				if (!fit_check_format(hdr))
					goto next_sector;

				printf("FIT Image at %08lX:\n", (ulong)hdr);
				fit_print_contents(hdr);
				break;
#endif
			default:
				goto next_sector;
			}

next_sector:		;
		}
next_bank:	;
	}
	return 0;
}
#endif

#if defined(CONFIG_CMD_IMLS_NAND)
static int nand_imls_legacyimage(nand_info_t *nand, int nand_dev, loff_t off,
		size_t len)
{
	void *imgdata;
	int ret;

	imgdata = malloc(len);
	if (!imgdata) {
		printf("May be a Legacy Image at NAND device %d offset %08llX:\n",
				nand_dev, off);
		printf("   Low memory(cannot allocate memory for image)\n");
		return -ENOMEM;
	}

	ret = nand_read_skip_bad(nand, off, &len,
			imgdata);
	if (ret < 0 && ret != -EUCLEAN) {
		free(imgdata);
		return ret;
	}

	if (!image_check_hcrc(imgdata)) {
		free(imgdata);
		return 0;
	}

	printf("Legacy Image at NAND device %d offset %08llX:\n",
			nand_dev, off);
	image_print_contents(imgdata);

	puts("   Verifying Checksum ... ");
	if (!image_check_dcrc(imgdata))
		puts("Bad Data CRC\n");
	else
		puts("OK\n");

	free(imgdata);

	return 0;
}

static int nand_imls_fitimage(nand_info_t *nand, int nand_dev, loff_t off,
		size_t len)
{
	void *imgdata;
	int ret;

	imgdata = malloc(len);
	if (!imgdata) {
		printf("May be a FIT Image at NAND device %d offset %08llX:\n",
				nand_dev, off);
		printf("   Low memory(cannot allocate memory for image)\n");
		return -ENOMEM;
	}

	ret = nand_read_skip_bad(nand, off, &len,
			imgdata);
	if (ret < 0 && ret != -EUCLEAN) {
		free(imgdata);
		return ret;
	}

	if (!fit_check_format(imgdata)) {
		free(imgdata);
		return 0;
	}

	printf("FIT Image at NAND device %d offset %08llX:\n", nand_dev, off);

	fit_print_contents(imgdata);
	free(imgdata);

	return 0;
}

static int do_imls_nand(void)
{
	nand_info_t *nand;
	int nand_dev = nand_curr_device;
	size_t len;
	loff_t off;
	u32 buffer[16];

	if (nand_dev < 0 || nand_dev >= CONFIG_SYS_MAX_NAND_DEVICE) {
		puts("\nNo NAND devices available\n");
		return -ENODEV;
	}

	printf("\n");

	for (nand_dev = 0; nand_dev < CONFIG_SYS_MAX_NAND_DEVICE; nand_dev++) {
		nand = &nand_info[nand_dev];
		if (!nand->name || !nand->size)
			continue;

		for (off = 0; off < nand->size; off += nand->erasesize) {
			const image_header_t *header;
			int ret;

			if (nand_block_isbad(nand, off))
				continue;

			len = sizeof(buffer);

			ret = nand_read(nand, off, &len, (u8 *)buffer);
			if (ret < 0 && ret != -EUCLEAN) {
				printf("NAND read error %d at offset %08llX\n",
						ret, off);
				continue;
			}

			switch (genimg_get_format(buffer)) {
			case IMAGE_FORMAT_LEGACY:
				header = (const image_header_t *)buffer;

				len = image_get_image_size(header);
				nand_imls_legacyimage(nand, nand_dev, off, len);
				break;
#if defined(CONFIG_FIT)
			case IMAGE_FORMAT_FIT:
				len = fit_get_size(buffer);
				nand_imls_fitimage(nand, nand_dev, off, len);
				break;
#endif
			}
		}
	}

	return 0;
}
#endif

#if defined(CONFIG_CMD_IMLS) || defined(CONFIG_CMD_IMLS_NAND)
static int do_imls(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret_nor = 0, ret_nand = 0;

#if defined(CONFIG_CMD_IMLS)
	ret_nor = do_imls_nor();
#endif

#if defined(CONFIG_CMD_IMLS_NAND)
	ret_nand = do_imls_nand();
#endif

	if (ret_nor)
		return ret_nor;

	if (ret_nand)
		return ret_nand;

	return (0);
}

U_BOOT_CMD(
	imls,	1,		1,	do_imls,
	"list all images found in flash",
	"\n"
	"    - Prints information about all images found at sector/block\n"
	"      boundaries in nor/nand flash."
);
#endif

/*******************************************************************/
/* helper routines */
/*******************************************************************/
#if defined(CONFIG_SILENT_CONSOLE) && !defined(CONFIG_SILENT_U_BOOT_ONLY)

#define CONSOLE_ARG     "console="
#define CONSOLE_ARG_LEN (sizeof(CONSOLE_ARG) - 1)

static void fixup_silent_linux(void)
{
	char *buf;
	const char *env_val;
	char *cmdline = getenv("bootargs");
	int want_silent;

	/*
	 * Only fix cmdline when requested. The environment variable can be:
	 *
	 *	no - we never fixup
	 *	yes - we always fixup
	 *	unset - we rely on the console silent flag
	 */
	want_silent = getenv_yesno("silent_linux");
	if (want_silent == 0)
		return;
	else if (want_silent == -1 && !(gd->flags & GD_FLG_SILENT))
		return;

	debug("before silent fix-up: %s\n", cmdline);
	if (cmdline && (cmdline[0] != '\0')) {
		char *start = strstr(cmdline, CONSOLE_ARG);

		/* Allocate space for maximum possible new command line */
		buf = malloc(strlen(cmdline) + 1 + CONSOLE_ARG_LEN + 1);
		if (!buf) {
			debug("%s: out of memory\n", __func__);
			return;
		}

		if (start) {
			char *end = strchr(start, ' ');
			int num_start_bytes = start - cmdline + CONSOLE_ARG_LEN;

			strncpy(buf, cmdline, num_start_bytes);
			if (end)
				strcpy(buf + num_start_bytes, end);
			else
				buf[num_start_bytes] = '\0';
		} else {
			sprintf(buf, "%s %s", cmdline, CONSOLE_ARG);
		}
		env_val = buf;
	} else {
		buf = NULL;
		env_val = CONSOLE_ARG;
	}

	setenv("bootargs", env_val);
	debug("after silent fix-up: %s\n", env_val);
	free(buf);
}
#endif /* CONFIG_SILENT_CONSOLE */

#if defined(CONFIG_BOOTM_NETBSD) || defined(CONFIG_BOOTM_PLAN9)
static void copy_args(char *dest, int argc, char * const argv[], char delim)
{
	int i;

	for (i = 0; i < argc; i++) {
		if (i > 0)
			*dest++ = delim;
		strcpy(dest, argv[i]);
		dest += strlen(argv[i]);
	}
}
#endif

/*******************************************************************/
/* OS booting routines */
/*******************************************************************/

#ifdef CONFIG_BOOTM_NETBSD
static int do_bootm_netbsd(int flag, int argc, char * const argv[],
			    bootm_headers_t *images)
{
	void (*loader)(bd_t *, image_header_t *, char *, char *);
	image_header_t *os_hdr, *hdr;
	ulong kernel_data, kernel_len;
	char *consdev;
	char *cmdline;

	if (flag != BOOTM_STATE_OS_GO)
		return 0;

#if defined(CONFIG_FIT)
	if (!images->legacy_hdr_valid) {
		fit_unsupported_reset("NetBSD");
		return 1;
	}
#endif
	hdr = images->legacy_hdr_os;

	/*
	 * Booting a (NetBSD) kernel image
	 *
	 * This process is pretty similar to a standalone application:
	 * The (first part of an multi-) image must be a stage-2 loader,
	 * which in turn is responsible for loading & invoking the actual
	 * kernel.  The only differences are the parameters being passed:
	 * besides the board info strucure, the loader expects a command
	 * line, the name of the console device, and (optionally) the
	 * address of the original image header.
	 */
	os_hdr = NULL;
	if (image_check_type(&images->legacy_hdr_os_copy, IH_TYPE_MULTI)) {
		image_multi_getimg(hdr, 1, &kernel_data, &kernel_len);
		if (kernel_len)
			os_hdr = hdr;
	}

	consdev = "";
#if   defined(CONFIG_8xx_CONS_SMC1)
	consdev = "smc1";
#elif defined(CONFIG_8xx_CONS_SMC2)
	consdev = "smc2";
#elif defined(CONFIG_8xx_CONS_SCC2)
	consdev = "scc2";
#elif defined(CONFIG_8xx_CONS_SCC3)
	consdev = "scc3";
#endif

	if (argc > 0) {
		ulong len;
		int   i;

		for (i = 0, len = 0; i < argc; i += 1)
			len += strlen(argv[i]) + 1;
		cmdline = malloc(len);
		copy_args(cmdline, argc, argv, ' ');
	} else if ((cmdline = getenv("bootargs")) == NULL) {
		cmdline = "";
	}

	loader = (void (*)(bd_t *, image_header_t *, char *, char *))images->ep;

	printf("## Transferring control to NetBSD stage-2 loader "
		"(at address %08lx) ...\n",
		(ulong)loader);

	bootstage_mark(BOOTSTAGE_ID_RUN_OS);

	/*
	 * NetBSD Stage-2 Loader Parameters:
	 *   arg[0]: pointer to board info data
	 *   arg[1]: image load address
	 *   arg[2]: char pointer to the console device to use
	 *   arg[3]: char pointer to the boot arguments
	 */
	(*loader)(gd->bd, os_hdr, consdev, cmdline);

	return 1;
}
#endif /* CONFIG_BOOTM_NETBSD*/

#ifdef CONFIG_LYNXKDI
static int do_bootm_lynxkdi(int flag, int argc, char * const argv[],
			     bootm_headers_t *images)
{
	image_header_t *hdr = &images->legacy_hdr_os_copy;

	if (flag != BOOTM_STATE_OS_GO)
		return 0;

#if defined(CONFIG_FIT)
	if (!images->legacy_hdr_valid) {
		fit_unsupported_reset("Lynx");
		return 1;
	}
#endif

	lynxkdi_boot((image_header_t *)hdr);

	return 1;
}
#endif /* CONFIG_LYNXKDI */

#ifdef CONFIG_BOOTM_RTEMS
static int do_bootm_rtems(int flag, int argc, char * const argv[],
			   bootm_headers_t *images)
{
	void (*entry_point)(bd_t *);

	if (flag != BOOTM_STATE_OS_GO)
		return 0;

#if defined(CONFIG_FIT)
	if (!images->legacy_hdr_valid) {
		fit_unsupported_reset("RTEMS");
		return 1;
	}
#endif

	entry_point = (void (*)(bd_t *))images->ep;

	printf("## Transferring control to RTEMS (at address %08lx) ...\n",
		(ulong)entry_point);

	bootstage_mark(BOOTSTAGE_ID_RUN_OS);

	/*
	 * RTEMS Parameters:
	 *   r3: ptr to board info data
	 */
	(*entry_point)(gd->bd);

	return 1;
}
#endif /* CONFIG_BOOTM_RTEMS */

#if defined(CONFIG_BOOTM_OSE)
static int do_bootm_ose(int flag, int argc, char * const argv[],
			   bootm_headers_t *images)
{
	void (*entry_point)(void);

	if (flag != BOOTM_STATE_OS_GO)
		return 0;

#if defined(CONFIG_FIT)
	if (!images->legacy_hdr_valid) {
		fit_unsupported_reset("OSE");
		return 1;
	}
#endif

	entry_point = (void (*)(void))images->ep;

	printf("## Transferring control to OSE (at address %08lx) ...\n",
		(ulong)entry_point);

	bootstage_mark(BOOTSTAGE_ID_RUN_OS);

	/*
	 * OSE Parameters:
	 *   None
	 */
	(*entry_point)();

	return 1;
}
#endif /* CONFIG_BOOTM_OSE */

#if defined(CONFIG_BOOTM_PLAN9)
static int do_bootm_plan9(int flag, int argc, char * const argv[],
			   bootm_headers_t *images)
{
	void (*entry_point)(void);
	char *s;

	if (flag != BOOTM_STATE_OS_GO)
		return 0;

#if defined(CONFIG_FIT)
	if (!images->legacy_hdr_valid) {
		fit_unsupported_reset("Plan 9");
		return 1;
	}
#endif

	/* See README.plan9 */
	s = getenv("confaddr");
	if (s != NULL) {
		char *confaddr = (char *)simple_strtoul(s, NULL, 16);

		if (argc > 0) {
			copy_args(confaddr, argc, argv, '\n');
		} else {
			s = getenv("bootargs");
			if (s != NULL)
				strcpy(confaddr, s);
		}
	}

	entry_point = (void (*)(void))images->ep;

	printf("## Transferring control to Plan 9 (at address %08lx) ...\n",
		(ulong)entry_point);

	bootstage_mark(BOOTSTAGE_ID_RUN_OS);

	/*
	 * Plan 9 Parameters:
	 *   None
	 */
	(*entry_point)();

	return 1;
}
#endif /* CONFIG_BOOTM_PLAN9 */

#if defined(CONFIG_BOOTM_VXWORKS) && \
	(defined(CONFIG_PPC) || defined(CONFIG_ARM))

void do_bootvx_fdt(bootm_headers_t *images)
{
#if defined(CONFIG_OF_LIBFDT)
	int ret;
	char *bootline;
	ulong of_size = images->ft_len;
	char **of_flat_tree = &images->ft_addr;
	struct lmb *lmb = &images->lmb;

	if (*of_flat_tree) {
		boot_fdt_add_mem_rsv_regions(lmb, *of_flat_tree);

		ret = boot_relocate_fdt(lmb, of_flat_tree, &of_size);
		if (ret)
			return;

		ret = fdt_add_subnode(*of_flat_tree, 0, "chosen");
		if ((ret >= 0 || ret == -FDT_ERR_EXISTS)) {
			bootline = getenv("bootargs");
			if (bootline) {
				ret = fdt_find_and_setprop(*of_flat_tree,
						"/chosen", "bootargs",
						bootline,
						strlen(bootline) + 1, 1);
				if (ret < 0) {
					printf("## ERROR: %s : %s\n", __func__,
					       fdt_strerror(ret));
					return;
				}
			}
		} else {
			printf("## ERROR: %s : %s\n", __func__,
			       fdt_strerror(ret));
			return;
		}
	}
#endif

	boot_prep_vxworks(images);

	bootstage_mark(BOOTSTAGE_ID_RUN_OS);

#if defined(CONFIG_OF_LIBFDT)
	printf("## Starting vxWorks at 0x%08lx, device tree at 0x%08lx ...\n",
	       (ulong)images->ep, (ulong)*of_flat_tree);
#else
	printf("## Starting vxWorks at 0x%08lx\n", (ulong)images->ep);
#endif

	boot_jump_vxworks(images);

	puts("## vxWorks terminated\n");
}

static int do_bootm_vxworks(int flag, int argc, char * const argv[],
			     bootm_headers_t *images)
{
	if (flag != BOOTM_STATE_OS_GO)
		return 0;

#if defined(CONFIG_FIT)
	if (!images->legacy_hdr_valid) {
		fit_unsupported_reset("VxWorks");
		return 1;
	}
#endif

	do_bootvx_fdt(images);

	return 1;
}
#endif

#if defined(CONFIG_CMD_ELF)
static int do_bootm_qnxelf(int flag, int argc, char * const argv[],
			    bootm_headers_t *images)
{
	char *local_args[2];
	char str[16];

	if (flag != BOOTM_STATE_OS_GO)
		return 0;

#if defined(CONFIG_FIT)
	if (!images->legacy_hdr_valid) {
		fit_unsupported_reset("QNX");
		return 1;
	}
#endif

	sprintf(str, "%lx", images->ep); /* write entry-point into string */
	local_args[0] = argv[0];
	local_args[1] = str;	/* and provide it via the arguments */
	do_bootelf(NULL, 0, 2, local_args);

	return 1;
}
#endif

#ifdef CONFIG_INTEGRITY
static int do_bootm_integrity(int flag, int argc, char * const argv[],
			   bootm_headers_t *images)
{
	void (*entry_point)(void);

	if (flag != BOOTM_STATE_OS_GO)
		return 0;

#if defined(CONFIG_FIT)
	if (!images->legacy_hdr_valid) {
		fit_unsupported_reset("INTEGRITY");
		return 1;
	}
#endif

	entry_point = (void (*)(void))images->ep;

	printf("## Transferring control to INTEGRITY (at address %08lx) ...\n",
		(ulong)entry_point);

	bootstage_mark(BOOTSTAGE_ID_RUN_OS);

	/*
	 * INTEGRITY Parameters:
	 *   None
	 */
	(*entry_point)();

	return 1;
}
#endif

#ifdef CONFIG_CMD_BOOTZ

int __weak bootz_setup(ulong image, ulong *start, ulong *end)
{
	/* Please define bootz_setup() for your platform */

	puts("Your platform's zImage format isn't supported yet!\n");
	return -1;
}

/*
 * zImage booting support
 */
static int bootz_start(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[], bootm_headers_t *images)
{
	int ret;
	ulong zi_start, zi_end;

	ret = do_bootm_states(cmdtp, flag, argc, argv, BOOTM_STATE_START,
			      images, 1);

	/* Setup Linux kernel zImage entry point */
	if (!argc) {
		images->ep = load_addr;
		debug("*  kernel: default image load address = 0x%08lx\n",
				load_addr);
	} else {
		images->ep = simple_strtoul(argv[0], NULL, 16);
		debug("*  kernel: cmdline image address = 0x%08lx\n",
			images->ep);
	}

	ret = bootz_setup(images->ep, &zi_start, &zi_end);
	if (ret != 0)
		return 1;

	lmb_reserve(&images->lmb, images->ep, zi_end - zi_start);

	/*
	 * Handle the BOOTM_STATE_FINDOTHER state ourselves as we do not
	 * have a header that provide this informaiton.
	 */
	if (bootm_find_ramdisk(flag, argc, argv))
		return 1;

#if defined(CONFIG_OF_LIBFDT)
	if (bootm_find_fdt(flag, argc, argv))
		return 1;
#endif

	return 0;
}

int do_bootz(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret;

	/* Consume 'bootz' */
	argc--; argv++;

	if (bootz_start(cmdtp, flag, argc, argv, &images))
		return 1;

	/*
	 * We are doing the BOOTM_STATE_LOADOS state ourselves, so must
	 * disable interrupts ourselves
	 */
	bootm_disable_interrupts();

	images.os.os = IH_OS_LINUX;
	ret = do_bootm_states(cmdtp, flag, argc, argv,
			      BOOTM_STATE_OS_PREP | BOOTM_STATE_OS_FAKE_GO |
			      BOOTM_STATE_OS_GO,
			      &images, 1);

	return ret;
}

#ifdef CONFIG_SYS_LONGHELP
static char bootz_help_text[] =
	"[addr [initrd[:size]] [fdt]]\n"
	"    - boot Linux zImage stored in memory\n"
	"\tThe argument 'initrd' is optional and specifies the address\n"
	"\tof the initrd in memory. The optional argument ':size' allows\n"
	"\tspecifying the size of RAW initrd.\n"
#if defined(CONFIG_OF_LIBFDT)
	"\tWhen booting a Linux kernel which requires a flat device-tree\n"
	"\ta third argument is required which is the address of the\n"
	"\tdevice-tree blob. To boot that kernel without an initrd image,\n"
	"\tuse a '-' for the second argument. If you do not pass a third\n"
	"\ta bd_info struct will be passed instead\n"
#endif
	"";
#endif

U_BOOT_CMD(
	bootz,	CONFIG_SYS_MAXARGS,	1,	do_bootz,
	"boot Linux zImage image from memory", bootz_help_text
);
#endif	/* CONFIG_CMD_BOOTZ */
