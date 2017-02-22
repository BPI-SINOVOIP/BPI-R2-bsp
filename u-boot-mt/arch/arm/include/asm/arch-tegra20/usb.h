/*
 * Copyright (c) 2011 The Chromium OS Authors.
 * Copyright (c) 2013 NVIDIA Corporation
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _TEGRA20_USB_H_
#define _TEGRA20_USB_H_

/* USB Controller (USBx_CONTROLLER_) regs */
struct usb_ctlr {
	/* 0x000 */
	uint id;
	uint reserved0;
	uint host;
	uint device;

	/* 0x010 */
	uint txbuf;
	uint rxbuf;
	uint reserved1[2];

	/* 0x020 */
	uint reserved2[56];

	/* 0x100 */
	u16 cap_length;
	u16 hci_version;
	uint hcs_params;
	uint hcc_params;
	uint reserved3[5];

	/* 0x120 */
	uint dci_version;
	uint dcc_params;
	uint reserved4[6];

	/* 0x140 */
	uint usb_cmd;
	uint usb_sts;
	uint usb_intr;
	uint frindex;

	/* 0x150 */
	uint reserved5;
	uint periodic_list_base;
	uint async_list_addr;
	uint async_tt_sts;

	/* 0x160 */
	uint burst_size;
	uint tx_fill_tuning;
	uint reserved6;   /* is this port_sc1 on some controllers? */
	uint icusb_ctrl;

	/* 0x170 */
	uint ulpi_viewport;
	uint reserved7;
	uint endpt_nak;
	uint endpt_nak_enable;

	/* 0x180 */
	uint reserved;
	uint port_sc1;
	uint reserved8[6];

	/* 0x1a0 */
	uint reserved9;
	uint otgsc;
	uint usb_mode;
	uint endpt_setup_stat;

	/* 0x1b0 */
	uint reserved10[20];

	/* 0x200 */
	uint reserved11[0x80];

	/* 0x400 */
	uint susp_ctrl;
	uint phy_vbus_sensors;
	uint phy_vbus_wakeup_id;
	uint phy_alt_vbus_sys;

	/* 0x410 */
	uint usb1_legacy_ctrl;
	uint reserved12[4];

	/* 0x424 */
	uint ulpi_timing_ctrl_0;
	uint ulpi_timing_ctrl_1;
	uint reserved13[53];

	/* 0x500 */
	uint reserved14[64 * 3];

	/* 0x800 */
	uint utmip_pll_cfg0;
	uint utmip_pll_cfg1;
	uint utmip_xcvr_cfg0;
	uint utmip_bias_cfg0;

	/* 0x810 */
	uint utmip_hsrx_cfg0;
	uint utmip_hsrx_cfg1;
	uint utmip_fslsrx_cfg0;
	uint utmip_fslsrx_cfg1;

	/* 0x820 */
	uint utmip_tx_cfg0;
	uint utmip_misc_cfg0;
	uint utmip_misc_cfg1;
	uint utmip_debounce_cfg0;

	/* 0x830 */
	uint utmip_bat_chrg_cfg0;
	uint utmip_spare_cfg0;
	uint utmip_xcvr_cfg1;
	uint utmip_bias_cfg1;
};

/* USB2_IF_ULPI_TIMING_CTRL_0 */
#define ULPI_OUTPUT_PINMUX_BYP			(1 << 10)
#define ULPI_CLKOUT_PINMUX_BYP			(1 << 11)

/* USB2_IF_ULPI_TIMING_CTRL_1 */
#define ULPI_DATA_TRIMMER_LOAD			(1 << 0)
#define ULPI_DATA_TRIMMER_SEL(x)		(((x) & 0x7) << 1)
#define ULPI_STPDIRNXT_TRIMMER_LOAD		(1 << 16)
#define ULPI_STPDIRNXT_TRIMMER_SEL(x)	(((x) & 0x7) << 17)
#define ULPI_DIR_TRIMMER_LOAD			(1 << 24)
#define ULPI_DIR_TRIMMER_SEL(x)			(((x) & 0x7) << 25)

/* PORTSC, USB2, USB3 */
#define PTS_SHIFT		30
#define PTS_MASK		(3U << PTS_SHIFT)

#define STS			(1 << 29)
#endif /* _TEGRA20_USB_H_ */
