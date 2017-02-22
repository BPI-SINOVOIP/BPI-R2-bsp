
/*
 * Copyright (C) 2012 Samsung Electronics
 *
 * Configuration settings for the SAMSUNG EXYNOS5250 board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_5250_H
#define __CONFIG_5250_H

#include <configs/exynos5-dt.h>
#define CONFIG_EXYNOS5250

#define CONFIG_SYS_SDRAM_BASE		0x40000000
#define CONFIG_SYS_TEXT_BASE		0x43E00000

/* MACH_TYPE_SMDK5250 macro will be removed once added to mach-types */
#define MACH_TYPE_SMDK5250		3774
#define CONFIG_MACH_TYPE		MACH_TYPE_SMDK5250

/* USB */
#define CONFIG_CMD_USB
#define CONFIG_USB_XHCI
#define CONFIG_USB_XHCI_EXYNOS
#define CONFIG_SYS_USB_XHCI_MAX_ROOT_PORTS	2
#define CONFIG_USB_STORAGE

/* USB boot mode */
#define CONFIG_USB_BOOTING
#define EXYNOS_COPY_USB_FNPTR_ADDR	0x02020070
#define EXYNOS_USB_SECONDARY_BOOT	0xfeed0002
#define EXYNOS_IRAM_SECONDARY_BASE	0x02020018

#define CONFIG_SPL_TEXT_BASE	0x02023400

#define CONFIG_BOOTCOMMAND	"mmc read 40007000 451 2000; bootm 40007000"

#define CONFIG_SYS_PROMPT		"SMDK5250 # "
#define CONFIG_IDENT_STRING		" for SMDK5250"

#define CONFIG_IRAM_STACK	0x02050000

#define CONFIG_SYS_INIT_SP_ADDR	CONFIG_IRAM_STACK

/* PMIC */
#define CONFIG_PMIC_MAX77686

/* Sound */
#define CONFIG_CMD_SOUND
#ifdef CONFIG_CMD_SOUND
#define CONFIG_SOUND
#define CONFIG_I2S
#define CONFIG_SOUND_MAX98095
#define CONFIG_SOUND_WM8994
#endif

/* I2C */
#define CONFIG_MAX_I2C_NUM	8

/* Display */
#define CONFIG_LCD
#ifdef CONFIG_LCD
#define CONFIG_EXYNOS_FB
#define CONFIG_EXYNOS_DP
#define LCD_XRES			2560
#define LCD_YRES			1600
#define LCD_BPP			LCD_COLOR16
#endif
#endif  /* __CONFIG_5250_H */
