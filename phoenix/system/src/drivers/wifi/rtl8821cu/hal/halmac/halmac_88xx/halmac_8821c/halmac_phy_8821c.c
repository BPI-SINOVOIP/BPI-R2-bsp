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

#include "../../halmac_type.h"

/**
 * ============ip sel item list============
 * HALMAC_IP_SEL_INTF_PHY
 *	USB2 : usb2 phy, 1byte value
 *	USB3 : usb3 phy, 2byte value
 *	PCIE1 : pcie gen1 mdio, 2byte value
 *	PCIE2 : pcie gen2 mdio, 2byte value
 * HALMAC_IP_SEL_MAC
 *	USB2, USB3, PCIE1, PCIE2 : mac ip, 1byte value
 * HALMAC_IP_SEL_PCIE_DBI
 *	USB2 USB3 : none
 *	PCIE1, PCIE2 : pcie dbi, 1byte value
 */

#if HALMAC_8821C_SUPPORT

struct halmac_intf_phy_para HALMAC_RTL8821C_USB2_PHY[] = {
	/* {offset, value, ip sel, cut mask, platform mask} */
	{0xFFFF, 0x00, HALMAC_IP_SEL_INTF_PHY, HALMAC_INTF_PHY_CUT_ALL, HALMAC_INTF_PHY_PLATFORM_ALL},
};

struct halmac_intf_phy_para HALMAC_RTL8821C_USB3_PHY[] = {
	/* {offset, value, cut mask, platform mask} */
	{0xFFFF, 0x0000, HALMAC_IP_SEL_INTF_PHY, HALMAC_INTF_PHY_CUT_ALL, HALMAC_INTF_PHY_PLATFORM_ALL},
};

struct halmac_intf_phy_para HALMAC_RTL8821C_PCIE_PHY_GEN1[] = {
	/* {offset, value, ip sel, cut mask, platform mask} */
	{0x000F, 0x1A10, HALMAC_IP_SEL_INTF_PHY, HALMAC_INTF_PHY_CUT_ALL, HALMAC_INTF_PHY_PLATFORM_ALL},
	{0xFFFF, 0x0000, HALMAC_IP_SEL_INTF_PHY, HALMAC_INTF_PHY_CUT_ALL, HALMAC_INTF_PHY_PLATFORM_ALL},
};

struct halmac_intf_phy_para HALMAC_RTL8821C_PCIE_PHY_GEN2[] = {
	/* {offset, value, ip sel, cut mask, platform mask} */
	{0xFFFF, 0x0000, HALMAC_IP_SEL_INTF_PHY, HALMAC_INTF_PHY_CUT_ALL, HALMAC_INTF_PHY_PLATFORM_ALL},
};

#endif /* HALMAC_8821C_SUPPORT */