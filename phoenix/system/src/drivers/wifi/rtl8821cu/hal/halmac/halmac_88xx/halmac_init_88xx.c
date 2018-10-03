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

#include "halmac_init_88xx.h"
#include "halmac_88xx_cfg.h"
#include "halmac_fw_88xx.h"
#include "halmac_common_88xx.h"
#include "halmac_cfg_wmac_88xx.h"
#include "halmac_efuse_88xx.h"
#include "halmac_mimo_88xx.h"
#include "halmac_bb_rf_88xx.h"
#include "halmac_sdio_88xx.h"
#include "halmac_usb_88xx.h"
#include "halmac_pcie_88xx.h"
#include "halmac_gpio_88xx.h"

#if HALMAC_8822B_SUPPORT
#include "halmac_8822b/halmac_init_8822b.h"
#endif

#if HALMAC_8821C_SUPPORT
#include "halmac_8821c/halmac_init_8821c.h"
#endif

#if HALMAC_8822C_SUPPORT
#include "halmac_8822c/halmac_init_8822c.h"
#endif

#if HALMAC_PLATFORM_TESTPROGRAM
#include "halmisc_api_88xx.h"
#endif

#if HALMAC_88XX_SUPPORT

static void
halmac_init_state_machine_88xx(
	IN struct halmac_adapter *adapter
);

static enum halmac_ret_status
halmac_verify_io_88xx(
	IN struct halmac_adapter *adapter
);

static enum halmac_ret_status
halmac_verify_send_rsvd_page_88xx(
	IN struct halmac_adapter *adapter
);

void
halmac_init_adapter_para_88xx(
	IN struct halmac_adapter *adapter
)
{
	adapter->api_registry.rx_expand_mode_en = 1;
	adapter->api_registry.la_mode_en = 1;
	adapter->api_registry.cfg_drv_rsvd_pg_en = 1;
	adapter->api_registry.sdio_cmd53_4byte_en = 1;

	adapter->pHalAdapter_backup = adapter;
	adapter->efuse_map = (u8 *)NULL;
	adapter->hal_efuse_map_valid = _FALSE;
	adapter->efuse_end = 0;
	adapter->pHal_mac_addr[0].Address_L_H.Address_Low = 0;
	adapter->pHal_mac_addr[0].Address_L_H.Address_High = 0;
	adapter->pHal_mac_addr[1].Address_L_H.Address_Low = 0;
	adapter->pHal_mac_addr[1].Address_L_H.Address_High = 0;
	adapter->pHal_bss_addr[0].Address_L_H.Address_Low = 0;
	adapter->pHal_bss_addr[0].Address_L_H.Address_High = 0;
	adapter->pHal_bss_addr[1].Address_L_H.Address_Low = 0;
	adapter->pHal_bss_addr[1].Address_L_H.Address_High = 0;

	adapter->low_clk = _FALSE;
	adapter->max_download_size = HALMAC_FW_MAX_DL_SIZE_88XX;
	adapter->ofld_func_info.halmac_malloc_max_sz = HALMAC_OFLD_FUNC_MALLOC_MAX_SIZE_88XX;
	adapter->ofld_func_info.rsvd_pg_drv_buf_max_sz = HALMAC_OFLD_FUNC_RSVD_PG_DRV_BUF_MAX_SIZE_88XX;

	adapter->config_para_info.pCfg_para_buf = NULL;
	adapter->config_para_info.pPara_buf_w = NULL;
	adapter->config_para_info.para_num = 0;
	adapter->config_para_info.full_fifo_mode = _FALSE;
	adapter->config_para_info.para_buf_size = 0;
	adapter->config_para_info.avai_para_buf_size = 0;
	adapter->config_para_info.offset_accumulation = 0;
	adapter->config_para_info.value_accumulation = 0;
	adapter->config_para_info.datapack_segment = 0;

	adapter->ch_sw_info.ch_info_buf = NULL;
	adapter->ch_sw_info.ch_info_buf_w = NULL;
	adapter->ch_sw_info.extra_info_en = 0;
	adapter->ch_sw_info.buf_size = 0;
	adapter->ch_sw_info.avai_buf_size = 0;
	adapter->ch_sw_info.total_size = 0;
	adapter->ch_sw_info.ch_num = 0;

	adapter->drv_info_size = 0;
	adapter->tx_desc_transfer = _FALSE;

	adapter->txff_allocation.tx_fifo_pg_num = 0;
	adapter->txff_allocation.ac_q_pg_num = 0;
	adapter->txff_allocation.rsvd_pg_bndy = 0;
	adapter->txff_allocation.rsvd_drv_pg_bndy = 0;
	adapter->txff_allocation.rsvd_h2c_extra_info_pg_bndy = 0;
	adapter->txff_allocation.rsvd_h2c_queue_pg_bndy = 0;
	adapter->txff_allocation.rsvd_cpu_instr_pg_bndy = 0;
	adapter->txff_allocation.rsvd_fw_txbuff_pg_bndy = 0;
	adapter->txff_allocation.pub_queue_pg_num = 0;
	adapter->txff_allocation.high_queue_pg_num = 0;
	adapter->txff_allocation.low_queue_pg_num = 0;
	adapter->txff_allocation.normal_queue_pg_num = 0;
	adapter->txff_allocation.extra_queue_pg_num = 0;

	adapter->txff_allocation.la_mode = HALMAC_LA_MODE_DISABLE;
	adapter->txff_allocation.rx_fifo_expanding_mode = HALMAC_RX_FIFO_EXPANDING_MODE_DISABLE;

	adapter->hw_cfg_info.security_check_keyid = 0;
	adapter->hw_cfg_info.ac_queue_num = 8;

	adapter->sdio_cmd53_4byte = HALMAC_SDIO_CMD53_4BYTE_MODE_DISABLE;
	adapter->sdio_hw_info.io_hi_speed_flag = 0;
	adapter->sdio_hw_info.spec_ver = HALMAC_SDIO_SPEC_VER_2_00;
	adapter->sdio_hw_info.clock_speed = 50;
	adapter->sdio_hw_info.io_wait_ready_flag = 0;
	adapter->sdio_hw_info.block_size = 512;
	adapter->sdio_free_space.macid_map = (u8 *)NULL;

	adapter->pinmux_info.wl_led = 0;
	adapter->pinmux_info.sdio_int = 0;
	adapter->pinmux_info.sw_io_0 = 0;
	adapter->pinmux_info.sw_io_1 = 0;
	adapter->pinmux_info.sw_io_2 = 0;
	adapter->pinmux_info.sw_io_3 = 0;
	adapter->pinmux_info.sw_io_4 = 0;
	adapter->pinmux_info.sw_io_5 = 0;
	adapter->pinmux_info.sw_io_6 = 0;
	adapter->pinmux_info.sw_io_7 = 0;
	adapter->pinmux_info.sw_io_8 = 0;
	adapter->pinmux_info.sw_io_9 = 0;
	adapter->pinmux_info.sw_io_10 = 0;
	adapter->pinmux_info.sw_io_11 = 0;
	adapter->pinmux_info.sw_io_12 = 0;
	adapter->pinmux_info.sw_io_13 = 0;
	adapter->pinmux_info.sw_io_14 = 0;
	adapter->pinmux_info.sw_io_15 = 0;

	halmac_init_adapter_dynamic_para_88xx(adapter);
	halmac_init_state_machine_88xx(adapter);
}

void
halmac_init_adapter_dynamic_para_88xx(
	IN struct halmac_adapter *adapter
)
{
	adapter->h2c_packet_seq = 0;
	adapter->h2c_buf_free_space = 0;
}

enum halmac_ret_status
halmac_mount_api_88xx(
	IN struct halmac_adapter *adapter
)
{
	struct halmac_api *api = NULL;

	adapter->pHalmac_api = (struct halmac_api *)PLTFM_MALLOC(sizeof(struct halmac_api));
	if (!adapter->pHalmac_api)
		return HALMAC_RET_MALLOC_FAIL;

	api = (struct halmac_api *)adapter->pHalmac_api;

	PLTFM_MSG_PRINT(HALMAC_DBG_ALWAYS, HALMAC_SVN_VER_88XX "\n");
	PLTFM_MSG_PRINT(HALMAC_DBG_ALWAYS, "HALMAC_MAJOR_VER_88XX = %x\n", HALMAC_MAJOR_VER_88XX);
	PLTFM_MSG_PRINT(HALMAC_DBG_ALWAYS, "HALMAC_PROTOTYPE_88XX = %x\n", HALMAC_PROTOTYPE_VER_88XX);
	PLTFM_MSG_PRINT(HALMAC_DBG_ALWAYS, "HALMAC_MINOR_VER_88XX = %x\n", HALMAC_MINOR_VER_88XX);
	PLTFM_MSG_PRINT(HALMAC_DBG_ALWAYS, "HALMAC_PATCH_VER_88XX = %x\n", HALMAC_PATCH_VER_88XX);

	/* Mount function pointer */
	api->halmac_register_api = halmac_register_api_88xx;
	api->halmac_download_firmware = halmac_download_firmware_88xx;
	api->halmac_free_download_firmware = halmac_free_download_firmware_88xx;
	api->halmac_get_fw_version = halmac_get_fw_version_88xx;
	api->halmac_cfg_mac_addr = halmac_cfg_mac_addr_88xx;
	api->halmac_cfg_bssid = halmac_cfg_bssid_88xx;
	api->halmac_cfg_transmitter_addr = halmac_cfg_transmitter_addr_88xx;
	api->halmac_cfg_net_type = halmac_cfg_net_type_88xx;
	api->halmac_cfg_tsf_rst = halmac_cfg_tsf_rst_88xx;
	api->halmac_cfg_bcn_space = halmac_cfg_bcn_space_88xx;
	api->halmac_rw_bcn_ctrl = halmac_rw_bcn_ctrl_88xx;
	api->halmac_cfg_multicast_addr = halmac_cfg_multicast_addr_88xx;
	api->halmac_pre_init_system_cfg = halmac_pre_init_system_cfg_88xx;
	api->halmac_init_system_cfg = halmac_init_system_cfg_88xx;
	api->halmac_init_edca_cfg = halmac_init_edca_cfg_88xx;
	api->halmac_cfg_operation_mode = halmac_cfg_operation_mode_88xx;
	api->halmac_cfg_ch_bw = halmac_cfg_ch_bw_88xx;
	api->halmac_cfg_bw = halmac_cfg_bw_88xx;
	api->halmac_init_wmac_cfg = halmac_init_wmac_cfg_88xx;
	api->halmac_init_mac_cfg = halmac_init_mac_cfg_88xx;
	api->halmac_dump_efuse_map = halmac_dump_efuse_map_88xx;
	api->halmac_dump_efuse_map_bt = halmac_dump_efuse_map_bt_88xx;
	api->halmac_write_efuse_bt = halmac_write_efuse_bt_88xx;
	api->halmac_read_efuse_bt = halmac_read_efuse_bt_88xx;
	api->halmac_cfg_efuse_auto_check = halmac_cfg_efuse_auto_check_88xx;
	api->halmac_dump_logical_efuse_map = halmac_dump_logical_efuse_map_88xx;
	api->halmac_pg_efuse_by_map = halmac_pg_efuse_by_map_88xx;
	api->halmac_mask_logical_efuse = halmac_mask_logical_efuse_88xx;
	api->halmac_get_efuse_size = halmac_get_efuse_size_88xx;
	api->halmac_get_efuse_available_size = halmac_get_efuse_available_size_88xx;
	api->halmac_get_c2h_info = halmac_get_c2h_info_88xx;

	api->halmac_get_logical_efuse_size = halmac_get_logical_efuse_size_88xx;

	api->halmac_write_logical_efuse = halmac_write_logical_efuse_88xx;
	api->halmac_read_logical_efuse = halmac_read_logical_efuse_88xx;

	api->halmac_ofld_func_cfg = halmac_ofld_func_cfg_88xx;
	api->halmac_h2c_lb = halmac_h2c_lb_88xx;
	api->halmac_debug = halmac_debug_88xx;
	api->halmac_cfg_parameter = halmac_cfg_parameter_88xx;
	api->halmac_update_datapack = halmac_update_datapack_88xx;
	api->halmac_run_datapack = halmac_run_datapack_88xx;
	api->halmac_send_bt_coex = halmac_send_bt_coex_88xx;
	api->halmac_verify_platform_api = halmac_verify_platform_api_88xx;
	api->halmac_update_packet = halmac_update_packet_88xx;
	api->halmac_bcn_ie_filter = halmac_bcn_ie_filter_88xx;
	api->halmac_cfg_txbf = halmac_cfg_txbf_88xx;
	api->halmac_cfg_mumimo = halmac_cfg_mumimo_88xx;
	api->halmac_cfg_sounding = halmac_cfg_sounding_88xx;
	api->halmac_del_sounding = halmac_del_sounding_88xx;
	api->halmac_su_bfer_entry_init = halmac_su_bfer_entry_init_88xx;
	api->halmac_su_bfee_entry_init = halmac_su_bfee_entry_init_88xx;
	api->halmac_mu_bfer_entry_init = halmac_mu_bfer_entry_init_88xx;
	api->halmac_mu_bfee_entry_init = halmac_mu_bfee_entry_init_88xx;
	api->halmac_su_bfer_entry_del = halmac_su_bfer_entry_del_88xx;
	api->halmac_su_bfee_entry_del = halmac_su_bfee_entry_del_88xx;
	api->halmac_mu_bfer_entry_del = halmac_mu_bfer_entry_del_88xx;
	api->halmac_mu_bfee_entry_del = halmac_mu_bfee_entry_del_88xx;

	api->halmac_add_ch_info = halmac_add_ch_info_88xx;
	api->halmac_add_extra_ch_info = halmac_add_extra_ch_info_88xx;
	api->halmac_ctrl_ch_switch = halmac_ctrl_ch_switch_88xx;
	api->halmac_p2pps = halmac_p2pps_88xx;
	api->halmac_clear_ch_info = halmac_clear_ch_info_88xx;
	api->halmac_send_general_info = halmac_send_general_info_88xx;

	api->halmac_start_iqk = halmac_start_iqk_88xx;
	api->halmac_ctrl_pwr_tracking = halmac_ctrl_pwr_tracking_88xx;
	api->halmac_psd = halmac_psd_88xx;
	api->halmac_cfg_la_mode = halmac_cfg_la_mode_88xx;
	api->halmac_cfg_rx_fifo_expanding_mode = halmac_cfg_rx_fifo_expanding_mode_88xx;

	api->halmac_config_security = halmac_config_security_88xx;
	api->halmac_get_used_cam_entry_num = halmac_get_used_cam_entry_num_88xx;
	api->halmac_read_cam_entry = halmac_read_cam_entry_88xx;
	api->halmac_write_cam = halmac_write_cam_88xx;
	api->halmac_clear_cam_entry = halmac_clear_cam_entry_88xx;

	api->halmac_cfg_drv_rsvd_pg_num = halmac_cfg_drv_rsvd_pg_num_88xx;
	api->halmac_get_chip_version = halmac_get_version_88xx;

	api->halmac_query_status = halmac_query_status_88xx;
	api->halmac_reset_feature = halmac_reset_feature_88xx;
	api->halmac_check_fw_status = halmac_check_fw_status_88xx;
	api->halmac_dump_fw_dmem = halmac_dump_fw_dmem_88xx;
	api->halmac_cfg_max_dl_size = halmac_cfg_max_dl_size_88xx;

	api->halmac_dump_fifo = halmac_dump_fifo_88xx;
	api->halmac_get_fifo_size = halmac_get_fifo_size_88xx;

	api->halmac_chk_txdesc = halmac_chk_txdesc_88xx;
	api->halmac_dl_drv_rsvd_page = halmac_dl_drv_rsvd_page_88xx;
	api->halmac_cfg_csi_rate = halmac_cfg_csi_rate_88xx;

	api->halmac_sdio_cmd53_4byte = halmac_sdio_cmd53_4byte_88xx;
	api->halmac_sdio_hw_info = halmac_sdio_hw_info_88xx;

	api->halmac_init_sdio_cfg = halmac_init_sdio_cfg_88xx;
	api->halmac_init_usb_cfg = halmac_init_usb_cfg_88xx;
	api->halmac_init_pcie_cfg = halmac_init_pcie_cfg_88xx;
	api->halmac_deinit_sdio_cfg = halmac_deinit_sdio_cfg_88xx;
	api->halmac_deinit_usb_cfg = halmac_deinit_usb_cfg_88xx;
	api->halmac_deinit_pcie_cfg = halmac_deinit_pcie_cfg_88xx;
	api->halmac_txfifo_is_empty = halmac_txfifo_is_empty_88xx;
	api->halmac_download_flash = halmac_download_flash_88xx;
	api->halmac_read_flash = halmac_read_flash_88xx;
	api->halmac_erase_flash = halmac_erase_flash_88xx;
	api->halmac_check_flash = halmac_check_flash_88xx;
	api->halmac_cfg_edca_para = halmac_cfg_edca_para_88xx;
	api->halmac_pinmux_wl_led_mode = halmac_pinmux_wl_led_mode_88xx;
	api->halmac_pinmux_wl_led_sw_ctrl = halmac_pinmux_wl_led_sw_ctrl_88xx;
	api->halmac_pinmux_sdio_int_polarity = halmac_pinmux_sdio_int_polarity_88xx;
	api->halmac_pinmux_gpio_mode = halmac_pinmux_gpio_mode_88xx;
	api->halmac_pinmux_gpio_output = halmac_pinmux_gpio_output_88xx;
	api->halmac_pinmux_pin_status = halmac_pinmux_pin_status_88xx;

	api->halmac_rx_cut_amsdu_cfg = halmac_rx_cut_amsdu_cfg_88xx;
	api->halmac_fw_snding = halmac_fw_snding_88xx;
	api->halmac_get_mac_addr = halmac_get_mac_addr_88xx;

	api->halmac_enter_cpu_sleep_mode = halmac_enter_cpu_sleep_mode_88xx;
	api->halmac_get_cpu_mode = halmac_get_cpu_mode_88xx;

	if (adapter->halmac_interface == HALMAC_INTERFACE_SDIO) {
		api->halmac_cfg_rx_aggregation = halmac_cfg_rx_aggregation_88xx_sdio;
		api->halmac_init_interface_cfg = halmac_init_sdio_cfg_88xx;
		api->halmac_deinit_interface_cfg = halmac_deinit_sdio_cfg_88xx;
		api->halmac_cfg_tx_agg_align = halmac_cfg_tx_agg_align_sdio_88xx;
		api->halmac_set_bulkout_num = halmac_set_bulkout_num_sdio_88xx;
		api->halmac_get_usb_bulkout_id = halmac_get_usb_bulkout_id_sdio_88xx;
		api->halmac_reg_read_8 = halmac_reg_read_8_sdio_88xx;
		api->halmac_reg_write_8 = halmac_reg_write_8_sdio_88xx;
		api->halmac_reg_read_16 = halmac_reg_read_16_sdio_88xx;
		api->halmac_reg_write_16 = halmac_reg_write_16_sdio_88xx;
		api->halmac_reg_read_32 = halmac_reg_read_32_sdio_88xx;
		api->halmac_reg_write_32 = halmac_reg_write_32_sdio_88xx;
		api->halmac_reg_read_indirect_32 = halmac_reg_read_indirect_32_sdio_88xx;
		api->halmac_reg_sdio_cmd53_read_n = halmac_reg_read_nbyte_sdio_88xx;
	} else if (adapter->halmac_interface == HALMAC_INTERFACE_USB) {
		api->halmac_cfg_rx_aggregation = halmac_cfg_rx_aggregation_88xx_usb;
		api->halmac_init_interface_cfg = halmac_init_usb_cfg_88xx;
		api->halmac_deinit_interface_cfg = halmac_deinit_usb_cfg_88xx;
		api->halmac_cfg_tx_agg_align = halmac_cfg_tx_agg_align_usb_88xx;
		api->halmac_tx_allowed_sdio = halmac_tx_allowed_usb_88xx;
		api->halmac_set_bulkout_num = halmac_set_bulkout_num_usb_88xx;
		api->halmac_get_sdio_tx_addr = halmac_get_sdio_tx_addr_usb_88xx;
		api->halmac_get_usb_bulkout_id = halmac_get_usb_bulkout_id_usb_88xx;
		api->halmac_reg_read_8 = halmac_reg_read_8_usb_88xx;
		api->halmac_reg_write_8 = halmac_reg_write_8_usb_88xx;
		api->halmac_reg_read_16 = halmac_reg_read_16_usb_88xx;
		api->halmac_reg_write_16 = halmac_reg_write_16_usb_88xx;
		api->halmac_reg_read_32 = halmac_reg_read_32_usb_88xx;
		api->halmac_reg_write_32 = halmac_reg_write_32_usb_88xx;
		api->halmac_reg_read_indirect_32 = halmac_reg_read_indirect_32_usb_88xx;
		api->halmac_reg_sdio_cmd53_read_n = halmac_reg_read_nbyte_usb_88xx;
	} else if (adapter->halmac_interface == HALMAC_INTERFACE_PCIE) {
		api->halmac_cfg_rx_aggregation = halmac_cfg_rx_aggregation_88xx_pcie;
		api->halmac_init_interface_cfg = halmac_init_pcie_cfg_88xx;
		api->halmac_deinit_interface_cfg = halmac_deinit_pcie_cfg_88xx;
		api->halmac_cfg_tx_agg_align = halmac_cfg_tx_agg_align_pcie_88xx;
		api->halmac_tx_allowed_sdio = halmac_tx_allowed_pcie_88xx;
		api->halmac_set_bulkout_num = halmac_set_bulkout_num_pcie_88xx;
		api->halmac_get_sdio_tx_addr = halmac_get_sdio_tx_addr_pcie_88xx;
		api->halmac_get_usb_bulkout_id = halmac_get_usb_bulkout_id_pcie_88xx;
		api->halmac_reg_read_8 = halmac_reg_read_8_pcie_88xx;
		api->halmac_reg_write_8 = halmac_reg_write_8_pcie_88xx;
		api->halmac_reg_read_16 = halmac_reg_read_16_pcie_88xx;
		api->halmac_reg_write_16 = halmac_reg_write_16_pcie_88xx;
		api->halmac_reg_read_32 = halmac_reg_read_32_pcie_88xx;
		api->halmac_reg_write_32 = halmac_reg_write_32_pcie_88xx;
		api->halmac_reg_read_indirect_32 = halmac_reg_read_indirect_32_pcie_88xx;
		api->halmac_reg_sdio_cmd53_read_n = halmac_reg_read_nbyte_pcie_88xx;
	} else {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Set halmac io function Error!!\n");
	}

	if (adapter->chip_id == HALMAC_CHIP_ID_8822B) {
#if HALMAC_8822B_SUPPORT
		halmac_mount_api_8822b(adapter);
#endif
	} else if (adapter->chip_id == HALMAC_CHIP_ID_8821C) {
#if HALMAC_8821C_SUPPORT
		halmac_mount_api_8821c(adapter);
#endif
	} else if (adapter->chip_id == HALMAC_CHIP_ID_8822C) {
#if HALMAC_8822C_SUPPORT
		halmac_mount_api_8822c(adapter);
#endif
	} else {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Chip ID undefine!!\n");
		return HALMAC_RET_CHIP_NOT_SUPPORT;
	}

#if HALMAC_PLATFORM_TESTPROGRAM
	halmac_mount_misc_api_88xx(adapter);
#endif

	return HALMAC_RET_SUCCESS;
}

static void
halmac_init_state_machine_88xx(
	IN struct halmac_adapter *adapter
)
{
	struct halmac_state *state = &adapter->halmac_state;

	halmac_init_offload_feature_state_machine_88xx(adapter);

	state->api_state = HALMAC_API_STATE_INIT;

	state->dlfw_state = HALMAC_DLFW_NONE;
	state->mac_power = HALMAC_MAC_POWER_OFF;
	state->gpio_cfg_state = HALMAC_GPIO_CFG_STATE_IDLE;
	state->rsvd_pg_state = HALMAC_RSVD_PG_STATE_IDLE;
}

void
halmac_init_offload_feature_state_machine_88xx(
	IN struct halmac_adapter *adapter
)
{
	struct halmac_state *state = &adapter->halmac_state;

	state->efuse_state_set.efuse_cmd_construct_state = HALMAC_EFUSE_CMD_IDLE;
	state->efuse_state_set.process_status = HALMAC_CMD_PROCESS_IDLE;
	state->efuse_state_set.seq_num = adapter->h2c_packet_seq;

	state->cfg_para_state_set.cfg_param_cmd_state = HALMAC_CFG_PARA_CMD_IDLE;
	state->cfg_para_state_set.process_status = HALMAC_CMD_PROCESS_IDLE;
	state->cfg_para_state_set.seq_num = adapter->h2c_packet_seq;

	state->scan_state_set.scan_cmd_construct_state = HALMAC_SCAN_CMD_IDLE;
	state->scan_state_set.process_status = HALMAC_CMD_PROCESS_IDLE;
	state->scan_state_set.seq_num = adapter->h2c_packet_seq;

	state->update_packet_set.process_status = HALMAC_CMD_PROCESS_IDLE;
	state->update_packet_set.seq_num = adapter->h2c_packet_seq;

	state->iqk_set.process_status = HALMAC_CMD_PROCESS_IDLE;
	state->iqk_set.seq_num = adapter->h2c_packet_seq;

	state->power_tracking_set.process_status = HALMAC_CMD_PROCESS_IDLE;
	state->power_tracking_set.seq_num = adapter->h2c_packet_seq;

	state->psd_set.process_status = HALMAC_CMD_PROCESS_IDLE;
	state->psd_set.seq_num = adapter->h2c_packet_seq;
	state->psd_set.data_size = 0;
	state->psd_set.segment_size = 0;
	state->psd_set.data = NULL;

	state->fw_snding_set.fw_snding_cmd_construct_state = HALMAC_FW_SNDING_CMD_CONSTRUCT_IDLE;
	state->fw_snding_set.process_status = HALMAC_CMD_PROCESS_IDLE;
	state->fw_snding_set.seq_num = adapter->h2c_packet_seq;

	state->wlcpu_mode = HALMAC_WLCPU_ACTIVE;
}

/**
 * halmac_register_api_88xx() - register feature list
 * @adapter
 * @registry : feature list, 1->enable 0->disable
 * Author : Ivan Lin
 *
 * Default is enable all api registry
 *
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_register_api_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_api_registry *registry
)
{
	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (!registry)
		return HALMAC_RET_NULL_POINTER;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	PLTFM_MEMCPY(&adapter->api_registry, registry, sizeof(*registry));

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]rx_expand : %d\n", adapter->api_registry.rx_expand_mode_en);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]la_mode : %d\n", adapter->api_registry.la_mode_en);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]cfg_drv_rsvd_pg : %d\n", adapter->api_registry.cfg_drv_rsvd_pg_en);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]sdio_cmd53_4byte : %d\n", adapter->api_registry.sdio_cmd53_4byte_en);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_pre_init_system_cfg_88xx() - pre-init system config
 * @adapter : the adapter of halmac
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_pre_init_system_cfg_88xx(
	IN struct halmac_adapter *adapter
)
{
	u32 value32, cnt;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	u8 enable_bb;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	HALMAC_REG_W8(REG_RSV_CTRL, 0);

	if (adapter->halmac_interface == HALMAC_INTERFACE_SDIO) {
		HALMAC_REG_W8(REG_SDIO_HSUS_CTRL, HALMAC_REG_R8(REG_SDIO_HSUS_CTRL) & ~(BIT(0)));
		cnt = 10000;
		while (!(HALMAC_REG_R8(REG_SDIO_HSUS_CTRL) & 0x02)) {
			cnt--;
			if (cnt == 0)
				return HALMAC_RET_SDIO_LEAVE_SUSPEND_FAIL;
		}

		if (adapter->sdio_hw_info.spec_ver == HALMAC_SDIO_SPEC_VER_3_00)
			HALMAC_REG_W8(REG_HCI_OPT_CTRL + 2, HALMAC_REG_R8(REG_HCI_OPT_CTRL + 2) | BIT(2));
		else
			HALMAC_REG_W8(REG_HCI_OPT_CTRL + 2, HALMAC_REG_R8(REG_HCI_OPT_CTRL + 2) & ~(BIT(2)));
	} else if (adapter->halmac_interface == HALMAC_INTERFACE_USB) {
		if (HALMAC_REG_R8(REG_SYS_CFG2 + 3) == 0x20)	 /* usb3.0 */
			HALMAC_REG_W8(0xFE5B, HALMAC_REG_R8(0xFE5B) | BIT(4));
	} else if (adapter->halmac_interface == HALMAC_INTERFACE_PCIE) {
		/* For PCIE power on fail issue */
		HALMAC_REG_W8(REG_HCI_OPT_CTRL + 1, HALMAC_REG_R8(REG_HCI_OPT_CTRL + 1) | BIT(0));
	}

	/* Config PIN Mux */
	value32 = HALMAC_REG_R32(REG_PAD_CTRL1);
	value32 = value32 & (~(BIT(28) | BIT(29)));
	value32 = value32 | BIT(28) | BIT(29);
	HALMAC_REG_W32(REG_PAD_CTRL1, value32);

	value32 = HALMAC_REG_R32(REG_LED_CFG);
	value32 = value32 & (~(BIT(25) | BIT(26)));
	HALMAC_REG_W32(REG_LED_CFG, value32);

	value32 = HALMAC_REG_R32(REG_GPIO_MUXCFG);
	value32 = value32 & (~(BIT(2)));
	value32 = value32 | BIT(2);
	HALMAC_REG_W32(REG_GPIO_MUXCFG, value32);

	enable_bb = _FALSE;
	halmac_set_hw_value_88xx(adapter, HALMAC_HW_EN_BB_RF, &enable_bb);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_init_system_cfg_88xx() -  init system config
 * @adapter : the adapter of halmac
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_init_system_cfg_88xx(
	IN struct halmac_adapter *adapter
)
{
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	u32 tmp = 0;
	u32 value32;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	HALMAC_REG_W8(REG_SYS_FUNC_EN + 1, HALMAC_FUNCTION_ENABLE_88XX);
	HALMAC_REG_W32(REG_SYS_SDIO_CTRL, (u32)(HALMAC_REG_R32(REG_SYS_SDIO_CTRL) | BIT_LTE_MUX_CTRL_PATH));

	value32 = (u32)(HALMAC_REG_R32(REG_CPU_DMEM_CON) | BIT_WL_PLATFORM_RST);
#if HALMAC_8822C_SUPPORT
	if (adapter->chip_id != HALMAC_CHIP_ID_8822B && adapter->chip_id != HALMAC_CHIP_ID_8821C)
		value32 |= BIT_DDMA_EN;
#endif
	HALMAC_REG_W32(REG_CPU_DMEM_CON, value32);

	/*disable boot-from-flash for driver's DL FW*/
	tmp = HALMAC_REG_R32(REG_MCUFW_CTRL);
	if (tmp & BIT_BOOT_FSPI_EN) {
		HALMAC_REG_W32(REG_MCUFW_CTRL, tmp & (~BIT_BOOT_FSPI_EN));
		HALMAC_REG_W32(REG_GPIO_MUXCFG, HALMAC_REG_R32(REG_GPIO_MUXCFG) & (~BIT_FSPI_EN));
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_init_edca_cfg_88xx() - init EDCA config
 * @adapter : the adapter of halmac
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_init_edca_cfg_88xx(
	IN struct halmac_adapter *adapter
)
{
	u32 value32;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	/* Clear TX pause */
	HALMAC_REG_W16(REG_TXPAUSE, 0x0000);

	HALMAC_REG_W8(REG_SLOT, HALMAC_SLOT_TIME_88XX);
	HALMAC_REG_W8(REG_PIFS, HALMAC_PIFS_TIME_88XX);
	value32 = HALMAC_SIFS_CCK_CTX_88XX | (HALMAC_SIFS_OFDM_CTX_88XX << BIT_SHIFT_SIFS_OFDM_CTX) |
		  (HALMAC_SIFS_CCK_TRX_88XX << BIT_SHIFT_SIFS_CCK_TRX) | (HALMAC_SIFS_OFDM_TRX_88XX << BIT_SHIFT_SIFS_OFDM_TRX);
	HALMAC_REG_W32(REG_SIFS, value32);

	HALMAC_REG_W32(REG_EDCA_VO_PARAM, HALMAC_REG_R32(REG_EDCA_VO_PARAM) & 0xFFFF);
	HALMAC_REG_W16(REG_EDCA_VO_PARAM + 2, HALMAC_VO_TXOP_LIMIT_88XX);
	HALMAC_REG_W16(REG_EDCA_VI_PARAM + 2, HALMAC_VI_TXOP_LIMIT_88XX);

	HALMAC_REG_W32(REG_RD_NAV_NXT, HALMAC_RDG_NAV_88XX | (HALMAC_TXOP_NAV_88XX << 16));
	HALMAC_REG_W16(REG_RXTSF_OFFSET_CCK, HALMAC_CCK_RX_TSF_88XX | (HALMAC_OFDM_RX_TSF_88XX) << 8);

	/* Set beacon cotnrol - enable TSF and other related functions */
	HALMAC_REG_W8(REG_BCN_CTRL, (u8)(HALMAC_REG_R8(REG_BCN_CTRL) | BIT_EN_BCN_FUNCTION));

	/* Set send beacon related registers */
	HALMAC_REG_W32(REG_TBTT_PROHIBIT, HALMAC_TBTT_PROHIBIT_88XX | (HALMAC_TBTT_HOLD_TIME_88XX << BIT_SHIFT_TBTT_HOLD_TIME_AP));
	HALMAC_REG_W8(REG_DRVERLYINT, HALMAC_DRIVER_EARLY_INT_88XX);
	HALMAC_REG_W8(REG_BCNDMATIM, HALMAC_BEACON_DMA_TIM_88XX);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_init_wmac_cfg_88xx() - init wmac config
 * @adapter : the adapter of halmac
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_init_wmac_cfg_88xx(
	IN struct halmac_adapter *adapter
)
{
	u32 value32;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	HALMAC_REG_W32(REG_RXFLTMAP0, HALMAC_RX_FILTER0_88XX);
	HALMAC_REG_W16(REG_RXFLTMAP2, HALMAC_RX_FILTER_88XX);

	HALMAC_REG_W32(REG_RCR, HALMAC_RCR_CONFIG_88XX);

	HALMAC_REG_W8(REG_RX_PKT_LIMIT, HALMAC_RXPKT_MAX_SIZE_BASE512);

	HALMAC_REG_W8(REG_TCR + 2, 0x30);
	HALMAC_REG_W8(REG_TCR + 1, 0x30);

#if HALMAC_8821C_SUPPORT
	if (adapter->chip_id == HALMAC_CHIP_ID_8821C)
		HALMAC_REG_W8(REG_ACKTO_CCK, HALMAC_ACK_TO_CCK_88XX);
#endif
	HALMAC_REG_W32(REG_WMAC_OPTION_FUNCTION + 8, 0x30810041);

	value32 = (adapter->hw_cfg_info.trx_mode == HALMAC_TRNSFER_NORMAL) ? 0x50802098 : 0x50802080;
	HALMAC_REG_W32(REG_WMAC_OPTION_FUNCTION + 4, value32);

	status = api->halmac_init_low_pwr(adapter);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_init_mac_cfg_88xx() - config page1~page7 register
 * @adapter : the adapter of halmac
 * @mode : trx mode
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_init_mac_cfg_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_trx_mode mode
)
{
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__, mode);

	status = api->halmac_init_trx_cfg(adapter, mode);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]halmac_init_trx_cfg error = %x\n", status);
		return status;
	}

	status = api->halmac_init_protocol_cfg(adapter);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]halmac_init_protocol_cfg_88xx error = %x\n", status);
		return status;
	}

	status = halmac_init_edca_cfg_88xx(adapter);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]halmac_init_edca_cfg_88xx error = %x\n", status);
		return status;
	}

	status = halmac_init_wmac_cfg_88xx(adapter);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]halmac_init_wmac_cfg_88xx error = %x\n", status);
		return status;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return status;
}

/**
 * halmac_reset_feature_88xx() -reset async api cmd status
 * @adapter : the adapter of halmac
 * @feature_id : feature_id
 * Author : Ivan Lin/KaiYuan Chang
 * Return : enum halmac_ret_status.
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_reset_feature_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_feature_id feature_id
)
{
	struct halmac_state *state = &adapter->halmac_state;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	switch (feature_id) {
	case HALMAC_FEATURE_CFG_PARA:
		state->cfg_para_state_set.process_status = HALMAC_CMD_PROCESS_IDLE;
		state->cfg_para_state_set.cfg_param_cmd_state = HALMAC_CFG_PARA_CMD_IDLE;
		break;
	case HALMAC_FEATURE_DUMP_PHYSICAL_EFUSE:
	case HALMAC_FEATURE_DUMP_LOGICAL_EFUSE:
		state->efuse_state_set.process_status = HALMAC_CMD_PROCESS_IDLE;
		state->efuse_state_set.efuse_cmd_construct_state = HALMAC_EFUSE_CMD_IDLE;
		break;
	case HALMAC_FEATURE_CHANNEL_SWITCH:
		state->scan_state_set.process_status = HALMAC_CMD_PROCESS_IDLE;
		state->scan_state_set.scan_cmd_construct_state = HALMAC_SCAN_CMD_IDLE;
		break;
	case HALMAC_FEATURE_UPDATE_PACKET:
		state->update_packet_set.process_status = HALMAC_CMD_PROCESS_IDLE;
		break;
	case HALMAC_FEATURE_IQK:
		state->iqk_set.process_status = HALMAC_CMD_PROCESS_IDLE;
		break;
	case HALMAC_FEATURE_POWER_TRACKING:
		state->power_tracking_set.process_status = HALMAC_CMD_PROCESS_IDLE;
		break;
	case HALMAC_FEATURE_PSD:
		state->psd_set.process_status = HALMAC_CMD_PROCESS_IDLE;
		break;
	case HALMAC_FEATURE_FW_SNDING:
		state->fw_snding_set.process_status = HALMAC_CMD_PROCESS_IDLE;
		state->fw_snding_set.fw_snding_cmd_construct_state = HALMAC_FW_SNDING_CMD_CONSTRUCT_IDLE;
		break;
	case HALMAC_FEATURE_ALL:
		state->cfg_para_state_set.process_status = HALMAC_CMD_PROCESS_IDLE;
		state->cfg_para_state_set.cfg_param_cmd_state = HALMAC_CFG_PARA_CMD_IDLE;
		state->efuse_state_set.process_status = HALMAC_CMD_PROCESS_IDLE;
		state->efuse_state_set.efuse_cmd_construct_state = HALMAC_EFUSE_CMD_IDLE;
		state->scan_state_set.process_status = HALMAC_CMD_PROCESS_IDLE;
		state->scan_state_set.scan_cmd_construct_state = HALMAC_SCAN_CMD_IDLE;
		state->update_packet_set.process_status = HALMAC_CMD_PROCESS_IDLE;
		state->iqk_set.process_status = HALMAC_CMD_PROCESS_IDLE;
		state->power_tracking_set.process_status = HALMAC_CMD_PROCESS_IDLE;
		state->psd_set.process_status = HALMAC_CMD_PROCESS_IDLE;
		state->fw_snding_set.process_status = HALMAC_CMD_PROCESS_IDLE;
		state->fw_snding_set.fw_snding_cmd_construct_state = HALMAC_FW_SNDING_CMD_CONSTRUCT_IDLE;
		break;
	default:
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]%s invalid feature id %d\n", __func__, feature_id);
		return HALMAC_RET_INVALID_FEATURE_ID;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * (debug API)halmac_verify_platform_api_88xx() - verify platform api
 * @adapter : the adapter of halmac
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_verify_platform_api_88xx(
	IN struct halmac_adapter *adapter
)
{
	enum halmac_ret_status ret_status = HALMAC_RET_SUCCESS;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	ret_status = halmac_verify_io_88xx(adapter);

	if (ret_status != HALMAC_RET_SUCCESS)
		return ret_status;

	if (adapter->txff_allocation.la_mode != HALMAC_LA_MODE_FULL)
		ret_status = halmac_verify_send_rsvd_page_88xx(adapter);

	if (ret_status != HALMAC_RET_SUCCESS)
		return ret_status;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return ret_status;
}

void
halmac_tx_desc_checksum_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 enable
)
{
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	adapter->tx_desc_checksum = enable;

	if (enable == _TRUE)
		HALMAC_REG_W16(REG_TXDMA_OFFSET_CHK, (u16)(HALMAC_REG_R16(REG_TXDMA_OFFSET_CHK) | BIT_SDIO_TXDESC_CHKSUM_EN));
	else
		HALMAC_REG_W16(REG_TXDMA_OFFSET_CHK, (u16)(HALMAC_REG_R16(REG_TXDMA_OFFSET_CHK) & ~BIT_SDIO_TXDESC_CHKSUM_EN));
}

static enum halmac_ret_status
halmac_verify_io_88xx(
	IN struct halmac_adapter *adapter
)
{
	u8 value8, wvalue8;
	u32 value32, value32_2, wvalue32;
	u32 offset;
	enum halmac_ret_status ret_status = HALMAC_RET_SUCCESS;

	if (adapter->halmac_interface == HALMAC_INTERFACE_SDIO) {
		offset = REG_PAGE5_DUMMY;
		if (0 == (offset & 0xFFFF0000))
			offset |= WLAN_IOREG_OFFSET;

		ret_status = halmac_convert_to_sdio_bus_offset_88xx(adapter, &offset);

		/* Verify CMD52 R/W */
		wvalue8 = 0xab;
		PLTFM_SDIO_CMD52_W(offset, wvalue8);

		value8 = PLTFM_SDIO_CMD52_R(offset);

		if (value8 != wvalue8) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]cmd52 r/w fail write = %X read = %X\n", wvalue8, value8);
			ret_status = HALMAC_RET_PLATFORM_API_INCORRECT;
		} else {
			PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]cmd52 r/w ok\n");
		}

		/* Verify CMD53 R/W */
		PLTFM_SDIO_CMD52_W(offset, 0xaa);
		PLTFM_SDIO_CMD52_W(offset + 1, 0xbb);
		PLTFM_SDIO_CMD52_W(offset + 2, 0xcc);
		PLTFM_SDIO_CMD52_W(offset + 3, 0xdd);

		value32 = PLTFM_SDIO_CMD53_R32(offset);

		if (value32 != 0xddccbbaa) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]cmd53 r fail : read = %X\n");
			ret_status = HALMAC_RET_PLATFORM_API_INCORRECT;
		} else {
			PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]cmd53 r ok\n");
		}

		wvalue32 = 0x11223344;
		PLTFM_SDIO_CMD53_W32(offset, wvalue32);

		value32 = PLTFM_SDIO_CMD53_R32(offset);

		if (value32 != wvalue32) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]cmd53 w fail\n");
			ret_status = HALMAC_RET_PLATFORM_API_INCORRECT;
		} else {
			PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]cmd53 w ok\n");
		}

		value32 = PLTFM_SDIO_CMD53_R32(offset + 2); /* value32 should be 0x33441122 */

		wvalue32 = 0x11225566;
		PLTFM_SDIO_CMD53_W32(offset, wvalue32);

		value32_2 = PLTFM_SDIO_CMD53_R32(offset + 2); /* value32 should be 0x55661122 */
		if (value32_2 == value32) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]cmd52 is used for HAL_SDIO_CMD53_READ_32\n");
			ret_status = HALMAC_RET_PLATFORM_API_INCORRECT;
		} else {
			PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]cmd53 is correctly used\n");
		}
	} else {
		wvalue32 = 0x77665511;
		PLTFM_REG_W32(REG_PAGE5_DUMMY, wvalue32);

		value32 = PLTFM_REG_R32(REG_PAGE5_DUMMY);
		if (value32 != wvalue32) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]reg rw\n");
			ret_status = HALMAC_RET_PLATFORM_API_INCORRECT;
		} else {
			PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]reg rw ok\n");
		}
	}

	return ret_status;
}

static enum halmac_ret_status
halmac_verify_send_rsvd_page_88xx(
	IN struct halmac_adapter *adapter
)
{
	u8 *rsvd_buf = NULL;
	u8 *rsvd_page = NULL;
	u32 i;
	u32 pkt_size = 64, payload = 0xab;
	enum halmac_ret_status ret_status = HALMAC_RET_SUCCESS;

	rsvd_buf = (u8 *)PLTFM_MALLOC(pkt_size);

	if (!rsvd_buf) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]rsvd buffer malloc fail!!\n");
		return HALMAC_RET_MALLOC_FAIL;
	}

	PLTFM_MEMSET(rsvd_buf, (u8)payload, pkt_size);

	ret_status = halmac_download_rsvd_page_88xx(adapter, adapter->txff_allocation.rsvd_pg_bndy,
						    rsvd_buf, pkt_size);
	if (ret_status != HALMAC_RET_SUCCESS) {
		PLTFM_FREE(rsvd_buf, pkt_size);
		return ret_status;
	}

	rsvd_page = (u8 *)PLTFM_MALLOC(pkt_size + adapter->hw_cfg_info.txdesc_size);

	if (!rsvd_page) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]rsvd page malloc fail!!\n");
		PLTFM_FREE(rsvd_buf, pkt_size);
		return HALMAC_RET_MALLOC_FAIL;
	}

	PLTFM_MEMSET(rsvd_page, 0x00, pkt_size + adapter->hw_cfg_info.txdesc_size);

	ret_status = halmac_dump_fifo_88xx(adapter, HAL_FIFO_SEL_RSVD_PAGE, 0, pkt_size + adapter->hw_cfg_info.txdesc_size, rsvd_page);

	if (ret_status != HALMAC_RET_SUCCESS) {
		PLTFM_FREE(rsvd_buf, pkt_size);
		PLTFM_FREE(rsvd_page, pkt_size + adapter->hw_cfg_info.txdesc_size);
		return ret_status;
	}

	for (i = 0; i < pkt_size; i++) {
		if (*(rsvd_buf + i) != *(rsvd_page + (i + adapter->hw_cfg_info.txdesc_size))) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Compare RSVD page Fail\n");
			ret_status = HALMAC_RET_PLATFORM_API_INCORRECT;
		}
	}

	PLTFM_FREE(rsvd_buf, pkt_size);
	PLTFM_FREE(rsvd_page, pkt_size + adapter->hw_cfg_info.txdesc_size);

	return ret_status;
}

enum halmac_ret_status
halmac_pg_num_parser_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_trx_mode mode,
	IN struct halmac_pg_num *tbl
)
{
	u8 flag;
	u16 hpq_num = 0, lpq_num = 0, npq_num = 0, gapq_num = 0;
	u16 expq_num = 0, pubq_num = 0;
	u32 i = 0;

	flag = 0;
	for (i = 0; i < HALMAC_TRX_MODE_MAX; i++) {
		if (mode == tbl[i].mode) {
			hpq_num = tbl[i].hq_num;
			lpq_num = tbl[i].lq_num;
			npq_num = tbl[i].nq_num;
			expq_num = tbl[i].exq_num;
			gapq_num = tbl[i].gap_num;
			pubq_num = adapter->txff_allocation.ac_q_pg_num - hpq_num - lpq_num - npq_num - expq_num - gapq_num;
			flag = 1;
			PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s done\n", __func__);
			break;
		}
	}

	if (flag == 0) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]HALMAC_RET_TRX_MODE_NOT_SUPPORT 1 switch case not support\n");
		return HALMAC_RET_TRX_MODE_NOT_SUPPORT;
	}

	if (adapter->txff_allocation.ac_q_pg_num < hpq_num + lpq_num + npq_num + expq_num + gapq_num) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]acqnum = %d\n", adapter->txff_allocation.ac_q_pg_num);
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]hpq_num = %d\n", hpq_num);
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]LPQ_num = %d\n", lpq_num);
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]npq_num = %d\n", npq_num);
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]EPQ_num = %d\n", expq_num);
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]gapq_num = %d\n", gapq_num);
		return HALMAC_RET_CFG_TXFIFO_PAGE_FAIL;
	}

	adapter->txff_allocation.high_queue_pg_num = hpq_num;
	adapter->txff_allocation.low_queue_pg_num = lpq_num;
	adapter->txff_allocation.normal_queue_pg_num = npq_num;
	adapter->txff_allocation.extra_queue_pg_num = expq_num;
	adapter->txff_allocation.pub_queue_pg_num = pubq_num;

	return HALMAC_RET_SUCCESS;
}

enum halmac_ret_status
halmac_rqpn_parser_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_trx_mode mode,
	IN struct halmac_rqpn *tbl
)
{
	u8 flag;
	u32 i;

	flag = 0;
	for (i = 0; i < HALMAC_TRX_MODE_MAX; i++) {
		if (mode == tbl[i].mode) {
			adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_VO] = tbl[i].dma_map_vo;
			adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_VI] = tbl[i].dma_map_vi;
			adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_BE] = tbl[i].dma_map_be;
			adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_BK] = tbl[i].dma_map_bk;
			adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_MG] = tbl[i].dma_map_mg;
			adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_HI] = tbl[i].dma_map_hi;
			flag = 1;
			PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s done\n", __func__);
			break;
		}
	}

	if (flag == 0) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]HALMAC_RET_TRX_MODE_NOT_SUPPORT 1 switch case not support\n");
		return HALMAC_RET_TRX_MODE_NOT_SUPPORT;
	}

	return HALMAC_RET_SUCCESS;
}

#endif /* HALMAC_88XX_SUPPORT */
