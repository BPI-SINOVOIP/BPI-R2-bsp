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

#ifndef _HALMAC_SDIO_88XX_H_
#define _HALMAC_SDIO_88XX_H_

#include "../halmac_api.h"

#if HALMAC_88XX_SUPPORT

enum halmac_ret_status
halmac_init_sdio_cfg_88xx(
	IN struct halmac_adapter *adapter
);

enum halmac_ret_status
halmac_deinit_sdio_cfg_88xx(
	IN struct halmac_adapter *adapter
);

enum halmac_ret_status
halmac_cfg_rx_aggregation_88xx_sdio(
	IN struct halmac_adapter *adapter,
	IN struct halmac_rxagg_cfg *cfg
);

u8
halmac_reg_read_8_sdio_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset
);

enum halmac_ret_status
halmac_reg_write_8_sdio_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u8 value
);

u16
halmac_reg_read_16_sdio_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset
);

enum halmac_ret_status
halmac_reg_write_16_sdio_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u16 value
);

u32
halmac_reg_read_32_sdio_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset
);

enum halmac_ret_status
halmac_reg_write_32_sdio_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u32 value
);

enum halmac_ret_status
halmac_cfg_tx_agg_align_sdio_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 enable,
	IN u16 align_size
);

u32
halmac_reg_read_indirect_32_sdio_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset
);

enum halmac_ret_status
halmac_reg_read_nbyte_sdio_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u32 size,
	OUT u8 *value
);

enum halmac_ret_status
halmac_set_bulkout_num_sdio_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 num
);

enum halmac_ret_status
halmac_get_usb_bulkout_id_sdio_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *halmac_buf,
	IN u32 size,
	OUT u8 *bulkout_id
);

enum halmac_ret_status
halmac_sdio_cmd53_4byte_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_sdio_cmd53_4byte_mode mode
);

enum halmac_ret_status
halmac_sdio_hw_info_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_sdio_hw_info *info
);

void
halmac_config_sdio_tx_page_threshold_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_tx_page_threshold_info *info
);

enum halmac_ret_status
halmac_convert_to_sdio_bus_offset_88xx(
	IN struct halmac_adapter *adapter,
	INOUT u32 *offset
);

#endif /* HALMAC_88XX_SUPPORT */

#endif/* _HALMAC_SDIO_88XX_H_ */
