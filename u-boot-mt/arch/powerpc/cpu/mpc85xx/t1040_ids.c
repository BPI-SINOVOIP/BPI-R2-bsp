/*
 * Copyright 2012 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/fsl_portals.h>
#include <asm/fsl_liodn.h>

#ifdef CONFIG_SYS_DPAA_QBMAN
struct qportal_info qp_info[CONFIG_SYS_QMAN_NUM_PORTALS] = {
	/* dqrr liodn, frame data liodn, liodn off, sdest */
	SET_QP_INFO(1, 27, 1, 0),
	SET_QP_INFO(2, 28, 1, 0),
	SET_QP_INFO(3, 29, 1, 1),
	SET_QP_INFO(4, 30, 1, 1),
	SET_QP_INFO(5, 31, 1, 2),
	SET_QP_INFO(6, 32, 1, 2),
	SET_QP_INFO(7, 33, 1, 3),
	SET_QP_INFO(8, 34, 1, 3),
	SET_QP_INFO(9, 35, 1, 0),
	SET_QP_INFO(10, 36, 1, 0),
};
#endif

struct srio_liodn_id_table srio_liodn_tbl[] = {
	SET_SRIO_LIODN_1(1, 307),
	SET_SRIO_LIODN_1(2, 387),
};
int srio_liodn_tbl_sz = ARRAY_SIZE(srio_liodn_tbl);

struct liodn_id_table liodn_tbl[] = {
#ifdef CONFIG_SYS_DPAA_QBMAN
	SET_QMAN_LIODN(62),
	SET_BMAN_LIODN(63),
#endif

	SET_SDHC_LIODN(1, 552),

	SET_USB_LIODN(1, "fsl-usb2-mph", 553),

	SET_PCI_LIODN("fsl,qoriq-pcie-v2.2", 1, 148),

	SET_DMA_LIODN(1, 147),
	SET_DMA_LIODN(2, 227),

	/* SET_NEXUS_LIODN(557), -- not yet implemented */
};
int liodn_tbl_sz = ARRAY_SIZE(liodn_tbl);

#ifdef CONFIG_SYS_DPAA_FMAN
struct liodn_id_table fman1_liodn_tbl[] = {
	SET_FMAN_RX_1G_LIODN(1, 0, 88),
	SET_FMAN_RX_1G_LIODN(1, 1, 89),
	SET_FMAN_RX_1G_LIODN(1, 2, 90),
	SET_FMAN_RX_1G_LIODN(1, 3, 91),
	SET_FMAN_RX_1G_LIODN(1, 4, 92),
	SET_FMAN_RX_1G_LIODN(1, 5, 93),
};
int fman1_liodn_tbl_sz = ARRAY_SIZE(fman1_liodn_tbl);
#endif

struct liodn_id_table sec_liodn_tbl[] = {
	SET_SEC_JR_LIODN_ENTRY(0, 454, 458),
	SET_SEC_JR_LIODN_ENTRY(1, 455, 459),
	SET_SEC_JR_LIODN_ENTRY(2, 456, 460),
	SET_SEC_JR_LIODN_ENTRY(3, 457, 461),
	SET_SEC_RTIC_LIODN_ENTRY(a, 453),
	SET_SEC_RTIC_LIODN_ENTRY(b, 549),
	SET_SEC_RTIC_LIODN_ENTRY(c, 550),
	SET_SEC_RTIC_LIODN_ENTRY(d, 551),
	SET_SEC_DECO_LIODN_ENTRY(0, 541, 610),
	SET_SEC_DECO_LIODN_ENTRY(1, 542, 611),
};
int sec_liodn_tbl_sz = ARRAY_SIZE(sec_liodn_tbl);

struct liodn_id_table liodn_bases[] = {
	[FSL_HW_PORTAL_SEC]  = SET_LIODN_BASE_2(462, 558),
#ifdef CONFIG_SYS_DPAA_FMAN
	[FSL_HW_PORTAL_FMAN1] = SET_LIODN_BASE_1(973),
#endif
};
