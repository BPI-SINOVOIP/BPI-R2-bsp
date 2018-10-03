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

#include "halmac_usb_8821c.h"
#include "halmac_pwr_seq_8821c.h"
#include "../halmac_init_88xx.h"
#include "../halmac_common_88xx.h"

#if HALMAC_8821C_SUPPORT

/**
 * halmac_mac_power_switch_8821c_usb() - switch mac power
 * @adapter : the adapter of halmac
 * @pwr : power state
 * Author : KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_mac_power_switch_8821c_usb(
	IN struct halmac_adapter *adapter,
	IN enum halmac_mac_power pwr
)
{
	u8 intf_mask;
	u8 value8;
	u8 rpwm;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s\n", __func__);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%x\n", pwr);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]8821C pwr seq ver = %s\n", HALMAC_8821C_PWR_SEQ_VER);

	intf_mask = HALMAC_PWR_INTF_USB_MSK;

	adapter->rpwm_record = HALMAC_REG_R8(0xFE58);

	/* Check FW still exist or not */
	if (HALMAC_REG_R16(REG_MCUFW_CTRL) == 0xC078) {
		/* Leave 32K */
		rpwm = (u8)((adapter->rpwm_record ^ BIT(7)) & 0x80);
		HALMAC_REG_W8(0xFE58, rpwm);
	}

	value8 = HALMAC_REG_R8(REG_CR);
	if (value8 == 0xEA) {
		adapter->halmac_state.mac_power = HALMAC_MAC_POWER_OFF;
	} else {
		if (BIT(0) == (HALMAC_REG_R8(REG_SYS_STATUS1 + 1) & BIT(0)))
			adapter->halmac_state.mac_power = HALMAC_MAC_POWER_OFF;
		else
			adapter->halmac_state.mac_power = HALMAC_MAC_POWER_ON;
	}

	/*Check if power switch is needed*/
	if (pwr == HALMAC_MAC_POWER_ON && adapter->halmac_state.mac_power == HALMAC_MAC_POWER_ON) {
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]halmac_mac_power_switch power state unchange!\n");
		return HALMAC_RET_PWR_UNCHANGE;
	}

	if (pwr == HALMAC_MAC_POWER_OFF) {
		if (halmac_pwr_seq_parser_88xx(adapter, HALMAC_PWR_CUT_ALL_MSK, HALMAC_PWR_FAB_TSMC_MSK,
					       intf_mask, halmac_8821c_card_disable_flow) != HALMAC_RET_SUCCESS) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Handle power off cmd error\n");
			return HALMAC_RET_POWER_OFF_FAIL;
		}

		adapter->halmac_state.mac_power = HALMAC_MAC_POWER_OFF;
		adapter->halmac_state.dlfw_state = HALMAC_DLFW_NONE;
		halmac_init_adapter_dynamic_para_88xx(adapter);
	} else {
		if (halmac_pwr_seq_parser_88xx(adapter, HALMAC_PWR_CUT_ALL_MSK, HALMAC_PWR_FAB_TSMC_MSK,
					       intf_mask, halmac_8821c_card_enable_flow) != HALMAC_RET_SUCCESS) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Handle power on cmd error\n");
			return HALMAC_RET_POWER_ON_FAIL;
		}

		HALMAC_REG_W8(REG_SYS_STATUS1 + 1, HALMAC_REG_R8(REG_SYS_STATUS1 + 1) & ~(BIT(0)));

		if ((HALMAC_REG_R8(REG_SW_MDIO + 3) & BIT(0)) == BIT(0))
			PLTFM_MSG_PRINT(HALMAC_DBG_ALWAYS, "[WARN]This version shall R register twice!!\n");

		adapter->halmac_state.mac_power = HALMAC_MAC_POWER_ON;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_phy_cfg_8821c_usb() - phy config
 * @adapter : the adapter of halmac
 * Author : KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_phy_cfg_8821c_usb(
	IN struct halmac_adapter *adapter,
	IN enum halmac_intf_phy_platform pltfm
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	status = halmac_parse_intf_phy_88xx(adapter, HALMAC_RTL8821C_USB2_PHY, pltfm, HAL_INTF_PHY_USB2);

	if (status != HALMAC_RET_SUCCESS)
		return status;

	status = halmac_parse_intf_phy_88xx(adapter, HALMAC_RTL8821C_USB3_PHY, pltfm, HAL_INTF_PHY_USB3);

	if (status != HALMAC_RET_SUCCESS)
		return status;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_pcie_switch_8821c() - pcie gen1/gen2 switch
 * @adapter : the adapter of halmac
 * @pcie_cfg : gen1/gen2 selection
 * Author : KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_pcie_switch_8821c_usb(
	IN struct halmac_adapter *adapter,
	IN enum halmac_pcie_cfg	cfg
)
{
	return HALMAC_RET_NOT_SUPPORT;
}

/**
 * halmac_interface_integration_tuning_8821c_usb() - usb interface fine tuning
 * @adapter : the adapter of halmac
 * Author : Ivan
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_interface_integration_tuning_8821c_usb(
	IN struct halmac_adapter *adapter
)
{
	return HALMAC_RET_SUCCESS;
}

#endif /* HALMAC_8821C_SUPPORT */
