/******************************************************************************
 *
 * Copyright(c) 2017 Realtek Corporation. All rights reserved.
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

#ifndef _HALMAC_CFG_WMAC_8821C_H_
#define _HALMAC_CFG_WMAC_8821C_H_

#include "../../halmac_api.h"

#if HALMAC_8821C_SUPPORT

enum halmac_ret_status
halmac_cfg_drv_info_8821c(
	IN struct halmac_adapter *adapter,
	IN enum halmac_drv_info drv_info
);

enum halmac_ret_status
halmac_init_low_pwr_8821c(
	IN struct halmac_adapter *adapter
);

void
halmac_cfg_mac_rx_ignore_8821c(
	IN struct halmac_adapter *adapter,
	IN struct halmac_mac_rx_ignore_cfg *cfg
);

#endif/* HALMAC_8821C_SUPPORT */

#endif/* _HALMAC_CFG_WMAC_8821C_H_ */
