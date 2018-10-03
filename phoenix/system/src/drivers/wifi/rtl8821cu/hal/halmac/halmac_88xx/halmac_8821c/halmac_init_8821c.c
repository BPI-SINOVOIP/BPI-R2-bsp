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

#include "halmac_init_8821c.h"
#include "halmac_8821c_cfg.h"
#include "halmac_pcie_8821c.h"
#include "halmac_sdio_8821c.h"
#include "halmac_usb_8821c.h"
#include "halmac_gpio_8821c.h"
#include "halmac_common_8821c.h"
#include "halmac_cfg_wmac_8821c.h"
#include "../halmac_common_88xx.h"
#include "../halmac_init_88xx.h"

#if HALMAC_8821C_SUPPORT

#define FIFO_PAGE_2_ADDR(page)	(page << HALMAC_TX_PAGE_SIZE_2_POWER_8821C)
#define FIFO_ADDR_2_PAGE(addr)	(addr >> HALMAC_TX_PAGE_SIZE_2_POWER_8821C)

#if HALMAC_PLATFORM_WINDOWS
/*SDIO RQPN Mapping for Windows, extra queue is not implemented in Driver code*/
struct halmac_rqpn HALMAC_RQPN_SDIO_8821C[] = {
	/* { mode, vo_map, vi_map, be_map, bk_map, mg_map, hi_map } */
	{HALMAC_TRX_MODE_NORMAL, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_TRXSHARE, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_WMM, HALMAC_MAP2_HQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ, HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_P2P, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_LOOPBACK, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
};
#else
/*SDIO RQPN Mapping*/
struct halmac_rqpn HALMAC_RQPN_SDIO_8821C[] = {
	/* { mode, vo_map, vi_map, be_map, bk_map, mg_map, hi_map } */
	{HALMAC_TRX_MODE_NORMAL, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ, HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_TRXSHARE, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ, HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_WMM, HALMAC_MAP2_HQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ, HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_P2P, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ, HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_LOOPBACK, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ, HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ, HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
};
#endif

/*PCIE RQPN Mapping*/
struct halmac_rqpn HALMAC_RQPN_PCIE_8821C[] = {
	/* { mode, vo_map, vi_map, be_map, bk_map, mg_map, hi_map } */
	{HALMAC_TRX_MODE_NORMAL, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ, HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_TRXSHARE, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ, HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_WMM, HALMAC_MAP2_HQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ, HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_P2P, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ, HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_LOOPBACK, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ, HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ, HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
};

/*USB 2 Bulkout RQPN Mapping*/
struct halmac_rqpn HALMAC_RQPN_2BULKOUT_8821C[] = {
	/* { mode, vo_map, vi_map, be_map, bk_map, mg_map, hi_map } */
	{HALMAC_TRX_MODE_NORMAL, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_TRXSHARE, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_WMM, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_P2P, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_NQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_LOOPBACK, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_NQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_NQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
};

/*USB 3 Bulkout RQPN Mapping*/
struct halmac_rqpn HALMAC_RQPN_3BULKOUT_8821C[] = {
	/* { mode, vo_map, vi_map, be_map, bk_map, mg_map, hi_map } */
	{HALMAC_TRX_MODE_NORMAL, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_TRXSHARE, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_WMM, HALMAC_MAP2_HQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_P2P, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_LOOPBACK, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
};

/*USB 4 Bulkout RQPN Mapping*/
struct halmac_rqpn HALMAC_RQPN_4BULKOUT_8821C[] = {
	/* { mode, vo_map, vi_map, be_map, bk_map, mg_map, hi_map } */
	{HALMAC_TRX_MODE_NORMAL, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ, HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_TRXSHARE, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ, HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_WMM, HALMAC_MAP2_HQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ, HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_P2P, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ, HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_LOOPBACK, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ, HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ, HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
};

#if HALMAC_PLATFORM_WINDOWS
/*SDIO Page Number*/
struct halmac_pg_num HALMAC_PG_NUM_SDIO_8821C[] = {
	/* { mode, hq_num, nq_num, lq_num, exq_num, gap_num} */
	{HALMAC_TRX_MODE_NORMAL, 16, 16, 16, 0, 1},
	{HALMAC_TRX_MODE_TRXSHARE, 8, 8, 8, 0, 1},
	{HALMAC_TRX_MODE_WMM, 16, 16, 16, 0, 1},
	{HALMAC_TRX_MODE_P2P, 16, 16, 16, 0, 1},
	{HALMAC_TRX_MODE_LOOPBACK, 16, 16, 16, 0, 256},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, 16, 16, 16, 0, 256},
};
#else
/*SDIO Page Number*/
struct halmac_pg_num HALMAC_PG_NUM_SDIO_8821C[] = {
	/* { mode, hq_num, nq_num, lq_num, exq_num, gap_num} */
	{HALMAC_TRX_MODE_NORMAL, 16, 16, 16, 14, 1},
	{HALMAC_TRX_MODE_TRXSHARE, 8, 8, 8, 8, 1},
	{HALMAC_TRX_MODE_WMM, 16, 16, 16, 14, 1},
	{HALMAC_TRX_MODE_P2P, 16, 16, 16, 14, 1},
	{HALMAC_TRX_MODE_LOOPBACK, 16, 16, 16, 14, 256},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, 16, 16, 16, 14, 256},
};
#endif

/*PCIE Page Number*/
struct halmac_pg_num HALMAC_PG_NUM_PCIE_8821C[] = {
	/* { mode, hq_num, nq_num, lq_num, exq_num, gap_num} */
	{HALMAC_TRX_MODE_NORMAL, 16, 16, 16, 14, 1},
	{HALMAC_TRX_MODE_TRXSHARE, 16, 16, 16, 14, 1},
	{HALMAC_TRX_MODE_WMM, 16, 16, 16, 14, 1},
	{HALMAC_TRX_MODE_P2P, 16, 16, 16, 14, 1},
	{HALMAC_TRX_MODE_LOOPBACK, 16, 16, 16, 14, 256},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, 16, 16, 16, 14, 256},
};

/*USB 2 Bulkout Page Number*/
struct halmac_pg_num HALMAC_PG_NUM_2BULKOUT_8821C[] = {
	/* { mode, hq_num, nq_num, lq_num, exq_num, gap_num} */
	{HALMAC_TRX_MODE_NORMAL, 16, 16, 0, 0, 1},
	{HALMAC_TRX_MODE_TRXSHARE, 16, 16, 0, 0, 1},
	{HALMAC_TRX_MODE_WMM, 16, 16, 0, 0, 1},
	{HALMAC_TRX_MODE_P2P, 16, 16, 0, 0, 1},
	{HALMAC_TRX_MODE_LOOPBACK, 16, 16, 0, 0, 256},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, 16, 16, 0, 0, 256},
};

/*USB 3 Bulkout Page Number*/
struct halmac_pg_num HALMAC_PG_NUM_3BULKOUT_8821C[] = {
	/* { mode, hq_num, nq_num, lq_num, exq_num, gap_num} */
	{HALMAC_TRX_MODE_NORMAL, 16, 16, 16, 0, 1},
	{HALMAC_TRX_MODE_TRXSHARE, 16, 16, 16, 0, 1},
	{HALMAC_TRX_MODE_WMM, 16, 16, 16, 0, 1},
	{HALMAC_TRX_MODE_P2P, 16, 16, 16, 0, 1},
	{HALMAC_TRX_MODE_LOOPBACK, 16, 16, 16, 0, 256},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, 16, 16, 16, 0, 256},
};

/*USB 4 Bulkout Page Number*/
struct halmac_pg_num HALMAC_PG_NUM_4BULKOUT_8821C[] = {
	/* { mode, hq_num, nq_num, lq_num, exq_num, gap_num} */
	{HALMAC_TRX_MODE_NORMAL, 16, 16, 16, 14, 1},
	{HALMAC_TRX_MODE_TRXSHARE, 16, 16, 16, 14, 1},
	{HALMAC_TRX_MODE_WMM, 16, 16, 16, 14, 1},
	{HALMAC_TRX_MODE_P2P, 16, 16, 16, 14, 1},
	{HALMAC_TRX_MODE_LOOPBACK, 16, 16, 16, 14, 256},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, 16, 16, 16, 14, 256},
};

static enum halmac_ret_status
halmac_txdma_queue_mapping_8821c(
	IN struct halmac_adapter *adapter,
	IN enum halmac_trx_mode mode
);

static enum halmac_ret_status
halmac_priority_queue_config_8821c(
	IN struct halmac_adapter *adapter,
	IN enum halmac_trx_mode mode
);

static enum halmac_ret_status
halmac_set_trx_fifo_info_8821c(
	IN struct halmac_adapter *adapter,
	IN enum halmac_trx_mode mode
);

enum halmac_ret_status
halmac_mount_api_8821c(
	IN struct halmac_adapter *adapter
)
{
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	adapter->chip_id = HALMAC_CHIP_ID_8821C;
	adapter->hw_cfg_info.efuse_size = HALMAC_EFUSE_SIZE_8821C;
	adapter->hw_cfg_info.eeprom_size = HALMAC_EEPROM_SIZE_8821C;
	adapter->hw_cfg_info.bt_efuse_size = HALMAC_BT_EFUSE_SIZE_8821C;
	adapter->hw_cfg_info.cam_entry_num = HALMAC_SECURITY_CAM_ENTRY_NUM_8821C;
	adapter->hw_cfg_info.txdesc_size = HALMAC_TX_DESC_SIZE_8821C;
	adapter->hw_cfg_info.rxdesc_size = HALMAC_RX_DESC_SIZE_8821C;
	adapter->hw_cfg_info.tx_fifo_size = HALMAC_TX_FIFO_SIZE_8821C;
	adapter->hw_cfg_info.rx_fifo_size = HALMAC_RX_FIFO_SIZE_8821C;
	adapter->hw_cfg_info.page_size = HALMAC_TX_PAGE_SIZE_8821C;
	adapter->hw_cfg_info.tx_align_size = HALMAC_TX_ALIGN_SIZE_8821C;
	adapter->hw_cfg_info.page_size_2_power = HALMAC_TX_PAGE_SIZE_2_POWER_8821C;
	adapter->hw_cfg_info.ac_oqt_size = HALMAC_OQT_ENTRY_AC_8821C;
	adapter->hw_cfg_info.non_ac_oqt_size = HALMAC_OQT_ENTRY_NOAC_8821C;
	adapter->hw_cfg_info.usb_txagg_num = HALMAC_BLK_DESC_NUM_8821C;
	adapter->txff_allocation.rsvd_drv_pg_num = HALMAC_RSVD_DRV_PGNUM_8821C;

	if (adapter->halmac_interface == HALMAC_INTERFACE_SDIO) {
		adapter->sdio_free_space.macid_map_size = HALMAC_MACID_MAX_8821C * 2;
		if (!adapter->sdio_free_space.macid_map) {
			adapter->sdio_free_space.macid_map =
				(u8 *)PLTFM_MALLOC(
				adapter->sdio_free_space.macid_map_size);
			if (!adapter->sdio_free_space.macid_map)
				PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
						"[ERR]mac id map malloc!!\n");
		}
	}

	api->halmac_init_trx_cfg = halmac_init_trx_cfg_8821c;
	api->halmac_init_protocol_cfg = halmac_init_protocol_cfg_8821c;
	api->halmac_init_h2c = halmac_init_h2c_8821c;
	api->halmac_pinmux_get_func = halmac_pinmux_get_func_8821c;
	api->halmac_pinmux_set_func = halmac_pinmux_set_func_8821c;
	api->halmac_pinmux_free_func = halmac_pinmux_free_func_8821c;
	api->halmac_get_hw_value = halmac_get_hw_value_8821c;
	api->halmac_set_hw_value = halmac_set_hw_value_8821c;
	api->halmac_cfg_drv_info = halmac_cfg_drv_info_8821c;
	api->halmac_fill_txdesc_checksum = halmac_fill_txdesc_check_sum_8821c;
	api->halmac_init_low_pwr = halmac_init_low_pwr_8821c;

	if (adapter->halmac_interface == HALMAC_INTERFACE_SDIO) {
		api->halmac_mac_power_switch = halmac_mac_power_switch_8821c_sdio;
		api->halmac_phy_cfg = halmac_phy_cfg_8821c_sdio;
		api->halmac_pcie_switch = halmac_pcie_switch_8821c_sdio;
		api->halmac_interface_integration_tuning = halmac_interface_integration_tuning_8821c_sdio;
		api->halmac_tx_allowed_sdio = halmac_tx_allowed_8821c_sdio;
		api->halmac_get_sdio_tx_addr = halmac_get_sdio_tx_addr_8821c_sdio;
	} else if (adapter->halmac_interface == HALMAC_INTERFACE_USB) {
		api->halmac_mac_power_switch = halmac_mac_power_switch_8821c_usb;
		api->halmac_phy_cfg = halmac_phy_cfg_8821c_usb;
		api->halmac_pcie_switch = halmac_pcie_switch_8821c_usb;
		api->halmac_interface_integration_tuning = halmac_interface_integration_tuning_8821c_usb;
	} else if (adapter->halmac_interface == HALMAC_INTERFACE_PCIE) {
		api->halmac_mac_power_switch = halmac_mac_power_switch_8821c_pcie;
		api->halmac_phy_cfg = halmac_phy_cfg_8821c_pcie;
		api->halmac_pcie_switch = halmac_pcie_switch_8821c_pcie;
		api->halmac_interface_integration_tuning = halmac_interface_integration_tuning_8821c_pcie;
	} else {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Undefined IC\n");
		return HALMAC_RET_CHIP_NOT_SUPPORT;
	}

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_init_trx_cfg_8821c() - config trx dma register
 * @adapter : the adapter of halmac
 * @mode : trx mode selection
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_init_trx_cfg_8821c(
	IN struct halmac_adapter *adapter,
	IN enum halmac_trx_mode mode
)
{
	u8 value8;
	u32 value32;
	u32 h2c_q_addr;
	struct halmac_txff_allocation *txff_info = &adapter->txff_allocation;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	adapter->trx_mode = mode;

	status = halmac_txdma_queue_mapping_8821c(adapter, mode);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]queue mapping\n");
		return status;
	}

	value8 = 0;
	HALMAC_REG_W8(REG_CR, value8);
	value8 = HALMAC_CR_TRX_ENABLE_8821C;
	HALMAC_REG_W8(REG_CR, value8);
	HALMAC_REG_W32(REG_H2CQ_CSR, BIT(31));

	status = halmac_priority_queue_config_8821c(adapter, mode);
	if (txff_info->rx_fifo_expanding_mode !=
	    HALMAC_RX_FIFO_EXPANDING_MODE_DISABLE)
		HALMAC_REG_W8(REG_RX_DRVINFO_SZ,
			      HALMAC_RX_DESC_DUMMY_SIZE_MAX_8821C >> 3);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]priority queue cfg\n");
		return status;
	}

	/* Config H2C packet buffer */
	h2c_q_addr = FIFO_PAGE_2_ADDR(txff_info->rsvd_h2c_queue_pg_bndy);
	value32 = HALMAC_REG_R32(REG_H2C_HEAD);
	value32 = (value32 & 0xFFFC0000) | h2c_q_addr;
	HALMAC_REG_W32(REG_H2C_HEAD, value32);

	value32 = HALMAC_REG_R32(REG_H2C_READ_ADDR);
	value32 = (value32 & 0xFFFC0000) | h2c_q_addr;
	HALMAC_REG_W32(REG_H2C_READ_ADDR, value32);

	value32 = HALMAC_REG_R32(REG_H2C_TAIL);
	value32 &= 0xFFFC0000;
	value32 |= (h2c_q_addr + FIFO_PAGE_2_ADDR(HALMAC_RSVD_H2C_QUEUE_PGNUM_8821C));
	HALMAC_REG_W32(REG_H2C_TAIL, value32);

	value8 = HALMAC_REG_R8(REG_H2C_INFO);
	value8 = (u8)((value8 & 0xFC) | 0x01);
	HALMAC_REG_W8(REG_H2C_INFO, value8);

	value8 = HALMAC_REG_R8(REG_H2C_INFO);
	value8 = (u8)((value8 & 0xFB) | 0x04);
	HALMAC_REG_W8(REG_H2C_INFO, value8);

	value8 = HALMAC_REG_R8(REG_TXDMA_OFFSET_CHK + 1);
	value8 = (u8)((value8 & 0x7f) | 0x80);
	HALMAC_REG_W8(REG_TXDMA_OFFSET_CHK + 1, value8);

	adapter->h2c_buff_size = FIFO_PAGE_2_ADDR(HALMAC_RSVD_H2C_QUEUE_PGNUM_8821C);
	halmac_get_h2c_buff_free_space_88xx(adapter);

	if (adapter->h2c_buff_size != adapter->h2c_buf_free_space) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]h2c buf\n");
		return HALMAC_RET_GET_H2C_SPACE_ERR;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_txdma_queue_mapping_8821c(
	IN struct halmac_adapter *adapter,
	IN enum halmac_trx_mode mode
)
{
	u16 value16;
	struct halmac_rqpn *cur_rqpn_sel = NULL;
	enum halmac_ret_status status;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (adapter->halmac_interface == HALMAC_INTERFACE_SDIO) {
		cur_rqpn_sel = HALMAC_RQPN_SDIO_8821C;
	} else if (adapter->halmac_interface == HALMAC_INTERFACE_PCIE) {
		cur_rqpn_sel = HALMAC_RQPN_PCIE_8821C;
	} else if (adapter->halmac_interface == HALMAC_INTERFACE_USB) {
		if (adapter->halmac_bulkout_num == 2) {
			cur_rqpn_sel = HALMAC_RQPN_2BULKOUT_8821C;
		} else if (adapter->halmac_bulkout_num == 3) {
			cur_rqpn_sel = HALMAC_RQPN_3BULKOUT_8821C;
		} else if (adapter->halmac_bulkout_num == 4) {
			cur_rqpn_sel = HALMAC_RQPN_4BULKOUT_8821C;
		} else {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]invalid intf\n");
			return HALMAC_RET_NOT_SUPPORT;
		}
	}

	status = halmac_rqpn_parser_88xx(adapter, mode, cur_rqpn_sel);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	value16 = 0;
	value16 |= BIT_TXDMA_HIQ_MAP(adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_HI]);
	value16 |= BIT_TXDMA_MGQ_MAP(adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_MG]);
	value16 |= BIT_TXDMA_BKQ_MAP(adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_BK]);
	value16 |= BIT_TXDMA_BEQ_MAP(adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_BE]);
	value16 |= BIT_TXDMA_VIQ_MAP(adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_VI]);
	value16 |= BIT_TXDMA_VOQ_MAP(adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_VO]);
	HALMAC_REG_W16(REG_TXDMA_PQ_MAP, value16);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_priority_queue_config_8821c(
	IN struct halmac_adapter *adapter,
	IN enum halmac_trx_mode mode
)
{
	u8 transfer_mode = 0;
	u8 value8;
	u32 cnt;
	enum halmac_ret_status status;
	struct halmac_pg_num *cur_pg_num = NULL;
	struct halmac_txff_allocation *txff_info = &adapter->txff_allocation;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	status = halmac_set_trx_fifo_info_8821c(adapter, mode);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	if (adapter->halmac_interface == HALMAC_INTERFACE_SDIO) {
		cur_pg_num = HALMAC_PG_NUM_SDIO_8821C;
	} else if (adapter->halmac_interface == HALMAC_INTERFACE_PCIE) {
		cur_pg_num = HALMAC_PG_NUM_PCIE_8821C;
	} else if (adapter->halmac_interface == HALMAC_INTERFACE_USB) {
		if (adapter->halmac_bulkout_num == 2) {
			cur_pg_num = HALMAC_PG_NUM_2BULKOUT_8821C;
		} else if (adapter->halmac_bulkout_num == 3) {
			cur_pg_num = HALMAC_PG_NUM_3BULKOUT_8821C;
		} else if (adapter->halmac_bulkout_num == 4) {
			cur_pg_num = HALMAC_PG_NUM_4BULKOUT_8821C;
		} else {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]invalid intf\n");
			return HALMAC_RET_NOT_SUPPORT;
		}
	}

	status = halmac_pg_num_parser_88xx(adapter, mode, cur_pg_num);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Set FIFO page\n");

	HALMAC_REG_W16(REG_FIFOPAGE_INFO_1, txff_info->high_queue_pg_num);
	HALMAC_REG_W16(REG_FIFOPAGE_INFO_2, txff_info->low_queue_pg_num);
	HALMAC_REG_W16(REG_FIFOPAGE_INFO_3, txff_info->normal_queue_pg_num);
	HALMAC_REG_W16(REG_FIFOPAGE_INFO_4, txff_info->extra_queue_pg_num);
	HALMAC_REG_W16(REG_FIFOPAGE_INFO_5, txff_info->pub_queue_pg_num);

	adapter->sdio_free_space.high_queue_number = txff_info->high_queue_pg_num;
	adapter->sdio_free_space.normal_queue_number = txff_info->normal_queue_pg_num;
	adapter->sdio_free_space.low_queue_number = txff_info->low_queue_pg_num;
	adapter->sdio_free_space.public_queue_number = txff_info->pub_queue_pg_num;
	adapter->sdio_free_space.extra_queue_number = txff_info->extra_queue_pg_num;

	HALMAC_REG_W32(REG_RQPN_CTRL_2, HALMAC_REG_R32(REG_RQPN_CTRL_2) | BIT(31));

	HALMAC_REG_W16(REG_FIFOPAGE_CTRL_2, (u16)(txff_info->rsvd_pg_bndy & BIT_MASK_BCN_HEAD_1_V1));

	HALMAC_REG_W8(REG_FWHW_TXQ_CTRL + 2, HALMAC_REG_R8(REG_FWHW_TXQ_CTRL + 2) | BIT(4));
	/*20170411 Soar*/
	/*SDIO sometimes use two CMD52 to do HALMAC_REG_W16 and may cause a mismatch between HW status and Reg value.*/
	/*A patch is to write high byte first, suggested by Argis*/
	if (adapter->halmac_interface == HALMAC_INTERFACE_SDIO) {
		HALMAC_REG_W8(REG_BCNQ_BDNY_V1 + 1, (u8)((txff_info->rsvd_pg_bndy & BIT_MASK_BCNQ_PGBNDY_V1) >> 8));
		HALMAC_REG_W8(REG_BCNQ_BDNY_V1, (u8)(txff_info->rsvd_pg_bndy & BIT_MASK_BCNQ_PGBNDY_V1));
	} else {
		HALMAC_REG_W16(REG_BCNQ_BDNY_V1, (u16)(txff_info->rsvd_pg_bndy & BIT_MASK_BCNQ_PGBNDY_V1));
	}

	HALMAC_REG_W16(REG_FIFOPAGE_CTRL_2 + 2, (u16)(txff_info->rsvd_pg_bndy & BIT_MASK_BCN_HEAD_1_V1));
	/*20170411 Soar*/
	/*SDIO sometimes use two CMD52 to do HALMAC_REG_W16 and may cause a mismatch between HW status and Reg value.*/
	/*A patch is to write high byte first, suggested by Argis*/
	if (adapter->halmac_interface == HALMAC_INTERFACE_SDIO) {
		HALMAC_REG_W8(REG_BCNQ1_BDNY_V1 + 1, (u8)((txff_info->rsvd_pg_bndy & BIT_MASK_BCNQ_PGBNDY_V1) >> 8));
		HALMAC_REG_W8(REG_BCNQ1_BDNY_V1, (u8)(txff_info->rsvd_pg_bndy & BIT_MASK_BCNQ_PGBNDY_V1));
	} else {
		HALMAC_REG_W16(REG_BCNQ1_BDNY_V1, (u16)(txff_info->rsvd_pg_bndy & BIT_MASK_BCNQ1_PGBNDY_V1));
	}

	HALMAC_REG_W32(REG_RXFF_BNDY, adapter->hw_cfg_info.rx_fifo_size - HALMAC_C2H_PKT_BUF_8821C - 1);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Init LLT table\n");

	if (adapter->halmac_interface == HALMAC_INTERFACE_USB) {
		value8 = (u8)(HALMAC_REG_R8(REG_AUTO_LLT_V1) & ~(BIT_MASK_BLK_DESC_NUM << BIT_SHIFT_BLK_DESC_NUM));
		value8 = (u8)(value8 | (HALMAC_BLK_DESC_NUM_8821C << BIT_SHIFT_BLK_DESC_NUM));
		HALMAC_REG_W8(REG_AUTO_LLT_V1, value8);

		HALMAC_REG_W8(REG_AUTO_LLT_V1 + 3, HALMAC_BLK_DESC_NUM_8821C);
		HALMAC_REG_W8(REG_TXDMA_OFFSET_CHK + 1, HALMAC_REG_R8(REG_TXDMA_OFFSET_CHK + 1) | BIT(1));
	}

	HALMAC_REG_W8(REG_AUTO_LLT_V1, (u8)(HALMAC_REG_R8(REG_AUTO_LLT_V1) | BIT_AUTO_INIT_LLT_V1));
	cnt = 1000;
	while (HALMAC_REG_R8(REG_AUTO_LLT_V1) & BIT_AUTO_INIT_LLT_V1) {
		cnt--;
		if (cnt == 0)
			return HALMAC_RET_INIT_LLT_FAIL;
	}

	if (mode == HALMAC_TRX_MODE_DELAY_LOOPBACK) {
		transfer_mode = HALMAC_TRNSFER_LOOPBACK_DELAY;
		HALMAC_REG_W16(REG_WMAC_LBK_BUF_HD_V1, (u16)txff_info->rsvd_pg_bndy);
	} else if (mode == HALMAC_TRX_MODE_LOOPBACK) {
		transfer_mode = HALMAC_TRNSFER_LOOPBACK_DIRECT;
	} else {
		transfer_mode = HALMAC_TRNSFER_NORMAL;
	}

	adapter->hw_cfg_info.trx_mode = transfer_mode;
	HALMAC_REG_W8(REG_CR + 3, (u8)transfer_mode);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_set_trx_fifo_info_8821c(
	IN struct halmac_adapter *adapter,
	IN enum halmac_trx_mode mode
)
{
	struct halmac_txff_allocation *txff_info = &adapter->txff_allocation;
	enum halmac_rx_fifo_expanding_mode expand_mode;

	adapter->hw_cfg_info.tx_fifo_size = HALMAC_TX_FIFO_SIZE_8821C;
	adapter->hw_cfg_info.rx_fifo_size = HALMAC_RX_FIFO_SIZE_8821C;
	txff_info->tx_fifo_pg_num = FIFO_ADDR_2_PAGE(HALMAC_TX_FIFO_SIZE_8821C);

	expand_mode = txff_info->rx_fifo_expanding_mode;
	if (expand_mode == HALMAC_RX_FIFO_EXPANDING_MODE_1_BLOCK) {
		adapter->hw_cfg_info.tx_fifo_size = HALMAC_TX_FIFO_SIZE_RX_FIFO_EXPANDING_1_BLOCK_8821C;
		if (HALMAC_RX_FIFO_SIZE_RX_FIFO_EXPANDING_1_BLOCK_8821C <=
		    HALMAC_RX_FIFO_SIZE_RX_FIFO_EXPANDING_1_BLOCK_MAX_8821C)
			adapter->hw_cfg_info.rx_fifo_size = HALMAC_RX_FIFO_SIZE_RX_FIFO_EXPANDING_1_BLOCK_8821C;
		else
			adapter->hw_cfg_info.rx_fifo_size = HALMAC_RX_FIFO_SIZE_RX_FIFO_EXPANDING_1_BLOCK_MAX_8821C;
		txff_info->tx_fifo_pg_num = FIFO_ADDR_2_PAGE(HALMAC_TX_FIFO_SIZE_RX_FIFO_EXPANDING_1_BLOCK_8821C);
	}

	if (txff_info->la_mode != HALMAC_LA_MODE_DISABLE) {
		adapter->hw_cfg_info.tx_fifo_size = HALMAC_TX_FIFO_SIZE_8821C;
		adapter->hw_cfg_info.rx_fifo_size = HALMAC_RX_FIFO_SIZE_8821C;
		txff_info->tx_fifo_pg_num = FIFO_ADDR_2_PAGE(HALMAC_TX_FIFO_SIZE_LA_8821C);
	}

	txff_info->rsvd_pg_num = txff_info->rsvd_drv_pg_num +
				HALMAC_RSVD_H2C_EXTRAINFO_PGNUM_8821C +
				HALMAC_RSVD_H2C_STATICINFO_PGNUM_8821C +
				HALMAC_RSVD_H2C_QUEUE_PGNUM_8821C +
				HALMAC_RSVD_CPU_INSTRUCTION_PGNUM_8821C +
				HALMAC_RSVD_FW_TXBUFF_PGNUM_8821C +
				HALMAC_RSVD_CSIBUF_PGNUM_8821C;

	if (mode == HALMAC_TRX_MODE_DELAY_LOOPBACK)
		txff_info->rsvd_pg_num += HALMAC_RSVD_DLLB_PGNUM_8821C;

	if (txff_info->rsvd_pg_num > txff_info->tx_fifo_pg_num)
		return HALMAC_RET_CFG_TXFIFO_PAGE_FAIL;

	txff_info->ac_q_pg_num = txff_info->tx_fifo_pg_num - txff_info->rsvd_pg_num;
	txff_info->rsvd_pg_bndy = txff_info->tx_fifo_pg_num - txff_info->rsvd_pg_num;
	txff_info->rsvd_csibuf_pg_bndy = txff_info->tx_fifo_pg_num - HALMAC_RSVD_CSIBUF_PGNUM_8821C;
	txff_info->rsvd_fw_txbuff_pg_bndy = txff_info->rsvd_csibuf_pg_bndy - HALMAC_RSVD_FW_TXBUFF_PGNUM_8821C;
	txff_info->rsvd_cpu_instr_pg_bndy = txff_info->rsvd_fw_txbuff_pg_bndy - HALMAC_RSVD_CPU_INSTRUCTION_PGNUM_8821C;
	txff_info->rsvd_h2c_queue_pg_bndy = txff_info->rsvd_cpu_instr_pg_bndy - HALMAC_RSVD_H2C_QUEUE_PGNUM_8821C;
	txff_info->rsvd_h2c_static_info_pg_bndy = txff_info->rsvd_h2c_queue_pg_bndy - HALMAC_RSVD_H2C_STATICINFO_PGNUM_8821C;
	txff_info->rsvd_h2c_extra_info_pg_bndy = txff_info->rsvd_h2c_static_info_pg_bndy - HALMAC_RSVD_H2C_EXTRAINFO_PGNUM_8821C;
	txff_info->rsvd_drv_pg_bndy = txff_info->rsvd_h2c_extra_info_pg_bndy - txff_info->rsvd_drv_pg_num;

	if (mode == HALMAC_TRX_MODE_DELAY_LOOPBACK)
		txff_info->rsvd_drv_pg_bndy -= HALMAC_RSVD_DLLB_PGNUM_8821C;

	if (txff_info->rsvd_pg_bndy != txff_info->rsvd_drv_pg_bndy)
		return HALMAC_RET_CFG_TXFIFO_PAGE_FAIL;

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_init_protocol_cfg_8821c() - config protocol register
 * @adapter : the adapter of halmac
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_init_protocol_cfg_8821c(
	IN struct halmac_adapter *adapter
)
{
	u16 pre_txcnt;
	u32 max_agg_num;
	u32 max_rts_agg_num;
	u32 value32;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	HALMAC_REG_W8(REG_AMPDU_MAX_TIME_V1, HALMAC_AMPDU_MAX_TIME_8821C);
	HALMAC_REG_W8(REG_TX_HANG_CTRL, BIT_EN_EOF_V1);

	pre_txcnt = HALMAC_PRE_TXCNT_TIME_TH_8821C | BIT_EN_PRECNT;
	HALMAC_REG_W8(REG_PRECNT_CTRL, (u8)(pre_txcnt & 0xFF));
	HALMAC_REG_W8(REG_PRECNT_CTRL + 1, (u8)(pre_txcnt >> 8));

	max_agg_num = HALMAC_PROT_MAX_AGG_PKT_LIMIT_8821C;
	max_rts_agg_num = HALMAC_PROT_RTS_MAX_AGG_PKT_LIMIT_8821C;

	if (adapter->halmac_interface == HALMAC_INTERFACE_SDIO) {
		max_agg_num = HALMAC_PROT_MAX_AGG_PKT_LIMIT_8821C_SDIO;
		max_rts_agg_num = HALMAC_PROT_RTS_MAX_AGG_PKT_LIMIT_8821C_SDIO;
	}

	value32 = HALMAC_PROT_RTS_LEN_TH_8821C | (HALMAC_PROT_RTS_TX_TIME_TH_8821C << 8) | (max_agg_num << 16) | (max_rts_agg_num << 24);
	HALMAC_REG_W32(REG_PROT_MODE_CTRL, value32);

	HALMAC_REG_W16(REG_BAR_MODE_CTRL + 2, HALMAC_BAR_RETRY_LIMIT_8821C | HALMAC_RA_TRY_RATE_AGG_LIMIT_8821C << 8);

	HALMAC_REG_W8(REG_FAST_EDCA_VOVI_SETTING, HALMAC_FAST_EDCA_VO_TH_8821C);
	HALMAC_REG_W8(REG_FAST_EDCA_VOVI_SETTING + 2, HALMAC_FAST_EDCA_VI_TH_8821C);
	HALMAC_REG_W8(REG_FAST_EDCA_BEBK_SETTING, HALMAC_FAST_EDCA_BE_TH_8821C);
	HALMAC_REG_W8(REG_FAST_EDCA_BEBK_SETTING + 2, HALMAC_FAST_EDCA_BK_TH_8821C);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_init_h2c_8821c() - config h2c packet buffer
 * @adapter : the adapter of halmac
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_init_h2c_8821c(
	IN struct halmac_adapter *adapter
)
{
	u8 value8;
	u32 value32;
	u32 h2c_q_addr;
	struct halmac_txff_allocation *txff_info = &adapter->txff_allocation;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	value8 = 0;
	HALMAC_REG_W8(REG_CR, value8);
	value8 = HALMAC_CR_TRX_ENABLE_8821C;
	HALMAC_REG_W8(REG_CR, value8);

	h2c_q_addr = FIFO_PAGE_2_ADDR(txff_info->rsvd_h2c_queue_pg_bndy);
	value32 = HALMAC_REG_R32(REG_H2C_HEAD);
	value32 = (value32 & 0xFFFC0000) | h2c_q_addr;
	HALMAC_REG_W32(REG_H2C_HEAD, value32);

	value32 = HALMAC_REG_R32(REG_H2C_READ_ADDR);
	value32 = (value32 & 0xFFFC0000) | h2c_q_addr;
	HALMAC_REG_W32(REG_H2C_READ_ADDR, value32);

	value32 = HALMAC_REG_R32(REG_H2C_TAIL);
	value32 &= 0xFFFC0000;
	value32 |= (h2c_q_addr + FIFO_PAGE_2_ADDR(HALMAC_RSVD_H2C_QUEUE_PGNUM_8821C));
	HALMAC_REG_W32(REG_H2C_TAIL, value32);
	value8 = HALMAC_REG_R8(REG_H2C_INFO);
	value8 = (u8)((value8 & 0xFC) | 0x01);
	HALMAC_REG_W8(REG_H2C_INFO, value8);

	value8 = HALMAC_REG_R8(REG_H2C_INFO);
	value8 = (u8)((value8 & 0xFB) | 0x04);
	HALMAC_REG_W8(REG_H2C_INFO, value8);

	value8 = HALMAC_REG_R8(REG_TXDMA_OFFSET_CHK + 1);
	value8 = (u8)((value8 & 0x7f) | 0x80);
	HALMAC_REG_W8(REG_TXDMA_OFFSET_CHK + 1, value8);

	adapter->h2c_buff_size = FIFO_PAGE_2_ADDR(HALMAC_RSVD_H2C_QUEUE_PGNUM_8821C);
	halmac_get_h2c_buff_free_space_88xx(adapter);

	if (adapter->h2c_buff_size != adapter->h2c_buf_free_space) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]h2c buf\n");
		return HALMAC_RET_GET_H2C_SPACE_ERR;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]h2c free space : %d\n",
			adapter->h2c_buf_free_space);

	return HALMAC_RET_SUCCESS;
}

#endif /* HALMAC_8821C_SUPPORT */
