/******************************************************************************
 *
 * Copyright(c) 2016 - 2017 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/

#ifndef _HALMAC_8821C_CFG_H_
#define _HALMAC_8821C_CFG_H_

#include "../../halmac_hw_cfg.h"

#if HALMAC_8821C_SUPPORT

#define HALMAC_TX_FIFO_SIZE_8821C		65536 /* 64k */
#define HALMAC_RX_FIFO_SIZE_8821C		16384 /* 16k */
#define HALMAC_TX_SHARE_RX_SIZE_8821C	32768 /* 32k */

#define HALMAC_TX_PAGE_SIZE_8821C			128 /* PageSize 128Byte */
#define HALMAC_TX_ALIGN_SIZE_8821C			8
#define HALMAC_TX_PAGE_SIZE_2_POWER_8821C	7   /* 128 = 2^7 */

#define HALMAC_SECURITY_CAM_ENTRY_NUM_8821C		64 /* CAM Entry Size */

#define HALMAC_TX_DESC_SIZE_8821C				48
#define HALMAC_RX_DESC_SIZE_8821C				24

#define HALMAC_C2H_PKT_BUF_8821C		256
#define HALMAC_RX_DESC_DUMMY_SIZE_MAX_8821C      72 /*8*9 Bytes*/
#define HALMAC_RX_FIFO_EXPANDING_MODE_PKT_SIZE_MAX_8821C    80 /* should be 8 Byte alignment*/

#define HALMAC_RX_FIFO_EXPANDING_UNIT_8821C		(HALMAC_RX_DESC_SIZE_8821C + HALMAC_RX_DESC_DUMMY_SIZE_MAX_8821C + HALMAC_RX_FIFO_EXPANDING_MODE_PKT_SIZE) /* should be 8 Byte alignment*/
#define HALMAC_RX_FIFO_EXPANDING_UNIT_MAX_8821C		(HALMAC_RX_DESC_SIZE_8821C + HALMAC_RX_DESC_DUMMY_SIZE_MAX_8821C + HALMAC_RX_FIFO_EXPANDING_MODE_PKT_SIZE_MAX_8821C) /* should be 8 Byte alignment*/

#define HALMAC_TX_FIFO_SIZE_LA_8821C						 (HALMAC_TX_FIFO_SIZE_8821C >> 1)  /* 32k */
#define HALMAC_TX_FIFO_SIZE_RX_FIFO_EXPANDING_1_BLOCK_8821C  (HALMAC_TX_FIFO_SIZE_8821C - HALMAC_TX_SHARE_RX_SIZE_8821C)  /* 32k */
#define HALMAC_RX_FIFO_SIZE_RX_FIFO_EXPANDING_1_BLOCK_8821C			((((HALMAC_RX_FIFO_EXPANDING_UNIT_8821C << 8) - 1) >> 10) << 10) /*  < 48k*/
#define HALMAC_RX_FIFO_SIZE_RX_FIFO_EXPANDING_1_BLOCK_MAX_8821C		((((HALMAC_RX_FIFO_EXPANDING_UNIT_MAX_8821C << 8) - 1) >> 10) << 10) /* 45k < 48k*/

#define HALMAC_RSVD_DRV_PGNUM_8821C					16 /*2048*/
#define HALMAC_RSVD_H2C_EXTRAINFO_PGNUM_8821C		24 /*3072*/
#define HALMAC_RSVD_H2C_STATICINFO_PGNUM_8821C		8  /*1024*/
#define HALMAC_RSVD_H2C_QUEUE_PGNUM_8821C			8  /*1024*/
#define HALMAC_RSVD_CPU_INSTRUCTION_PGNUM_8821C		0  /*0*/
#define HALMAC_RSVD_FW_TXBUFF_PGNUM_8821C			4  /*512*/
#define HALMAC_RSVD_CSIBUF_PGNUM_8821C				0  /*no support*/
#define HALMAC_RSVD_DLLB_PGNUM_8821C				32 /*4096*/

#define HALMAC_EFUSE_SIZE_8821C					512
#define HALMAC_EEPROM_SIZE_8821C				512
#define HALMAC_BT_EFUSE_SIZE_8821C				128

#define HALMAC_CR_TRX_ENABLE_8821C      (BIT_HCI_TXDMA_EN | BIT_HCI_RXDMA_EN | BIT_TXDMA_EN | \
					 BIT_RXDMA_EN | BIT_PROTOCOL_EN | BIT_SCHEDULE_EN | \
					 BIT_MACTXEN | BIT_MACRXEN)

#define HALMAC_BLK_DESC_NUM_8821C   0x3 /* Only for USB */

#define HALMAC_INTF_INTGRA_MINREF_8821C 90 /* Only for PCIE */
#define HALMAC_INTF_INTGRA_HOSTREF_8821C 100 /* Only for PCIE */

/* AMPDU max time (unit : 32us) */
#define HALMAC_AMPDU_MAX_TIME_8821C		0x70

/* Protect mode control */
#define HALMAC_PROT_RTS_LEN_TH_8821C					0xFF
#define HALMAC_PROT_RTS_TX_TIME_TH_8821C				0x08
#define HALMAC_PROT_MAX_AGG_PKT_LIMIT_8821C				0x10
#define HALMAC_PROT_RTS_MAX_AGG_PKT_LIMIT_8821C			0x10
#define HALMAC_PROT_MAX_AGG_PKT_LIMIT_8821C_SDIO		0x2B
#define HALMAC_PROT_RTS_MAX_AGG_PKT_LIMIT_8821C_SDIO	0x2B
#define HALMAC_PRE_TXCNT_TIME_TH_8821C					0x1E4

/* Fast EDCA setting */
#define HALMAC_FAST_EDCA_VO_TH_8821C		0x06
#define HALMAC_FAST_EDCA_VI_TH_8821C		0x06
#define HALMAC_FAST_EDCA_BE_TH_8821C		0x06
#define HALMAC_FAST_EDCA_BK_TH_8821C		0x06

/* BAR setting */
#define HALMAC_BAR_RETRY_LIMIT_8821C			0x01
#define HALMAC_RA_TRY_RATE_AGG_LIMIT_8821C		0x08

/* OQT entry */
#define HALMAC_OQT_ENTRY_AC_8821C		32
#define HALMAC_OQT_ENTRY_NOAC_8821C		32

/* MACID number */
#define HALMAC_MACID_MAX_8821C		128

/* AC queue number */
#define HALMAC_ACQ_NUM_MAX_8821C	8

#endif /* HALMAC_8821C_SUPPORT */

#endif
