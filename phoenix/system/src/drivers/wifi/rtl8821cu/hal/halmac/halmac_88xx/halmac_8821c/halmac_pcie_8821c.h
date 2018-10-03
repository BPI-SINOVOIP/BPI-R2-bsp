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

#ifndef _HALMAC_PCIE_8821C_H_
#define _HALMAC_PCIE_8821C_H_

#include "../../halmac_api.h"

#if HALMAC_8821C_SUPPORT

extern struct halmac_intf_phy_para HALMAC_RTL8821C_PCIE_PHY_GEN1[];
extern struct halmac_intf_phy_para HALMAC_RTL8821C_PCIE_PHY_GEN2[];

enum halmac_ret_status
halmac_mac_power_switch_8821c_pcie(
	IN struct halmac_adapter *adapter,
	IN enum halmac_mac_power pwr
);

enum halmac_ret_status
halmac_pcie_switch_8821c_pcie(
	IN struct halmac_adapter *adapter,
	IN enum halmac_pcie_cfg	cfg
);

enum halmac_ret_status
halmac_phy_cfg_8821c_pcie(
	IN struct halmac_adapter *adapter,
	IN enum halmac_intf_phy_platform pltfm
);

enum halmac_ret_status
halmac_interface_integration_tuning_8821c_pcie(
	IN struct halmac_adapter *adapter
);

#endif /* HALMAC_8821C_SUPPORT */

#endif/* _HALMAC_PCIE_8821C_H_ */
