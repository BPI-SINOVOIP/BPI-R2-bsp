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

#ifndef _HALMAC_MIMO_88XX_H_
#define _HALMAC_MIMO_88XX_H_

#include "../halmac_api.h"

#if HALMAC_88XX_SUPPORT

#endif /* HALMAC_88XX_SUPPORT */

enum halmac_ret_status
halmac_cfg_txbf_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 userid,
	IN enum halmac_bw bw,
	IN u8 txbf_en
);

enum halmac_ret_status
halmac_cfg_mumimo_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_cfg_mumimo_para *param
);

enum halmac_ret_status
halmac_cfg_sounding_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_snd_role role,
	IN enum halmac_data_rate rate
);

enum halmac_ret_status
halmac_del_sounding_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_snd_role role
);

enum halmac_ret_status
halmac_su_bfee_entry_init_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 userid,
	IN u16 paid
);

enum halmac_ret_status
halmac_su_bfer_entry_init_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_su_bfer_init_para *param
);

enum halmac_ret_status
halmac_mu_bfee_entry_init_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_mu_bfee_init_para *param
);

enum halmac_ret_status
halmac_mu_bfer_entry_init_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_mu_bfer_init_para *param
);

enum halmac_ret_status
halmac_su_bfee_entry_del_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 userid
);

enum halmac_ret_status
halmac_su_bfer_entry_del_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 userid
);

enum halmac_ret_status
halmac_mu_bfee_entry_del_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 userid
);

enum halmac_ret_status
halmac_mu_bfer_entry_del_88xx(
	IN struct halmac_adapter *adapter
);

enum halmac_ret_status
halmac_cfg_csi_rate_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 rssi,
	IN u8 cur_rate,
	IN u8 fixrate_en,
	OUT u8 *new_rate
);

enum halmac_ret_status
halmac_fw_snding_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_su_snding_info *su_info,
	IN struct halmac_mu_snding_info *mu_info,
	IN u8 period
);

enum halmac_ret_status
halmac_get_h2c_ack_fw_snding_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
);

enum halmac_ret_status
halmac_query_fw_snding_status_88xx(
	IN struct halmac_adapter *adapter,
	OUT enum halmac_cmd_process_status *proc_status,
	INOUT u8 *data,
	INOUT u32 *size
);

#endif/* _HALMAC_MIMO_88XX_H_ */
