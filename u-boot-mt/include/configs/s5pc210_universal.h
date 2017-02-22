/*
 * Copyright (C) 2010 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
 *
 * Configuation settings for the SAMSUNG Universal (EXYNOS4210) board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_SAMSUNG		1	/* in a SAMSUNG core */
#define CONFIG_S5P		1	/* which is in a S5P Family */
#define CONFIG_EXYNOS4210	1	/* which is in a EXYNOS4210 */
#define CONFIG_UNIVERSAL	1	/* working with Universal */
#define CONFIG_TIZEN		1	/* TIZEN lib */

#include <asm/arch/cpu.h>		/* get chip and board defs */

#define CONFIG_ARCH_CPU_INIT
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

/* Keep L2 Cache Disabled */
#define CONFIG_SYS_L2CACHE_OFF		1

#define CONFIG_SYS_SDRAM_BASE		0x40000000
#define CONFIG_SYS_TEXT_BASE		0x44800000

/* input clock of PLL: Universal has 24MHz input clock at EXYNOS4210 */
#define CONFIG_SYS_CLK_FREQ_C210	24000000
#define CONFIG_SYS_CLK_FREQ		CONFIG_SYS_CLK_FREQ_C210

#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG
#define CONFIG_REVISION_TAG
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SKIP_LOWLEVEL_INIT
#define CONFIG_BOARD_EARLY_INIT_F

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + (1 << 20))

/* select serial console configuration */
#define CONFIG_SERIAL2		1	/* use SERIAL 2 */
#define CONFIG_BAUDRATE		115200

/* MMC */
#define CONFIG_GENERIC_MMC
#define CONFIG_MMC
#define CONFIG_SDHCI
#define CONFIG_S5P_SDHCI

/* PWM */
#define CONFIG_PWM			1

/* It should define before config_cmd_default.h */
#define CONFIG_SYS_NO_FLASH		1

/* Command definition */
#include <config_cmd_default.h>

#undef CONFIG_CMD_FPGA
#undef CONFIG_CMD_MISC
#undef CONFIG_CMD_NET
#undef CONFIG_CMD_NFS
#undef CONFIG_CMD_XIMG
#define CONFIG_CMD_CACHE
#define CONFIG_CMD_ONENAND
#define CONFIG_CMD_MTDPARTS
#define CONFIG_CMD_MMC
#define CONFIG_CMD_FAT

#define CONFIG_BOOTDELAY		1
#define CONFIG_ZERO_BOOTDELAY_CHECK

#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS

/* Actual modem binary size is 16MiB. Add 2MiB for bad block handling */
#define MTDIDS_DEFAULT		"onenand0=samsung-onenand"

#define MTDPARTS_DEFAULT	"mtdparts=samsung-onenand:"\
				"128k(s-boot)"\
				",896k(bootloader)"\
				",256k(params)"\
				",2816k(config)"\
				",8m(csa)"\
				",7m(kernel)"\
				",1m(log)"\
				",12m(modem)"\
				",60m(qboot)"\
				",-(UBI)\0"

#define NORMAL_MTDPARTS_DEFAULT MTDPARTS_DEFAULT

#define MBRPARTS_DEFAULT	"20M(permanent)"\
				",20M(boot)"\
				",1G(system)"\
				",100M(swap)"\
				",-(UMS)\0"

#define CONFIG_BOOTARGS		"Please use defined boot"
#define CONFIG_BOOTCOMMAND	"run mmcboot"
#define CONFIG_DEFAULT_CONSOLE	"console=ttySAC2,115200n8\0"

#define CONFIG_ENV_UBI_MTD	" ubi.mtd=${ubiblock} ubi.mtd=4 ubi.mtd=7"
#define CONFIG_BOOTBLOCK	"10"
#define CONFIG_UBIBLOCK		"9"

#define CONFIG_ENV_UBIFS_OPTION	" rootflags=bulk_read,no_chk_data_crc "
#define CONFIG_ENV_FLASHBOOT	CONFIG_ENV_UBI_MTD CONFIG_ENV_UBIFS_OPTION \
				"${mtdparts}"

#define CONFIG_ENV_COMMON_BOOT	"${console} ${meminfo}"

#define CONFIG_ENV_OVERWRITE
#define CONFIG_SYS_CONSOLE_INFO_QUIET
#define CONFIG_SYS_CONSOLE_IS_IN_ENV

#define CONFIG_ENV_VARS_UBOOT_CONFIG
#define CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG

#define CONFIG_EXTRA_ENV_SETTINGS					\
	"updateb=" \
		"onenand erase 0x0 0x100000;" \
		"onenand write 0x42008000 0x0 0x100000\0" \
	"updatek=" \
		"onenand erase 0xc00000 0x500000;" \
		"onenand write 0x41008000 0xc00000 0x500000\0" \
	"bootk=" \
		"run loaduimage; bootm 0x40007FC0\0" \
	"updatemmc=" \
		"mmc boot 0 1 1 1; mmc write 0 0x42008000 0 0x200;" \
		"mmc boot 0 1 1 0\0" \
	"updatebackup=" \
		"mmc boot 0 1 1 2; mmc write 0 0x42100000 0 0x200;" \
		"mmc boot 0 1 1 0\0" \
	"updatebootb=" \
		"mmc read 0 0x42100000 0x80 0x200; run updatebackup\0" \
	"lpj=lpj=3981312\0" \
	"ubifsboot=" \
		"set bootargs root=ubi0!rootfs rootfstype=ubifs ${lpj} " \
		CONFIG_ENV_FLASHBOOT " ${opts} ${lcdinfo} " \
		CONFIG_ENV_COMMON_BOOT "; run bootk\0" \
	"tftpboot=" \
		"set bootargs root=ubi0!rootfs rootfstype=ubifs " \
		CONFIG_ENV_FLASHBOOT " ${opts} ${lcdinfo} " \
		CONFIG_ENV_COMMON_BOOT \
		"; tftp 0x40007FC0 uImage; bootm 0x40007FC0\0" \
	"nfsboot=" \
		"set bootargs root=/dev/nfs rw " \
		"nfsroot=${nfsroot},nolock,tcp " \
		"ip=${ipaddr}:${serverip}:${gatewayip}:" \
		"${netmask}:generic:usb0:off " CONFIG_ENV_COMMON_BOOT \
		"; run bootk\0" \
	"ramfsboot=" \
		"set bootargs root=/dev/ram0 rw rootfstype=ext2 " \
		"${console} ${meminfo} " \
		"initrd=0x43000000,8M ramdisk=8192\0" \
	"mmcboot=" \
		"set bootargs root=/dev/mmcblk${mmcdev}p${mmcrootpart} " \
		"${lpj} rootwait ${console} ${meminfo} ${opts} ${lcdinfo}; " \
		"run loaduimage; bootm 0x40007FC0\0" \
	"bootchart=set opts init=/sbin/bootchartd; run bootcmd\0" \
	"boottrace=setenv opts initcall_debug; run bootcmd\0" \
	"mmcoops=mmc read 0 0x40000000 0x40 8; md 0x40000000 0x400\0" \
	"verify=n\0" \
	"rootfstype=ext4\0" \
	"console=" CONFIG_DEFAULT_CONSOLE \
	"mtdparts=" MTDPARTS_DEFAULT \
	"mbrparts=" MBRPARTS_DEFAULT \
	"meminfo=crashkernel=32M@0x50000000\0" \
	"nfsroot=/nfsroot/arm\0" \
	"bootblock=" CONFIG_BOOTBLOCK "\0" \
	"ubiblock=" CONFIG_UBIBLOCK" \0" \
	"ubi=enabled\0" \
	"loaduimage=fatload mmc ${mmcdev}:${mmcbootpart} 0x40007FC0 uImage\0" \
	"mmcdev=0\0" \
	"mmcbootpart=2\0" \
	"mmcrootpart=3\0" \
	"opts=always_resume=1"

/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_HUSH_PARSER		/* use "hush" command parser	*/
#define CONFIG_SYS_PROMPT	"Universal # "
#define CONFIG_SYS_CBSIZE	256	/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE	384	/* Print Buffer Size */
#define CONFIG_SYS_MAXARGS	16	/* max number of command args */
/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
/* memtest works on */
#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_SDRAM_BASE + 0x5000000)
#define CONFIG_SYS_LOAD_ADDR		(CONFIG_SYS_SDRAM_BASE + 0x4800000)

/* Universal has 2 banks of DRAM */
#define CONFIG_NR_DRAM_BANKS	2
#define PHYS_SDRAM_1		CONFIG_SYS_SDRAM_BASE	/* LDDDR2 DMC 0 */
#define PHYS_SDRAM_1_SIZE	(256 << 20)		/* 256 MB in CS 0 */
#define PHYS_SDRAM_2		0x50000000		/* LPDDR2 DMC 1 */
#define PHYS_SDRAM_2_SIZE	(256 << 20)		/* 256 MB in CS 0 */

#define CONFIG_SYS_MEM_TOP_HIDE		(1 << 20)	/* ram console */

#define CONFIG_SYS_MONITOR_BASE		0x00000000
#define CONFIG_SYS_MONITOR_LEN		(256 << 10)	/* Reserve 2 sectors */

#define CONFIG_USE_ONENAND_BOARD_INIT
#define CONFIG_SAMSUNG_ONENAND
#define CONFIG_SYS_ONENAND_BASE		0x0C000000

#define CONFIG_ENV_IS_IN_MMC		1
#define CONFIG_SYS_MMC_ENV_DEV		0
#define CONFIG_ENV_SIZE			4096
#define CONFIG_ENV_OFFSET		((32 - 4) << 10)/* 32KiB - 4KiB */

#define CONFIG_DOS_PARTITION		1

#define CONFIG_SYS_INIT_SP_ADDR	(CONFIG_SYS_LOAD_ADDR - GENERATED_GBL_DATA_SIZE)

#define CONFIG_SYS_CACHELINE_SIZE       32

#include <asm/arch/gpio.h>
/*
 * I2C Settings
 */
#define CONFIG_SOFT_I2C_GPIO_SCL exynos4_gpio_get(1, b, 7)
#define CONFIG_SOFT_I2C_GPIO_SDA exynos4_gpio_get(1, b, 6)

#define CONFIG_SYS_I2C
#define CONFIG_SYS_I2C_SOFT		/* I2C bit-banged */
#define CONFIG_SYS_I2C_SOFT_SPEED	50000
#define CONFIG_SYS_I2C_SOFT_SLAVE	0
#define CONFIG_SOFT_I2C_READ_REPEATED_START
#define CONFIG_I2C_MULTI_BUS
#define CONFIG_SYS_MAX_I2C_BUS	7

#define CONFIG_POWER
#define CONFIG_POWER_I2C
#define CONFIG_POWER_MAX8998

#define CONFIG_USB_GADGET
#define CONFIG_USB_GADGET_S3C_UDC_OTG
#define CONFIG_USB_GADGET_DUALSPEED

/*
 * SPI Settings
 */
#define CONFIG_SOFT_SPI
#define CONFIG_SOFT_SPI_MODE SPI_MODE_3
#define CONFIG_SOFT_SPI_GPIO_SCLK exynos4_gpio_get(2, y3, 1)
#define CONFIG_SOFT_SPI_GPIO_MOSI exynos4_gpio_get(2, y3, 3)
#define CONFIG_SOFT_SPI_GPIO_MISO exynos4_gpio_get(2, y3, 0)
#define CONFIG_SOFT_SPI_GPIO_CS exynos4_gpio_get(2, y4, 3)

#define SPI_DELAY udelay(1)
#undef SPI_INIT
#define SPI_SCL(bit) universal_spi_scl(bit)
#define SPI_SDA(bit) universal_spi_sda(bit)
#define SPI_READ universal_spi_read()
#ifndef	__ASSEMBLY__
void universal_spi_scl(int bit);
void universal_spi_sda(int bit);
int universal_spi_read(void);
#endif

/* Common misc for Samsung */
#define CONFIG_MISC_COMMON

#define CONFIG_MISC_INIT_R

/* Download menu - Samsung common */
#define CONFIG_LCD_MENU
#define CONFIG_LCD_MENU_BOARD

/* Download menu - definitions for check keys */
#ifndef __ASSEMBLY__
#include <power/max8998_pmic.h>

#define KEY_PWR_PMIC_NAME		"MAX8998_PMIC"
#define KEY_PWR_STATUS_REG		MAX8998_REG_STATUS1
#define KEY_PWR_STATUS_MASK		(1 << 7)
#define KEY_PWR_INTERRUPT_REG		MAX8998_REG_IRQ1
#define KEY_PWR_INTERRUPT_MASK		(1 << 7)

#define KEY_VOL_UP_GPIO			exynos4_gpio_get(2, x2, 0)
#define KEY_VOL_DOWN_GPIO		exynos4_gpio_get(2, x2, 1)
#endif /* __ASSEMBLY__ */

/* LCD console */
#define LCD_BPP			LCD_COLOR16
#define CONFIG_SYS_WHITE_ON_BLACK

/*
 * LCD Settings
 */
#define CONFIG_EXYNOS_FB
#define CONFIG_LCD
#define CONFIG_CMD_BMP
#define CONFIG_BMP_16BPP
#define CONFIG_LD9040
#define CONFIG_EXYNOS_MIPI_DSIM
#define CONFIG_VIDEO_BMP_GZIP
#define CONFIG_SYS_VIDEO_LOGO_MAX_SIZE ((500 * 160 * 4) + 54)

#endif	/* __CONFIG_H */
