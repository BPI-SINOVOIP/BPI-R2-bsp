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

#include "halmac_common_88xx.h"
#include "halmac_88xx_cfg.h"
#include "halmac_init_88xx.h"
#include "halmac_cfg_wmac_88xx.h"
#include "halmac_efuse_88xx.h"
#include "halmac_bb_rf_88xx.h"
#include "halmac_usb_88xx.h"
#include "halmac_sdio_88xx.h"
#include "halmac_pcie_88xx.h"
#include "halmac_mimo_88xx.h"

#if HALMAC_88XX_SUPPORT

static enum halmac_ret_status
halmac_parse_c2h_packet_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
);

static enum halmac_ret_status
halmac_get_c2h_dbg_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
);

static enum halmac_ret_status
halmac_get_h2c_ack_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
);

static enum halmac_ret_status
halmac_get_scan_rpt_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
);

static enum halmac_ret_status
halmac_get_psd_data_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
);

static enum halmac_ret_status
halmac_get_h2c_ack_cfg_para_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
);

static enum halmac_ret_status
halmac_get_h2c_ack_update_pkt_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
);

static enum halmac_ret_status
halmac_get_h2c_ack_update_datapkt_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
);

static enum halmac_ret_status
halmac_get_h2c_ack_run_datapkt_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
);

static enum halmac_ret_status
halmac_get_h2c_ack_ch_switch_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
);

static enum halmac_ret_status
halmac_malloc_cfg_param_buf_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 full_fifo
);

static enum halmac_cfg_para_cmd_construct_state
halmac_cfg_param_cmd_state_88xx(
	IN struct halmac_adapter *adapter
);

static enum halmac_ret_status
halmac_proc_cfg_param_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_phy_parameter_info *param,
	IN u8 full_fifo
);

static enum halmac_ret_status
halmac_send_cfg_param_h2c_88xx(
	IN struct halmac_adapter *adapter
);

static enum halmac_ret_status
halmac_transform_cfg_para_state_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_cfg_para_cmd_construct_state dest_state
);

static enum halmac_ret_status
halmac_add_param_buf_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_phy_parameter_info *param,
	IN u8 *buf,
	OUT u8 *pEnd_cmd
);

static enum halmac_ret_status
halmac_gen_cfg_para_h2c_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buff
);

static enum halmac_ret_status
halmac_send_h2c_update_packet_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_packet_id pkt_id,
	IN u8 *pkt,
	IN u32 size
);

static enum halmac_ret_status
halmac_send_bt_coex_cmd_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size,
	IN u8 ack
);

static enum halmac_ret_status
halmac_func_send_original_h2c_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *org_h2c,
	IN u16 *seq,
	IN u8 ack
);

static enum halmac_ret_status
halmac_buffer_read_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u32 size,
	IN enum hal_fifo_sel sel,
	OUT u8 *data
);

static enum halmac_scan_cmd_construct_state
halmac_scan_cmd_state_88xx(
	IN struct halmac_adapter *adapter
);

static enum halmac_ret_status
halmac_transform_scan_state_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_scan_cmd_construct_state dest_state
);

static enum halmac_ret_status
halmac_func_ctrl_ch_switch_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_ch_switch_option *opt
);

static enum halmac_ret_status
halmac_func_send_general_info_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_general_info *info
);

static enum halmac_ret_status
halmac_func_send_phydm_info_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_general_info *info
);

static enum halmac_ret_status
halmac_func_p2pps_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_p2pps *info
);

static enum halmac_ret_status
halmac_cfg_param_status_88xx(
	IN struct halmac_adapter *adapter,
	OUT enum halmac_cmd_process_status *proc_status
);

static enum halmac_ret_status
halmac_ch_switch_status_88xx(
	IN struct halmac_adapter *adapter,
	OUT enum halmac_cmd_process_status *proc_status
);

static enum halmac_ret_status
halmac_update_packet_status_88xx(
	IN struct halmac_adapter *adapter,
	OUT enum halmac_cmd_process_status *proc_status
);

static enum halmac_ret_status
halmac_pwr_sub_seq_parer_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 cut,
	IN u8 fab,
	IN u8 intf,
	IN struct halmac_wlan_pwr_cfg *cmd
);

static void
halmac_power_state_88xx(
	IN struct halmac_adapter *adapter,
	OUT enum halmac_mac_power *state
);

static enum halmac_ret_status
halmac_pwr_cmd_polling_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_wlan_pwr_cfg *cmd
);

static void
halmac_get_gueue_mapping(
	IN struct halmac_adapter *adapter,
	IN struct halmac_rqpn_map *mapping
);

static void
halmac_dump_reg_sdio_88xx(
	IN struct halmac_adapter *adapter
);

static void
halmac_dump_reg_88xx(
	IN struct halmac_adapter *adapter
);

/**
 * halmac_ofld_func_cfg_88xx() - config offload function
 * @adapter : the adapter of halmac
 * @info : offload function information
 * Author : Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_ofld_func_cfg_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_ofld_func_info *info
)
{
	if (adapter->halmac_interface == HALMAC_INTERFACE_SDIO &&
	    info->rsvd_pg_drv_buf_max_sz > HALMAC_SDIO_TX_PKT_MAX_SIZE_88XX)
		return HALMAC_RET_FAIL;

	adapter->ofld_func_info.halmac_malloc_max_sz =
		info->halmac_malloc_max_sz;
	adapter->ofld_func_info.rsvd_pg_drv_buf_max_sz =
		info->rsvd_pg_drv_buf_max_sz;

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_dl_drv_rsvd_page_88xx() - download packet to rsvd page
 * @adapter : the adapter of halmac
 * @pg_offset : page offset of driver's rsvd page
 * @halmac_buf : data to be downloaded, tx_desc is not included
 * @halmac_size : data size to be downloaded
 * Author : KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_dl_drv_rsvd_page_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 pg_offset,
	IN u8 *buf,
	IN u32 size
)
{
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	enum halmac_ret_status status;
	u32 pg_size;
	u32 pg_num = 0;
	u16 pg_addr = 0;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	pg_size = adapter->hw_cfg_info.page_size;
	pg_num = size / pg_size + ((size & (pg_size - 1)) ? 1 : 0);
	if (pg_offset + pg_num > adapter->txff_allocation.rsvd_drv_pg_num) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR] pkt overflow!!\n");
		return HALMAC_RET_DRV_DL_ERR;
	}

	pg_addr = adapter->txff_allocation.rsvd_drv_pg_bndy + pg_offset;

	status = halmac_download_rsvd_page_88xx(adapter, pg_addr, buf, size);

	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]dl rsvd page fail!!\n");
		return status;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

enum halmac_ret_status
halmac_download_rsvd_page_88xx(
	IN struct halmac_adapter *adapter,
	IN u16 pg_addr,
	IN u8 *buf,
	IN u32 size
)
{
	u8 restore[2];
	u8 value8;
	u16 rsvd_pg_head;
	u32 cnt;
	enum halmac_rsvd_pg_state *state = &adapter->halmac_state.rsvd_pg_state;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	if (size == 0) {
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]pkt size = 0\n");
		return HALMAC_RET_ZERO_LEN_RSVD_PACKET;
	}

	if (*state == HALMAC_RSVD_PG_STATE_BUSY)
		return HALMAC_RET_BUSY_STATE;

	*state = HALMAC_RSVD_PG_STATE_BUSY;

	HALMAC_REG_W16(REG_FIFOPAGE_CTRL_2,
		       (u16)(pg_addr & BIT_MASK_BCN_HEAD_1_V1) | BIT(15));

	value8 = HALMAC_REG_R8(REG_CR + 1);
	restore[0] = value8;
	value8 = (u8)(value8 | BIT(0));
	HALMAC_REG_W8(REG_CR + 1, value8);

	value8 = HALMAC_REG_R8(REG_FWHW_TXQ_CTRL + 2);
	restore[1] = value8;
	value8 = (u8)(value8 & ~(BIT(6)));
	HALMAC_REG_W8(REG_FWHW_TXQ_CTRL + 2, value8);

	if (PLTFM_SEND_RSVD_PAGE(buf, size) == _FALSE) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]send rvsd pg(pltfm)!!\n");
		status = HALMAC_RET_DL_RSVD_PAGE_FAIL;
		goto DL_RSVD_PG_END;
	}

	cnt = 1000;
	while (!(HALMAC_REG_R8(REG_FIFOPAGE_CTRL_2 + 1) & BIT(7))) {
		PLTFM_DELAY_US(10);
		cnt--;
		if (cnt == 0) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]bcn valid!!\n");
			status = HALMAC_RET_POLLING_BCN_VALID_FAIL;
			break;
		}
	}
DL_RSVD_PG_END:
	rsvd_pg_head = adapter->txff_allocation.rsvd_pg_bndy &
		BIT_MASK_BCN_HEAD_1_V1;
	HALMAC_REG_W16(REG_FIFOPAGE_CTRL_2, rsvd_pg_head | BIT(15));
	HALMAC_REG_W8(REG_FWHW_TXQ_CTRL + 2, restore[1]);
	HALMAC_REG_W8(REG_CR + 1, restore[0]);

	*state = HALMAC_RSVD_PG_STATE_IDLE;

	return status;
}

enum halmac_ret_status
halmac_get_hw_value_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_hw_id hw_id,
	OUT void *value
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	switch (hw_id) {
	case HALMAC_HW_RQPN_MAPPING:
		halmac_get_gueue_mapping(adapter,
					 (struct halmac_rqpn_map *)value);
		break;
	case HALMAC_HW_EFUSE_SIZE:
		*(u32 *)value = adapter->hw_cfg_info.efuse_size;
		break;
	case HALMAC_HW_EEPROM_SIZE:
		*(u32 *)value = adapter->hw_cfg_info.eeprom_size;
		break;
	case HALMAC_HW_BT_BANK_EFUSE_SIZE:
		*(u32 *)value = adapter->hw_cfg_info.bt_efuse_size;
		break;
	case HALMAC_HW_BT_BANK1_EFUSE_SIZE:
	case HALMAC_HW_BT_BANK2_EFUSE_SIZE:
		*(u32 *)value = 0;
		break;
	case HALMAC_HW_TXFIFO_SIZE:
		*(u32 *)value = adapter->hw_cfg_info.tx_fifo_size;
		break;
	case HALMAC_HW_RXFIFO_SIZE:
		*(u32 *)value = adapter->hw_cfg_info.rx_fifo_size;
		break;
	case HALMAC_HW_RSVD_PG_BNDY:
		*(u16 *)value = adapter->txff_allocation.rsvd_drv_pg_bndy;
		break;
	case HALMAC_HW_CAM_ENTRY_NUM:
		*(u8 *)value = adapter->hw_cfg_info.cam_entry_num;
		break;
	case HALMAC_HW_WLAN_EFUSE_AVAILABLE_SIZE:
		halmac_get_efuse_available_size_88xx(adapter, (u32 *)value);
		break;
	case HALMAC_HW_IC_VERSION:
		*(u8 *)value = adapter->chip_version;
		break;
	case HALMAC_HW_PAGE_SIZE:
		*(u32 *)value = adapter->hw_cfg_info.page_size;
		break;
	case HALMAC_HW_TX_AGG_ALIGN_SIZE:
		*(u16 *)value = adapter->hw_cfg_info.tx_align_size;
		break;
	case HALMAC_HW_RX_AGG_ALIGN_SIZE:
		*(u8 *)value = 8;
		break;
	case HALMAC_HW_DRV_INFO_SIZE:
		*(u8 *)value = adapter->drv_info_size;
		break;
	case HALMAC_HW_TXFF_ALLOCATION:
		PLTFM_MEMCPY(value, &adapter->txff_allocation,
			     sizeof(struct halmac_txff_allocation));
		break;
	case HALMAC_HW_RSVD_EFUSE_SIZE:
		*(u32 *)value = HALMAC_PROTECTED_EFUSE_SIZE_88XX;
		break;
	case HALMAC_HW_FW_HDR_SIZE:
		*(u32 *)value = HALMAC_FWHDR_SIZE_88XX;
		break;
	case HALMAC_HW_TX_DESC_SIZE:
		*(u32 *)value = adapter->hw_cfg_info.txdesc_size;
		break;
	case HALMAC_HW_RX_DESC_SIZE:
		*(u32 *)value = adapter->hw_cfg_info.rxdesc_size;
		break;
	case HALMAC_HW_FW_MAX_SIZE:
		*(u32 *)value = HALMAC_FW_SIZE_MAX_88XX;
		break;
	case HALMAC_HW_ORI_H2C_SIZE:
		*(u32 *)value = HALMAC_H2C_CMD_ORIGINAL_SIZE_88XX;
		break;
	case HALMAC_HW_RSVD_DRV_PGNUM:
		*(u16 *)value = adapter->txff_allocation.rsvd_drv_pg_num;
		break;
	case HALMAC_HW_TX_PAGE_SIZE:
		*(u16 *)value = HALMAC_TX_PAGE_SIZE_88XX;
		break;
	case HALMAC_HW_USB_TXAGG_DESC_NUM:
		*(u8 *)value = adapter->hw_cfg_info.usb_txagg_num;
		break;
	case HALMAC_HW_AC_OQT_SIZE:
		*(u8 *)value = adapter->hw_cfg_info.ac_oqt_size;
		break;
	case HALMAC_HW_NON_AC_OQT_SIZE:
		*(u8 *)value = adapter->hw_cfg_info.non_ac_oqt_size;
		break;
	case HALMAC_HW_AC_QUEUE_NUM:
		*(u8 *)value = adapter->hw_cfg_info.ac_queue_num;
		break;
	case HALMAC_HW_PWR_STATE:
		halmac_power_state_88xx(adapter,
					(enum halmac_mac_power *)value);
		break;
	default:
		return HALMAC_RET_PARA_NOT_SUPPORT;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

static void
halmac_get_gueue_mapping(
	IN struct halmac_adapter *adapter,
	IN struct halmac_rqpn_map *mapping
)
{
	mapping->dma_map_vo = adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_VO];
	mapping->dma_map_vi = adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_VI];
	mapping->dma_map_be = adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_BE];
	mapping->dma_map_bk = adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_BK];
	mapping->dma_map_mg = adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_MG];
	mapping->dma_map_hi = adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_HI];
}

/**
 * halmac_set_hw_value_88xx() -set hw config value
 * @adapter : the adapter of halmac
 * @hw_id : hw id for driver to config
 * @value : hw value, reference table to get data type
 * Author : KaiYuan Chang / Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_set_hw_value_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_hw_id hw_id,
	IN void *value
)
{
	enum halmac_ret_status status;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (!value) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]null ptr-set hw value\n");
		return HALMAC_RET_NULL_POINTER;
	}

	switch (hw_id) {
	case HALMAC_HW_USB_MODE:
		status = halmac_set_usb_mode_88xx(
			adapter, *(enum halmac_usb_mode *)value);
		if (status != HALMAC_RET_SUCCESS)
			return status;
		break;
	case HALMAC_HW_BANDWIDTH:
		halmac_cfg_bw_88xx(adapter, *(enum halmac_bw *)value);
		break;
	case HALMAC_HW_CHANNEL:
		halmac_cfg_ch_88xx(adapter, *(u8 *)value);
		break;
	case HALMAC_HW_PRI_CHANNEL_IDX:
		halmac_cfg_pri_ch_idx_88xx(
			adapter, *(enum halmac_pri_ch_idx *)value);
		break;
	case HALMAC_HW_EN_BB_RF:
		halmac_enable_bb_rf_88xx(adapter, *(u8 *)value);
		break;
	case HALMAC_HW_SDIO_TX_PAGE_THRESHOLD:
		halmac_config_sdio_tx_page_threshold_88xx(
			adapter, (struct halmac_tx_page_threshold_info *)value);
		break;
	case HALMAC_HW_AMPDU_CONFIG:
		halmac_config_ampdu_88xx(
			adapter, (struct halmac_ampdu_config *)value);
		break;
	case HALMAC_HW_RX_SHIFT:
		halmac_rx_shift_88xx(adapter, *(u8 *)value);
		break;
	case HALMAC_HW_TXDESC_CHECKSUM:
		halmac_tx_desc_checksum_88xx(adapter, *(u8 *)value);
		break;
	case HALMAC_HW_RX_CLK_GATE:
		halmac_rx_clk_gate_88xx(adapter, *(u8 *)value);
		break;
	case HALMAC_HW_FAST_EDCA:
		halmac_fast_edca_cfg_88xx(adapter,
					  (struct halmac_fast_edca_cfg *)value);
		break;
	default:
		return HALMAC_RET_PARA_NOT_SUPPORT;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

enum halmac_ret_status
halmac_set_h2c_pkt_hdr_88xx(
	IN struct halmac_adapter *adapter,
	OUT u8 *hdr,
	IN struct halmac_h2c_header_info *info,
	OUT u16 *seq_num
)
{
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s!!\n", __func__);

	FW_OFFLOAD_H2C_SET_TOTAL_LEN(hdr, 8 + info->content_size);
	FW_OFFLOAD_H2C_SET_SUB_CMD_ID(hdr, info->sub_cmd_id);

	FW_OFFLOAD_H2C_SET_CATEGORY(hdr, 0x01);
	FW_OFFLOAD_H2C_SET_CMD_ID(hdr, 0xFF);

	PLTFM_MUTEX_LOCK(&adapter->h2c_seq_mutex);
	FW_OFFLOAD_H2C_SET_SEQ_NUM(hdr, adapter->h2c_packet_seq);
	*seq_num = adapter->h2c_packet_seq;
	adapter->h2c_packet_seq++;
	PLTFM_MUTEX_UNLOCK(&adapter->h2c_seq_mutex);

	if (info->ack == _TRUE)
		FW_OFFLOAD_H2C_SET_ACK(hdr, _TRUE);

	return HALMAC_RET_SUCCESS;
}

enum halmac_ret_status
halmac_send_h2c_pkt_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *pkt,
	IN u32 size
)
{
	u32 cnt = 100;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	while (adapter->h2c_buf_free_space <= HALMAC_H2C_CMD_SIZE_88XX) {
		halmac_get_h2c_buff_free_space_88xx(adapter);
		cnt--;
		if (cnt == 0) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
					"[ERR]h2c free space!!\n");
			return HALMAC_RET_H2C_SPACE_FULL;
		}
	}

	cnt = 100;
	do {
		if (PLTFM_SEND_H2C_PKT(pkt, size) == _TRUE)
			break;
		cnt--;
		if (cnt == 0) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
					"[ERR]pltfm - sned h2c pkt!!\n");
			return HALMAC_RET_SEND_H2C_FAIL;
		}
		PLTFM_DELAY_US(5);

	} while (1);

	adapter->h2c_buf_free_space -= HALMAC_H2C_CMD_SIZE_88XX;

	return status;
}

enum halmac_ret_status
halmac_get_h2c_buff_free_space_88xx(
	IN struct halmac_adapter *adapter
)
{
	u32 hw_wptr;
	u32 fw_rptr;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	hw_wptr = HALMAC_REG_R32(REG_H2C_PKT_WRITEADDR) & BIT_MASK_H2C_WR_ADDR;
	fw_rptr = HALMAC_REG_R32(REG_H2C_PKT_READADDR) & BIT_MASK_H2C_READ_ADDR;

	if (hw_wptr >= fw_rptr)
		adapter->h2c_buf_free_space =
			adapter->h2c_buff_size - (hw_wptr - fw_rptr);
	else
		adapter->h2c_buf_free_space = fw_rptr - hw_wptr;

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_get_c2h_info_88xx() - process halmac C2H packet
 * @adapter : the adapter of halmac
 * @buf : RX Packet pointer
 * @size : RX Packet size
 * Author : KaiYuan Chang/Ivan Lin
 *
 * Used to process c2h packet info from RX path. After receiving the packet,
 * user need to call this api and pass the packet pointer.
 *
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_get_c2h_info_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (GET_RX_DESC_C2H(buf) == _TRUE) {
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Parse c2h pkt\n");

		status = halmac_parse_c2h_packet_88xx(adapter, buf, size);

		if (status != HALMAC_RET_SUCCESS) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Parse c2h pkt\n");
			return status;
		}
	}

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_parse_c2h_packet_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
)
{
	u8 cmd_id, sub_cmd_id;
	u8 *c2h_pkt = buf + adapter->hw_cfg_info.rxdesc_size;
	u32 c2h_size = size - adapter->hw_cfg_info.rxdesc_size;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	cmd_id = (u8)C2H_HDR_GET_CMD_ID(c2h_pkt);

	if (cmd_id != 0xFF) {
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Not 0xFF cmd!!\n");
		return HALMAC_RET_C2H_NOT_HANDLED;
	}

	sub_cmd_id = (u8)C2H_HDR_GET_C2H_SUB_CMD_ID(c2h_pkt);

	switch (sub_cmd_id) {
	case C2H_SUB_CMD_ID_C2H_DBG:
		status = halmac_get_c2h_dbg_88xx(adapter, c2h_pkt, c2h_size);
		break;
	case C2H_SUB_CMD_ID_H2C_ACK_HDR:
		status = halmac_get_h2c_ack_88xx(adapter, c2h_pkt, c2h_size);
		break;
	case C2H_SUB_CMD_ID_BT_COEX_INFO:
		status = HALMAC_RET_C2H_NOT_HANDLED;
		break;
	case C2H_SUB_CMD_ID_SCAN_STATUS_RPT:
		status = halmac_get_scan_rpt_88xx(adapter, c2h_pkt, c2h_size);
		break;
	case C2H_SUB_CMD_ID_PSD_DATA:
		status = halmac_get_psd_data_88xx(adapter, c2h_pkt, c2h_size);
		break;
	case C2H_SUB_CMD_ID_EFUSE_DATA:
		status = halmac_get_efuse_data_88xx(adapter, c2h_pkt, c2h_size);
		break;
	default:
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]Sub cmd id!!\n");
		status = HALMAC_RET_C2H_NOT_HANDLED;
		break;
	}

	return status;
}

static enum halmac_ret_status
halmac_get_c2h_dbg_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
)
{
	u8 i;
	u8 next_msg = 0;
	u8 cur_msg = 0;
	u8 msg_len = 0;
	char *c2h_buf = (char *)NULL;
	u8 content_len = 0;
	u8 seq_num = 0;

	content_len = (u8)C2H_HDR_GET_LEN((u8 *)buf);

	if (content_len > C2H_DBG_CONTENT_MAX_LENGTH) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]c2h size > max len!\n");
		return HALMAC_RET_C2H_NOT_HANDLED;
	}

	for (i = 0; i < content_len; i++) {
		if (*(buf + C2H_DBG_HEADER_LENGTH + i) == '\n') {
			if ((*(buf + C2H_DBG_HEADER_LENGTH + i + 1) == '\0') ||
			    (*(buf + C2H_DBG_HEADER_LENGTH + i + 1) == 0xff)) {
				next_msg = C2H_DBG_HEADER_LENGTH + i + 1;
				goto _ENDFOUND;
			}
		}
	}

_ENDFOUND:
	msg_len = next_msg - C2H_DBG_HEADER_LENGTH;

	c2h_buf = (char *)PLTFM_MALLOC(msg_len);
	if (!c2h_buf)
		return HALMAC_RET_MALLOC_FAIL;

	PLTFM_MEMCPY(c2h_buf, buf + C2H_DBG_HEADER_LENGTH, msg_len);

	seq_num = (u8)(*(c2h_buf));
	*(c2h_buf + msg_len - 1) = '\0';
	PLTFM_MSG_PRINT(HALMAC_DBG_ALWAYS, "[RTKFW, SEQ=%d]: %s\n",
			seq_num, (char *)(c2h_buf + 1));
	PLTFM_FREE(c2h_buf, msg_len);

	while (*(buf + next_msg) != '\0') {
		cur_msg = next_msg;

		msg_len = (u8)(*(buf + cur_msg + 3)) - 1;
		next_msg += C2H_DBG_HEADER_LENGTH + msg_len;

		c2h_buf = (char *)PLTFM_MALLOC(msg_len);
		if (!c2h_buf)
			return HALMAC_RET_MALLOC_FAIL;

		PLTFM_MEMCPY(c2h_buf, buf + cur_msg + C2H_DBG_HEADER_LENGTH,
			     msg_len);
		*(c2h_buf + msg_len - 1) = '\0';
		seq_num = (u8)(*(c2h_buf));
		PLTFM_MSG_PRINT(HALMAC_DBG_ALWAYS, "[RTKFW, SEQ=%d]: %s\n",
				seq_num, (char *)(c2h_buf + 1));
		PLTFM_FREE(c2h_buf, msg_len);
	}

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_get_h2c_ack_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
)
{
	u8 cmd_id, sub_cmd_id;
	u8 fw_rc;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Ack for C2H!!\n");

	fw_rc = (u8)H2C_ACK_HDR_GET_H2C_RETURN_CODE(buf);
	if (HALMAC_H2C_RETURN_SUCCESS != (enum halmac_h2c_return_code)fw_rc)
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]fw rc = %d\n", fw_rc);

	cmd_id = (u8)H2C_ACK_HDR_GET_H2C_CMD_ID(buf);

	if (cmd_id != 0xFF) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]h2c ack cmd id!!\n");
		return HALMAC_RET_C2H_NOT_HANDLED;
	}

	sub_cmd_id = (u8)H2C_ACK_HDR_GET_H2C_SUB_CMD_ID(buf);

	switch (sub_cmd_id) {
	case H2C_SUB_CMD_ID_DUMP_PHYSICAL_EFUSE_ACK:
		status = halmac_get_h2c_ack_phy_efuse_88xx(adapter, buf, size);
		break;
	case H2C_SUB_CMD_ID_CFG_PARAMETER_ACK:
		status = halmac_get_h2c_ack_cfg_para_88xx(adapter, buf, size);
		break;
	case H2C_SUB_CMD_ID_UPDATE_PACKET_ACK:
		status = halmac_get_h2c_ack_update_pkt_88xx(adapter, buf, size);
		break;
	case H2C_SUB_CMD_ID_UPDATE_DATAPACK_ACK:
		status = halmac_get_h2c_ack_update_datapkt_88xx(adapter,
								buf, size);
		break;
	case H2C_SUB_CMD_ID_RUN_DATAPACK_ACK:
		status = halmac_get_h2c_ack_run_datapkt_88xx(adapter,
							     buf, size);
		break;
	case H2C_SUB_CMD_ID_CHANNEL_SWITCH_ACK:
		status = halmac_get_h2c_ack_ch_switch_88xx(adapter, buf, size);
		break;
	case H2C_SUB_CMD_ID_IQK_ACK:
		status = halmac_get_h2c_ack_iqk_88xx(adapter, buf, size);
		break;
	case H2C_SUB_CMD_ID_POWER_TRACKING_ACK:
		status = halmac_get_h2c_ack_power_tracking_88xx(adapter,
								buf, size);
		break;
	case H2C_SUB_CMD_ID_PSD_ACK:
		break;
	case H2C_SUB_CMD_ID_FW_SNDING_ACK:
		status = halmac_get_h2c_ack_fw_snding_88xx(adapter, buf, size);
		break;
	default:
		status = HALMAC_RET_C2H_NOT_HANDLED;
		break;
	}

	return status;
}

static enum halmac_ret_status
halmac_get_scan_rpt_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
)
{
	u8 fw_rc;
	enum halmac_cmd_process_status proc_status;

	fw_rc = (u8)SCAN_STATUS_RPT_GET_H2C_RETURN_CODE(buf);
	proc_status = (HALMAC_H2C_RETURN_SUCCESS ==
		(enum halmac_h2c_return_code)fw_rc) ?
		HALMAC_CMD_PROCESS_DONE : HALMAC_CMD_PROCESS_ERROR;

	PLTFM_EVENT_INDICATION(HALMAC_FEATURE_CHANNEL_SWITCH,
			       proc_status, NULL, 0);

	adapter->halmac_state.scan_state_set.process_status = proc_status;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]scan : %X\n", proc_status);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_get_psd_data_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
)
{
	u8 seg_id;
	u8 seg_size;
	u8 seq_num;
	u16 total_size;
	enum halmac_cmd_process_status proc_status;
	struct halmac_psd_state_set *state = &adapter->halmac_state.psd_set;

	seq_num = (u8)PSD_DATA_GET_H2C_SEQ(buf);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]seq num : h2c->%d c2h->%d\n",
			state->seq_num, seq_num);
	if (seq_num != state->seq_num) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
				"[ERR]seq num mismatch : h2c->%d c2h->%d\n",
				state->seq_num, seq_num);
		return HALMAC_RET_SUCCESS;
	}

	if (state->process_status != HALMAC_CMD_PROCESS_SENDING) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]not cmd sending\n");
		return HALMAC_RET_SUCCESS;
	}

	total_size = (u16)PSD_DATA_GET_TOTAL_SIZE(buf);
	seg_id = (u8)PSD_DATA_GET_SEGMENT_ID(buf);
	seg_size = (u8)PSD_DATA_GET_SEGMENT_SIZE(buf);
	state->data_size = total_size;

	if (!state->data)
		state->data = (u8 *)PLTFM_MALLOC(state->data_size);

	if (seg_id == 0)
		state->segment_size = seg_size;

	PLTFM_MEMCPY(state->data + seg_id * state->segment_size,
		     buf + HALMAC_C2H_DATA_OFFSET_88XX, seg_size);

	if (PSD_DATA_GET_END_SEGMENT(buf) == _FALSE)
		return HALMAC_RET_SUCCESS;

	proc_status = HALMAC_CMD_PROCESS_DONE;
	state->process_status = proc_status;

	PLTFM_EVENT_INDICATION(HALMAC_FEATURE_PSD, proc_status,
			       state->data, state->data_size);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_get_h2c_ack_cfg_para_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
)
{
	u8 seq_num;
	u8 fw_rc;
	u32 offset_accu;
	u32 value_accu;
	struct halmac_cfg_para_state_set *state =
		&adapter->halmac_state.cfg_para_state_set;
	enum halmac_cmd_process_status proc_status =
		HALMAC_CMD_PROCESS_UNDEFINE;

	seq_num = (u8)H2C_ACK_HDR_GET_H2C_SEQ(buf);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Seq num : h2c->%d c2h->%d\n",
			state->seq_num, seq_num);
	if (seq_num != state->seq_num) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
				"[ERR]Seq num mismatch : h2c->%d c2h->%d\n",
				state->seq_num, seq_num);
		return HALMAC_RET_SUCCESS;
	}

	if (state->process_status != HALMAC_CMD_PROCESS_SENDING) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]not cmd sending\n");
		return HALMAC_RET_SUCCESS;
	}

	fw_rc = (u8)H2C_ACK_HDR_GET_H2C_RETURN_CODE(buf);
	state->fw_return_code = fw_rc;
	offset_accu = CFG_PARAMETER_ACK_GET_OFFSET_ACCUMULATION(buf);
	value_accu = CFG_PARAMETER_ACK_GET_VALUE_ACCUMULATION(buf);

	if ((offset_accu != adapter->config_para_info.offset_accumulation) ||
	    (value_accu != adapter->config_para_info.value_accumulation)) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
				"[ERR][C2H]offset_accu : %x, value_accu : %xn",
				offset_accu, value_accu);
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
				"[ERR][Ada]offset_accu : %x, value_accu : %x\n",
				adapter->config_para_info.offset_accumulation,
				adapter->config_para_info.value_accumulation);
		proc_status = HALMAC_CMD_PROCESS_ERROR;
	}

	if ((enum halmac_h2c_return_code)fw_rc == HALMAC_H2C_RETURN_SUCCESS &&
	    proc_status != HALMAC_CMD_PROCESS_ERROR) {
		proc_status = HALMAC_CMD_PROCESS_DONE;
		state->process_status = proc_status;
		PLTFM_EVENT_INDICATION(HALMAC_FEATURE_CFG_PARA, proc_status,
				       NULL, 0);
	} else {
		proc_status = HALMAC_CMD_PROCESS_ERROR;
		state->process_status = proc_status;
		PLTFM_EVENT_INDICATION(HALMAC_FEATURE_CFG_PARA, proc_status,
				       &state->fw_return_code, 1);
	}

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_get_h2c_ack_update_pkt_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
)
{
	u8 seq_num;
	u8 fw_rc;
	struct halmac_update_packet_state_set *state =
		&adapter->halmac_state.update_packet_set;
	enum halmac_cmd_process_status proc_status;

	seq_num = (u8)H2C_ACK_HDR_GET_H2C_SEQ(buf);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Seq num : h2c->%d c2h->%d\n",
			state->seq_num, seq_num);
	if (seq_num != state->seq_num) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
				"[ERR]Seq num mismatch : h2c->%d c2h->%d\n",
				state->seq_num, seq_num);
		return HALMAC_RET_SUCCESS;
	}

	if (state->process_status != HALMAC_CMD_PROCESS_SENDING) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]not cmd sending\n");
		return HALMAC_RET_SUCCESS;
	}

	fw_rc = (u8)H2C_ACK_HDR_GET_H2C_RETURN_CODE(buf);
	state->fw_return_code = fw_rc;

	if (HALMAC_H2C_RETURN_SUCCESS == (enum halmac_h2c_return_code)fw_rc) {
		proc_status = HALMAC_CMD_PROCESS_DONE;
		state->process_status = proc_status;
		PLTFM_EVENT_INDICATION(HALMAC_FEATURE_UPDATE_PACKET,
				       proc_status, NULL, 0);
	} else {
		proc_status = HALMAC_CMD_PROCESS_ERROR;
		state->process_status = proc_status;
		PLTFM_EVENT_INDICATION(HALMAC_FEATURE_UPDATE_PACKET,
				       proc_status, &state->fw_return_code, 1);
	}

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_get_h2c_ack_update_datapkt_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
)
{
	PLTFM_EVENT_INDICATION(HALMAC_FEATURE_UPDATE_DATAPACK,
			       HALMAC_CMD_PROCESS_UNDEFINE, NULL, 0);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_get_h2c_ack_run_datapkt_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
)
{
	PLTFM_EVENT_INDICATION(HALMAC_FEATURE_RUN_DATAPACK,
			       HALMAC_CMD_PROCESS_UNDEFINE, NULL, 0);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_get_h2c_ack_ch_switch_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
)
{
	u8 seq_num;
	u8 fw_rc;
	struct halmac_scan_state_set *state =
		&adapter->halmac_state.scan_state_set;
	enum halmac_cmd_process_status proc_status;

	seq_num = (u8)H2C_ACK_HDR_GET_H2C_SEQ(buf);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Seq num : h2c->%d c2h->%d\n",
			state->seq_num, seq_num);
	if (seq_num != state->seq_num) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
				"[ERR]Seq num mismatch : h2c->%d c2h->%d\n",
				state->seq_num, seq_num);
		return HALMAC_RET_SUCCESS;
	}

	if (state->process_status != HALMAC_CMD_PROCESS_SENDING) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]not cmd sending\n");
		return HALMAC_RET_SUCCESS;
	}

	fw_rc = (u8)H2C_ACK_HDR_GET_H2C_RETURN_CODE(buf);
	state->fw_return_code = fw_rc;

	if ((enum halmac_h2c_return_code)fw_rc == HALMAC_H2C_RETURN_SUCCESS) {
		proc_status = HALMAC_CMD_PROCESS_RCVD;
		state->process_status = proc_status;
		PLTFM_EVENT_INDICATION(HALMAC_FEATURE_CHANNEL_SWITCH,
				       proc_status, NULL, 0);
	} else {
		proc_status = HALMAC_CMD_PROCESS_ERROR;
		state->process_status = proc_status;
		PLTFM_EVENT_INDICATION(HALMAC_FEATURE_CHANNEL_SWITCH,
				       proc_status, &state->fw_return_code, 1);
	}

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_debug_88xx() - dump debug information
 * @adapter : the adapter of halmac
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_debug_88xx(
	IN struct halmac_adapter *adapter
)
{	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (adapter->halmac_interface == HALMAC_INTERFACE_SDIO)
		halmac_dump_reg_sdio_88xx(adapter);
	else
		halmac_dump_reg_88xx(adapter);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

static void
halmac_dump_reg_sdio_88xx(
	IN struct halmac_adapter *adapter
)
{
	u8 tmp8;
	u32 i;

	/* Dump CCCR, it needs new platform api */

	/*Dump SDIO Local Register, use CMD52*/
	for (i = 0x10250000; i < 0x102500ff; i++) {
		tmp8 = PLTFM_SDIO_CMD52_R(i);
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE,
				"[TRACE]dbg-sdio[%x]=%x\n", i, tmp8);
	}

	/*Dump MAC Register*/
	for (i = 0x0000; i < 0x17ff; i++) {
		tmp8 = PLTFM_SDIO_CMD52_R(i);
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE,
				"[TRACE]dbg-mac[%x]=%x\n", i, tmp8);
	}

	/*Check RX Fifo status*/
	i = REG_RXFF_PTR_V1;
	tmp8 = PLTFM_SDIO_CMD52_R(i);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]dbg-mac[%x]=%x\n", i, tmp8);
	i = REG_RXFF_WTR_V1;
	tmp8 = PLTFM_SDIO_CMD52_R(i);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]dbg-mac[%x]=%x\n", i, tmp8);
	i = REG_RXFF_PTR_V1;
	tmp8 = PLTFM_SDIO_CMD52_R(i);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]dbg-mac[%x]=%x\n", i, tmp8);
	i = REG_RXFF_WTR_V1;
	tmp8 = PLTFM_SDIO_CMD52_R(i);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]dbg-mac[%x]=%x\n", i, tmp8);
}

static void
halmac_dump_reg_88xx(
	IN struct halmac_adapter *adapter
)
{
	u32 tmp32;
	u32 i;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	/*Dump MAC Register*/
	for (i = 0x0000; i < 0x17fc; i += 4) {
		tmp32 = HALMAC_REG_R32(i);
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE,
				"[TRACE]dbg-mac[%x]=%x\n", i, tmp32);
	}

	/*Check RX Fifo status*/
	i = REG_RXFF_PTR_V1;
	tmp32 = HALMAC_REG_R32(i);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]dbg-mac[%x]=%x\n", i, tmp32);
	i = REG_RXFF_WTR_V1;
	tmp32 = HALMAC_REG_R32(i);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]dbg-mac[%x]=%x\n", i, tmp32);
	i = REG_RXFF_PTR_V1;
	tmp32 = HALMAC_REG_R32(i);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]dbg-mac[%x]=%x\n", i, tmp32);
	i = REG_RXFF_WTR_V1;
	tmp32 = HALMAC_REG_R32(i);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]dbg-mac[%x]=%x\n", i, tmp32);
}

/**
 * halmac_cfg_parameter_88xx() - config parameter by FW
 * @adapter : the adapter of halmac
 * @info : cmd id, content
 * @full_fifo : parameter information
 *
 * If msk_en = _TRUE, the format of array is {reg_info, mask, value}.
 * If msk_en =_FAUSE, the format of array is {reg_info, value}
 * The format of reg_info is
 * reg_info[31]=rf_reg, 0: MAC_BB reg, 1: RF reg
 * reg_info[27:24]=rf_path, 0: path_A, 1: path_B
 * if rf_reg=0(MAC_BB reg), rf_path is meaningless.
 * ref_info[15:0]=offset
 *
 * Example: msk_en = _FALSE
 * {0x8100000a, 0x00001122}
 * =>Set RF register, path_B, offset 0xA to 0x00001122
 * {0x00000824, 0x11224433}
 * =>Set MAC_BB register, offset 0x800 to 0x11224433
 *
 * Note : full fifo mode only for init flow
 *
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_cfg_parameter_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_phy_parameter_info *info,
	IN u8 full_fifo
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	enum halmac_cmd_process_status *proc_status =
		&adapter->halmac_state.cfg_para_state_set.process_status;
	enum halmac_cfg_para_cmd_construct_state cmd_state;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (halmac_fw_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_NO_DLFW;

	if (adapter->fw_version.h2c_version < 4)
		return HALMAC_RET_FW_NO_SUPPORT;

	if (*proc_status == HALMAC_CMD_PROCESS_SENDING) {
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Wait event(para)\n");
		return HALMAC_RET_BUSY_STATE;
	}

	cmd_state = halmac_cfg_param_cmd_state_88xx(adapter);
	if ((cmd_state != HALMAC_CFG_PARA_CMD_IDLE) &&
	    (cmd_state != HALMAC_CFG_PARA_CMD_CONSTRUCT)) {
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Not idle(para)\n");
		return HALMAC_RET_BUSY_STATE;
	}

	*proc_status = HALMAC_CMD_PROCESS_IDLE;

	status = halmac_proc_cfg_param_88xx(adapter, info, full_fifo);

	if ((status != HALMAC_RET_SUCCESS) &&
	    (status != HALMAC_RET_PARA_SENDING)) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]send param h2c\n");
		return status;
	}

	return status;
}

static enum halmac_cfg_para_cmd_construct_state
halmac_cfg_param_cmd_state_88xx(
	IN struct halmac_adapter *adapter
)
{
	return adapter->halmac_state.cfg_para_state_set.cfg_param_cmd_state;
}

static enum halmac_ret_status
halmac_proc_cfg_param_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_phy_parameter_info *param,
	IN u8 full_fifo
)
{
	u8 end_cmd = _FALSE;
	u32 rsvd_size;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	struct halmac_config_para_info *info = &adapter->config_para_info;
	enum halmac_cmd_process_status *proc_status =
		&adapter->halmac_state.cfg_para_state_set.process_status;

	status = halmac_malloc_cfg_param_buf_88xx(adapter, full_fifo);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	if (halmac_transform_cfg_para_state_88xx(
		adapter, HALMAC_CFG_PARA_CMD_CONSTRUCT) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_ERROR_STATE;

	halmac_add_param_buf_88xx(adapter, param, info->pPara_buf_w, &end_cmd);
	if (param->cmd_id != HALMAC_PARAMETER_CMD_END) {
		info->para_num++;
		info->pPara_buf_w += HALMAC_FW_OFFLOAD_CMD_SIZE_88XX;
		info->avai_para_buf_size -= HALMAC_FW_OFFLOAD_CMD_SIZE_88XX;
	}

	rsvd_size = info->avai_para_buf_size - adapter->hw_cfg_info.txdesc_size;
	if ((rsvd_size > HALMAC_FW_OFFLOAD_CMD_SIZE_88XX) &&
	    (end_cmd == _FALSE)) {
		return HALMAC_RET_SUCCESS;
	}

	if (info->para_num == 0) {
		PLTFM_FREE(info->pCfg_para_buf, info->para_buf_size);
		info->pCfg_para_buf = NULL;
		info->pPara_buf_w = NULL;
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]param num = 0!!\n");

		*proc_status = HALMAC_CMD_PROCESS_DONE;
		PLTFM_EVENT_INDICATION(HALMAC_FEATURE_CFG_PARA,
				       *proc_status, NULL, 0);

		halmac_reset_feature_88xx(adapter, HALMAC_FEATURE_CFG_PARA);

		return HALMAC_RET_SUCCESS;
	}

	status = halmac_send_cfg_param_h2c_88xx(adapter);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	if (end_cmd == _FALSE) {
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]send h2c-buf full\n");
		return HALMAC_RET_PARA_SENDING;
	}

	return status;
}

static enum halmac_ret_status
halmac_send_cfg_param_h2c_88xx(
	IN struct halmac_adapter *adapter
)
{
	u8 h2c_buf[HALMAC_H2C_CMD_SIZE_88XX] = { 0 };
	u16 pg_addr;
	u16 seq_num = 0;
	u32 info_size;
	struct halmac_h2c_header_info hdr_info;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	struct halmac_config_para_info *info = &adapter->config_para_info;
	enum halmac_cmd_process_status *proc_status =
		&adapter->halmac_state.cfg_para_state_set.process_status;

	if (halmac_transform_cfg_para_state_88xx(
		adapter, HALMAC_CFG_PARA_CMD_H2C_SENT) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_ERROR_STATE;

	*proc_status = HALMAC_CMD_PROCESS_SENDING;

	if (info->full_fifo_mode == _TRUE)
		pg_addr = 0;
	else
		pg_addr = adapter->txff_allocation.rsvd_h2c_extra_info_pg_bndy;

	info_size = info->para_num * HALMAC_FW_OFFLOAD_CMD_SIZE_88XX;

	status = halmac_download_rsvd_page_88xx(
			adapter, pg_addr, (u8 *)info->pCfg_para_buf, info_size);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]dl rsvd pg!!\n");
		goto CFG_PARAM_H2C_FAIL;
	}

	halmac_gen_cfg_para_h2c_88xx(adapter, h2c_buf);

	hdr_info.sub_cmd_id = SUB_CMD_ID_CFG_PARAMETER;
	hdr_info.content_size = 4;
	hdr_info.ack = _TRUE;
	halmac_set_h2c_pkt_hdr_88xx(adapter, h2c_buf, &hdr_info, &seq_num);

	adapter->halmac_state.cfg_para_state_set.seq_num = seq_num;

	status = halmac_send_h2c_pkt_88xx(adapter, h2c_buf,
					  HALMAC_H2C_CMD_SIZE_88XX);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]send h2c!!\n");
		halmac_reset_feature_88xx(adapter, HALMAC_FEATURE_CFG_PARA);
	}

CFG_PARAM_H2C_FAIL:
	PLTFM_FREE(info->pCfg_para_buf, info->para_buf_size);
	info->pCfg_para_buf = NULL;
	info->pPara_buf_w = NULL;

	if (halmac_transform_cfg_para_state_88xx(
		adapter, HALMAC_CFG_PARA_CMD_IDLE) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_ERROR_STATE;

	return status;
}

static enum halmac_ret_status
halmac_transform_cfg_para_state_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_cfg_para_cmd_construct_state dest_state
)
{
	enum halmac_cfg_para_cmd_construct_state *state =
		&adapter->halmac_state.cfg_para_state_set.cfg_param_cmd_state;

	if ((*state != HALMAC_CFG_PARA_CMD_IDLE) &&
	    (*state != HALMAC_CFG_PARA_CMD_CONSTRUCT) &&
	    (*state != HALMAC_CFG_PARA_CMD_H2C_SENT))
		return HALMAC_RET_ERROR_STATE;

	if (dest_state == HALMAC_CFG_PARA_CMD_IDLE) {
		if (*state == HALMAC_CFG_PARA_CMD_CONSTRUCT)
			return HALMAC_RET_ERROR_STATE;
	} else if (dest_state == HALMAC_CFG_PARA_CMD_CONSTRUCT) {
		if (*state == HALMAC_CFG_PARA_CMD_H2C_SENT)
			return HALMAC_RET_ERROR_STATE;
	} else if (dest_state == HALMAC_CFG_PARA_CMD_H2C_SENT) {
		if ((*state == HALMAC_CFG_PARA_CMD_IDLE) ||
		    (*state == HALMAC_CFG_PARA_CMD_H2C_SENT))
			return HALMAC_RET_ERROR_STATE;
	}

	*state = dest_state;

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_add_param_buf_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_phy_parameter_info *param,
	IN u8 *buf,
	OUT u8 *pEnd_cmd
)
{
	struct halmac_config_para_info *info = &adapter->config_para_info;
	union halmac_parameter_content *content = &param->content;

	*pEnd_cmd = _FALSE;

	PHY_PARAMETER_INFO_SET_LENGTH(buf, HALMAC_FW_OFFLOAD_CMD_SIZE_88XX);
	PHY_PARAMETER_INFO_SET_IO_CMD(buf, param->cmd_id);

	switch (param->cmd_id) {
	case HALMAC_PARAMETER_CMD_BB_W8:
	case HALMAC_PARAMETER_CMD_BB_W16:
	case HALMAC_PARAMETER_CMD_BB_W32:
	case HALMAC_PARAMETER_CMD_MAC_W8:
	case HALMAC_PARAMETER_CMD_MAC_W16:
	case HALMAC_PARAMETER_CMD_MAC_W32:
		PHY_PARAMETER_INFO_SET_IO_ADDR(buf, content->MAC_REG_W.offset);
		PHY_PARAMETER_INFO_SET_DATA(buf, content->MAC_REG_W.value);
		PHY_PARAMETER_INFO_SET_MASK(buf, content->MAC_REG_W.msk);
		PHY_PARAMETER_INFO_SET_MSK_EN(buf, content->MAC_REG_W.msk_en);
		info->value_accumulation += content->MAC_REG_W.value;
		info->offset_accumulation += content->MAC_REG_W.offset;
		break;
	case HALMAC_PARAMETER_CMD_RF_W:
		/*In rf register, the address is only 1 byte*/
		PHY_PARAMETER_INFO_SET_RF_ADDR(buf, content->RF_REG_W.offset);
		PHY_PARAMETER_INFO_SET_RF_PATH(buf, content->RF_REG_W.rf_path);
		PHY_PARAMETER_INFO_SET_DATA(buf, content->RF_REG_W.value);
		PHY_PARAMETER_INFO_SET_MASK(buf, content->RF_REG_W.msk);
		PHY_PARAMETER_INFO_SET_MSK_EN(buf, content->RF_REG_W.msk_en);
		info->value_accumulation += content->RF_REG_W.value;
		info->offset_accumulation += (content->RF_REG_W.offset +
					(content->RF_REG_W.rf_path << 8));
		break;
	case HALMAC_PARAMETER_CMD_DELAY_US:
	case HALMAC_PARAMETER_CMD_DELAY_MS:
		PHY_PARAMETER_INFO_SET_DELAY_VALUE(
			buf, content->DELAY_TIME.delay_time);
		break;
	case HALMAC_PARAMETER_CMD_END:
		*pEnd_cmd = _TRUE;
		break;
	default:
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]cmd id!!\n");
		break;
	}

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_gen_cfg_para_h2c_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buff
)
{
	struct halmac_config_para_info *info = &adapter->config_para_info;

	CFG_PARAMETER_SET_NUM(buff, info->para_num);

	if (info->full_fifo_mode == _TRUE) {
		CFG_PARAMETER_SET_INIT_CASE(buff, 0x1);
		CFG_PARAMETER_SET_PHY_PARAMETER_LOC(buff, 0);
	} else {
		CFG_PARAMETER_SET_INIT_CASE(buff, 0x0);
		CFG_PARAMETER_SET_PHY_PARAMETER_LOC(
			buff,
			adapter->txff_allocation.rsvd_h2c_extra_info_pg_bndy -
			adapter->txff_allocation.rsvd_pg_bndy);
	}

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_malloc_cfg_param_buf_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 full_fifo
)
{
	struct halmac_config_para_info *info = &adapter->config_para_info;
	struct halmac_ofld_func_info *ofld_func_info = &adapter->ofld_func_info;

	if (info->pCfg_para_buf)
		return HALMAC_RET_SUCCESS;

	if (full_fifo == _TRUE)
		info->para_buf_size = ofld_func_info->halmac_malloc_max_sz;
	else
		info->para_buf_size = HALMAC_CFG_PARA_RSVDPG_SZ_88XX;

	if (info->para_buf_size > ofld_func_info->rsvd_pg_drv_buf_max_sz)
		info->para_buf_size = ofld_func_info->rsvd_pg_drv_buf_max_sz;

	info->pCfg_para_buf = halmac_adaptive_malloc_88xx(
				adapter,
				info->para_buf_size, &info->para_buf_size);
	if (info->pCfg_para_buf) {
		PLTFM_MEMSET(info->pCfg_para_buf, 0x00, info->para_buf_size);
		info->full_fifo_mode = full_fifo;
		info->pPara_buf_w = info->pCfg_para_buf;
		info->para_num = 0;
		info->avai_para_buf_size = info->para_buf_size;
		info->value_accumulation = 0;
		info->offset_accumulation = 0;
	} else {
		return HALMAC_RET_MALLOC_FAIL;
	}

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_update_packet_88xx() - send specific packet to FW
 * @adapter : the adapter of halmac
 * @pkt_id : packet id, to know the purpose of this packet
 * @pkt : packet
 * @size : packet size
 *
 * Note : TX_DESC is not included in the pkt
 *
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_update_packet_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_packet_id pkt_id,
	IN u8 *pkt,
	IN u32 size
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	enum halmac_cmd_process_status *proc_status =
		&adapter->halmac_state.update_packet_set.process_status;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (halmac_fw_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_NO_DLFW;

	if (adapter->fw_version.h2c_version < 4)
		return HALMAC_RET_FW_NO_SUPPORT;

	if (size > HALMAC_UPDATE_PKT_RSVDPG_SZ_88XX)
		return HALMAC_RET_RSVD_PG_OVERFLOW_FAIL;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (*proc_status == HALMAC_CMD_PROCESS_SENDING) {
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Wait event(upd)\n");
		return HALMAC_RET_BUSY_STATE;
	}

	*proc_status = HALMAC_CMD_PROCESS_SENDING;

	status = halmac_send_h2c_update_packet_88xx(adapter, pkt_id, pkt, size);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]send h2c!!\n");
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]pkt id : %X!!\n", pkt_id);
		return status;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_send_h2c_update_packet_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_packet_id pkt_id,
	IN u8 *pkt,
	IN u32 size
)
{
	u8 h2c_buf[HALMAC_H2C_CMD_SIZE_88XX] = { 0 };
	u16 seq_num = 0;
	u16 pg_addr = adapter->txff_allocation.rsvd_h2c_extra_info_pg_bndy;
	u16 pg_offset = adapter->txff_allocation.rsvd_h2c_extra_info_pg_bndy -
			adapter->txff_allocation.rsvd_pg_bndy;
	struct halmac_h2c_header_info hdr_info;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	status = halmac_download_rsvd_page_88xx(adapter, pg_addr, pkt, size);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]dl rsvd pg!!\n");
		return status;
	}

	UPDATE_PACKET_SET_SIZE(h2c_buf,
			       size + adapter->hw_cfg_info.txdesc_size);
	UPDATE_PACKET_SET_PACKET_ID(h2c_buf, pkt_id);
	UPDATE_PACKET_SET_PACKET_LOC(h2c_buf, pg_offset);

	hdr_info.sub_cmd_id = SUB_CMD_ID_UPDATE_PACKET;
	hdr_info.content_size = 8;
	hdr_info.ack = _TRUE;
	halmac_set_h2c_pkt_hdr_88xx(adapter, h2c_buf, &hdr_info, &seq_num);
	adapter->halmac_state.update_packet_set.seq_num = seq_num;

	status = halmac_send_h2c_pkt_88xx(adapter, h2c_buf,
					  HALMAC_H2C_CMD_SIZE_88XX);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]send h2c!!\n");
		halmac_reset_feature_88xx(adapter,
					  HALMAC_FEATURE_UPDATE_PACKET);
		return status;
	}

	return status;
}

enum halmac_ret_status
halmac_bcn_ie_filter_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_bcn_ie_info *info
)
{
	return HALMAC_RET_NOT_SUPPORT;
}

enum halmac_ret_status
halmac_update_datapack_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_data_type data_type,
	IN struct halmac_phy_parameter_info *info
)
{
	return HALMAC_RET_NOT_SUPPORT;
}

enum halmac_ret_status
halmac_run_datapack_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_data_type data_type
)
{
	return HALMAC_RET_NOT_SUPPORT;
}

enum halmac_ret_status
halmac_send_bt_coex_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size,
	IN u8 ack
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (halmac_fw_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_NO_DLFW;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	status = halmac_send_bt_coex_cmd_88xx(adapter, buf, size, ack);

	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]bt coex cmd!!\n");
		return status;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_send_bt_coex_cmd_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size,
	IN u8 ack
)
{
	u8 h2c_buf[HALMAC_H2C_CMD_SIZE_88XX] = { 0 };
	u16 seq_num = 0;
	struct halmac_h2c_header_info hdr_info;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	PLTFM_MEMCPY(h2c_buf + 8, buf, size);

	hdr_info.sub_cmd_id = SUB_CMD_ID_BT_COEX;
	hdr_info.content_size = (u16)size;
	hdr_info.ack = ack;
	halmac_set_h2c_pkt_hdr_88xx(adapter, h2c_buf, &hdr_info, &seq_num);

	status = halmac_send_h2c_pkt_88xx(adapter, h2c_buf,
					  HALMAC_H2C_CMD_SIZE_88XX);

	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]send h2c!!\n");
		return status;
	}

	return HALMAC_RET_SUCCESS;
}

enum halmac_ret_status
halmac_send_original_h2c_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *org_h2c,
	IN u16 *seq,
	IN u8 ack
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (halmac_fw_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_NO_DLFW;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	status = halmac_func_send_original_h2c_88xx(adapter, org_h2c, seq, ack);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]send org h2c!!\n");
		return status;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_func_send_original_h2c_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *org_h2c,
	IN u16 *seq,
	IN u8 ack
)
{
	u8 h2c_buf[HALMAC_H2C_CMD_SIZE_88XX] = { 0 };
	u8 *h2c_hdr, *h2c_cmd;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	h2c_hdr = h2c_buf;
	h2c_cmd = h2c_hdr + HALMAC_H2C_CMD_HDR_SIZE_88XX;
	PLTFM_MEMCPY(h2c_cmd, org_h2c, 8); /* original h2c = 8 byte */

	halmac_set_h2c_header_88xx(adapter, h2c_hdr, seq, ack);
	status = halmac_send_h2c_pkt_88xx(adapter, h2c_buf,
					  HALMAC_H2C_CMD_SIZE_88XX);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]sned h2c!!\n");
		return status;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_dump_fifo_88xx() - dump fifo data
 * @adapter : the adapter of halmac
 * @sel : FIFO selection
 * @start_addr : start address of selected FIFO
 * @size : dump size of selected FIFO
 * @data : FIFO data
 *
 * Note : before dump fifo, user need to call halmac_get_fifo_size to
 * get fifo size. Then input this size to halmac_dump_fifo.
 *
 * Author : Ivan Lin/KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_dump_fifo_88xx(
	IN struct halmac_adapter *adapter,
	IN enum hal_fifo_sel sel,
	IN u32 start_addr,
	IN u32 size,
	OUT u8 *data
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	u8 tmp8, enable;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (sel == HAL_FIFO_SEL_TX &&
	    (start_addr + size) > adapter->hw_cfg_info.tx_fifo_size) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]size overflow!!\n");
		return HALMAC_RET_DUMP_FIFOSIZE_INCORRECT;
	}

	if (sel == HAL_FIFO_SEL_RX &&
	    (start_addr + size) > adapter->hw_cfg_info.rx_fifo_size) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]size overflow!!\n");
		return HALMAC_RET_DUMP_FIFOSIZE_INCORRECT;
	}

	if ((size & (4 - 1)) != 0) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]not 4byte alignment!!\n");
		return HALMAC_RET_DUMP_FIFOSIZE_INCORRECT;
	}

	if (!data)
		return HALMAC_RET_NULL_POINTER;

	tmp8 = HALMAC_REG_R8(REG_RCR + 2);
	enable = _FALSE;
	status = api->halmac_set_hw_value(
			adapter, HALMAC_HW_RX_CLK_GATE, &enable);
	if (status != HALMAC_RET_SUCCESS)
		return status;
	status = halmac_buffer_read_88xx(adapter, start_addr, size, sel, data);

	HALMAC_REG_W8(REG_RCR + 2, tmp8);

	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]read buf!!\n");
		return status;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_buffer_read_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u32 size,
	IN enum hal_fifo_sel sel,
	OUT u8 *data
)
{
	u32 start_page;
	u32 value32;
	u32 i;
	u32 residue;
	u32 cnt = 0;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (sel == HAL_FIFO_SEL_RSVD_PAGE)
		offset += (adapter->txff_allocation.rsvd_pg_bndy <<
			   HALMAC_TX_PAGE_SIZE_2_POWER_88XX);

	start_page = offset >> 12;
	residue = offset & (4096 - 1);

	if ((sel == HAL_FIFO_SEL_TX) || (sel == HAL_FIFO_SEL_RSVD_PAGE))
		start_page += 0x780;
	else if (sel == HAL_FIFO_SEL_RX)
		start_page += 0x700;
	else if (sel == HAL_FIFO_SEL_REPORT)
		start_page += 0x660;
	else if (sel == HAL_FIFO_SEL_LLT)
		start_page += 0x650;
	else if (sel == HAL_FIFO_SEL_RXBUF_FW)
		start_page += 0x680;
	else
		return HALMAC_RET_NOT_SUPPORT;

	value32 = HALMAC_REG_R16(REG_PKTBUF_DBG_CTRL);

	do {
		HALMAC_REG_W16(REG_PKTBUF_DBG_CTRL,
			       (u16)(start_page | (value32 & 0xF000)));

		for (i = 0x8000 + residue; i <= 0x8FFF; i += 4) {
			*(u32 *)(data + cnt) = HALMAC_REG_R32(i);
			*(u32 *)(data + cnt) =
				rtk_le32_to_cpu(*(u32 *)(data + cnt));
			cnt += 4;
			if (size == cnt)
				goto HALMAC_BUF_READ_OK;
		}

		residue = 0;
		start_page++;
	} while (1);

HALMAC_BUF_READ_OK:
	HALMAC_REG_W16(REG_PKTBUF_DBG_CTRL, (u16)value32);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_get_fifo_size_88xx() - get fifo size
 * @adapter : the adapter of halmac
 * @sel : FIFO selection
 * Author : Ivan Lin/KaiYuan Chang
 * Return : u32
 * More details of status code can be found in prototype document
 */
u32
halmac_get_fifo_size_88xx(
	IN struct halmac_adapter *adapter,
	IN enum hal_fifo_sel sel
)
{
	u32 size = 0;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (sel == HAL_FIFO_SEL_TX)
		size = adapter->hw_cfg_info.tx_fifo_size;
	else if (sel == HAL_FIFO_SEL_RX)
		size = adapter->hw_cfg_info.rx_fifo_size;
	else if (sel == HAL_FIFO_SEL_RSVD_PAGE)
		size = ((adapter->hw_cfg_info.tx_fifo_size >>
			HALMAC_TX_PAGE_SIZE_2_POWER_88XX) -
			adapter->txff_allocation.rsvd_pg_bndy)
			<< HALMAC_TX_PAGE_SIZE_2_POWER_88XX;
	else if (sel == HAL_FIFO_SEL_REPORT)
		size = 65536;
	else if (sel == HAL_FIFO_SEL_LLT)
		size = 65536;
	else if (sel == HAL_FIFO_SEL_RXBUF_FW)
		size = HALMAC_RX_BUF_FW_88XX;

	return size;
}

enum halmac_ret_status
halmac_set_h2c_header_88xx(
	IN struct halmac_adapter *adapter,
	OUT u8 *hdr,
	IN u16 *seq,
	IN u8 ack
)
{
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s!!\n", __func__);

	H2C_CMD_HEADER_SET_CATEGORY(hdr, 0x00);
	H2C_CMD_HEADER_SET_TOTAL_LEN(hdr, 16);

	PLTFM_MUTEX_LOCK(&adapter->h2c_seq_mutex);
	H2C_CMD_HEADER_SET_SEQ_NUM(hdr, adapter->h2c_packet_seq);
	*seq = adapter->h2c_packet_seq;
	adapter->h2c_packet_seq++;
	PLTFM_MUTEX_UNLOCK(&adapter->h2c_seq_mutex);

	if (ack == _TRUE)
		H2C_CMD_HEADER_SET_ACK(hdr, _TRUE);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_add_ch_info_88xx() -add channel information
 * @adapter : the adapter of halmac
 * @info : channel information
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_add_ch_info_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_ch_info *info
)
{
	struct halmac_cs_info *ch_sw_info = &adapter->ch_sw_info;
	enum halmac_scan_cmd_construct_state state;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (adapter->halmac_state.dlfw_state != HALMAC_GEN_INFO_SENT) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]gen info\n");
		return HALMAC_RET_GEN_INFO_NOT_SENT;
	}

	state = halmac_scan_cmd_state_88xx(adapter);
	if ((state != HALMAC_SCAN_CMD_BUFFER_CLEARED) &&
	    (state != HALMAC_SCAN_CMD_CONSTRUCT)) {
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]cmd state (scan)\n");
		return HALMAC_RET_ERROR_STATE;
	}

	if (!ch_sw_info->ch_info_buf) {
		ch_sw_info->ch_info_buf =
			(u8 *)PLTFM_MALLOC(HALMAC_SCAN_INFO_RSVDPG_SZ_88XX);
		if (!ch_sw_info->ch_info_buf)
			return HALMAC_RET_NULL_POINTER;
		ch_sw_info->ch_info_buf_w = ch_sw_info->ch_info_buf;
		ch_sw_info->buf_size = HALMAC_SCAN_INFO_RSVDPG_SZ_88XX;
		ch_sw_info->avai_buf_size = HALMAC_SCAN_INFO_RSVDPG_SZ_88XX;
		ch_sw_info->total_size = 0;
		ch_sw_info->extra_info_en = 0;
		ch_sw_info->ch_num = 0;
	}

	if (ch_sw_info->extra_info_en == 1) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]extra info = 1!!\n");
		return HALMAC_RET_CH_SW_SEQ_WRONG;
	}

	if (ch_sw_info->avai_buf_size < 4) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]buf full!!\n");
		return HALMAC_RET_CH_SW_NO_BUF;
	}

	if (halmac_transform_scan_state_88xx(
		adapter, HALMAC_SCAN_CMD_CONSTRUCT) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_ERROR_STATE;

	CHANNEL_INFO_SET_CHANNEL(ch_sw_info->ch_info_buf_w, info->channel);
	CHANNEL_INFO_SET_PRI_CH_IDX(ch_sw_info->ch_info_buf_w,
				    info->pri_ch_idx);
	CHANNEL_INFO_SET_BANDWIDTH(ch_sw_info->ch_info_buf_w, info->bw);
	CHANNEL_INFO_SET_TIMEOUT(ch_sw_info->ch_info_buf_w, info->timeout);
	CHANNEL_INFO_SET_ACTION_ID(ch_sw_info->ch_info_buf_w, info->action_id);
	CHANNEL_INFO_SET_CH_EXTRA_INFO(ch_sw_info->ch_info_buf_w,
				       info->extra_info);

	ch_sw_info->avai_buf_size = ch_sw_info->avai_buf_size - 4;
	ch_sw_info->total_size = ch_sw_info->total_size + 4;
	ch_sw_info->ch_num++;
	ch_sw_info->extra_info_en = info->extra_info;
	ch_sw_info->ch_info_buf_w = ch_sw_info->ch_info_buf_w + 4;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_scan_cmd_construct_state
halmac_scan_cmd_state_88xx(
	IN struct halmac_adapter *adapter
)
{
	return adapter->halmac_state.scan_state_set.scan_cmd_construct_state;
}

static enum halmac_ret_status
halmac_transform_scan_state_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_scan_cmd_construct_state dest_state
)
{
	enum halmac_scan_cmd_construct_state *state =
		&adapter->halmac_state.scan_state_set.scan_cmd_construct_state;

	if (*state > HALMAC_SCAN_CMD_H2C_SENT)
		return HALMAC_RET_ERROR_STATE;

	if (dest_state == HALMAC_SCAN_CMD_IDLE) {
		if ((*state == HALMAC_SCAN_CMD_BUFFER_CLEARED) ||
		    (*state == HALMAC_SCAN_CMD_CONSTRUCT))
			return HALMAC_RET_ERROR_STATE;
	} else if (dest_state == HALMAC_SCAN_CMD_BUFFER_CLEARED) {
		if (*state == HALMAC_SCAN_CMD_H2C_SENT)
			return HALMAC_RET_ERROR_STATE;
	} else if (dest_state == HALMAC_SCAN_CMD_CONSTRUCT) {
		if ((*state == HALMAC_SCAN_CMD_IDLE) ||
		    (*state == HALMAC_SCAN_CMD_H2C_SENT))
			return HALMAC_RET_ERROR_STATE;
	} else if (dest_state == HALMAC_SCAN_CMD_H2C_SENT) {
		if ((*state != HALMAC_SCAN_CMD_CONSTRUCT) &&
		    (*state != HALMAC_SCAN_CMD_BUFFER_CLEARED))
			return HALMAC_RET_ERROR_STATE;
	}

	*state = dest_state;

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_add_extra_ch_info_88xx() -add extra channel information
 * @adapter : the adapter of halmac
 * @info : extra channel information
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_add_extra_ch_info_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_ch_extra_info *info
)
{
	struct halmac_cs_info *ch_sw_info = &adapter->ch_sw_info;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (!ch_sw_info->ch_info_buf) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]ch_info_buf = null!!\n");
		return HALMAC_RET_CH_SW_SEQ_WRONG;
	}

	if (ch_sw_info->extra_info_en == 0) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]extra info = 0!!\n");
		return HALMAC_RET_CH_SW_SEQ_WRONG;
	}

	/* ch_extra_info_id, ch_extra_info, ch_extra_info_size are totally 2Byte */
	if (ch_sw_info->avai_buf_size < (u32)(info->extra_info_size + 2)) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]no available buffer!!\n");
		return HALMAC_RET_CH_SW_NO_BUF;
	}

	if (halmac_scan_cmd_state_88xx(adapter) != HALMAC_SCAN_CMD_CONSTRUCT) {
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]cmd state (ex scan)\n");
		return HALMAC_RET_ERROR_STATE;
	}

	if (halmac_transform_scan_state_88xx(
		adapter, HALMAC_SCAN_CMD_CONSTRUCT) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_ERROR_STATE;

	CH_EXTRA_INFO_SET_CH_EXTRA_INFO_ID(ch_sw_info->ch_info_buf_w,
					   info->extra_action_id);
	CH_EXTRA_INFO_SET_CH_EXTRA_INFO(ch_sw_info->ch_info_buf_w,
					info->extra_info);
	CH_EXTRA_INFO_SET_CH_EXTRA_INFO_SIZE(ch_sw_info->ch_info_buf_w,
					     info->extra_info_size);
	PLTFM_MEMCPY(ch_sw_info->ch_info_buf_w + 2, info->extra_info_data,
		     info->extra_info_size);

	ch_sw_info->avai_buf_size -= (2 + info->extra_info_size);
	ch_sw_info->total_size += (2 + info->extra_info_size);
	ch_sw_info->extra_info_en = info->extra_info;
	ch_sw_info->ch_info_buf_w += (2 + info->extra_info_size);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_ctrl_ch_switch_88xx() -send channel switch cmd
 * @adapter : the adapter of halmac
 * @opt : channel switch config
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_ctrl_ch_switch_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_ch_switch_option *opt
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	enum halmac_scan_cmd_construct_state state;
	enum halmac_cmd_process_status *proc_status =
		&adapter->halmac_state.scan_state_set.process_status;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (halmac_fw_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_NO_DLFW;

	if (adapter->fw_version.h2c_version < 4)
		return HALMAC_RET_FW_NO_SUPPORT;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (opt->switch_en == _FALSE)
		*proc_status = HALMAC_CMD_PROCESS_IDLE;

	if ((*proc_status == HALMAC_CMD_PROCESS_SENDING) ||
	    (*proc_status == HALMAC_CMD_PROCESS_RCVD)) {
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Wait event(scan)\n");
		return HALMAC_RET_BUSY_STATE;
	}

	state = halmac_scan_cmd_state_88xx(adapter);
	if (opt->switch_en == _TRUE) {
		if (state != HALMAC_SCAN_CMD_CONSTRUCT) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]state(en = 1)\n");
			return HALMAC_RET_ERROR_STATE;
		}
	} else {
		if (state != HALMAC_SCAN_CMD_BUFFER_CLEARED) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]state(en = 0)\n");
			return HALMAC_RET_ERROR_STATE;
		}
	}

	status = halmac_func_ctrl_ch_switch_88xx(adapter, opt);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]ctrl ch sw!!\n");
		return status;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_func_ctrl_ch_switch_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_ch_switch_option *opt
)
{
	u8 h2c_buf[HALMAC_H2C_CMD_SIZE_88XX] = { 0 };
	u16 seq_num = 0;
	u16 pg_addr = adapter->txff_allocation.rsvd_h2c_extra_info_pg_bndy;
	struct halmac_h2c_header_info hdr_info;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	enum halmac_cmd_process_status *proc_status =
		&adapter->halmac_state.scan_state_set.process_status;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (halmac_transform_scan_state_88xx(
		adapter, HALMAC_SCAN_CMD_H2C_SENT) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_ERROR_STATE;

	*proc_status = HALMAC_CMD_PROCESS_SENDING;

	if (opt->switch_en != 0) {
		status = halmac_download_rsvd_page_88xx(
				adapter, pg_addr,
				adapter->ch_sw_info.ch_info_buf,
				adapter->ch_sw_info.total_size);
		if (status != HALMAC_RET_SUCCESS) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]dl rsvd pg!!\n");
			return status;
		}
	}

	CHANNEL_SWITCH_SET_SWITCH_START(h2c_buf, opt->switch_en);
	CHANNEL_SWITCH_SET_CHANNEL_NUM(h2c_buf, adapter->ch_sw_info.ch_num);
	CHANNEL_SWITCH_SET_CHANNEL_INFO_LOC(
		h2c_buf, pg_addr - adapter->txff_allocation.rsvd_pg_bndy);
	CHANNEL_SWITCH_SET_DEST_CH_EN(h2c_buf, opt->dest_ch_en);
	CHANNEL_SWITCH_SET_DEST_CH(h2c_buf, opt->dest_ch);
	CHANNEL_SWITCH_SET_PRI_CH_IDX(h2c_buf, opt->dest_pri_ch_idx);
	CHANNEL_SWITCH_SET_ABSOLUTE_TIME(h2c_buf, opt->absolute_time_en);
	CHANNEL_SWITCH_SET_TSF_LOW(h2c_buf, opt->tsf_low);
	CHANNEL_SWITCH_SET_PERIODIC_OPTION(h2c_buf, opt->periodic_option);
	CHANNEL_SWITCH_SET_NORMAL_CYCLE(h2c_buf, opt->normal_cycle);
	CHANNEL_SWITCH_SET_NORMAL_PERIOD(h2c_buf, opt->normal_period);
	CHANNEL_SWITCH_SET_SLOW_PERIOD(h2c_buf, opt->phase_2_period);
	CHANNEL_SWITCH_SET_NORMAL_PERIOD_SEL(h2c_buf, opt->normal_period_sel);
	CHANNEL_SWITCH_SET_SLOW_PERIOD_SEL(h2c_buf, opt->phase_2_period_sel);
	CHANNEL_SWITCH_SET_CHANNEL_INFO_SIZE(
		h2c_buf, adapter->ch_sw_info.total_size);

	hdr_info.sub_cmd_id = SUB_CMD_ID_CHANNEL_SWITCH;
	hdr_info.content_size = 20;
	hdr_info.ack = _TRUE;
	halmac_set_h2c_pkt_hdr_88xx(adapter, h2c_buf, &hdr_info, &seq_num);
	adapter->halmac_state.scan_state_set.seq_num = seq_num;

	status = halmac_send_h2c_pkt_88xx(adapter, h2c_buf,
					  HALMAC_H2C_CMD_SIZE_88XX);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]send h2c!!\n");
		halmac_reset_feature_88xx(adapter,
					  HALMAC_FEATURE_CHANNEL_SWITCH);
	}
	PLTFM_FREE(adapter->ch_sw_info.ch_info_buf,
		   adapter->ch_sw_info.buf_size);
	adapter->ch_sw_info.ch_info_buf = NULL;
	adapter->ch_sw_info.ch_info_buf_w = NULL;
	adapter->ch_sw_info.extra_info_en = 0;
	adapter->ch_sw_info.buf_size = 0;
	adapter->ch_sw_info.avai_buf_size = 0;
	adapter->ch_sw_info.total_size = 0;
	adapter->ch_sw_info.ch_num = 0;

	if (halmac_transform_scan_state_88xx(
		adapter, HALMAC_SCAN_CMD_IDLE) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_ERROR_STATE;

	return status;
}

/**
 * halmac_clear_ch_info_88xx() -clear channel information
 * @adapter : the adapter of halmac
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_clear_ch_info_88xx(
	IN struct halmac_adapter *adapter
)
{
	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (halmac_scan_cmd_state_88xx(adapter) == HALMAC_SCAN_CMD_H2C_SENT) {
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]state(clear)\n");
		return HALMAC_RET_ERROR_STATE;
	}

	if (halmac_transform_scan_state_88xx(
		adapter, HALMAC_SCAN_CMD_BUFFER_CLEARED) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_ERROR_STATE;

	PLTFM_FREE(adapter->ch_sw_info.ch_info_buf,
		   adapter->ch_sw_info.buf_size);
	adapter->ch_sw_info.ch_info_buf = NULL;
	adapter->ch_sw_info.ch_info_buf_w = NULL;
	adapter->ch_sw_info.extra_info_en = 0;
	adapter->ch_sw_info.buf_size = 0;
	adapter->ch_sw_info.avai_buf_size = 0;
	adapter->ch_sw_info.total_size = 0;
	adapter->ch_sw_info.ch_num = 0;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_send_general_info_88xx() -send general information to FW
 * @adapter : the adapter of halmac
 * @info : general information
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_send_general_info_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_general_info *info
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (halmac_fw_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_NO_DLFW;

	if (adapter->fw_version.h2c_version < 4)
		return HALMAC_RET_FW_NO_SUPPORT;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (adapter->halmac_state.dlfw_state == HALMAC_DLFW_NONE) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]no dl fw!!\n");
		return HALMAC_RET_NO_DLFW;
	}

	status = halmac_func_send_general_info_88xx(adapter, info);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]send gen info!!\n");
		return status;
	}

	status = halmac_func_send_phydm_info_88xx(adapter, info);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]send phydm info\n");
		return status;
	}

	if (adapter->halmac_state.dlfw_state == HALMAC_DLFW_DONE)
		adapter->halmac_state.dlfw_state = HALMAC_GEN_INFO_SENT;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_func_send_general_info_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_general_info *info
)
{
	u8 h2c_buf[HALMAC_H2C_CMD_SIZE_88XX] = { 0 };
	u16 seq_num = 0;
	struct halmac_h2c_header_info hdr_info;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s\n", __func__);

	GENERAL_INFO_SET_FW_TX_BOUNDARY(
			h2c_buf,
			adapter->txff_allocation.rsvd_fw_txbuff_pg_bndy -
			adapter->txff_allocation.rsvd_pg_bndy);

	hdr_info.sub_cmd_id = SUB_CMD_ID_GENERAL_INFO;
	hdr_info.content_size = 4;
	hdr_info.ack = _FALSE;
	halmac_set_h2c_pkt_hdr_88xx(adapter, h2c_buf, &hdr_info, &seq_num);

	status = halmac_send_h2c_pkt_88xx(adapter, h2c_buf,
					  HALMAC_H2C_CMD_SIZE_88XX);
	if (status != HALMAC_RET_SUCCESS)
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]send h2c!!\n");

	return status;
}

static enum halmac_ret_status
halmac_func_send_phydm_info_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_general_info *info
)
{
	u8 h2c_buf[HALMAC_H2C_CMD_SIZE_88XX] = { 0 };
	u16 seq_num = 0;
	struct halmac_h2c_header_info hdr_info;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s\n", __func__);

	PHYDM_INFO_SET_REF_TYPE(h2c_buf, info->rfe_type);
	PHYDM_INFO_SET_RF_TYPE(h2c_buf, info->rf_type);
	PHYDM_INFO_SET_CUT_VER(h2c_buf, adapter->chip_version);
	PHYDM_INFO_SET_RX_ANT_STATUS(h2c_buf, info->rx_ant_status);
	PHYDM_INFO_SET_TX_ANT_STATUS(h2c_buf, info->tx_ant_status);

	hdr_info.sub_cmd_id = SUB_CMD_ID_PHYDM_INFO;
	hdr_info.content_size = 8;
	hdr_info.ack = _FALSE;
	halmac_set_h2c_pkt_hdr_88xx(adapter, h2c_buf, &hdr_info, &seq_num);

	status = halmac_send_h2c_pkt_88xx(adapter, h2c_buf,
					  HALMAC_H2C_CMD_SIZE_88XX);

	if (status != HALMAC_RET_SUCCESS)
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]send h2c!!\n");

	return status;
}

/**
 * halmac_chk_txdesc_88xx() -check if the tx packet format is incorrect
 * @adapter : the adapter of halmac
 * @buf : tx Packet buffer, tx desc is included
 * @size : tx packet size
 * Author : KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_chk_txdesc_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
)
{
	u32 mac_clk = 0;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (GET_TX_DESC_BMC(buf) == _TRUE)
		if (GET_TX_DESC_AGG_EN(buf) == _TRUE)
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
					"[ERR]txdesc - agg + bmc\n");

	if (size < (GET_TX_DESC_TXPKTSIZE(buf) + GET_TX_DESC_OFFSET(buf)))
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]txdesc - total size\n");

	if (GET_TX_DESC_AMSDU_PAD_EN(buf) != 0)
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]txdesc - amsdu_pad\n");

	switch (BIT_GET_MAC_CLK_SEL(HALMAC_REG_R32(REG_AFE_CTRL1))) {
	case 0x0:
		mac_clk = 80;
		break;
	case 0x1:
		mac_clk = 40;
		break;
	case 0x2:
		mac_clk = 20;
		break;
	case 0x3:
		mac_clk = 10;
		break;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_ALWAYS, "MAC clock : 0x%XM\n", mac_clk);
	PLTFM_MSG_PRINT(HALMAC_DBG_ALWAYS,
			"mac agg en : 0x%X\n", GET_TX_DESC_AGG_EN(buf));
	PLTFM_MSG_PRINT(HALMAC_DBG_ALWAYS,
			"mac agg num : 0x%X\n", GET_TX_DESC_MAX_AGG_NUM(buf));

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_get_version() - get HALMAC version
 * @ver : return version of major, prototype and minor information
 * Author : KaiYuan Chang / Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_get_version_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_ver *ver
)
{
	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	ver->major_ver = (u8)HALMAC_MAJOR_VER_88XX;
	ver->prototype_ver = (u8)HALMAC_PROTOTYPE_VER_88XX;
	ver->minor_ver = (u8)HALMAC_MINOR_VER_88XX;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

enum halmac_ret_status
halmac_p2pps_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_p2pps *info
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (halmac_fw_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_NO_DLFW;

	if (adapter->fw_version.h2c_version < 6)
		return HALMAC_RET_FW_NO_SUPPORT;

	status = halmac_func_p2pps_88xx(adapter, info);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]p2pps!!\n");
		return status;
	}

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_func_p2pps_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_p2pps *info
)
{
	u8 h2c_buf[HALMAC_H2C_CMD_SIZE_88XX] = { 0 };
	u16 seq_num = 0;
	struct halmac_h2c_header_info hdr_info;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	P2PPS_SET_OFFLOAD_EN(h2c_buf, info->offload_en);
	P2PPS_SET_ROLE(h2c_buf, info->role);
	P2PPS_SET_CTWINDOW_EN(h2c_buf, info->ctwindow_en);
	P2PPS_SET_NOA_EN(h2c_buf, info->noa_en);
	P2PPS_SET_NOA_SEL(h2c_buf, info->noa_sel);
	P2PPS_SET_ALLSTASLEEP(h2c_buf, info->all_sta_sleep);
	P2PPS_SET_DISCOVERY(h2c_buf, info->discovery);
	P2PPS_SET_DISABLE_CLOSERF(h2c_buf, info->disable_close_rf);
	P2PPS_SET_P2P_PORT_ID(h2c_buf, info->p2p_port_id);
	P2PPS_SET_P2P_GROUP(h2c_buf, info->p2p_group);
	P2PPS_SET_P2P_MACID(h2c_buf, info->p2p_macid);

	P2PPS_SET_CTWINDOW_LENGTH(h2c_buf, info->ctwindow_length);

	P2PPS_SET_NOA_DURATION_PARA(h2c_buf, info->noa_duration_para);
	P2PPS_SET_NOA_INTERVAL_PARA(h2c_buf, info->noa_interval_para);
	P2PPS_SET_NOA_START_TIME_PARA(h2c_buf, info->noa_start_time_para);
	P2PPS_SET_NOA_COUNT_PARA(h2c_buf, info->noa_count_para);

	hdr_info.sub_cmd_id = SUB_CMD_ID_P2PPS;
	hdr_info.content_size = 24;
	hdr_info.ack = _FALSE;
	halmac_set_h2c_pkt_hdr_88xx(adapter, h2c_buf, &hdr_info, &seq_num);

	status = halmac_send_h2c_pkt_88xx(adapter, h2c_buf,
					  HALMAC_H2C_CMD_SIZE_88XX);
	if (status != HALMAC_RET_SUCCESS)
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]send h2c!!\n");

	return status;
}

/**
 * halmac_query_status_88xx() -query the offload feature status
 * @adapter : the adapter of halmac
 * @feature_id : feature_id
 * @proc_status : feature_status
 * @data : data buffer
 * @size : data size
 *
 * Note :
 * If user wants to know the data size, user can allocate zero
 * size buffer first. If this size less than the data size, halmac
 * will return  HALMAC_RET_BUFFER_TOO_SMALL. User need to
 * re-allocate data buffer with correct data size.
 *
 * Author : Ivan Lin/KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_query_status_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_feature_id feature_id,
	OUT enum halmac_cmd_process_status *proc_status,
	INOUT u8 *data,
	INOUT u32 *size
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (!proc_status)
		return HALMAC_RET_NULL_POINTER;

	switch (feature_id) {
	case HALMAC_FEATURE_CFG_PARA:
		status = halmac_cfg_param_status_88xx(adapter, proc_status);
		break;
	case HALMAC_FEATURE_DUMP_PHYSICAL_EFUSE:
		status = halmac_query_dump_physical_efuse_status_88xx(
				adapter, proc_status, data, size);
		break;
	case HALMAC_FEATURE_DUMP_LOGICAL_EFUSE:
		status = halmac_query_dump_logical_efuse_status_88xx(
				adapter, proc_status, data, size);
		break;
	case HALMAC_FEATURE_CHANNEL_SWITCH:
		status = halmac_ch_switch_status_88xx(adapter, proc_status);
		break;
	case HALMAC_FEATURE_UPDATE_PACKET:
		status = halmac_update_packet_status_88xx(adapter, proc_status);
		break;
	case HALMAC_FEATURE_IQK:
		status = halmac_query_iqk_status_88xx(
				adapter, proc_status, data, size);
		break;
	case HALMAC_FEATURE_POWER_TRACKING:
		status = halmac_query_power_tracking_status_88xx(
				adapter, proc_status, data, size);
		break;
	case HALMAC_FEATURE_PSD:
		status = halmac_query_psd_status_88xx(
				adapter, proc_status, data, size);
		break;
	case HALMAC_FEATURE_FW_SNDING:
		status = halmac_query_fw_snding_status_88xx(
				adapter, proc_status, data, size);
		break;
	default:
		return HALMAC_RET_INVALID_FEATURE_ID;
	}

	return status;
}

static enum halmac_ret_status
halmac_cfg_param_status_88xx(
	IN struct halmac_adapter *adapter,
	OUT enum halmac_cmd_process_status *proc_status
)
{
	struct halmac_cfg_para_state_set *state =
			&adapter->halmac_state.cfg_para_state_set;

	*proc_status = state->process_status;

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_ch_switch_status_88xx(
	IN struct halmac_adapter *adapter,
	OUT enum halmac_cmd_process_status *proc_status
)
{
	struct halmac_scan_state_set *state =
			&adapter->halmac_state.scan_state_set;

	*proc_status = state->process_status;

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_update_packet_status_88xx(
	IN struct halmac_adapter *adapter,
	OUT enum halmac_cmd_process_status *proc_status
)
{
	struct halmac_update_packet_state_set *state =
			&adapter->halmac_state.update_packet_set;

	*proc_status = state->process_status;

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_cfg_drv_rsvd_pg_num_88xx() -config reserved page number for driver
 * @adapter : the adapter of halmac
 * @pg_num : page number
 * Author : KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_cfg_drv_rsvd_pg_num_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_drv_rsvd_pg_num pg_num
)
{
	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (adapter->api_registry.cfg_drv_rsvd_pg_en == 0)
		return HALMAC_RET_NOT_SUPPORT;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]pg_num = %d\n", pg_num);

	switch (pg_num) {
	case HALMAC_RSVD_PG_NUM8:
		adapter->txff_allocation.rsvd_drv_pg_num = 8;
		break;
	case HALMAC_RSVD_PG_NUM16:
		adapter->txff_allocation.rsvd_drv_pg_num = 16;
		break;
	case HALMAC_RSVD_PG_NUM24:
		adapter->txff_allocation.rsvd_drv_pg_num = 24;
		break;
	case HALMAC_RSVD_PG_NUM32:
		adapter->txff_allocation.rsvd_drv_pg_num = 32;
		break;
	case HALMAC_RSVD_PG_NUM64:
		adapter->txff_allocation.rsvd_drv_pg_num = 64;
		break;
	case HALMAC_RSVD_PG_NUM128:
		adapter->txff_allocation.rsvd_drv_pg_num = 128;
		break;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * (debug API)halmac_h2c_lb_88xx() - send h2c loopback packet
 * @adapter : the adapter of halmac
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_h2c_lb_88xx(
	IN struct halmac_adapter *adapter
)
{
	return HALMAC_RET_SUCCESS;
}

enum halmac_ret_status
halmac_pwr_seq_parser_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 cut,
	IN u8 fab,
	IN u8 intf,
	IN struct halmac_wlan_pwr_cfg **cmd_seq
)
{
	u32 idx = 0;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	struct halmac_wlan_pwr_cfg *cmd;

	do {
		cmd = cmd_seq[idx];

		if (!cmd)
			break;

		status = halmac_pwr_sub_seq_parer_88xx(
					adapter, cut, fab, intf, cmd);
		if (status != HALMAC_RET_SUCCESS) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]pwr sub seq!!\n");
			return status;
		}

		idx++;
	} while (1);

	return status;
}

static enum halmac_ret_status
halmac_pwr_sub_seq_parer_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 cut,
	IN u8 fab,
	IN u8 intf,
	IN struct halmac_wlan_pwr_cfg *cmd
)
{
	u8 value;
	u32 offset;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	do {
		if ((cmd->interface_msk & intf) && (cmd->fab_msk & fab) &&
		    (cmd->cut_msk & cut)) {
			switch (cmd->cmd) {
			case HALMAC_PWR_CMD_WRITE:
				if (cmd->base == HALMAC_PWR_BASEADDR_SDIO)
					offset = cmd->offset |
							SDIO_LOCAL_OFFSET;
				else
					offset = cmd->offset;

				value = HALMAC_REG_R8(offset);
				value = (u8)(value & (u8)(~(cmd->msk)));
				value = (u8)(value | (cmd->value & cmd->msk));

				HALMAC_REG_W8(offset, value);
				break;
			case HALMAC_PWR_CMD_POLLING:
				if (halmac_pwr_cmd_polling_88xx(adapter, cmd) !=
					HALMAC_RET_SUCCESS)
					return HALMAC_RET_PWRSEQ_POLLING_FAIL;
				break;
			case HALMAC_PWR_CMD_DELAY:
				if (cmd->value == HALMAC_PWRSEQ_DELAY_US)
					PLTFM_DELAY_US(cmd->offset);
				else
					PLTFM_DELAY_US(1000 * cmd->offset);
				break;
			case HALMAC_PWR_CMD_READ:
				break;
			case HALMAC_PWR_CMD_END:
				return HALMAC_RET_SUCCESS;
			default:
				return HALMAC_RET_PWRSEQ_CMD_INCORRECT;
			}
		}
		cmd++;
	} while (1);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_pwr_cmd_polling_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_wlan_pwr_cfg *cmd
)
{
	u8 value;
	u8 flg;
	u8 poll_bit;
	u32 offset;
	u32 cnt;
	static u32 stats;
	enum halmac_interface intf;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	poll_bit = 0;
	cnt = HALMAC_PWR_POLLING_CNT;
	flg = 0;
	intf = adapter->halmac_interface;

	if (cmd->base == HALMAC_PWR_BASEADDR_SDIO)
		offset = cmd->offset | SDIO_LOCAL_OFFSET;
	else
		offset = cmd->offset;

	do {
		cnt--;
		value = HALMAC_REG_R8(offset);
		value = (u8)(value & cmd->msk);

		if (value == (cmd->value & cmd->msk)) {
			poll_bit = 1;
		} else {
			if (cnt == 0) {
				if (intf == HALMAC_INTERFACE_PCIE && flg == 0) {
					/* PCIE + USB package */
					/* power bit polling timeout issue */
					stats++;
					PLTFM_MSG_PRINT(HALMAC_DBG_WARN,
							"[WARN]PCIE stats:%d\n",
							stats);
					value = HALMAC_REG_R8(REG_SYS_PW_CTRL);
					value |= BIT(3);
					HALMAC_REG_W8(REG_SYS_PW_CTRL, value);
					value &= ~BIT(3);
					HALMAC_REG_W8(REG_SYS_PW_CTRL, value);
					poll_bit = 0;
					cnt = HALMAC_PWR_POLLING_CNT;
					flg = 1;
				} else {
					PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
							"[ERR]polling to!!\n");
					PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
							"[ERR]cmd offset:%X\n",
							cmd->offset);
					PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
							"[ERR]cmd value:%X\n",
							cmd->value);
					PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
							"[ERR]cmd msk:%X\n",
							cmd->msk);
					PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
							"[ERR]offset = %X\n",
							offset);
					PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
							"[ERR]value = %X\n",
							value);
					return HALMAC_RET_PWRSEQ_POLLING_FAIL;
				}
			} else {
				PLTFM_DELAY_US(50);
			}
		}
	} while (!poll_bit);

	return HALMAC_RET_SUCCESS;
}

enum halmac_ret_status
halmac_parse_intf_phy_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_intf_phy_para *param,
	IN enum halmac_intf_phy_platform pltfm,
	IN enum hal_intf_phy intf_phy
)
{
	u16 value;
	u16 cur_cut;
	u16 offset;
	u16 ip_sel;
	struct halmac_intf_phy_para *cur_param;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	u8 result = HALMAC_RET_SUCCESS;

	switch (adapter->chip_version) {
	case HALMAC_CHIP_VER_A_CUT:
		cur_cut = (u16)HALMAC_INTF_PHY_CUT_A;
		break;
	case HALMAC_CHIP_VER_B_CUT:
		cur_cut = (u16)HALMAC_INTF_PHY_CUT_B;
		break;
	case HALMAC_CHIP_VER_C_CUT:
		cur_cut = (u16)HALMAC_INTF_PHY_CUT_C;
		break;
	case HALMAC_CHIP_VER_D_CUT:
		cur_cut = (u16)HALMAC_INTF_PHY_CUT_D;
		break;
	case HALMAC_CHIP_VER_E_CUT:
		cur_cut = (u16)HALMAC_INTF_PHY_CUT_E;
		break;
	case HALMAC_CHIP_VER_F_CUT:
		cur_cut = (u16)HALMAC_INTF_PHY_CUT_F;
		break;
	case HALMAC_CHIP_VER_TEST:
		cur_cut = (u16)HALMAC_INTF_PHY_CUT_TESTCHIP;
		break;
	default:
		return HALMAC_RET_FAIL;
	}

	cur_param = param;

	do {
		if ((cur_param->cut & cur_cut) &&
		    (cur_param->plaform & (u16)pltfm)) {
			offset =  cur_param->offset;
			value = cur_param->value;
			ip_sel = cur_param->ip_sel;

			if (offset == 0xFFFF)
				break;

			if (ip_sel == HALMAC_IP_SEL_MAC) {
				HALMAC_REG_W8((u32)offset, (u8)value);
			} else if (intf_phy == HAL_INTF_PHY_USB2) {
				result = halmac_usbphy_write_88xx(
						adapter, (u8)offset, value,
						HAL_INTF_PHY_USB2);
				if (result != HALMAC_RET_SUCCESS)
					PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
							"[ERR]usb2 phy!!\n");

			} else if (intf_phy == HAL_INTF_PHY_USB3) {
				result = halmac_usbphy_write_88xx(
						adapter, (u8)offset, value,
						HAL_INTF_PHY_USB3);
				if (result != HALMAC_RET_SUCCESS)
					PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
							"[ERR]usb3 phy!!\n");
			} else if (intf_phy == HAL_INTF_PHY_PCIE_GEN1) {
				if (ip_sel == HALMAC_IP_SEL_INTF_PHY)
					result = halmac_mdio_write_88xx(
						adapter, (u8)offset, value,
						HAL_INTF_PHY_PCIE_GEN1);
				else
					result = halmac_dbi_write8_88xx(
						adapter, offset, (u8)value);
				if (result != HALMAC_RET_SUCCESS)
					PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
							"[ERR]g1-mdio/dbi!!\n");

			} else if (intf_phy == HAL_INTF_PHY_PCIE_GEN2) {
				if (ip_sel == HALMAC_IP_SEL_INTF_PHY)
					result = halmac_mdio_write_88xx(
						adapter, (u8)offset, value,
						HAL_INTF_PHY_PCIE_GEN2);
				else
					result = halmac_dbi_write8_88xx(
						adapter, offset, (u8)value);
				if (result != HALMAC_RET_SUCCESS)
					PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
							"[ERR]g2-mdio/dbi!!\n");
			} else {
				PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
						"[ERR]intf phy sel!!\n");
			}
		}
		cur_param++;
	} while (1);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_txfifo_is_empty_88xx() -check if txfifo is empty
 * @adapter : the adapter of halmac
 * @chk_num : check number
 * Author : Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_txfifo_is_empty_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 chk_num
)
{
	u32 cnt;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	cnt = (chk_num <= 10) ? 10 : chk_num;
	do {
		if (HALMAC_REG_R8(REG_TXPKT_EMPTY) != 0xFF)
			return HALMAC_RET_TXFIFO_NO_EMPTY;

		if ((HALMAC_REG_R8(REG_TXPKT_EMPTY + 1) & 0x06) != 0x06)
			return HALMAC_RET_TXFIFO_NO_EMPTY;
		cnt--;

	} while (cnt != 0);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * (internal use)
 * halmac_adaptive_malloc_88xx() - adapt malloc size
 * @adapter : the adapter of halmac
 * @size : expected malloc size
 * @pNew_size : real malloc size
 * Author : Ivan Lin
 * Return : address pointer
 */
u8*
halmac_adaptive_malloc_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 size,
	OUT u32 *new_size
)
{
	u8 retry_num;
	u8 *malloc_buf = NULL;

	for (retry_num = 0; retry_num < 5; retry_num++) {
		malloc_buf = (u8 *)PLTFM_MALLOC(size);

		if (malloc_buf) {
			*new_size = size;
			return malloc_buf;
		}

		size = size >> 1;

		if (size == 0)
			break;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]adptive malloc!!\n");

	return NULL;
}

/**
 * (internal use)
 * halmac_ltecoex_reg_read_88xx() - read ltecoex register
 * @adapter : the adapter of halmac
 * @offset : offset
 * @pValue : value
 * Author : Ivan Lin
 * Return : enum halmac_ret_status
 */
enum halmac_ret_status
halmac_ltecoex_reg_read_88xx(
	IN struct halmac_adapter *adapter,
	IN u16 offset,
	OUT u32 *value
)
{
	u32 cnt;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	cnt = 10000;
	while ((HALMAC_REG_R8(REG_WL2LTECOEX_INDIRECT_ACCESS_CTRL_V1 + 3) &
	       BIT(5)) == 0) {
		if (cnt == 0) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]lte ready(R)\n");
			return HALMAC_RET_LTECOEX_READY_FAIL;
		}
		cnt--;
		PLTFM_DELAY_US(50);
	}

	HALMAC_REG_W32(REG_WL2LTECOEX_INDIRECT_ACCESS_CTRL_V1,
		       0x800F0000 | offset);
	*value = HALMAC_REG_R32(REG_WL2LTECOEX_INDIRECT_ACCESS_READ_DATA_V1);

	return HALMAC_RET_SUCCESS;
}

/**
 * (internal use)
 * halmac_ltecoex_reg_write_88xx() - write ltecoex register
 * @adapter : the adapter of halmac
 * @offset : offset
 * @value : value
 * Author : Ivan Lin
 * Return : enum halmac_ret_status
 */
enum halmac_ret_status
halmac_ltecoex_reg_write_88xx(
	IN struct halmac_adapter *adapter,
	IN u16 offset,
	IN u32 value
)
{
	u32 cnt;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	cnt = 10000;
	while ((HALMAC_REG_R8(REG_WL2LTECOEX_INDIRECT_ACCESS_CTRL_V1 + 3)
	       & BIT(5)) == 0) {
		if (cnt == 0) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]lte ready(W)\n");
			return HALMAC_RET_LTECOEX_READY_FAIL;
		}
		cnt--;
		PLTFM_DELAY_US(50);
	}

	HALMAC_REG_W32(REG_WL2LTECOEX_INDIRECT_ACCESS_WRITE_DATA_V1, value);
	HALMAC_REG_W32(REG_WL2LTECOEX_INDIRECT_ACCESS_CTRL_V1,
		       0xC00F0000 | offset);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_download_flash_88xx() -download firmware to flash
 * @adapter : the adapter of halmac
 * @fw_bin : pointer to fw
 * @size : fw size
 * @rom_addr : flash start address where fw should be download
 * Author : Pablo Chiu
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_download_flash_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *fw_bin,
	IN u32 size,
	IN u32 rom_addr
)
{
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	enum halmac_ret_status status;
	struct halmac_h2c_header_info hdr_info;
	u8 value8;
	u8 restore[3];
	u8 h2c_buf[HALMAC_H2C_CMD_SIZE_88XX] = {0};
	u16 seq_num = 0;
	u32 pkt_size;
	u32 mem_offset;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	value8 = HALMAC_DMA_MAPPING_HIGH << 6;
	HALMAC_REG_W8(REG_TXDMA_PQ_MAP + 1, value8);

	/* use HIQ, map HIQ to hi priority */
	adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_HI] =
						HALMAC_DMA_MAPPING_HIGH;
	value8 = BIT_HCI_TXDMA_EN | BIT_TXDMA_EN;
	HALMAC_REG_W8(REG_CR, value8);
	HALMAC_REG_W32(REG_H2CQ_CSR, BIT(31));

	/* Config hi priority queue and public priority queue page number */
	HALMAC_REG_W16(REG_FIFOPAGE_INFO_1, 0x200);
	HALMAC_REG_W32(REG_RQPN_CTRL_2,
		       HALMAC_REG_R32(REG_RQPN_CTRL_2) | BIT(31));

	if (adapter->halmac_interface == HALMAC_INTERFACE_SDIO) {
		HALMAC_REG_R32(REG_SDIO_FREE_TXPG);
		HALMAC_REG_W32(REG_SDIO_TX_CTRL,
			       HALMAC_REG_R32(REG_SDIO_TX_CTRL));
	}

	value8 = HALMAC_REG_R8(REG_CR + 1);
	restore[0] = value8;
	value8 = (u8)(value8 | BIT(0));
	HALMAC_REG_W8(REG_CR + 1, value8);

	value8 = HALMAC_REG_R8(REG_BCN_CTRL);
	restore[1] = value8;
	value8 = (u8)((value8 & ~(BIT(3))) | BIT(4));
	HALMAC_REG_W8(REG_BCN_CTRL, value8);

	value8 = HALMAC_REG_R8(REG_FWHW_TXQ_CTRL + 2);
	restore[2] = value8;
	value8 = (u8)(value8 & ~(BIT(6)));
	HALMAC_REG_W8(REG_FWHW_TXQ_CTRL + 2, value8);

	/* Download FW to Flash flow */
	mem_offset = 0;

	while (size != 0) {
		if (size >= (HALMAC_EXTRA_INFO_BUFF_SIZE_88XX - 48))
			pkt_size = HALMAC_EXTRA_INFO_BUFF_SIZE_88XX - 48;
		else
			pkt_size = size;

		status = halmac_download_rsvd_page_88xx(
			adapter,
			adapter->txff_allocation.rsvd_h2c_extra_info_pg_bndy,
			fw_bin + mem_offset, pkt_size);
		if (status != HALMAC_RET_SUCCESS) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]dl rsvd pg!!\n");
			return status;
		}

		DOWNLOAD_FLASH_SET_SPI_CMD(h2c_buf, 0x02);
		DOWNLOAD_FLASH_SET_LOCATION(
			h2c_buf,
			adapter->txff_allocation.rsvd_h2c_extra_info_pg_bndy -
			adapter->txff_allocation.rsvd_pg_bndy);
		DOWNLOAD_FLASH_SET_SIZE(h2c_buf, pkt_size);
		DOWNLOAD_FLASH_SET_START_ADDR(h2c_buf, rom_addr);

		hdr_info.sub_cmd_id = SUB_CMD_ID_DOWNLOAD_FLASH;
		hdr_info.content_size = 20;
		hdr_info.ack = _TRUE;
		halmac_set_h2c_pkt_hdr_88xx(adapter, h2c_buf, &hdr_info,
					    &seq_num);

		status = halmac_send_h2c_pkt_88xx(adapter, h2c_buf,
						  HALMAC_H2C_CMD_SIZE_88XX);
		if (status != HALMAC_RET_SUCCESS) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]send h2c!!\n");
			return status;
		}

		value8 = HALMAC_REG_R8(REG_MCUTST_I);
		value8 |= BIT(0);
		HALMAC_REG_W8(REG_MCUTST_I, value8);

		rom_addr += pkt_size;
		mem_offset += pkt_size;
		size -= pkt_size;

		while (((HALMAC_REG_R8(REG_MCUTST_I)) & BIT(0)) != 0)
			PLTFM_DELAY_US(1000);

		if (((HALMAC_REG_R8(REG_MCUTST_I)) & BIT(0)) != 0) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]dl flash!!\n");
			return  HALMAC_RET_DLFW_FAIL;
		}
	}

	HALMAC_REG_W8(REG_FWHW_TXQ_CTRL + 2, restore[2]);
	HALMAC_REG_W8(REG_BCN_CTRL, restore[1]);
	HALMAC_REG_W8(REG_CR + 1, restore[0]);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_read_flash_88xx() -read data from flash
 * @adapter : the adapter of halmac
 * @addr : flash start address where fw should be read
 * Author : Pablo Chiu
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_read_flash_88xx(
	IN struct halmac_adapter *adapter,
	u32 addr
)
{
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	enum halmac_ret_status status;
	struct halmac_h2c_header_info hdr_info;
	u8 value8;
	u8 restore[3];
	u8 h2c_buf[HALMAC_H2C_CMD_SIZE_88XX] = {0};
	u16 seq_num = 0;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	value8 = HALMAC_DMA_MAPPING_HIGH << 6;
	HALMAC_REG_W8(REG_TXDMA_PQ_MAP + 1, value8);

	/* DLFW only use HIQ, map HIQ to hi priority */
	adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_HI] =
						HALMAC_DMA_MAPPING_HIGH;
	value8 = BIT_HCI_TXDMA_EN | BIT_TXDMA_EN;
	HALMAC_REG_W8(REG_CR, value8);
	HALMAC_REG_W32(REG_H2CQ_CSR, BIT(31));

	/* Config hi priority queue and public priority queue page number */
	HALMAC_REG_W16(REG_FIFOPAGE_INFO_1, 0x200);
	HALMAC_REG_W32(REG_RQPN_CTRL_2,
		       HALMAC_REG_R32(REG_RQPN_CTRL_2) | BIT(31));

	if (adapter->halmac_interface == HALMAC_INTERFACE_SDIO) {
		HALMAC_REG_R32(REG_SDIO_FREE_TXPG);
		HALMAC_REG_W32(REG_SDIO_TX_CTRL,
			       HALMAC_REG_R32(REG_SDIO_TX_CTRL));
	}

	value8 = HALMAC_REG_R8(REG_CR + 1);
	restore[0] = value8;
	value8 = (u8)(value8 | BIT(0));
	HALMAC_REG_W8(REG_CR + 1, value8);

	value8 = HALMAC_REG_R8(REG_BCN_CTRL);
	restore[1] = value8;
	value8 = (u8)((value8 & ~(BIT(3))) | BIT(4));
	HALMAC_REG_W8(REG_BCN_CTRL, value8);

	value8 = HALMAC_REG_R8(REG_FWHW_TXQ_CTRL + 2);
	restore[2] = value8;
	value8 = (u8)(value8 & ~(BIT(6)));
	HALMAC_REG_W8(REG_FWHW_TXQ_CTRL + 2, value8);

	HALMAC_REG_W16(
		REG_FIFOPAGE_CTRL_2,
		(u16)(adapter->txff_allocation.rsvd_h2c_extra_info_pg_bndy &
		BIT_MASK_BCN_HEAD_1_V1));
	value8 = HALMAC_REG_R8(REG_MCUTST_I);
	value8 |= BIT(0);
	HALMAC_REG_W8(REG_MCUTST_I, value8);

	/* Construct H2C Content */
	DOWNLOAD_FLASH_SET_SPI_CMD(h2c_buf, 0x03);
	DOWNLOAD_FLASH_SET_LOCATION(
		h2c_buf,
		adapter->txff_allocation.rsvd_h2c_extra_info_pg_bndy -
		adapter->txff_allocation.rsvd_pg_bndy);
	DOWNLOAD_FLASH_SET_SIZE(h2c_buf, 4096);
	DOWNLOAD_FLASH_SET_START_ADDR(h2c_buf, addr);

	/* Fill in H2C Header */
	hdr_info.sub_cmd_id = SUB_CMD_ID_DOWNLOAD_FLASH;
	hdr_info.content_size = 16;
	hdr_info.ack = _TRUE;
	halmac_set_h2c_pkt_hdr_88xx(adapter, h2c_buf, &hdr_info, &seq_num);

	/* Send H2C Cmd Packet */
	status = halmac_send_h2c_pkt_88xx(adapter, h2c_buf,
					  HALMAC_H2C_CMD_SIZE_88XX);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]send h2c!!\n");
		return status;
	}

	while (((HALMAC_REG_R8(REG_MCUTST_I)) & BIT(0)) != 0)
		PLTFM_DELAY_US(1000);

	HALMAC_REG_W16(REG_FIFOPAGE_CTRL_2,
		       (u16)(adapter->txff_allocation.rsvd_pg_bndy &
		       BIT_MASK_BCN_HEAD_1_V1));
	HALMAC_REG_W8(REG_FWHW_TXQ_CTRL + 2, restore[2]);
	HALMAC_REG_W8(REG_BCN_CTRL, restore[1]);
	HALMAC_REG_W8(REG_CR + 1, restore[0]);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_erase_flash_88xx() -erase flash data
 * @adapter : the adapter of halmac
 * @erase_cmd : erase command
 * @addr : flash start address where fw should be erased
 * Author : Pablo Chiu
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_erase_flash_88xx(
	IN struct halmac_adapter *adapter,
	u8 erase_cmd,
	u32 addr
)
{
	enum halmac_ret_status status;
	struct halmac_h2c_header_info hdr_info;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	u8 value8;
	u8 h2c_buf[HALMAC_H2C_CMD_SIZE_88XX] = {0};
	u16 seq_num = 0;
	u32 cnt;

	/* Construct H2C Content */
	DOWNLOAD_FLASH_SET_SPI_CMD(h2c_buf, erase_cmd);
	DOWNLOAD_FLASH_SET_LOCATION(h2c_buf, 0);
	DOWNLOAD_FLASH_SET_START_ADDR(h2c_buf, addr);
	DOWNLOAD_FLASH_SET_SIZE(h2c_buf, 0);

	value8 = HALMAC_REG_R8(REG_MCUTST_I);
	value8 |= BIT(0);
	HALMAC_REG_W8(REG_MCUTST_I, value8);

	/* Fill in H2C Header */
	hdr_info.sub_cmd_id = SUB_CMD_ID_DOWNLOAD_FLASH;
	hdr_info.content_size = 16;
	hdr_info.ack = _TRUE;
	halmac_set_h2c_pkt_hdr_88xx(adapter, h2c_buf, &hdr_info, &seq_num);

	/* Send H2C Cmd Packet */
	status = halmac_send_h2c_pkt_88xx(adapter, h2c_buf,
					  HALMAC_H2C_CMD_SIZE_88XX);
	if (status != HALMAC_RET_SUCCESS)
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]send h2c!!\n");

	cnt = 5000;
	while (((HALMAC_REG_R8(REG_MCUTST_I)) & BIT(0)) != 0 && cnt != 0) {
		PLTFM_DELAY_US(1000);
		cnt--;
	}

	if (cnt == 0)
		return HALMAC_RET_FAIL;
	else
		return HALMAC_RET_SUCCESS;
}

/**
 * halmac_check_flash_88xx() -check flash data
 * @adapter : the adapter of halmac
 * @fw_bin : pointer to fw
 * @size : fw size
 * @addr : flash start address where fw should be checked
 * Author : Pablo Chiu
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_check_flash_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *fw_bin,
	IN u32 size,
	IN u32 addr
)
{
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	u8 value8;
	u16 value16;
	u16 residue;
	u16 pg_addr;
	u32 pkt_size;
	u32 start_page;
	u32 cnt;

	pg_addr = adapter->txff_allocation.rsvd_h2c_extra_info_pg_bndy;

	while (size != 0) {
		start_page = ((pg_addr << 7) >> 12) + 0x780;
		residue = (pg_addr << 7) & (4096 - 1);

		if (size >= HALMAC_EXTRA_INFO_BUFF_SIZE_88XX)
			pkt_size = HALMAC_EXTRA_INFO_BUFF_SIZE_88XX;
		else
			pkt_size = size;

		halmac_read_flash_88xx(adapter, addr);

		value16 = HALMAC_REG_R16(REG_PKTBUF_DBG_CTRL);
		cnt = 0;
		while (cnt < pkt_size) {
			HALMAC_REG_W16(REG_PKTBUF_DBG_CTRL,
				       (u16)(start_page | (value16 & 0xF000)));
			for (value16 = 0x8000 + residue; value16 <= 0x8FFF;
			     value16++) {
				value8 = HALMAC_REG_R8(value16);
				if (*fw_bin != value8) {
					PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
							"[ERR]check flash!!\n");
					return HALMAC_RET_FAIL;
				}

				fw_bin++;
				cnt++;
				if (cnt == pkt_size)
					break;
			}
			residue = 0;
			start_page++;
		}
		addr += pkt_size;
		size -= pkt_size;
	}

	return HALMAC_RET_SUCCESS;
}

static void
halmac_power_state_88xx(
	IN struct halmac_adapter *adapter,
	OUT enum halmac_mac_power *state
)
{
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if ((HALMAC_REG_R8(REG_SYS_FUNC_EN + 1) & BIT(3)) == 0)
		*state = HALMAC_MAC_POWER_OFF;
	else
		*state = HALMAC_MAC_POWER_ON;
}

#endif /* HALMAC_88XX_SUPPORT */
