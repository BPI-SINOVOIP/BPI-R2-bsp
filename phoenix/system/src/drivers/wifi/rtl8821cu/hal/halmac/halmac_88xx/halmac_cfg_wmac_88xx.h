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

#ifndef _HALMAC_CFG_WMAC_88XX_H_
#define _HALMAC_CFG_WMAC_88XX_H_

#include "../halmac_api.h"

#if HALMAC_88XX_SUPPORT

enum halmac_ret_status
halmac_cfg_mac_addr_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 port,
	IN union halmac_wlan_addr *addr
);

enum halmac_ret_status
halmac_cfg_bssid_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 port,
	IN union halmac_wlan_addr *addr
);

enum halmac_ret_status
halmac_cfg_transmitter_addr_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 port,
	IN union halmac_wlan_addr *addr
);

enum halmac_ret_status
halmac_cfg_net_type_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 port,
	IN enum halmac_network_type_select net_type
);

enum halmac_ret_status
halmac_cfg_tsf_rst_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 port
);

enum halmac_ret_status
halmac_cfg_bcn_space_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 port,
	IN u32 bcn_space
);

enum halmac_ret_status
halmac_rw_bcn_ctrl_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 port,
	IN u8 write_en,
	INOUT struct halmac_bcn_ctrl *ctrl
);

enum halmac_ret_status
halmac_cfg_multicast_addr_88xx(
	IN struct halmac_adapter *adapter,
	IN union halmac_wlan_addr *addr
);

enum halmac_ret_status
halmac_cfg_operation_mode_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_wireless_mode mode
);

enum halmac_ret_status
halmac_cfg_ch_bw_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 ch,
	IN enum halmac_pri_ch_idx idx,
	IN enum halmac_bw bw
);

enum halmac_ret_status
halmac_cfg_ch_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 ch
);

enum halmac_ret_status
halmac_cfg_pri_ch_idx_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_pri_ch_idx idx
);

enum halmac_ret_status
halmac_cfg_bw_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_bw bw
);

void
halmac_enable_bb_rf_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 enable
);

void
halmac_config_ampdu_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_ampdu_config *cfg
);

enum halmac_ret_status
halmac_cfg_la_mode_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_la_mode mode
);

enum halmac_ret_status
halmac_cfg_rx_fifo_expanding_mode_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_rx_fifo_expanding_mode mode
);

enum halmac_ret_status
halmac_config_security_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_security_setting *setting
);

u8
halmac_get_used_cam_entry_num_88xx(
	IN struct halmac_adapter *adapter,
	IN enum hal_security_type sec_type
);

enum halmac_ret_status
halmac_write_cam_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 idx,
	IN struct halmac_cam_entry_info *info
);

enum halmac_ret_status
halmac_read_cam_entry_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 idx,
	OUT struct halmac_cam_entry_format *content
);

enum halmac_ret_status
halmac_clear_cam_entry_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 idx
);

void
halmac_rx_shift_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 enable
);

enum halmac_ret_status
halmac_cfg_edca_para_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_acq_id acq_id,
	IN struct halmac_edca_para *param
);

void
halmac_rx_clk_gate_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 enable
);

enum halmac_ret_status
halmac_rx_cut_amsdu_cfg_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_cut_amsdu_cfg *cfg
);

enum halmac_ret_status
halmac_fast_edca_cfg_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_fast_edca_cfg *cfg
);

enum halmac_ret_status
halmac_get_mac_addr_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 port,
	OUT union halmac_wlan_addr *addr
);

#endif/* HALMAC_88XX_SUPPORT */

#endif/* _HALMAC_CFG_WMAC_88XX_H_ */
