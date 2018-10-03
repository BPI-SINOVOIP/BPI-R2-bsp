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

#include "halmac_fw_88xx.h"
#include "halmac_88xx_cfg.h"
#include "halmac_common_88xx.h"
#include "halmac_init_88xx.h"

#if HALMAC_88XX_SUPPORT

#define DLFW_RESTORE_REG_NUM		6
#define ILLEGAL_KEY_GROUP		0xFAAAAA00

#define ID_INFORM_DLEMEM_RDY		0x80
#define ID_INFORM_ENETR_CPU_SLEEP	0x20
#define ID_CHECK_DLEMEM_RDY		0x80
#define ID_CHECK_ENETR_CPU_SLEEP	0x05

#define FW_STATUS_CHK_FATAL	(BIT(1) | BIT(20))
#define FW_STATUS_CHK_ERR	(BIT(4) | BIT(5) | BIT(6) | BIT(7) | BIT(8) | \
				 BIT(9) | BIT(12) | BIT(14) | BIT(15) | \
				 BIT(16) | BIT(17) | BIT(18) | BIT(19) | \
				 BIT(21) | BIT(22) | BIT(25))
#define FW_STATUS_CHK_WARN	~(FW_STATUS_CHK_FATAL | FW_STATUS_CHK_ERR)

struct halmac_backup_info {
	u32 mac_register;
	u32 value;
	u8 length;
};

static enum halmac_ret_status
halmac_update_fw_info_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *fw_bin
);

static void
halmac_restore_mac_register_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_backup_info *info,
	IN u32 num
);

static enum halmac_ret_status
halmac_dlfw_to_mem_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *fw_bin,
	IN u32 src,
	IN u32 dest,
	IN u32 size
);

static enum halmac_ret_status
halmac_dlfw_end_flow_88xx(
	IN struct halmac_adapter *adapter
);

static enum halmac_ret_status
halmac_free_dl_fw_end_flow_88xx(
	IN struct halmac_adapter *adapter
);

static enum halmac_ret_status
halmac_send_fwpkt_88xx(
	IN struct halmac_adapter *adapter,
	IN u16 pg_addr,
	IN u8 *fw_bin,
	IN u32 size
);

static enum halmac_ret_status
halmac_iddma_dlfw_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 src,
	IN u32 dest,
	IN u32 len,
	IN u8 first
);

static enum halmac_ret_status
halmac_iddma_en_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 src,
	IN u32 dest,
	IN u32 ctrl
);

static enum halmac_ret_status
halmac_check_fw_chksum_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 mem_addr
);

static void
halmac_fw_fatal_status_debug_88xx(
	IN struct halmac_adapter *adapter
);

static enum halmac_ret_status
halmac_start_dlfw_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *fw_bin,
	IN u32 size,
	IN u32 dl_addr,
	IN u8 emem_only
);

static enum halmac_ret_status
halmac_chk_fw_size_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *fw_bin,
	IN u32 size
);

static void
halmac_chk_h2c_ver_88xx(
	IN struct halmac_adapter *adapter,
	IN IN u8 *fw_bin
);

static void
halmac_wlan_cpu_enable(
	IN struct halmac_adapter *adapter,
	IN IN u8 enable
);

static void
halmac_platform_reset(
	IN struct halmac_adapter *adapter
);

/**
 * halmac_download_firmware_88xx() - download Firmware
 * @adapter : the adapter of halmac
 * @fw_bin : firmware bin
 * @size : firmware size
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_download_firmware_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *fw_bin,
	IN u32 size
)
{
	u8 value8;
	u32 bckp_idx = 0;
	u32 lte_coex_backup = 0;
	struct halmac_backup_info bckp[DLFW_RESTORE_REG_NUM];
	enum halmac_ret_status status;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (adapter->halmac_state.mac_power == HALMAC_MAC_POWER_OFF)
		return HALMAC_RET_POWER_STATE_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	status = halmac_chk_fw_size_88xx(adapter, fw_bin, size);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	halmac_chk_h2c_ver_88xx(adapter, fw_bin);

	if (adapter->halmac_state.wlcpu_mode == HALMAC_WLCPU_ENTER_SLEEP)
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]Enter Sleep..zZZ\n");

	adapter->halmac_state.dlfw_state = HALMAC_DLFW_NONE;

	status = halmac_ltecoex_reg_read_88xx(adapter, 0x38, &lte_coex_backup);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	halmac_wlan_cpu_enable(adapter, 0);

	/* set HIQ to hi priority */
	bckp[bckp_idx].length = 1;
	bckp[bckp_idx].mac_register = REG_TXDMA_PQ_MAP + 1;
	bckp[bckp_idx].value = HALMAC_REG_R8(REG_TXDMA_PQ_MAP + 1);
	bckp_idx++;
	value8 = HALMAC_DMA_MAPPING_HIGH << 6;
	HALMAC_REG_W8(REG_TXDMA_PQ_MAP + 1, value8);

	/* DLFW only use HIQ, map HIQ to hi priority */
	adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_HI] =
						HALMAC_DMA_MAPPING_HIGH;
	bckp[bckp_idx].length = 1;
	bckp[bckp_idx].mac_register = REG_CR;
	bckp[bckp_idx].value = HALMAC_REG_R8(REG_CR);
	bckp_idx++;
	bckp[bckp_idx].length = 4;
	bckp[bckp_idx].mac_register = REG_H2CQ_CSR;
	bckp[bckp_idx].value = BIT(31);
	bckp_idx++;
	value8 = BIT_HCI_TXDMA_EN | BIT_TXDMA_EN;
	HALMAC_REG_W8(REG_CR, value8);
	HALMAC_REG_W32(REG_H2CQ_CSR, BIT(31));

	/* Config hi priority queue and public priority queue page number */
	bckp[bckp_idx].length = 2;
	bckp[bckp_idx].mac_register = REG_FIFOPAGE_INFO_1;
	bckp[bckp_idx].value = HALMAC_REG_R16(REG_FIFOPAGE_INFO_1);
	bckp_idx++;
	bckp[bckp_idx].length = 4;
	bckp[bckp_idx].mac_register = REG_RQPN_CTRL_2;
	bckp[bckp_idx].value = HALMAC_REG_R32(REG_RQPN_CTRL_2) | BIT(31);
	bckp_idx++;
	HALMAC_REG_W16(REG_FIFOPAGE_INFO_1, 0x200);
	HALMAC_REG_W32(REG_RQPN_CTRL_2, bckp[bckp_idx - 1].value);

	/* Disable beacon related functions */
	value8 = HALMAC_REG_R8(REG_BCN_CTRL);
	bckp[bckp_idx].length = 1;
	bckp[bckp_idx].mac_register = REG_BCN_CTRL;
	bckp[bckp_idx].value = value8;
	bckp_idx++;
	value8 = (u8)((value8 & (~BIT(3))) | BIT(4));
	HALMAC_REG_W8(REG_BCN_CTRL, value8);

	if (adapter->halmac_interface == HALMAC_INTERFACE_SDIO)
		HALMAC_REG_R32(REG_SDIO_FREE_TXPG);

	halmac_platform_reset(adapter);

	status = halmac_start_dlfw_88xx(adapter, fw_bin, size, 0, 0);

	halmac_restore_mac_register_88xx(adapter, bckp, DLFW_RESTORE_REG_NUM);

	if (status != HALMAC_RET_SUCCESS)
		goto DLFW_FAIL;

	status = halmac_dlfw_end_flow_88xx(adapter);
	if (status != HALMAC_RET_SUCCESS)
		goto DLFW_FAIL;

	status = halmac_ltecoex_reg_write_88xx(adapter, 0x38, lte_coex_backup);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	adapter->halmac_state.dlfw_state = HALMAC_DLFW_DONE;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;

DLFW_FAIL:

	/* Disable FWDL_EN */
	value8 = HALMAC_REG_R8(REG_MCUFW_CTRL);
	value8 &= ~BIT(0);
	HALMAC_REG_W8(REG_MCUFW_CTRL, value8);

	value8 = HALMAC_REG_R8(REG_SYS_FUNC_EN + 1);
	value8 |= BIT(2);
	HALMAC_REG_W8(REG_SYS_FUNC_EN + 1, value8);

	if (halmac_ltecoex_reg_write_88xx(adapter, 0x38, lte_coex_backup) !=
	    HALMAC_RET_SUCCESS)
		return HALMAC_RET_LTECOEX_READY_FAIL;

	return status;
}

static enum halmac_ret_status
halmac_start_dlfw_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *fw_bin,
	IN u32 size,
	IN u32 dl_addr,
	IN u8 emem_only
)
{
	u8 *cur_fw;
	u16 value16;
	u32 imem_size;
	u32 dmem_size;
	u32 emem_size = 0;
	u32 addr;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	enum halmac_ret_status status;

	dmem_size = *((u32 *)(fw_bin + HALMAC_FWHDR_DMEM_SIZE_88XX));
	imem_size = *((u32 *)(fw_bin + HALMAC_FWHDR_IMEM_SIZE_88XX));
	if (0 != ((*(fw_bin + HALMAC_FWHDR_MEM_USAGE_88XX)) & BIT(4)))
		emem_size = *((u32 *)(fw_bin + HALMAC_FWHDR_EMEM_SIZE_88XX));

	dmem_size = rtk_le32_to_cpu(dmem_size);
	imem_size = rtk_le32_to_cpu(imem_size);
	emem_size = rtk_le32_to_cpu(emem_size);

	dmem_size += HALMAC_FW_CHKSUM_DUMMY_SIZE_88XX;
	imem_size += HALMAC_FW_CHKSUM_DUMMY_SIZE_88XX;
	if (emem_size != 0)
		emem_size += HALMAC_FW_CHKSUM_DUMMY_SIZE_88XX;

	if (emem_only == 1) {
		if (!emem_size)
			return HALMAC_RET_SUCCESS;
		goto DLFW_EMEM;
	}

	value16 = (u16)(HALMAC_REG_R16(REG_MCUFW_CTRL) & 0x3800);
	value16 |= BIT(0);
	HALMAC_REG_W16(REG_MCUFW_CTRL, value16);

	cur_fw = fw_bin + HALMAC_FWHDR_SIZE_88XX;
	addr = *((u32 *)(fw_bin + HALMAC_FWHDR_DMEM_ADDR_88XX));
	addr = rtk_le32_to_cpu(addr) & ~BIT(31);
	status = halmac_dlfw_to_mem_88xx(adapter, cur_fw, 0, addr, dmem_size);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	cur_fw = fw_bin + HALMAC_FWHDR_SIZE_88XX + dmem_size;
	addr = *((u32 *)(fw_bin + HALMAC_FWHDR_IMEM_ADDR_88XX));
	addr = rtk_le32_to_cpu(addr) & ~BIT(31);
	status = halmac_dlfw_to_mem_88xx(adapter, cur_fw, 0, addr, imem_size);
	if (status != HALMAC_RET_SUCCESS)
		return status;

DLFW_EMEM:
	if (emem_size) {
		cur_fw = fw_bin + HALMAC_FWHDR_SIZE_88XX +
				dmem_size + imem_size;
		addr = *((u32 *)(fw_bin + HALMAC_FWHDR_EMEM_ADDR_88XX));
		addr = rtk_le32_to_cpu(addr) & ~BIT(31);
		status = halmac_dlfw_to_mem_88xx(adapter, cur_fw, dl_addr << 7,
						 addr, emem_size);
		if (status != HALMAC_RET_SUCCESS)
			return status;

		if (emem_only == 1)
			return HALMAC_RET_SUCCESS;
	}

	halmac_update_fw_info_88xx(adapter, fw_bin);
	halmac_init_offload_feature_state_machine_88xx(adapter);

	return HALMAC_RET_SUCCESS;
}

static void
halmac_chk_h2c_ver_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *fw_bin
)
{
	u16 halmac_h2c_ver;
	u16 fw_h2c_ver;

	fw_h2c_ver = *((u16 *)(fw_bin + HALMAC_FWHDR_H2C_FMT_VER_88XX));
	fw_h2c_ver = rtk_le16_to_cpu(fw_h2c_ver);
	halmac_h2c_ver = H2C_FORMAT_VERSION;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE,
			"[TRACE]halmac h2c ver = %x, fw h2c ver = %x!!\n",
			halmac_h2c_ver, fw_h2c_ver);

	if (fw_h2c_ver != halmac_h2c_ver)
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN,
				"[WARN]H2C/C2H ver is compatible!!\n");
}

static enum halmac_ret_status
halmac_chk_fw_size_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *fw_bin,
	IN u32 size
)
{
	u32 imem_size;
	u32 dmem_size;
	u32 emem_size = 0;
	u32 real_size;

	if (size < HALMAC_FWHDR_SIZE_88XX) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]FW size error!\n");
		return HALMAC_RET_FW_SIZE_ERR;
	}

	dmem_size = *((u32 *)(fw_bin + HALMAC_FWHDR_DMEM_SIZE_88XX));
	imem_size = *((u32 *)(fw_bin + HALMAC_FWHDR_IMEM_SIZE_88XX));
	if (0 != ((*(fw_bin + HALMAC_FWHDR_MEM_USAGE_88XX)) & BIT(4)))
		emem_size = *((u32 *)(fw_bin + HALMAC_FWHDR_EMEM_SIZE_88XX));

	dmem_size = rtk_le32_to_cpu(dmem_size);
	imem_size = rtk_le32_to_cpu(imem_size);
	emem_size = rtk_le32_to_cpu(emem_size);

	dmem_size += HALMAC_FW_CHKSUM_DUMMY_SIZE_88XX;
	imem_size += HALMAC_FW_CHKSUM_DUMMY_SIZE_88XX;
	if (emem_size != 0)
		emem_size += HALMAC_FW_CHKSUM_DUMMY_SIZE_88XX;

	real_size = HALMAC_FWHDR_SIZE_88XX + dmem_size + imem_size + emem_size;
	if (size != real_size) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]size != real size!\n");
		return HALMAC_RET_FW_SIZE_ERR;
	}

	return HALMAC_RET_SUCCESS;
}

static void
halmac_wlan_cpu_enable(
	IN struct halmac_adapter *adapter,
	IN IN u8 enable
)
{
	u8 value8;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (enable == 1) {
		/* cpu io interface enable or disable */
		value8 = HALMAC_REG_R8(REG_RSV_CTRL + 1);
		value8 |= BIT(0);
		HALMAC_REG_W8(REG_RSV_CTRL + 1, value8);

		/* cpu enable or disable */
		value8 = HALMAC_REG_R8(REG_SYS_FUNC_EN + 1);
		value8 |= BIT(2);
		HALMAC_REG_W8(REG_SYS_FUNC_EN + 1, value8);

	} else {
		/* cpu enable or disable */
		value8 = HALMAC_REG_R8(REG_SYS_FUNC_EN + 1);
		value8 &= ~BIT(2);
		HALMAC_REG_W8(REG_SYS_FUNC_EN + 1, value8);

		/* cpu io interface enable or disable */
		value8 = HALMAC_REG_R8(REG_RSV_CTRL + 1);
		value8 &= ~BIT(0);
		HALMAC_REG_W8(REG_RSV_CTRL + 1, value8);
	}
}

static void
halmac_platform_reset(
	IN struct halmac_adapter *adapter
)
{
	u8 value8;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	value8 = HALMAC_REG_R8(REG_CPU_DMEM_CON + 2) & ~BIT(0);
	HALMAC_REG_W8(REG_CPU_DMEM_CON + 2, value8);

	/* For 8822B & 8821C clock sync issue */
	if (adapter->chip_id == HALMAC_CHIP_ID_8821C ||
	    adapter->chip_id == HALMAC_CHIP_ID_8822B) {
		value8 = HALMAC_REG_R8(REG_SYS_CLK_CTRL + 1) & ~BIT(6);
		HALMAC_REG_W8(REG_SYS_CLK_CTRL + 1, value8);
	}

	value8 = HALMAC_REG_R8(REG_CPU_DMEM_CON + 2) | BIT(0);
	HALMAC_REG_W8(REG_CPU_DMEM_CON + 2, value8);

	if (adapter->chip_id == HALMAC_CHIP_ID_8821C ||
	    adapter->chip_id == HALMAC_CHIP_ID_8822B) {
		value8 = HALMAC_REG_R8(REG_SYS_CLK_CTRL + 1) | BIT(6);
		HALMAC_REG_W8(REG_SYS_CLK_CTRL + 1, value8);
	}
}

/**
 * halmac_free_download_firmware_88xx() - download specific memory firmware
 * @adapter
 * @mem_sel : memory selection
 * @fw_bin : firmware bin
 * @size : firmware size
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 */
enum halmac_ret_status
halmac_free_download_firmware_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_dlfw_mem mem_sel,
	IN u8 *fw_bin,
	IN u32 size
)
{
	u8 tx_pause_bckp;
	u32 dl_addr;
	u32 dlfw_size_bckp;
	enum halmac_ret_status status;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (halmac_fw_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_NO_DLFW;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	status = halmac_chk_fw_size_88xx(adapter, fw_bin, size);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	if (((*(fw_bin + HALMAC_FWHDR_MEM_USAGE_88XX)) & BIT(4)) == 0)
		return HALMAC_RET_SUCCESS;

	dlfw_size_bckp = adapter->max_download_size;
	if (mem_sel == HALMAC_DLFW_MEM_EMEM) {
		dl_addr = 0;
	} else {
		dl_addr = adapter->txff_allocation.rsvd_h2c_extra_info_pg_bndy;
		adapter->max_download_size =
			(dlfw_size_bckp > HALMAC_DLFW_WITH_RSVDPG_SZ_88XX) ?
			HALMAC_DLFW_WITH_RSVDPG_SZ_88XX : dlfw_size_bckp;
	}

	tx_pause_bckp = HALMAC_REG_R8(REG_TXPAUSE);
	HALMAC_REG_W8(REG_TXPAUSE, tx_pause_bckp | BIT(7));

	status = halmac_start_dlfw_88xx(adapter, fw_bin, size, dl_addr, 1);
	if (status != HALMAC_RET_SUCCESS)
		goto DL_FREE_FW_END;

	status = halmac_free_dl_fw_end_flow_88xx(adapter);

DL_FREE_FW_END:
	HALMAC_REG_W8(REG_TXPAUSE, tx_pause_bckp);
	adapter->max_download_size = dlfw_size_bckp;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return status;
}

/**
 * halmac_get_fw_version_88xx() - get FW version
 * @adapter : the adapter of halmac
 * @ver : fw version info
 * Author : Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_get_fw_version_88xx(
	IN struct halmac_adapter *adapter,
	OUT struct halmac_fw_version *ver
)
{
	struct halmac_fw_version *info = &adapter->fw_version;

	if (!ver)
		return HALMAC_RET_NULL_POINTER;

	if (adapter->halmac_state.dlfw_state == HALMAC_DLFW_NONE)
		return HALMAC_RET_NO_DLFW;

	ver->version = info->version;
	ver->sub_version = info->sub_version;
	ver->sub_index = info->sub_index;
	ver->h2c_version = info->h2c_version;
	ver->build_time.month = info->build_time.month;
	ver->build_time.date = info->build_time.date;
	ver->build_time.hour = info->build_time.hour;
	ver->build_time.min = info->build_time.min;
	ver->build_time.year = info->build_time.year;

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_update_fw_info_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *fw_bin
)
{
	struct halmac_fw_version *info = &adapter->fw_version;

	info->version = *((u16 *)(fw_bin + HALMAC_FWHDR_VERSION_88XX));
	info->version = rtk_le16_to_cpu(info->version);
	info->sub_version = *(fw_bin + HALMAC_FWHDR_SUBVERSION_88XX);
	info->sub_index = *(fw_bin + HALMAC_FWHDR_SUBINDEX_88XX);
	info->h2c_version = *((u16 *)(fw_bin + HALMAC_FWHDR_H2C_FMT_VER_88XX));
	info->h2c_version = rtk_le16_to_cpu(info->h2c_version);
	info->build_time.month = *(fw_bin + HALMAC_FWHDR_MONTH_88XX);
	info->build_time.date = *(fw_bin + HALMAC_FWHDR_DATE_88XX);
	info->build_time.hour = *(fw_bin + HALMAC_FWHDR_HOUR_88XX);
	info->build_time.min = *(fw_bin + HALMAC_FWHDR_MIN_88XX);
	info->build_time.year = *((u16 *)(fw_bin + HALMAC_FWHDR_YEAR_88XX));
	info->build_time.year = rtk_le16_to_cpu(info->build_time.year);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]=== FW info ===\n");
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]ver : %X\n", info->version);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]sub-ver : %X\n",
			info->sub_version);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]sub-idx : %X\n",
			info->sub_index);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]build : %d/%d/%d %d:%d\n",
			info->build_time.year, info->build_time.month,
			info->build_time.date, info->build_time.hour,
			info->build_time.min);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_dlfw_to_mem_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *fw_bin,
	IN u32 src,
	IN u32 dest,
	IN u32 size
)
{
	u8 first_part;
	u32 mem_offset;
	u32 residue_size;
	u32 pkt_size;
	enum halmac_ret_status status;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	mem_offset = 0;
	first_part = 1;
	residue_size = size;

	HALMAC_REG_W32(
		REG_DDMA_CH0CTRL, HALMAC_REG_R32(REG_DDMA_CH0CTRL) |
						 BIT_DDMACH0_RESET_CHKSUM_STS);

	while (residue_size != 0) {
		if (residue_size >= adapter->max_download_size)
			pkt_size = adapter->max_download_size;
		else
			pkt_size = residue_size;

		status = halmac_send_fwpkt_88xx(adapter, (u16)(src >> 7),
						fw_bin + mem_offset, pkt_size);
		if (status != HALMAC_RET_SUCCESS) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]send fw pkt!!");
			return status;
		}

		status = halmac_iddma_dlfw_88xx(
			adapter,
			HALMAC_OCPBASE_TXBUF_88XX +
			src + adapter->hw_cfg_info.txdesc_size,
			dest + mem_offset, pkt_size, first_part);
		if (status != HALMAC_RET_SUCCESS) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]iddma dlfw!!");
			return status;
		}

		first_part = 0;
		mem_offset += pkt_size;
		residue_size -= pkt_size;
	}

	status = halmac_check_fw_chksum_88xx(adapter, dest);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]chk fw chksum!!");
		return status;
	}

	return HALMAC_RET_SUCCESS;
}

static void
halmac_restore_mac_register_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_backup_info *info,
	IN u32 num
)
{
	u8 len;
	u32 i;
	u32 reg;
	u32 value;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	struct halmac_backup_info *curr_info = info;

	for (i = 0; i < num; i++) {
		reg = curr_info->mac_register;
		value = curr_info->value;
		len = curr_info->length;

		if (len == 1)
			HALMAC_REG_W8(reg, (u8)value);
		else if (len == 2)
			HALMAC_REG_W16(reg, (u16)value);
		else if (len == 4)
			HALMAC_REG_W32(reg, value);

		curr_info++;
	}
}

static enum halmac_ret_status
halmac_dlfw_end_flow_88xx(
	IN struct halmac_adapter *adapter
)
{
	u16 fw_ctrl;
	u32 cnt;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	HALMAC_REG_W32(REG_TXDMA_STATUS, BIT(2));

	/* Check IMEM & DMEM checksum is OK or not */
	fw_ctrl = HALMAC_REG_R16(REG_MCUFW_CTRL);
	if ((fw_ctrl & 0x50) == 0x50)
		HALMAC_REG_W16(REG_MCUFW_CTRL,
			       (fw_ctrl | BIT_FW_DW_RDY) & ~BIT(0));
	else
		return HALMAC_RET_IDMEM_CHKSUM_FAIL;

	halmac_wlan_cpu_enable(adapter, 1);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Dlfw OK, enable CPU\n");

	cnt = 10000;
	while (HALMAC_REG_R16(REG_MCUFW_CTRL) != 0xC078) {
		if (cnt == 0) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
					"[ERR]Check 0x80 = 0xC078 fail\n");
			if ((HALMAC_REG_R32(REG_FW_DBG7) & 0xFFFFFF00) ==
			    ILLEGAL_KEY_GROUP) {
				PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Key!!\n");
				return HALMAC_RET_ILLEGAL_KEY_FAIL;
			}
			return HALMAC_RET_FW_READY_CHK_FAIL;
		}
		cnt--;
		PLTFM_DELAY_US(50);
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]0x80=0xC078, cnt=%d\n", cnt);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_free_dl_fw_end_flow_88xx(
	IN struct halmac_adapter *adapter
)
{
	u32 cnt;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	cnt = 100;
	while (HALMAC_REG_R8(REG_HMETFR + 3) != 0) {
		cnt--;
		if (cnt == 0) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]0x1CF != 0\n");
			return HALMAC_RET_DLFW_FAIL;
		}
		PLTFM_DELAY_US(50);
	}

	HALMAC_REG_W8(REG_HMETFR + 3, ID_INFORM_DLEMEM_RDY);

	cnt = 10000;
	while (HALMAC_REG_R8(REG_MCU_TST_CFG) != ID_CHECK_DLEMEM_RDY) {
		cnt--;
		if (cnt == 0) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]0x84 != 0x80\n");
			return HALMAC_RET_DLFW_FAIL;
		}
		PLTFM_DELAY_US(50);
	}

	HALMAC_REG_W8(REG_MCU_TST_CFG, 0);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_send_fwpkt_88xx(
	IN struct halmac_adapter *adapter,
	IN u16 pg_addr,
	IN u8 *fw_bin,
	IN u32 size
)
{
	enum halmac_ret_status status;

	status = halmac_download_rsvd_page_88xx(adapter, pg_addr, fw_bin, size);
	if (status != HALMAC_RET_SUCCESS)
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]dl rsvd page!!\n");

	return status;
}

static enum halmac_ret_status
halmac_iddma_dlfw_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 src,
	IN u32 dest,
	IN u32 len,
	IN u8 first
)
{
	u32 cnt;
	u32 ch0_ctrl = (u32)(BIT_DDMACH0_CHKSUM_EN | BIT_DDMACH0_OWN);
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	cnt = HALMC_DDMA_POLLING_COUNT;
	while (HALMAC_REG_R32(REG_DDMA_CH0CTRL) & BIT_DDMACH0_OWN) {
		cnt--;
		if (cnt == 0) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]ch0 ready!!\n");
			return HALMAC_RET_DDMA_FAIL;
		}
	}

	ch0_ctrl |= (len & BIT_MASK_DDMACH0_DLEN);
	if (first == 0)
		ch0_ctrl |= BIT_DDMACH0_CHKSUM_CONT;

	if (halmac_iddma_en_88xx(adapter, src, dest, ch0_ctrl) !=
	    HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]iddma en!!\n");
		return HALMAC_RET_DDMA_FAIL;
	}

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_iddma_en_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 src,
	IN u32 dest,
	IN u32 ctrl
)
{
	u32 cnt = HALMC_DDMA_POLLING_COUNT;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	HALMAC_REG_W32(REG_DDMA_CH0SA, src);
	HALMAC_REG_W32(REG_DDMA_CH0DA, dest);
	HALMAC_REG_W32(REG_DDMA_CH0CTRL, ctrl);

	while (HALMAC_REG_R32(REG_DDMA_CH0CTRL) & BIT_DDMACH0_OWN) {
		cnt--;
		if (cnt == 0)
			return HALMAC_RET_DDMA_FAIL;
	}

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_check_fw_chksum_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 mem_addr
)
{
	u8 fw_ctrl;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	fw_ctrl = HALMAC_REG_R8(REG_MCUFW_CTRL);

	if (HALMAC_REG_R32(REG_DDMA_CH0CTRL) & BIT_DDMACH0_CHKSUM_STS) {
		if (mem_addr < HALMAC_OCPBASE_DMEM_88XX) {
			fw_ctrl |= BIT_IMEM_DW_OK;
			fw_ctrl &= ~BIT_IMEM_CHKSUM_OK;
			HALMAC_REG_W8(REG_MCUFW_CTRL, fw_ctrl);
		} else {
			fw_ctrl |= BIT_DMEM_DW_OK;
			fw_ctrl &= ~BIT_DMEM_CHKSUM_OK;
			HALMAC_REG_W8(REG_MCUFW_CTRL, fw_ctrl);
		}

		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]fw chksum!!\n");

		return HALMAC_RET_FW_CHECKSUM_FAIL;
	}

	if (mem_addr < HALMAC_OCPBASE_DMEM_88XX) {
		fw_ctrl |= (BIT_IMEM_DW_OK | BIT_IMEM_CHKSUM_OK);
		HALMAC_REG_W8(REG_MCUFW_CTRL, fw_ctrl);
	} else {
		fw_ctrl |= (BIT_DMEM_DW_OK | BIT_DMEM_CHKSUM_OK);
		HALMAC_REG_W8(REG_MCUFW_CTRL, fw_ctrl);
	}

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_check_fw_status_88xx() -check fw status
 * @adapter : the adapter of halmac
 * @status : fw status
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_check_fw_status_88xx(
	IN struct halmac_adapter *adapter,
	OUT u8 *fw_status
)
{
	u32 cnt;
	u32 fw_dbg6;
	u32 fw_pc;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	*fw_status = _TRUE;

	fw_dbg6 = HALMAC_REG_R32(REG_FW_DBG6);

	if (fw_dbg6 != 0) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]REG_FW_DBG6 !=0\n");
		if ((fw_dbg6 & FW_STATUS_CHK_WARN) != 0)
			PLTFM_MSG_PRINT(HALMAC_DBG_WARN,
					"[WARN]fw status(warn):%X\n", fw_dbg6);

		if ((fw_dbg6 & FW_STATUS_CHK_ERR) != 0)
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
					"[ERR]fw status(err):%X\n", fw_dbg6);

		if ((fw_dbg6 & FW_STATUS_CHK_FATAL) != 0) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
					"[ERR]fw status(fatal):%X\n", fw_dbg6);
			halmac_fw_fatal_status_debug_88xx(adapter);
			*fw_status = _FALSE;
			return status;
		}
	}

	fw_pc = HALMAC_REG_R32(REG_FW_DBG7);
	cnt = 10;
	while (HALMAC_REG_R32(REG_FW_DBG7) == fw_pc) {
		cnt--;
		if (cnt == 0)
			break;
	}

	if (cnt == 0) {
		cnt = 200;
		while (HALMAC_REG_R32(REG_FW_DBG7) == fw_pc) {
			cnt--;
			if (cnt == 0) {
				PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]fw pc\n");
				*fw_status = _FALSE;
				return status;
			}
			PLTFM_DELAY_US(50);
		}
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return status;
}

static void
halmac_fw_fatal_status_debug_88xx(
	IN struct halmac_adapter *adapter
)
{
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]0x%X = %X\n", REG_FW_DBG6,
			HALMAC_REG_R32(REG_FW_DBG6));

	PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]0x%X = %X\n", REG_ARFR5,
			HALMAC_REG_R32(REG_ARFR5));

	PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]0x%X = %X\n", REG_MCUTST_I,
			HALMAC_REG_R32(REG_MCUTST_I));
}

enum halmac_ret_status
halmac_dump_fw_dmem_88xx(
	IN struct halmac_adapter *adapter,
	INOUT u8 *dmem,
	INOUT u32 *size
)
{
	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_cfg_max_dl_size_88xx() - config max download FW size
 * @adapter : the adapter of halmac
 * @size : max download fw size
 *
 * Halmac uses this setting to set max packet size for
 * download FW.
 * If user has not called this API, halmac use default
 * setting for download FW
 * Note1 : size need multiple of 2
 * Note2 : max size is 31K
 *
 * Author : Ivan Lin/KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_cfg_max_dl_size_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 size
)
{
	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (size > HALMAC_FW_CFG_MAX_DL_SIZE_MAX_88XX) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]size > max dl size!\n");
		return HALMAC_RET_CFG_DLFW_SIZE_FAIL;
	}

	if ((size & (2 - 1)) != 0) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]not multiple of 2!\n");
		return HALMAC_RET_CFG_DLFW_SIZE_FAIL;
	}

	adapter->max_download_size = size;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Cfg max size:%X\n", size);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_enter_cpu_sleep_mode_88xx() -wlan cpu enter sleep mode
 * @adapter : the adapter of halmac
 * Author : Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_enter_cpu_sleep_mode_88xx(
	IN struct halmac_adapter *adapter
)
{
	u32 cnt;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	enum halmac_wlcpu_mode *cur_mode = &adapter->halmac_state.wlcpu_mode;

	if (halmac_fw_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_NO_DLFW;

	if (*cur_mode != HALMAC_WLCPU_ACTIVE)
		return HALMAC_RET_ERROR_STATE;

	cnt = 100;
	while (HALMAC_REG_R8(REG_HMETFR + 3) != 0) {
		cnt--;
		if (cnt == 0) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]0x1CF != 0\n");
			return HALMAC_RET_STATE_INCORRECT;
		}
		PLTFM_DELAY_US(50);
	}

	HALMAC_REG_W8(REG_HMETFR + 3, ID_INFORM_ENETR_CPU_SLEEP);

	*cur_mode = HALMAC_WLCPU_ENTER_SLEEP;

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_get_cpu_mode_88xx() -get wlcpu mode
 * @adapter : the adapter of halmac
 * @mode : cpu mode
 * Author : Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_get_cpu_mode_88xx(
	IN struct halmac_adapter *adapter,
	OUT enum halmac_wlcpu_mode *mode
)
{
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	enum halmac_wlcpu_mode *cur_mode = &adapter->halmac_state.wlcpu_mode;

	if (halmac_fw_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_NO_DLFW;

	if (*cur_mode == HALMAC_WLCPU_ACTIVE) {
		*mode = HALMAC_WLCPU_ACTIVE;
		return HALMAC_RET_SUCCESS;
	}

	if (*cur_mode == HALMAC_WLCPU_SLEEP) {
		*mode = HALMAC_WLCPU_SLEEP;
		return HALMAC_RET_SUCCESS;
	}

	if (HALMAC_REG_R8(REG_MCU_TST_CFG) == ID_CHECK_ENETR_CPU_SLEEP)
		*mode = HALMAC_WLCPU_SLEEP;
	else
		*mode = HALMAC_WLCPU_ENTER_SLEEP;

	HALMAC_REG_W8(REG_MCU_TST_CFG, 0);

	return HALMAC_RET_SUCCESS;
}

#endif /* HALMAC_88XX_SUPPORT */
