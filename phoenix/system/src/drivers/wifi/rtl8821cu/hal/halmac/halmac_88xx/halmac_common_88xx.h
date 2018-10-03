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

#ifndef _HALMAC_COMMON_88XX_H_
#define _HALMAC_COMMON_88XX_H_

#include "../halmac_api.h"
#include "../halmac_pwr_seq_cmd.h"
#include "../halmac_gpio_cmd.h"

#if HALMAC_88XX_SUPPORT

enum halmac_ret_status
halmac_ofld_func_cfg_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_ofld_func_info *info
);

enum halmac_ret_status
halmac_dl_drv_rsvd_page_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 pg_offset,
	IN u8 *buf,
	IN u32 size
);

enum halmac_ret_status
halmac_download_rsvd_page_88xx(
	IN struct halmac_adapter *adapter,
	IN u16 pg_addr,
	IN u8 *buf,
	IN u32 size
);

enum halmac_ret_status
halmac_get_hw_value_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_hw_id hw_id,
	OUT void *value
);

enum halmac_ret_status
halmac_set_hw_value_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_hw_id hw_id,
	IN void *value
);

enum halmac_ret_status
halmac_set_h2c_pkt_hdr_88xx(
	IN struct halmac_adapter *adapter,
	OUT u8 *hdr,
	IN struct halmac_h2c_header_info *info,
	OUT u16 *seq_num
);

enum halmac_ret_status
halmac_send_h2c_pkt_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *pkt,
	IN u32 size
);

enum halmac_ret_status
halmac_get_h2c_buff_free_space_88xx(
	IN struct halmac_adapter *adapter
);

enum halmac_ret_status
halmac_get_c2h_info_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
);

enum halmac_ret_status
halmac_debug_88xx(
	IN struct halmac_adapter *adapter
);

enum halmac_ret_status
halmac_cfg_parameter_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_phy_parameter_info *info,
	IN u8 full_fifo
);

enum halmac_ret_status
halmac_update_packet_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_packet_id pkt_id,
	IN u8 *pkt,
	IN u32 size
);

enum halmac_ret_status
halmac_bcn_ie_filter_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_bcn_ie_info *info
);

enum halmac_ret_status
halmac_update_datapack_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_data_type data_type,
	IN struct halmac_phy_parameter_info *info
);

enum halmac_ret_status
halmac_run_datapack_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_data_type data_type
);

enum halmac_ret_status
halmac_send_bt_coex_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size,
	IN u8 ack
);

enum halmac_ret_status
halmac_send_original_h2c_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *org_h2c,
	IN u16 *seq,
	IN u8 ack
);

enum halmac_ret_status
halmac_dump_fifo_88xx(
	IN struct halmac_adapter *adapter,
	IN enum hal_fifo_sel sel,
	IN u32 start_addr,
	IN u32 size,
	OUT u8 *data
);

u32
halmac_get_fifo_size_88xx(
	IN struct halmac_adapter *adapter,
	IN enum hal_fifo_sel sel
);

enum halmac_ret_status
halmac_set_h2c_header_88xx(
	IN struct halmac_adapter *adapter,
	OUT u8 *hdr,
	IN u16 *seq,
	IN u8 ack
);

enum halmac_ret_status
halmac_add_ch_info_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_ch_info *info
);

enum halmac_ret_status
halmac_add_extra_ch_info_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_ch_extra_info *info
);

enum halmac_ret_status
halmac_ctrl_ch_switch_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_ch_switch_option *opt
);

enum halmac_ret_status
halmac_clear_ch_info_88xx(
	IN struct halmac_adapter *adapter
);

enum halmac_ret_status
halmac_send_general_info_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_general_info *info
);

enum halmac_ret_status
halmac_chk_txdesc_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
);

enum halmac_ret_status
halmac_get_version_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_ver *ver
);

enum halmac_ret_status
halmac_p2pps_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_p2pps *info
);

enum halmac_ret_status
halmac_query_status_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_feature_id feature_id,
	OUT enum halmac_cmd_process_status *proc_status,
	INOUT u8 *data,
	INOUT u32 *size
);

enum halmac_ret_status
halmac_cfg_drv_rsvd_pg_num_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_drv_rsvd_pg_num pg_num
);

enum halmac_ret_status
halmac_h2c_lb_88xx(
	IN struct halmac_adapter *adapter
);

enum halmac_ret_status
halmac_pwr_seq_parser_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 cut,
	IN u8 fab,
	IN u8 intf,
	IN struct halmac_wlan_pwr_cfg **cmd_seq

);

enum halmac_ret_status
halmac_parse_intf_phy_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_intf_phy_para *param,
	IN enum halmac_intf_phy_platform pltfm,
	IN enum hal_intf_phy intf_phy
);

enum halmac_ret_status
halmac_txfifo_is_empty_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 chk_num
);

u8*
halmac_adaptive_malloc_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 size,
	OUT u32 *new_size
);

enum halmac_ret_status
halmac_ltecoex_reg_read_88xx(
	IN struct halmac_adapter *adapter,
	IN u16 offset,
	OUT u32 *value
);

enum halmac_ret_status
halmac_ltecoex_reg_write_88xx(
	IN struct halmac_adapter *adapter,
	IN u16 offset,
	IN u32 value
);

enum halmac_ret_status
halmac_download_flash_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *fw_bin,
	IN u32 size,
	IN u32 rom_addr
);

enum halmac_ret_status
halmac_read_flash_88xx(
	IN struct halmac_adapter *adapter,
	u32 addr
);

enum halmac_ret_status
halmac_erase_flash_88xx(
	IN struct halmac_adapter *adapter,
	u8 erase_cmd,
	u32 addr
);

enum halmac_ret_status
halmac_check_flash_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *fw_bin,
	IN u32 size,
	IN u32 addr
);

#endif/* HALMAC_88XX_SUPPORT */

#endif/* _HALMAC_COMMON_88XX_H_ */
