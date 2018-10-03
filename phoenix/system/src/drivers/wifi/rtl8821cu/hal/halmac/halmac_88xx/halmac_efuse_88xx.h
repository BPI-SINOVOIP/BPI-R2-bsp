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

#ifndef _HALMAC_EFUSE_88XX_H_
#define _HALMAC_EFUSE_88XX_H_

#include "../halmac_api.h"

#if HALMAC_88XX_SUPPORT

enum halmac_ret_status
halmac_dump_efuse_map_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_efuse_read_cfg cfg
);

enum halmac_ret_status
halmac_dump_efuse_map_bt_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_efuse_bank bank,
	IN u32 size,
	OUT u8 *map
);

enum halmac_ret_status
halmac_write_efuse_bt_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u8 value,
	IN enum halmac_efuse_bank bank
);

enum halmac_ret_status
halmac_read_efuse_bt_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	OUT u8 *value,
	IN enum halmac_efuse_bank bank
);

enum halmac_ret_status
halmac_cfg_efuse_auto_check_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 enable
);

enum halmac_ret_status
halmac_get_efuse_available_size_88xx(
	IN struct halmac_adapter *adapter,
	OUT u32 *size
);

enum halmac_ret_status
halmac_get_efuse_size_88xx(
	IN struct halmac_adapter *adapter,
	OUT u32 *size
);

enum halmac_ret_status
halmac_get_logical_efuse_size_88xx(
	IN struct halmac_adapter *adapter,
	OUT u32 *size
);

enum halmac_ret_status
halmac_dump_logical_efuse_map_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_efuse_read_cfg cfg
);

enum halmac_ret_status
halmac_read_logical_efuse_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	OUT u8 *value
);

enum halmac_ret_status
halmac_write_logical_efuse_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u8 value
);

enum halmac_ret_status
halmac_pg_efuse_by_map_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_pg_efuse_info *info,
	IN enum halmac_efuse_read_cfg cfg
);

enum halmac_ret_status
halmac_mask_logical_efuse_88xx(
	IN struct halmac_adapter *adapter,
	INOUT struct halmac_pg_efuse_info *info
);

enum halmac_ret_status
halmac_func_read_efuse_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u32 size,
	OUT u8 *map
);

enum halmac_ret_status
halmac_func_write_efuse_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u8 value
);

enum halmac_ret_status
halmac_switch_efuse_bank_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_efuse_bank bank
);

enum halmac_ret_status
halmac_get_efuse_data_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
);

enum halmac_ret_status
halmac_transform_efuse_state_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_efuse_cmd_construct_state dest_state
);

enum halmac_ret_status
halmac_query_dump_physical_efuse_status_88xx(
	IN struct halmac_adapter *adapter,
	OUT enum halmac_cmd_process_status *proc_status,
	INOUT u8 *data,
	INOUT u32 *size
);

enum halmac_ret_status
halmac_query_dump_logical_efuse_status_88xx(
	IN struct halmac_adapter *adapter,
	OUT enum halmac_cmd_process_status *proc_status,
	INOUT u8 *data,
	INOUT u32 *size
);

enum halmac_ret_status
halmac_get_h2c_ack_phy_efuse_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
);

#endif /* HALMAC_88XX_SUPPORT */

#endif/* _HALMAC_EFUSE_88XX_H_ */
