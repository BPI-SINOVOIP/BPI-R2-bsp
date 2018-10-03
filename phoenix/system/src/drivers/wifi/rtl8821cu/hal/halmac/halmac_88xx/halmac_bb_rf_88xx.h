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

#ifndef _HALMAC_BB_RF_88XX_H_
#define _HALMAC_BB_RF_88XX_H_

#include "../halmac_api.h"

#if HALMAC_88XX_SUPPORT

enum halmac_ret_status
halmac_start_iqk_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_iqk_para *param
);

enum halmac_ret_status
halmac_ctrl_pwr_tracking_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_pwr_tracking_option *opt
);

enum halmac_ret_status
halmac_query_iqk_status_88xx(
	IN struct halmac_adapter *adapter,
	OUT enum halmac_cmd_process_status *proc_status,
	INOUT u8 *data,
	INOUT u32 *size
);

enum halmac_ret_status
halmac_query_power_tracking_status_88xx(
	IN struct halmac_adapter *adapter,
	OUT enum halmac_cmd_process_status *proc_status,
	INOUT u8 *data,
	INOUT u32 *size
);

enum halmac_ret_status
halmac_query_psd_status_88xx(
	IN struct halmac_adapter *adapter,
	OUT enum halmac_cmd_process_status *proc_status,
	INOUT u8 *data,
	INOUT u32 *size
);

enum halmac_ret_status
halmac_psd_88xx(
	IN struct halmac_adapter *adapter,
	IN u16 start_psd,
	IN u16 end_psd
);

enum halmac_ret_status
halmac_get_h2c_ack_iqk_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
);

enum halmac_ret_status
halmac_get_h2c_ack_power_tracking_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
);

#endif /* HALMAC_88XX_SUPPORT */

#endif/* _HALMAC_BB_RF_88XX_H_ */
