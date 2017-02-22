/*
 * board.h
 *
 * (C) Copyright 2013 Siemens Schweiz AG
 * (C) Heiko Schocher, DENX Software Engineering, hs@denx.de.
 *
 * Based on:
 * TI AM335x boards information header
 * u-boot:/board/ti/am335x/board.h
 *
 * Copyright (C) 2011, Texas Instruments, Incorporated - http://www.ti.com/
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#define PARGS3(x)	settings.ddr3.x-ddr3_default.x, \
			settings.ddr3.x, ddr3_default.x
#define PRINTARGS(y)	printf("%x, %8x, %8x : "#y"\n", PARGS3(y))
#define MAGIC_CHIP	0x50494843

/* Automatic generated definition */
/* Wed, 18 Sep 2013 18:58:27 +0200 */
/* From file: draco/ddr3-data-micron-v2.txt */
struct ddr3_data {
	unsigned int magic;			/* 0x33524444 */
	unsigned int version;			/* 0x56312e34 */
	unsigned short int ddr3_sratio;		/* 0x0100 */
	unsigned short int iclkout;		/* 0x0001 */
	unsigned short int dt0rdsratio0;	/* 0x003A */
	unsigned short int dt0wdsratio0;	/* 0x008A */
	unsigned short int dt0fwsratio0;	/* 0x010B */
	unsigned short int dt0wrsratio0;	/* 0x00C4 */
	unsigned int sdram_tim1;		/* 0x0888A39B */
	unsigned int sdram_tim2;		/* 0x26247FDA */
	unsigned int sdram_tim3;		/* 0x501F821F */
	unsigned int emif_ddr_phy_ctlr_1;	/* 0x00100206 */
	unsigned int sdram_config;		/* 0x61C04AB2 */
	unsigned int ref_ctrl;			/* 0x00000618 */
	unsigned int ioctr_val;			/* 0x0000018B */
};

struct chip_data {
	unsigned int  magic;
	char sdevname[16];
	char shwver[7];
};

struct dxr2_baseboard_id {
	struct ddr3_data ddr3;
	struct chip_data chip;
};

/*
 * We have three pin mux functions that must exist.  We must be able to enable
 * uart0, for initial output and i2c0 to read the main EEPROM.  We then have a
 * main pinmux function that can be overridden to enable all other pinmux that
 * is required on the board.
 */
void enable_uart0_pin_mux(void);
void enable_uart1_pin_mux(void);
void enable_uart2_pin_mux(void);
void enable_uart3_pin_mux(void);
void enable_uart4_pin_mux(void);
void enable_uart5_pin_mux(void);
void enable_i2c0_pin_mux(void);
void enable_board_pin_mux(void);
#endif
