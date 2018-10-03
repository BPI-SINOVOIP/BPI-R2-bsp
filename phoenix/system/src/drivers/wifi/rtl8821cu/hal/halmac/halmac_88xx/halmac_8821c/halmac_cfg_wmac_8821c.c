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

#include "halmac_cfg_wmac_8821c.h"
#include "halmac_8821c_cfg.h"

#if HALMAC_8821C_SUPPORT

/**
 * halmac_cfg_drv_info_88xx() - config driver info
 * @adapter : the adapter of halmac
 * @drv_info : driver information selection
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 * Need to build halmac_cfg_drv_info_8821C, halmac_cfg_drv_info_8822B, halmac_cfg_drv_info_88OO
 * Because 88OO has no need to patch Rx packet counter. Soar 20161110
 */
enum halmac_ret_status
halmac_cfg_drv_info_8821c(
	IN struct halmac_adapter *adapter,
	IN enum halmac_drv_info drv_info
)
{
	u8 drv_info_size = 0;
	u8 phy_status_en = 0;
	u8 sniffer_en = 0;
	u8 plcp_hdr_en = 0;
	u8 value8;
	u32 value32;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	switch (drv_info) {
	case HALMAC_DRV_INFO_NONE:
		drv_info_size = 0;
		phy_status_en = 0;
		sniffer_en = 0;
		plcp_hdr_en = 0;
		break;
	case HALMAC_DRV_INFO_PHY_STATUS:
		drv_info_size = 4;
		phy_status_en = 1;
		sniffer_en = 0;
		plcp_hdr_en = 0;
		break;
	case HALMAC_DRV_INFO_PHY_SNIFFER:
		drv_info_size = 5; /* phy status 4byte, sniffer info 1byte */
		phy_status_en = 1;
		sniffer_en = 1;
		plcp_hdr_en = 0;
		break;
	case HALMAC_DRV_INFO_PHY_PLCP:
		drv_info_size = 6; /* phy status 4byte, plcp header 2byte */
		phy_status_en = 1;
		sniffer_en = 0;
		plcp_hdr_en = 1;
		break;
	default:
		return HALMAC_RET_SW_CASE_NOT_SUPPORT;
	}

	if (adapter->txff_allocation.rx_fifo_expanding_mode != HALMAC_RX_FIFO_EXPANDING_MODE_DISABLE)
		drv_info_size = HALMAC_RX_DESC_DUMMY_SIZE_MAX_8821C >> 3;

	HALMAC_REG_W8(REG_RX_DRVINFO_SZ, drv_info_size);

	value8 = HALMAC_REG_R8(REG_TRXFF_BNDY + 1);
	value8 &= 0xF0;
	/* For rxdesc len = 0 issue. set to correct value after finding root cause */
	value8 |= 0xF;
	HALMAC_REG_W8(REG_TRXFF_BNDY + 1, value8);

	adapter->drv_info_size = drv_info_size;

	value32 = HALMAC_REG_R32(REG_RCR);
	value32 = (value32 & (~BIT_APP_PHYSTS));
	if (phy_status_en == 1)
		value32 = value32 | BIT_APP_PHYSTS;
	HALMAC_REG_W32(REG_RCR, value32);

	value32 = HALMAC_REG_R32(REG_WMAC_OPTION_FUNCTION + 4);
	value32 = (value32 & (~(BIT(8) | BIT(9))));
	if (sniffer_en == 1)
		value32 = value32 | BIT(9);
	if (plcp_hdr_en == 1)
		value32 = value32 | BIT(8);
	HALMAC_REG_W32(REG_WMAC_OPTION_FUNCTION + 4, value32);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_init_low_pwr_8821c() - config WMAC register
 * @adapter
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_init_low_pwr_8821c(
	IN struct halmac_adapter *adapter
)
{
	return HALMAC_RET_SUCCESS;
}

void
halmac_cfg_mac_rx_ignore_8821c(
	IN struct halmac_adapter *adapter,
	IN struct halmac_mac_rx_ignore_cfg *cfg
)
{
	u8 value8;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	value8 = HALMAC_REG_R8(REG_BBPSF_CTRL);

	/*mac header check enable*/
	if (cfg->hdr_chk_en == _TRUE)
		value8 |= BIT_BBPSF_MHCHKEN | BIT_BBPSF_MPDUCHKEN;
	else
		value8 &= ~(BIT_BBPSF_MHCHKEN) & (~(BIT_BBPSF_MPDUCHKEN));

	HALMAC_REG_W8(REG_BBPSF_CTRL, value8);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);
}

#endif /* HALMAC_8821C_SUPPORT */
