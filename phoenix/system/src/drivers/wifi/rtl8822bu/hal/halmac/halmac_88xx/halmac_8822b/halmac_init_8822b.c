/******************************************************************************
 *
 * Copyright(c) 2016 - 2018 Realtek Corporation. All rights reserved.
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

#include "halmac_init_8822b.h"
#include "halmac_8822b_cfg.h"
#include "halmac_pcie_8822b.h"
#include "halmac_sdio_8822b.h"
#include "halmac_usb_8822b.h"
#include "halmac_gpio_8822b.h"
#include "halmac_common_8822b.h"
#include "halmac_cfg_wmac_8822b.h"
#include "../halmac_common_88xx.h"
#include "../halmac_init_88xx.h"

#if HALMAC_8822B_SUPPORT

#define RSVD_PG_DRV_NUM			16
#define RSVD_PG_H2C_EXTRAINFO_NUM	24
#define RSVD_PG_H2C_STATICINFO_NUM	8
#define RSVD_PG_H2CQ_NUM		8
#define RSVD_PG_CPU_INSTRUCTION_NUM	0
#define RSVD_PG_FW_TXBUF_NUM		4
#define RSVD_PG_CSIBUF_NUM		0
#define RSVD_PG_DLLB_NUM		32

#define MAC_TRX_ENABLE	(BIT_HCI_TXDMA_EN | BIT_HCI_RXDMA_EN | BIT_TXDMA_EN | \
			BIT_RXDMA_EN | BIT_PROTOCOL_EN | BIT_SCHEDULE_EN | \
			BIT_MACTXEN | BIT_MACRXEN)

#define BLK_DESC_NUM	0x3

#define WLAN_AMPDU_MAX_TIME		0x70
#define WLAN_RTS_LEN_TH			0xFF
#define WLAN_RTS_TX_TIME_TH		0x08
#define WLAN_MAX_AGG_PKT_LIMIT		0x20
#define WLAN_RTS_MAX_AGG_PKT_LIMIT	0x20
#define WALN_FAST_EDCA_VO_TH		0x06
#define WLAN_FAST_EDCA_VI_TH		0x06
#define WLAN_FAST_EDCA_BE_TH		0x06
#define WLAN_FAST_EDCA_BK_TH		0x06
#define WLAN_BAR_RETRY_LIMIT		0x01
#define WLAN_RA_TRY_RATE_AGG_LIMIT	0x08

#if HALMAC_PLATFORM_WINDOWS
/*SDIO RQPN Mapping for Windows, extra queue is not implemented in Driver code*/
struct halmac_rqpn HALMAC_RQPN_SDIO_8822B[] = {
	/* { mode, vo_map, vi_map, be_map, bk_map, mg_map, hi_map } */
	{HALMAC_TRX_MODE_NORMAL,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_TRXSHARE,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_WMM,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_P2P,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_LOOPBACK,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
};
#else
/*SDIO RQPN Mapping*/
struct halmac_rqpn HALMAC_RQPN_SDIO_8822B[] = {
	/* { mode, vo_map, vi_map, be_map, bk_map, mg_map, hi_map } */
	{HALMAC_TRX_MODE_NORMAL,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_TRXSHARE,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_WMM,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_P2P,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_LOOPBACK,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
};
#endif

/*PCIE RQPN Mapping*/
struct halmac_rqpn HALMAC_RQPN_PCIE_8822B[] = {
	/* { mode, vo_map, vi_map, be_map, bk_map, mg_map, hi_map } */
	{HALMAC_TRX_MODE_NORMAL,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_TRXSHARE,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_WMM,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_P2P,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_LOOPBACK,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
};

/*USB 2 Bulkout RQPN Mapping*/
struct halmac_rqpn HALMAC_RQPN_2BULKOUT_8822B[] = {
	/* { mode, vo_map, vi_map, be_map, bk_map, mg_map, hi_map } */
	{HALMAC_TRX_MODE_NORMAL,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_HQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_TRXSHARE,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_HQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_WMM,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_HQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_P2P,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_NQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_LOOPBACK,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_NQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_NQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
};

/*USB 3 Bulkout RQPN Mapping*/
struct halmac_rqpn HALMAC_RQPN_3BULKOUT_8822B[] = {
	/* { mode, vo_map, vi_map, be_map, bk_map, mg_map, hi_map } */
	{HALMAC_TRX_MODE_NORMAL,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_TRXSHARE,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_WMM,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_P2P,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_LOOPBACK,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
};

/*USB 4 Bulkout RQPN Mapping*/
struct halmac_rqpn HALMAC_RQPN_4BULKOUT_8822B[] = {
	/* { mode, vo_map, vi_map, be_map, bk_map, mg_map, hi_map } */
	{HALMAC_TRX_MODE_NORMAL,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_TRXSHARE,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_WMM,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_P2P,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_LOOPBACK,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
};

#if HALMAC_PLATFORM_WINDOWS
/*SDIO Page Number*/
struct halmac_pg_num HALMAC_PG_NUM_SDIO_8822B[] = {
	/* { mode, hq_num, nq_num, lq_num, exq_num, gap_num} */
	{HALMAC_TRX_MODE_NORMAL, 64, 64, 64, 0, 1},
	{HALMAC_TRX_MODE_TRXSHARE, 32, 32, 32, 0, 1},
	{HALMAC_TRX_MODE_WMM, 64, 64, 64, 0, 1},
	{HALMAC_TRX_MODE_P2P, 64, 64, 64, 0, 1},
	{HALMAC_TRX_MODE_LOOPBACK, 64, 64, 64, 0, 640},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, 64, 64, 64, 0, 640},
};
#else
/*SDIO Page Number*/
struct halmac_pg_num HALMAC_PG_NUM_SDIO_8822B[] = {
	/* { mode, hq_num, nq_num, lq_num, exq_num, gap_num} */
	{HALMAC_TRX_MODE_NORMAL, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_TRXSHARE, 32, 32, 32, 32, 1},
	{HALMAC_TRX_MODE_WMM, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_P2P, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_LOOPBACK, 64, 64, 64, 64, 640},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, 64, 64, 64, 64, 640},
};
#endif

/*PCIE Page Number*/
struct halmac_pg_num HALMAC_PG_NUM_PCIE_8822B[] = {
	/* { mode, hq_num, nq_num, lq_num, exq_num, gap_num} */
	{HALMAC_TRX_MODE_NORMAL, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_TRXSHARE, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_WMM, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_P2P, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_LOOPBACK, 64, 64, 64, 64, 640},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, 64, 64, 64, 64, 640},
};

/*USB 2 Bulkout Page Number*/
struct halmac_pg_num HALMAC_PG_NUM_2BULKOUT_8822B[] = {
	/* { mode, hq_num, nq_num, lq_num, exq_num, gap_num} */
	{HALMAC_TRX_MODE_NORMAL, 64, 64, 0, 0, 1},
	{HALMAC_TRX_MODE_TRXSHARE, 64, 64, 0, 0, 1},
	{HALMAC_TRX_MODE_WMM, 64, 64, 0, 0, 1},
	{HALMAC_TRX_MODE_P2P, 64, 64, 0, 0, 1},
	{HALMAC_TRX_MODE_LOOPBACK, 64, 64, 0, 0, 1024},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, 64, 64, 0, 0, 1024},
};

/*USB 3 Bulkout Page Number*/
struct halmac_pg_num HALMAC_PG_NUM_3BULKOUT_8822B[] = {
	/* { mode, hq_num, nq_num, lq_num, exq_num, gap_num} */
	{HALMAC_TRX_MODE_NORMAL, 64, 64, 64, 0, 1},
	{HALMAC_TRX_MODE_TRXSHARE, 64, 64, 64, 0, 1},
	{HALMAC_TRX_MODE_WMM, 64, 64, 64, 0, 1},
	{HALMAC_TRX_MODE_P2P, 64, 64, 64, 0, 1},
	{HALMAC_TRX_MODE_LOOPBACK, 64, 64, 64, 0, 1024},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, 64, 64, 64, 0, 1024},
};

/*USB 4 Bulkout Page Number*/
struct halmac_pg_num HALMAC_PG_NUM_4BULKOUT_8822B[] = {
	/* { mode, hq_num, nq_num, lq_num, exq_num, gap_num} */
	{HALMAC_TRX_MODE_NORMAL, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_TRXSHARE, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_WMM, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_P2P, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_LOOPBACK, 64, 64, 64, 64, 640},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, 64, 64, 64, 64, 640},
};

static enum halmac_ret_status
txdma_queue_mapping_8822b(struct halmac_adapter *adapter,
			  enum halmac_trx_mode mode);

static enum halmac_ret_status
priority_queue_cfg_8822b(struct halmac_adapter *adapter,
			 enum halmac_trx_mode mode);

static enum halmac_ret_status
set_trx_fifo_info_8822b(struct halmac_adapter *adapter,
			enum halmac_trx_mode mode);
enum halmac_ret_status
mount_api_8822b(struct halmac_adapter *adapter)
{
	struct halmac_api *api = (struct halmac_api *)adapter->halmac_api;

	adapter->chip_id = HALMAC_CHIP_ID_8822B;
	adapter->hw_cfg_info.efuse_size = EFUSE_SIZE_8822B;
	adapter->hw_cfg_info.eeprom_size = EEPROM_SIZE_8822B;
	adapter->hw_cfg_info.bt_efuse_size = BT_EFUSE_SIZE_8822B;
	adapter->hw_cfg_info.cam_entry_num = SEC_CAM_NUM_8822B;
	adapter->hw_cfg_info.tx_fifo_size = TX_FIFO_SIZE_8822B;
	adapter->hw_cfg_info.rx_fifo_size = RX_FIFO_SIZE_8822B;
	adapter->hw_cfg_info.ac_oqt_size = OQT_ENTRY_AC_8822B;
	adapter->hw_cfg_info.non_ac_oqt_size = OQT_ENTRY_NOAC_8822B;
	adapter->hw_cfg_info.usb_txagg_num = BLK_DESC_NUM;
	adapter->txff_alloc.rsvd_drv_pg_num = RSVD_PG_DRV_NUM;

	api->halmac_init_trx_cfg = init_trx_cfg_8822b;
	api->halmac_init_protocol_cfg = init_protocol_cfg_8822b;
	api->halmac_init_h2c = init_h2c_8822b;
	api->halmac_pinmux_get_func = pinmux_get_func_8822b;
	api->halmac_pinmux_set_func = pinmux_set_func_8822b;
	api->halmac_pinmux_free_func = pinmux_free_func_8822b;
	api->halmac_get_hw_value = get_hw_value_8822b;
	api->halmac_set_hw_value = set_hw_value_8822b;
	api->halmac_cfg_drv_info = cfg_drv_info_8822b;
	api->halmac_fill_txdesc_checksum = fill_txdesc_check_sum_8822b;
	api->halmac_init_low_pwr = init_low_pwr_8822b;

	if (adapter->intf == HALMAC_INTERFACE_SDIO) {
		api->halmac_mac_power_switch = mac_pwr_switch_sdio_8822b;
		api->halmac_phy_cfg = phy_cfg_sdio_8822b;
		api->halmac_pcie_switch = pcie_switch_sdio_8822b;
		api->halmac_interface_integration_tuning = intf_tun_sdio_8822b;
		api->halmac_tx_allowed_sdio = tx_allowed_sdio_8822b;
		api->halmac_get_sdio_tx_addr = get_sdio_tx_addr_8822b;
		api->halmac_reg_read_8 = reg_r8_sdio_8822b;
		api->halmac_reg_write_8 = reg_w8_sdio_8822b;
		api->halmac_reg_read_16 = reg_r16_sdio_8822b;
		api->halmac_reg_write_16 = reg_w16_sdio_8822b;
		api->halmac_reg_read_32 = reg_r32_sdio_8822b;
		api->halmac_reg_write_32 = reg_w32_sdio_8822b;

		adapter->sdio_fs.macid_map_size = MACID_MAX_8822B * 2;
		if (!adapter->sdio_fs.macid_map) {
			adapter->sdio_fs.macid_map =
			(u8 *)PLTFM_MALLOC(adapter->sdio_fs.macid_map_size);
			if (!adapter->sdio_fs.macid_map)
				PLTFM_MSG_ERR("[ERR]allocate macid_map!!\n");
		}
	} else if (adapter->intf == HALMAC_INTERFACE_USB) {
		api->halmac_mac_power_switch = mac_pwr_switch_usb_8822b;
		api->halmac_phy_cfg = phy_cfg_usb_8822b;
		api->halmac_pcie_switch = pcie_switch_usb_8822b;
		api->halmac_interface_integration_tuning = intf_tun_usb_8822b;
	} else if (adapter->intf == HALMAC_INTERFACE_PCIE) {
		api->halmac_mac_power_switch = mac_pwr_switch_pcie_8822b;
		api->halmac_phy_cfg = phy_cfg_pcie_8822b;
		api->halmac_pcie_switch = pcie_switch_8822b;
		api->halmac_interface_integration_tuning = intf_tun_pcie_8822b;
	} else {
		PLTFM_MSG_ERR("[ERR]Undefined IC\n");
		return HALMAC_RET_CHIP_NOT_SUPPORT;
	}

	return HALMAC_RET_SUCCESS;
}

/**
 * init_trx_cfg_8822b() - config trx dma register
 * @adapter : the adapter of halmac
 * @mode : trx mode selection
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
init_trx_cfg_8822b(struct halmac_adapter *adapter, enum halmac_trx_mode mode)
{
	u8 value8;
	struct halmac_api *api = (struct halmac_api *)adapter->halmac_api;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	adapter->trx_mode = mode;

	PLTFM_MSG_TRACE("[TRACE]%s ===>\n", __func__);

	status = txdma_queue_mapping_8822b(adapter, mode);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_ERR("[ERR]queue mapping\n");
		return status;
	}

	value8 = 0;
	HALMAC_REG_W8(REG_CR, value8);
	value8 = MAC_TRX_ENABLE;
	HALMAC_REG_W8(REG_CR, value8);
	HALMAC_REG_W32(REG_H2CQ_CSR, BIT(31));

	status = priority_queue_cfg_8822b(adapter, mode);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_ERR("[ERR]halmac_txdma_queue_mapping fail!\n");
		return status;
	}

	if (adapter->txff_alloc.rx_fifo_exp_mode !=
	    HALMAC_RX_FIFO_EXPANDING_MODE_DISABLE)
		HALMAC_REG_W8(REG_RX_DRVINFO_SZ, RX_DESC_DUMMY_SIZE_8822B >> 3);

	status = init_h2c_8822b(adapter);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_ERR("[ERR]init h2cq!\n");
		return status;
	}

	if (adapter->intf == HALMAC_INTERFACE_USB)
		HALMAC_REG_W8_SET(REG_TXDMA_PQ_MAP, BIT(0));

	PLTFM_MSG_TRACE("[TRACE]%s <===\n", __func__);
	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
txdma_queue_mapping_8822b(struct halmac_adapter *adapter,
			  enum halmac_trx_mode mode)
{
	u16 value16;
	struct halmac_rqpn *cur_rqpn_sel = NULL;
	enum halmac_ret_status status;
	struct halmac_api *api = (struct halmac_api *)adapter->halmac_api;

	if (adapter->intf == HALMAC_INTERFACE_SDIO) {
		cur_rqpn_sel = HALMAC_RQPN_SDIO_8822B;
	} else if (adapter->intf == HALMAC_INTERFACE_PCIE) {
		cur_rqpn_sel = HALMAC_RQPN_PCIE_8822B;
	} else if (adapter->intf == HALMAC_INTERFACE_USB) {
		if (adapter->bulkout_num == 2) {
			cur_rqpn_sel = HALMAC_RQPN_2BULKOUT_8822B;
		} else if (adapter->bulkout_num == 3) {
			cur_rqpn_sel = HALMAC_RQPN_3BULKOUT_8822B;
		} else if (adapter->bulkout_num == 4) {
			cur_rqpn_sel = HALMAC_RQPN_4BULKOUT_8822B;
		} else {
			PLTFM_MSG_ERR("[ERR]invalid intf\n");
			return HALMAC_RET_NOT_SUPPORT;
		}
	} else {
		return HALMAC_RET_NOT_SUPPORT;
	}

	status = rqpn_parser_88xx(adapter, mode, cur_rqpn_sel);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	value16 = 0;
	value16 |= BIT_TXDMA_HIQ_MAP(adapter->pq_map[HALMAC_PQ_MAP_HI]);
	value16 |= BIT_TXDMA_MGQ_MAP(adapter->pq_map[HALMAC_PQ_MAP_MG]);
	value16 |= BIT_TXDMA_BKQ_MAP(adapter->pq_map[HALMAC_PQ_MAP_BK]);
	value16 |= BIT_TXDMA_BEQ_MAP(adapter->pq_map[HALMAC_PQ_MAP_BE]);
	value16 |= BIT_TXDMA_VIQ_MAP(adapter->pq_map[HALMAC_PQ_MAP_VI]);
	value16 |= BIT_TXDMA_VOQ_MAP(adapter->pq_map[HALMAC_PQ_MAP_VO]);
	HALMAC_REG_W16(REG_TXDMA_PQ_MAP, value16);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
priority_queue_cfg_8822b(struct halmac_adapter *adapter,
			 enum halmac_trx_mode mode)
{
	u8 transfer_mode = 0;
	u8 value8;
	u32 cnt;
	struct halmac_txff_allocation *txff_info = &adapter->txff_alloc;
	enum halmac_ret_status status;
	struct halmac_pg_num *cur_pg_num = NULL;
	struct halmac_api *api = (struct halmac_api *)adapter->halmac_api;

	status = set_trx_fifo_info_8822b(adapter, mode);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_ERR("[ERR]set trx fifo!!\n");
		return status;
	}
	if (adapter->intf == HALMAC_INTERFACE_SDIO) {
		cur_pg_num = HALMAC_PG_NUM_SDIO_8822B;
	} else if (adapter->intf == HALMAC_INTERFACE_PCIE) {
		cur_pg_num = HALMAC_PG_NUM_PCIE_8822B;
	} else if (adapter->intf == HALMAC_INTERFACE_USB) {
		if (adapter->bulkout_num == 2) {
			cur_pg_num = HALMAC_PG_NUM_2BULKOUT_8822B;
		} else if (adapter->bulkout_num == 3) {
			cur_pg_num = HALMAC_PG_NUM_3BULKOUT_8822B;
		} else if (adapter->bulkout_num == 4) {
			cur_pg_num = HALMAC_PG_NUM_4BULKOUT_8822B;
		} else {
			PLTFM_MSG_ERR("[ERR]interface not support\n");
			return HALMAC_RET_NOT_SUPPORT;
		}
	} else {
		return HALMAC_RET_NOT_SUPPORT;
	}

	status = pg_num_parser_88xx(adapter, mode, cur_pg_num);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	HALMAC_REG_W16(REG_FIFOPAGE_INFO_1, txff_info->high_queue_pg_num);
	HALMAC_REG_W16(REG_FIFOPAGE_INFO_2, txff_info->low_queue_pg_num);
	HALMAC_REG_W16(REG_FIFOPAGE_INFO_3, txff_info->normal_queue_pg_num);
	HALMAC_REG_W16(REG_FIFOPAGE_INFO_4, txff_info->extra_queue_pg_num);
	HALMAC_REG_W16(REG_FIFOPAGE_INFO_5, txff_info->pub_queue_pg_num);
	HALMAC_REG_W32_SET(REG_RQPN_CTRL_2, BIT(31));

	adapter->sdio_fs.hiq_pg_num = txff_info->high_queue_pg_num;
	adapter->sdio_fs.miq_pg_num = txff_info->normal_queue_pg_num;
	adapter->sdio_fs.lowq_pg_num = txff_info->low_queue_pg_num;
	adapter->sdio_fs.pubq_pg_num = txff_info->pub_queue_pg_num;
	adapter->sdio_fs.exq_pg_num = txff_info->extra_queue_pg_num;

	HALMAC_REG_W16(REG_FIFOPAGE_CTRL_2, txff_info->rsvd_boundary);
	HALMAC_REG_W8_SET(REG_FWHW_TXQ_CTRL + 2, BIT(4));

	/*20170411 Soar*/
	/* SDIO sometimes use two CMD52 to do HALMAC_REG_W16 */
	/* and may cause a mismatch between HW status and Reg value. */
	/* A patch is to write high byte first, suggested by Argis */
	if (adapter->intf == HALMAC_INTERFACE_SDIO) {
		value8 = (u8)(txff_info->rsvd_boundary >> 8 & 0xFF);
		HALMAC_REG_W8(REG_BCNQ_BDNY_V1 + 1, value8);
		value8 = (u8)(txff_info->rsvd_boundary & 0xFF);
		HALMAC_REG_W8(REG_BCNQ_BDNY_V1, value8);
	} else {
		HALMAC_REG_W16(REG_BCNQ_BDNY_V1, txff_info->rsvd_boundary);
	}

	HALMAC_REG_W16(REG_FIFOPAGE_CTRL_2 + 2, txff_info->rsvd_boundary);

	/*20170411 Soar*/
	/* SDIO sometimes use two CMD52 to do HALMAC_REG_W16 */
	/* and may cause a mismatch between HW status and Reg value. */
	/* A patch is to write high byte first, suggested by Argis */
	if (adapter->intf == HALMAC_INTERFACE_SDIO) {
		value8 = (u8)(txff_info->rsvd_boundary >> 8 & 0xFF);
		HALMAC_REG_W8(REG_BCNQ1_BDNY_V1 + 1, value8);
		value8 = (u8)(txff_info->rsvd_boundary & 0xFF);
		HALMAC_REG_W8(REG_BCNQ1_BDNY_V1, value8);
	} else {
		HALMAC_REG_W16(REG_BCNQ1_BDNY_V1, txff_info->rsvd_boundary);
	}

	HALMAC_REG_W32(REG_RXFF_BNDY,
		       adapter->hw_cfg_info.rx_fifo_size -
		       C2H_PKT_BUF_88XX - 1);

	if (adapter->intf == HALMAC_INTERFACE_USB) {
		value8 = HALMAC_REG_R8(REG_AUTO_LLT_V1);
		value8 &= ~(BIT_MASK_BLK_DESC_NUM << BIT_SHIFT_BLK_DESC_NUM);
		value8 |= (BLK_DESC_NUM << BIT_SHIFT_BLK_DESC_NUM);
		HALMAC_REG_W8(REG_AUTO_LLT_V1, value8);

		HALMAC_REG_W8(REG_AUTO_LLT_V1 + 3, BLK_DESC_NUM);
		HALMAC_REG_W8_SET(REG_TXDMA_OFFSET_CHK + 1, BIT(1));
	}

	HALMAC_REG_W8_SET(REG_AUTO_LLT_V1, BIT_AUTO_INIT_LLT_V1);
	cnt = 1000;
	while (HALMAC_REG_R8(REG_AUTO_LLT_V1) & BIT_AUTO_INIT_LLT_V1) {
		cnt--;
		if (cnt == 0)
			return HALMAC_RET_INIT_LLT_FAIL;
	}

	if (mode == HALMAC_TRX_MODE_DELAY_LOOPBACK) {
		transfer_mode = HALMAC_TRNSFER_LOOPBACK_DELAY;
		HALMAC_REG_W16(REG_WMAC_LBK_BUF_HD_V1,
			       adapter->txff_alloc.rsvd_boundary);
	} else if (mode == HALMAC_TRX_MODE_LOOPBACK) {
		transfer_mode = HALMAC_TRNSFER_LOOPBACK_DIRECT;
	} else {
		transfer_mode = HALMAC_TRNSFER_NORMAL;
	}

	adapter->hw_cfg_info.trx_mode = transfer_mode;
	HALMAC_REG_W8(REG_CR + 3, transfer_mode);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
set_trx_fifo_info_8822b(struct halmac_adapter *adapter,
			enum halmac_trx_mode mode)
{
	u16 cur_pg_addr;
	u32 txff_size = TX_FIFO_SIZE_8822B;
	u32 rxff_size = RX_FIFO_SIZE_8822B;
	struct halmac_txff_allocation *info = &adapter->txff_alloc;

	if (info->rx_fifo_exp_mode == HALMAC_RX_FIFO_EXPANDING_MODE_1_BLOCK) {
		txff_size = TX_FIFO_SIZE_RX_EXPAND_1BLK_8822B;
		rxff_size = RX_FIFO_SIZE_RX_EXPAND_1BLK_8822B;
	}

	if (info->la_mode != HALMAC_LA_MODE_DISABLE) {
		txff_size = TX_FIFO_SIZE_LA_8822B;
		rxff_size = RX_FIFO_SIZE_8822B;
	}

	adapter->hw_cfg_info.tx_fifo_size = txff_size;
	adapter->hw_cfg_info.rx_fifo_size = rxff_size;
	info->tx_fifo_pg_num = (u16)(txff_size >> TX_PAGE_SIZE_SHIFT_88XX);

	info->rsvd_pg_num = info->rsvd_drv_pg_num +
					RSVD_PG_H2C_EXTRAINFO_NUM +
					RSVD_PG_H2C_STATICINFO_NUM +
					RSVD_PG_H2CQ_NUM +
					RSVD_PG_CPU_INSTRUCTION_NUM +
					RSVD_PG_FW_TXBUF_NUM +
					RSVD_PG_CSIBUF_NUM;

	if (mode == HALMAC_TRX_MODE_DELAY_LOOPBACK)
		info->rsvd_pg_num += RSVD_PG_DLLB_NUM;

	if (info->rsvd_pg_num > info->tx_fifo_pg_num)
		return HALMAC_RET_CFG_TXFIFO_PAGE_FAIL;

	info->acq_pg_num = info->tx_fifo_pg_num - info->rsvd_pg_num;
	info->rsvd_boundary = info->tx_fifo_pg_num - info->rsvd_pg_num;

	cur_pg_addr = info->tx_fifo_pg_num;
	cur_pg_addr -= RSVD_PG_CSIBUF_NUM;
	info->rsvd_csibuf_addr = cur_pg_addr;
	cur_pg_addr -= RSVD_PG_FW_TXBUF_NUM;
	info->rsvd_fw_txbuf_addr = cur_pg_addr;
	cur_pg_addr -= RSVD_PG_CPU_INSTRUCTION_NUM;
	info->rsvd_cpu_instr_addr = cur_pg_addr;
	cur_pg_addr -= RSVD_PG_H2CQ_NUM;
	info->rsvd_h2cq_addr = cur_pg_addr;
	cur_pg_addr -= RSVD_PG_H2C_STATICINFO_NUM;
	info->rsvd_h2c_sta_info_addr = cur_pg_addr;
	cur_pg_addr -= RSVD_PG_H2C_EXTRAINFO_NUM;
	info->rsvd_h2c_info_addr = cur_pg_addr;
	cur_pg_addr -= info->rsvd_drv_pg_num;
	info->rsvd_drv_addr = cur_pg_addr;

	if (mode == HALMAC_TRX_MODE_DELAY_LOOPBACK)
		info->rsvd_drv_addr -= RSVD_PG_DLLB_NUM;

	if (info->rsvd_boundary != info->rsvd_drv_addr)
		return HALMAC_RET_CFG_TXFIFO_PAGE_FAIL;

	return HALMAC_RET_SUCCESS;
}

/**
 * init_protocol_cfg_8822b() - config protocol register
 * @adapter : the adapter of halmac
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
init_protocol_cfg_8822b(struct halmac_adapter *adapter)
{
	u32 value32;
	struct halmac_api *api = (struct halmac_api *)adapter->halmac_api;

	PLTFM_MSG_TRACE("[TRACE]%s ===>\n", __func__);

	HALMAC_REG_W8_CLR(REG_SW_AMPDU_BURST_MODE_CTRL, BIT(6));

	HALMAC_REG_W8(REG_AMPDU_MAX_TIME_V1, WLAN_AMPDU_MAX_TIME);
	HALMAC_REG_W8(REG_TX_HANG_CTRL, BIT_EN_EOF_V1);

	value32 = WLAN_RTS_LEN_TH | (WLAN_RTS_TX_TIME_TH << 8) |
					(WLAN_MAX_AGG_PKT_LIMIT << 16) |
					(WLAN_RTS_MAX_AGG_PKT_LIMIT << 24);
	HALMAC_REG_W32(REG_PROT_MODE_CTRL, value32);

	HALMAC_REG_W16(REG_BAR_MODE_CTRL + 2,
		       WLAN_BAR_RETRY_LIMIT | WLAN_RA_TRY_RATE_AGG_LIMIT << 8);

	HALMAC_REG_W8(REG_FAST_EDCA_VOVI_SETTING, WALN_FAST_EDCA_VO_TH);
	HALMAC_REG_W8(REG_FAST_EDCA_VOVI_SETTING + 2, WLAN_FAST_EDCA_VI_TH);
	HALMAC_REG_W8(REG_FAST_EDCA_BEBK_SETTING, WLAN_FAST_EDCA_BE_TH);
	HALMAC_REG_W8(REG_FAST_EDCA_BEBK_SETTING + 2, WLAN_FAST_EDCA_BK_TH);

	PLTFM_MSG_TRACE("[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * init_h2c_8822b() - config h2c packet buffer
 * @adapter : the adapter of halmac
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
init_h2c_8822b(struct halmac_adapter *adapter)
{
	u8 value8;
	u32 value32;
	u32 h2cq_addr;
	u32 h2cq_size;
	struct halmac_txff_allocation *txff_info = &adapter->txff_alloc;
	struct halmac_api *api = (struct halmac_api *)adapter->halmac_api;

	h2cq_addr = txff_info->rsvd_h2cq_addr << TX_PAGE_SIZE_SHIFT_88XX;
	h2cq_size = RSVD_PG_H2CQ_NUM << TX_PAGE_SIZE_SHIFT_88XX;

	value32 = HALMAC_REG_R32(REG_H2C_HEAD);
	value32 = (value32 & 0xFFFC0000) | h2cq_addr;
	HALMAC_REG_W32(REG_H2C_HEAD, value32);

	value32 = HALMAC_REG_R32(REG_H2C_READ_ADDR);
	value32 = (value32 & 0xFFFC0000) | h2cq_addr;
	HALMAC_REG_W32(REG_H2C_READ_ADDR, value32);

	value32 = HALMAC_REG_R32(REG_H2C_TAIL);
	value32 &= 0xFFFC0000;
	value32 |= (h2cq_addr + h2cq_size);
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

	adapter->h2c_info.buf_size = h2cq_size;
	get_h2c_buf_free_space_88xx(adapter);

	if (adapter->h2c_info.buf_size != adapter->h2c_info.buf_fs) {
		PLTFM_MSG_ERR("[ERR]get h2c free space error!\n");
		return HALMAC_RET_GET_H2C_SPACE_ERR;
	}

	PLTFM_MSG_TRACE("[TRACE]h2c fs : %d\n", adapter->h2c_info.buf_fs);

	return HALMAC_RET_SUCCESS;
}

#endif /* HALMAC_8822B_SUPPORT */
