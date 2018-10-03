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

#ifndef _HALMAC_PCIE_88XX_H_
#define _HALMAC_PCIE_88XX_H_

#include "../halmac_api.h"

#if HALMAC_88XX_SUPPORT

enum halmac_ret_status
halmac_init_pcie_cfg_88xx(
	IN struct halmac_adapter *adapter
);

enum halmac_ret_status
halmac_deinit_pcie_cfg_88xx(
	IN struct halmac_adapter *adapter
);

enum halmac_ret_status
halmac_cfg_rx_aggregation_88xx_pcie(
	IN struct halmac_adapter *adapter,
	IN struct halmac_rxagg_cfg *cfg
);

u8
halmac_reg_read_8_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset
);

enum halmac_ret_status
halmac_reg_write_8_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u8 value
);

u16
halmac_reg_read_16_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset
);

enum halmac_ret_status
halmac_reg_write_16_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u16 value
);

u32
halmac_reg_read_32_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset
);

enum halmac_ret_status
halmac_reg_write_32_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u32 value
);

enum halmac_ret_status
halmac_cfg_tx_agg_align_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u8	enable,
	IN u16	align_size
);

u32
halmac_reg_read_indirect_32_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset
);

enum halmac_ret_status
halmac_reg_read_nbyte_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u32 size,
	OUT u8 *value
);

enum halmac_ret_status
halmac_set_bulkout_num_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 num
);

enum halmac_ret_status
halmac_get_sdio_tx_addr_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size,
	OUT u32 *cmd53_addr
);

enum halmac_ret_status
halmac_get_usb_bulkout_id_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size,
	OUT u8 *id
);

enum halmac_ret_status
halmac_mdio_write_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 addr,
	IN u16 data,
	IN u8 speed
);

u16
halmac_mdio_read_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 addr,
	IN u8 speed

);

enum halmac_ret_status
halmac_dbi_write32_88xx(
	IN struct halmac_adapter *adapter,
	IN u16 addr,
	IN u32 data
);

u32
halmac_dbi_read32_88xx(
	IN struct halmac_adapter *adapter,
	IN u16 addr
);

enum halmac_ret_status
halmac_dbi_write8_88xx(
	IN struct halmac_adapter *adapter,
	IN u16 addr,
	IN u8 data
);

u8
halmac_dbi_read8_88xx(
	IN struct halmac_adapter *adapter,
	IN u16 addr
);

enum halmac_ret_status
halmac_trxdma_check_idle_88xx(
	IN struct halmac_adapter *adapter
);

#endif /* HALMAC_88XX_SUPPORT */

#endif/* _HALMAC_PCIE_88XX_H_ */
