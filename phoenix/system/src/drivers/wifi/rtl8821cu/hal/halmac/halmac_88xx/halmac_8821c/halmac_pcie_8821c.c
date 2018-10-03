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

#include "halmac_pcie_8821c.h"
#include "halmac_pwr_seq_8821c.h"
#include "../halmac_init_88xx.h"
#include "../halmac_common_88xx.h"
#include "../halmac_pcie_88xx.h"
#include "halmac_8821c_cfg.h"

#if HALMAC_8821C_SUPPORT

static enum halmac_ret_status
halmac_auto_refclk_cal_8821c_pcie(
	IN struct halmac_adapter *adapter
);

/**
 * halmac_mac_power_switch_8821c_pcie() - switch mac power
 * @adapter : the adapter of halmac
 * @pwr : power state
 * Author : KaiYuan Chang / Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_mac_power_switch_8821c_pcie(
	IN struct halmac_adapter *adapter,
	IN enum halmac_mac_power pwr
)
{
	u8 intf_mask;
	u8 value8;
	u8 rpwm;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	enum halmac_ret_status status;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]pwr = %x\n", pwr);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]8821C pwr seq ver = %s\n", HALMAC_8821C_PWR_SEQ_VER);

	intf_mask = HALMAC_PWR_INTF_PCI_MSK;

	adapter->rpwm_record = HALMAC_REG_R8(REG_PCIE_HRPWM1_V1);

	/* Check FW still exist or not */
	if (HALMAC_REG_R16(REG_MCUFW_CTRL) == 0xC078) {
		/* Leave 32K */
		rpwm = (u8)((adapter->rpwm_record ^ BIT(7)) & 0x80);
		HALMAC_REG_W8(REG_PCIE_HRPWM1_V1, rpwm);
	}

	value8 = HALMAC_REG_R8(REG_CR);
	if (value8 == 0xEA)
		adapter->halmac_state.mac_power = HALMAC_MAC_POWER_OFF;
	else
		adapter->halmac_state.mac_power = HALMAC_MAC_POWER_ON;

	/* Check if power switch is needed */
	if (pwr == HALMAC_MAC_POWER_ON && adapter->halmac_state.mac_power == HALMAC_MAC_POWER_ON) {
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]halmac_mac_power_switch power state unchange!\n");
		return HALMAC_RET_PWR_UNCHANGE;
	}

	if (pwr == HALMAC_MAC_POWER_OFF) {
		status = halmac_trxdma_check_idle_88xx(adapter);
		if (status != HALMAC_RET_SUCCESS)
			return status;
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

		adapter->halmac_state.mac_power = HALMAC_MAC_POWER_ON;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_pcie_switch_8821c() - pcie gen1/gen2 switch
 * @adapter : the adapter of halmac
 * @cfg : gen1/gen2 selection
 * Author : KaiYuan Chang / Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_pcie_switch_8821c_pcie(
	IN struct halmac_adapter *adapter,
	IN enum halmac_pcie_cfg	cfg
)
{
	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_phy_cfg_8821c_pcie() - phy config
 * @adapter : the adapter of halmac
 * Author : KaiYuan Chang / Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_phy_cfg_8821c_pcie(
	IN struct halmac_adapter *adapter,
	IN enum halmac_intf_phy_platform pltfm
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	status = halmac_parse_intf_phy_88xx(adapter, HALMAC_RTL8821C_PCIE_PHY_GEN1, pltfm, HAL_INTF_PHY_PCIE_GEN1);

	if (status != HALMAC_RET_SUCCESS)
		return status;

	status = halmac_parse_intf_phy_88xx(adapter, HALMAC_RTL8821C_PCIE_PHY_GEN2, pltfm, HAL_INTF_PHY_PCIE_GEN2);

	if (status != HALMAC_RET_SUCCESS)
		return status;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_interface_integration_tuning_8821c_pcie() - pcie interface fine tuning
 * @adapter : the adapter of halmac
 * Author : Rick Liu
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_interface_integration_tuning_8821c_pcie(
	IN struct halmac_adapter *adapter
)
{
	enum halmac_ret_status status;

	status = halmac_auto_refclk_cal_8821c_pcie(adapter);

	return status;
}

static enum halmac_ret_status
halmac_auto_refclk_cal_8821c_pcie(
	IN struct halmac_adapter *adapter
)
{
	u8 backdoor_org;
	u16 tmp_u16;
	u16 div_set;
	u16 margin_tmp16;
	u16 margin_set;
	u16 target_est;
	u16 target_final;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	u8 l1_write_flag = 0;
	u32 count_tar;

	/* Disable L1 backdoor at 0x719[4:3] */
	backdoor_org = halmac_dbi_read8_88xx(adapter, PCIE_L1_BACKDOOR);
	if (backdoor_org & (BIT(4) | BIT(3))) {
		status = halmac_dbi_write8_88xx(adapter, PCIE_L1_BACKDOOR, backdoor_org & ~(BIT(4) | BIT(3)));
		if (status != HALMAC_RET_SUCCESS)
			return status;
		l1_write_flag = 1;
	}

	/* Disable this function before configuration*/
	tmp_u16 = halmac_mdio_read_88xx(adapter, CLKCAL_CTRL_PHYPARA, HAL_INTF_PHY_PCIE_GEN1);
	if (tmp_u16 & BIT(9)) {
		status = halmac_mdio_write_88xx(adapter, CLKCAL_CTRL_PHYPARA, tmp_u16 & ~(BIT(9)), HAL_INTF_PHY_PCIE_GEN1);
		if (status != HALMAC_RET_SUCCESS)
			return status;
	}

	/* If minref value is 0 or large than 100, then disable this function */
	if ((HALMAC_INTF_INTGRA_HOSTREF_8821C <= HALMAC_INTF_INTGRA_MINREF_8821C) | (HALMAC_INTF_INTGRA_MINREF_8821C <= 0)) {
		if (l1_write_flag == 1)
			status = halmac_dbi_write8_88xx(adapter, PCIE_L1_BACKDOOR, backdoor_org);
		return status;
	}

	/* Set div 2048 at 0x00[7:6] to estimate slow clock*/
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);
	tmp_u16 = halmac_mdio_read_88xx(adapter, CLKCAL_CTRL_PHYPARA, HAL_INTF_PHY_PCIE_GEN1);
	status = halmac_mdio_write_88xx(adapter, CLKCAL_CTRL_PHYPARA, tmp_u16 & ~(BIT(7) | BIT(6)), HAL_INTF_PHY_PCIE_GEN1);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	/* Set 0x00[11]=1 to count 1T of reference clock and read target value at 0x21[11:0] */
	tmp_u16 = halmac_mdio_read_88xx(adapter, CLKCAL_CTRL_PHYPARA, HAL_INTF_PHY_PCIE_GEN1);
	status = halmac_mdio_write_88xx(adapter, CLKCAL_CTRL_PHYPARA, tmp_u16 | BIT(11), HAL_INTF_PHY_PCIE_GEN1);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	count_tar = 5;
	do {
		PLTFM_DELAY_US(22);
		target_est = halmac_mdio_read_88xx(adapter, CLKCAL_TRG_VAL_PHYPARA, HAL_INTF_PHY_PCIE_GEN1);
		count_tar--;
	} while ((count_tar > 0) && (target_est == 0));
	if (target_est == 0) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Estimated target reads failed.\n");
		return HALMAC_RET_FAIL;
	}

	tmp_u16 = halmac_mdio_read_88xx(adapter, CLKCAL_CTRL_PHYPARA, HAL_INTF_PHY_PCIE_GEN1);
	status = halmac_mdio_write_88xx(adapter, CLKCAL_CTRL_PHYPARA, tmp_u16 & ~(BIT(11)), HAL_INTF_PHY_PCIE_GEN1);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	/* Based on mininum tolerable refclk to calculate suitable div and margin */
	margin_tmp16 = target_est * HALMAC_INTF_INTGRA_HOSTREF_8821C / HALMAC_INTF_INTGRA_MINREF_8821C - target_est;

	if (margin_tmp16 >= 128) {
		div_set = 0x0003;
		margin_set = 0x000F;
	} else if (margin_tmp16 >= 64) {
		div_set = 0x0003;
		margin_set = margin_tmp16 >> 3;
	} else if (margin_tmp16 >= 32) {
		div_set = 0x0002;
		margin_set = margin_tmp16 >> 2;
	} else if (margin_tmp16 >= 16) {
		div_set = 0x0001;
		margin_set = margin_tmp16 >> 1;
	} else if (margin_tmp16 == 0) {
		div_set = 0x0000;
		margin_set = 0x0001;
	} else {
		div_set = 0x0000;
		margin_set = margin_tmp16;
	}

	/* Set div and count target */
	tmp_u16 = halmac_mdio_read_88xx(adapter, CLKCAL_CTRL_PHYPARA, HAL_INTF_PHY_PCIE_GEN1);
	status = halmac_mdio_write_88xx(adapter, CLKCAL_CTRL_PHYPARA, (tmp_u16 & ~(BIT(7) | BIT(6))) | (div_set << 6), HAL_INTF_PHY_PCIE_GEN1);
	if (status != HALMAC_RET_SUCCESS)
		return status;
	tmp_u16 = halmac_mdio_read_88xx(adapter, CLKCAL_CTRL_PHYPARA, HAL_INTF_PHY_PCIE_GEN1);
	status = halmac_mdio_write_88xx(adapter, CLKCAL_CTRL_PHYPARA, tmp_u16 | BIT(11), HAL_INTF_PHY_PCIE_GEN1);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	count_tar = 5;
	do {
		PLTFM_DELAY_US(22);
		target_final = halmac_mdio_read_88xx(adapter, CLKCAL_TRG_VAL_PHYPARA, HAL_INTF_PHY_PCIE_GEN1);
		count_tar--;
	} while ((count_tar > 0) && (target_final == 0));
	if (target_final == 0) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Final target reads failed.\n");
		return HALMAC_RET_FAIL;
	}

	tmp_u16 = halmac_mdio_read_88xx(adapter, CLKCAL_CTRL_PHYPARA, HAL_INTF_PHY_PCIE_GEN1);
	status = halmac_mdio_write_88xx(adapter, CLKCAL_CTRL_PHYPARA, tmp_u16 & ~(BIT(11)), HAL_INTF_PHY_PCIE_GEN1);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	/* Set calibration target at 0x20[11:0] and margin at 0x20[15:12] */
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]final target = 0x%X, div = 0x%X, margin = 0x%X\n", target_final, div_set, margin_set);
	status = halmac_mdio_write_88xx(adapter, CLKCAL_SET_PHYPARA, (target_final & 0x0FFF) | (margin_set << 12), HAL_INTF_PHY_PCIE_GEN1);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	/* Turn on calibration mechanium at 0x00[9] */
	tmp_u16 = halmac_mdio_read_88xx(adapter, CLKCAL_CTRL_PHYPARA, HAL_INTF_PHY_PCIE_GEN1);
	status = halmac_mdio_write_88xx(adapter, CLKCAL_CTRL_PHYPARA, tmp_u16 | BIT(9), HAL_INTF_PHY_PCIE_GEN1);
	if (status != HALMAC_RET_SUCCESS)
		return status;
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	/* Set L1 backdoor to ori value at 0x719[4:3] */
	if (l1_write_flag == 1)
		status = halmac_dbi_write8_88xx(adapter, PCIE_L1_BACKDOOR, backdoor_org);

	return status;
}

#endif /* HALMAC_8821C_SUPPORT */
