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

#ifndef _HALMAC_SDIO_8821C_H_
#define _HALMAC_SDIO_8821C_H_

#include "../../halmac_api.h"
#include "halmac_8821c_cfg.h"

#if HALMAC_8821C_SUPPORT

enum halmac_ret_status
halmac_mac_power_switch_8821c_sdio(
	IN struct halmac_adapter *adapter,
	IN enum halmac_mac_power pwr
);

enum halmac_ret_status
halmac_tx_allowed_8821c_sdio(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
);

enum halmac_ret_status
halmac_phy_cfg_8821c_sdio(
	IN struct halmac_adapter *adapter,
	IN enum halmac_intf_phy_platform pltfm
);

enum halmac_ret_status
halmac_pcie_switch_8821c_sdio(
	IN struct halmac_adapter *adapter,
	IN enum halmac_pcie_cfg	cfg
);

enum halmac_ret_status
halmac_interface_integration_tuning_8821c_sdio(
	IN struct halmac_adapter *adapter
);

enum halmac_ret_status
halmac_get_sdio_tx_addr_8821c_sdio(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size,
	OUT u32 *cmd53_addr
);

#endif /* HALMAC_8821C_SUPPORT */

#endif/* _HALMAC_SDIO_8821C_H_ */
