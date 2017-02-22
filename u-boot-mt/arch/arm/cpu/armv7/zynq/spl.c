/*
 * (C) Copyright 2014 Xilinx, Inc. Michal Simek
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <spl.h>

#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/arch/spl.h>
#include <asm/arch/sys_proto.h>

DECLARE_GLOBAL_DATA_PTR;

void board_init_f(ulong dummy)
{
	ps7_init();

	/* Clear the BSS. */
	memset(__bss_start, 0, __bss_end - __bss_start);

	/* Set global data pointer. */
	gd = &gdata;

	preloader_console_init();
	arch_cpu_init();
	board_init_r(NULL, 0);
}

u32 spl_boot_device(void)
{
	u32 mode;

	switch ((zynq_slcr_get_boot_mode()) & ZYNQ_BM_MASK) {
#ifdef CONFIG_SPL_SPI_SUPPORT
	case ZYNQ_BM_QSPI:
		puts("qspi boot\n");
		mode = BOOT_DEVICE_SPI;
		break;
#endif
#ifdef CONFIG_SPL_MMC_SUPPORT
	case ZYNQ_BM_SD:
		puts("mmc boot\n");
		mode = BOOT_DEVICE_MMC1;
		break;
#endif
	default:
		puts("Unsupported boot mode selected\n");
		hang();
	}

	return mode;
}

#ifdef CONFIG_SPL_MMC_SUPPORT
u32 spl_boot_mode(void)
{
	return MMCSD_MODE_FAT;
}
#endif

#ifdef CONFIG_SPL_OS_BOOT
int spl_start_uboot(void)
{
	/* boot linux */
	return 0;
}
#endif
