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

#include "halmac_sdio_8821c.h"
#include "halmac_pwr_seq_8821c.h"
#include "../halmac_init_88xx.h"
#include "../halmac_common_88xx.h"

#if HALMAC_8821C_SUPPORT

static enum halmac_ret_status
halmac_check_oqt_8821c(
	IN struct halmac_adapter *adapter,
	IN u32 tx_agg_num,
	IN u8 *buf,
	IN u8 *macid_cnt
);

static enum halmac_ret_status
halmac_update_oqt_free_space_8821c(
	IN struct halmac_adapter *adapter
);

static enum halmac_ret_status
halmac_update_sdio_free_page_8821c(
	IN struct halmac_adapter *adapter
);

static enum halmac_ret_status
halmac_check_qsel_8821c(
	IN struct halmac_adapter *adapter,
	IN u8 qsel_first,
	IN u8 *pkt,
	INOUT u8 *macid_cnt
);

static enum halmac_ret_status
halmac_check_dma_mapping_8821c(
	IN struct halmac_adapter *adapter,
	INOUT u16 *cur_free_space,
	IN u8 qsel_first
);

static enum halmac_ret_status
halmac_check_required_page_num_8821c(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	INOUT u32 *rqd_pg_num,
	INOUT u16 *cur_free_space,
	INOUT u8 *macid_cnt,
	IN u32 tx_agg_num
);

/**
 * halmac_mac_power_switch_8821c_sdio() - switch mac power
 * @adapter : the adapter of halmac
 * @pwr : power state
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_mac_power_switch_8821c_sdio(
	IN struct halmac_adapter *adapter,
	IN enum halmac_mac_power pwr
)
{
	u8 intf_mask;
	u8 value8;
	u8 rpwm;
	u32 imr_backup;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]halmac_mac_power_switch_88xx_sdio pwr\n");
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%x\n", pwr);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]8821C pwr seq ver = %s\n", HALMAC_8821C_PWR_SEQ_VER);

	intf_mask = HALMAC_PWR_INTF_SDIO_MSK;

	adapter->rpwm_record = HALMAC_REG_R8(REG_SDIO_HRPWM1);

	/* Check FW still exist or not */
	if (HALMAC_REG_R16(REG_MCUFW_CTRL) == 0xC078) {
		/* Leave 32K */
		rpwm = (u8)((adapter->rpwm_record ^ BIT(7)) & 0x80);
		HALMAC_REG_W8(REG_SDIO_HRPWM1, rpwm);
	}

	value8 = HALMAC_REG_R8(REG_CR);
	if (value8 == 0xEA)
		adapter->halmac_state.mac_power = HALMAC_MAC_POWER_OFF;
	else
		adapter->halmac_state.mac_power = HALMAC_MAC_POWER_ON;

	/*Check if power switch is needed*/
	if (pwr == HALMAC_MAC_POWER_ON && adapter->halmac_state.mac_power == HALMAC_MAC_POWER_ON) {
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]halmac_mac_power_switch power state unchange!\n");
		return HALMAC_RET_PWR_UNCHANGE;
	}

	imr_backup = HALMAC_REG_R32(REG_SDIO_HIMR);
	HALMAC_REG_W32(REG_SDIO_HIMR, 0);

	if (pwr == HALMAC_MAC_POWER_OFF) {
		if (halmac_pwr_seq_parser_88xx(adapter, HALMAC_PWR_CUT_ALL_MSK, HALMAC_PWR_FAB_TSMC_MSK,
					       intf_mask, halmac_8821c_card_disable_flow) != HALMAC_RET_SUCCESS) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Handle power off cmd error\n");
			HALMAC_REG_W32(REG_SDIO_HIMR, imr_backup);
			return HALMAC_RET_POWER_OFF_FAIL;
		}

		adapter->halmac_state.mac_power = HALMAC_MAC_POWER_OFF;
		adapter->halmac_state.dlfw_state = HALMAC_DLFW_NONE;
		halmac_init_adapter_dynamic_para_88xx(adapter);
	} else {
		if (halmac_pwr_seq_parser_88xx(adapter, HALMAC_PWR_CUT_ALL_MSK, HALMAC_PWR_FAB_TSMC_MSK,
					       intf_mask, halmac_8821c_card_enable_flow) != HALMAC_RET_SUCCESS) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Handle power on cmd error\n");
			HALMAC_REG_W32(REG_SDIO_HIMR, imr_backup);
			return HALMAC_RET_POWER_ON_FAIL;
		}

		adapter->halmac_state.mac_power = HALMAC_MAC_POWER_ON;
	}

	HALMAC_REG_W32(REG_SDIO_HIMR, imr_backup);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_tx_allowed_sdio_88xx() - check tx status
 * @adapter : the adapter of halmac
 * @buf : tx packet, include txdesc
 * @size : tx packet size, include txdesc
 * Author : Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_tx_allowed_8821c_sdio(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
)
{
	u16 cur_free_space = 0;
	u32 cnt;
	u32 tx_agg_num;
	u32 rqd_pg_num = 0;
	u8 macid_cnt = 0;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (!adapter->sdio_free_space.macid_map) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]halmac allocate Macid_map Fail!!\n");
		return HALMAC_RET_MALLOC_FAIL;
	}

	PLTFM_MEMSET(adapter->sdio_free_space.macid_map, 0x00, adapter->sdio_free_space.macid_map_size);

	tx_agg_num = GET_TX_DESC_DMA_TXAGG_NUM(buf);
	tx_agg_num = (tx_agg_num == 0) ? 1 : tx_agg_num;

	status = halmac_check_required_page_num_8821c(adapter, buf, &rqd_pg_num, &cur_free_space, &macid_cnt, tx_agg_num);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	cnt = 10;
	do {
		if ((u32)(cur_free_space + adapter->sdio_free_space.public_queue_number) > rqd_pg_num) {
			status = halmac_check_oqt_8821c(adapter, tx_agg_num, buf, &macid_cnt);
			if (status != HALMAC_RET_SUCCESS) {
				PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]oqt buffer full!!\n");
				return status;
			}

			if (cur_free_space >= rqd_pg_num) {
				cur_free_space -= (u16)rqd_pg_num;
			} else {
				adapter->sdio_free_space.public_queue_number -= (u16)(rqd_pg_num - cur_free_space);
				cur_free_space = 0;
			}

			break;
		}

		halmac_update_sdio_free_page_8821c(adapter);

		cnt--;
		if (cnt == 0)
			return HALMAC_RET_FREE_SPACE_NOT_ENOUGH;
	} while (1);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_check_oqt_8821c(
	IN struct halmac_adapter *adapter,
	IN u32 tx_agg_num,
	IN u8 *buf,
	IN u8 *macid_cnt
)
{
	u32 cnt = 10;

	/*S0, S1 are not allowed to use, 0x4E4[0] should be 0. Soar 20160323*/
	/*no need to check non_ac_oqt_number. HI and MGQ blocked will cause protocal issue before H_OQT being full*/
	switch ((enum halmac_queue_select)GET_TX_DESC_QSEL(buf)) {
	case HALMAC_QUEUE_SELECT_VO:
	case HALMAC_QUEUE_SELECT_VO_V2:
	case HALMAC_QUEUE_SELECT_VI:
	case HALMAC_QUEUE_SELECT_VI_V2:
	case HALMAC_QUEUE_SELECT_BE:
	case HALMAC_QUEUE_SELECT_BE_V2:
	case HALMAC_QUEUE_SELECT_BK:
	case HALMAC_QUEUE_SELECT_BK_V2:
		if ((*macid_cnt > HALMAC_ACQ_NUM_MAX_8821C) && (tx_agg_num > HALMAC_OQT_ENTRY_AC_8821C))
			PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]tx_agg_num %d > HALMAC_OQT_ENTRY_AC_8821C, macid_cnt %d > HALMAC_ACQ_NUM_MAX_8821C\n", tx_agg_num, *macid_cnt);

		cnt = 10;
		do {
			if (adapter->sdio_free_space.ac_empty >= *macid_cnt) {
				adapter->sdio_free_space.ac_empty -= *macid_cnt;
				break;
			}

			if (adapter->sdio_free_space.ac_oqt_number >= tx_agg_num) {
				adapter->sdio_free_space.ac_empty = 0;
				adapter->sdio_free_space.ac_oqt_number -= (u8)tx_agg_num;
				break;
			}

			halmac_update_oqt_free_space_8821c(adapter);

			cnt--;
			if (cnt == 0)
				return HALMAC_RET_OQT_NOT_ENOUGH;
		} while (1);
		break;
	case HALMAC_QUEUE_SELECT_MGNT:
	case HALMAC_QUEUE_SELECT_HIGH:
		if (tx_agg_num > HALMAC_OQT_ENTRY_NOAC_8821C)
			PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]tx_agg_num %d > HALMAC_OQT_ENTRY_NOAC_8821C %d\n", tx_agg_num, HALMAC_OQT_ENTRY_NOAC_8821C);
		cnt = 10;
		do {
			if (adapter->sdio_free_space.non_ac_oqt_number >= tx_agg_num) {
				adapter->sdio_free_space.non_ac_oqt_number -= (u8)tx_agg_num;
				break;
			}

			halmac_update_oqt_free_space_8821c(adapter);

			cnt--;
			if (cnt == 0)
				return HALMAC_RET_OQT_NOT_ENOUGH;
		} while (1);
		break;
	default:
		break;
	}

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_update_oqt_free_space_8821c(
	IN struct halmac_adapter *adapter
)
{
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	struct halmac_sdio_free_space *free_space;
	u8 value;
	u32 oqt_free_page;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	free_space = &adapter->sdio_free_space;

	oqt_free_page = HALMAC_REG_R32(REG_SDIO_OQT_FREE_TXPG_V1);
	free_space->ac_oqt_number = (u8)BIT_GET_AC_OQT_FREEPG_V1(oqt_free_page);
	free_space->non_ac_oqt_number = (u8)BIT_GET_NOAC_OQT_FREEPG_V1(oqt_free_page);
	free_space->ac_empty = 0;
	if (free_space->ac_oqt_number == HALMAC_OQT_ENTRY_AC_8821C) {
		value = HALMAC_REG_R8(REG_TXPKT_EMPTY);
		while (value > 0) {
			value = value & (value - 1);
			free_space->ac_empty++;
		};
	} else {
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]free_space->ac_oqt_number %d != %d\n", free_space->ac_oqt_number, HALMAC_OQT_ENTRY_AC_8821C);
	}
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_update_sdio_free_page_8821c(
	IN struct halmac_adapter *adapter
)
{
	u32 free_page = 0;
	u32 free_page2 = 0;
	u32 free_page3 = 0;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	struct halmac_sdio_free_space *free_space;
	u8 data[12] = {0};

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	free_space = &adapter->sdio_free_space;

	HALMAC_REG_SDIO_RN(REG_SDIO_FREE_TXPG, 12, data);

	free_page = rtk_le32_to_cpu(*(u32 *)(data + 0));
	free_page2 = rtk_le32_to_cpu(*(u32 *)(data + 4));
	free_page3 = rtk_le32_to_cpu(*(u32 *)(data + 8));

	free_space->high_queue_number = (u16)BIT_GET_HIQ_FREEPG_V1(free_page);
	free_space->normal_queue_number = (u16)BIT_GET_MID_FREEPG_V1(free_page);
	free_space->low_queue_number = (u16)BIT_GET_LOW_FREEPG_V1(free_page2);
	free_space->public_queue_number = (u16)BIT_GET_PUB_FREEPG_V1(free_page2);
	free_space->extra_queue_number = (u16)BIT_GET_EXQ_FREEPG_V1(free_page3);
	free_space->ac_oqt_number = (u8)BIT_GET_AC_OQT_FREEPG_V1(free_page3);
	free_space->non_ac_oqt_number = (u8)BIT_GET_NOAC_OQT_FREEPG_V1(free_page3);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_phy_cfg_8821c_sdio() - phy config
 * @adapter : the adapter of halmac
 * Author : KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_phy_cfg_8821c_sdio(
	IN struct halmac_adapter *adapter,
	IN enum halmac_intf_phy_platform pltfm
)
{
	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_pcie_switch_8821c() - pcie gen1/gen2 switch
 * @adapter : the adapter of halmac
 * @cfg : gen1/gen2 selection
 * Author : KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_pcie_switch_8821c_sdio(
	IN struct halmac_adapter *adapter,
	IN enum halmac_pcie_cfg	cfg
)
{
	return HALMAC_RET_NOT_SUPPORT;
}

/**
 * halmac_interface_integration_tuning_8821c_sdio() - sdio interface fine tuning
 * @adapter : the adapter of halmac
 * Author : Ivan
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_interface_integration_tuning_8821c_sdio(
	IN struct halmac_adapter *adapter
)
{
	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_get_sdio_tx_addr_sdio_88xx() - get CMD53 addr for the TX packet
 * @adapter : the adapter of halmac
 * @buf : tx packet, include txdesc
 * @size : tx packet size
 * @cmd53_addr : cmd53 addr value
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_get_sdio_tx_addr_8821c_sdio(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size,
	OUT u32 *cmd53_addr
)
{
	u32 len_unit4;
	enum halmac_queue_select queue_sel;
	enum halmac_dma_mapping dma_mapping;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (!buf) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]buf is NULL!!\n");
		return HALMAC_RET_DATA_BUF_NULL;
	}

	if (size == 0) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]size is 0!!\n");
		return HALMAC_RET_DATA_SIZE_INCORRECT;
	}

	queue_sel = (enum halmac_queue_select)GET_TX_DESC_QSEL(buf);

	switch (queue_sel) {
	case HALMAC_QUEUE_SELECT_VO:
	case HALMAC_QUEUE_SELECT_VO_V2:
		dma_mapping = adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_VO];
		break;
	case HALMAC_QUEUE_SELECT_VI:
	case HALMAC_QUEUE_SELECT_VI_V2:
		dma_mapping = adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_VI];
		break;
	case HALMAC_QUEUE_SELECT_BE:
	case HALMAC_QUEUE_SELECT_BE_V2:
		dma_mapping = adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_BE];
		break;
	case HALMAC_QUEUE_SELECT_BK:
	case HALMAC_QUEUE_SELECT_BK_V2:
		dma_mapping = adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_BK];
		break;
	case HALMAC_QUEUE_SELECT_MGNT:
		dma_mapping = adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_MG];
		break;
	case HALMAC_QUEUE_SELECT_HIGH:
	case HALMAC_QUEUE_SELECT_BCN:
	case HALMAC_QUEUE_SELECT_CMD:
		dma_mapping = adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_HI];
		break;
	default:
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Qsel is out of range\n");
		return HALMAC_RET_QSEL_INCORRECT;
	}

	len_unit4 = (size >> 2) + ((size & (4 - 1)) ? 1 : 0);

	switch (dma_mapping) {
	case HALMAC_DMA_MAPPING_HIGH:
		*cmd53_addr = HALMAC_SDIO_CMD_ADDR_TXFF_HIGH;
		break;
	case HALMAC_DMA_MAPPING_NORMAL:
		*cmd53_addr = HALMAC_SDIO_CMD_ADDR_TXFF_NORMAL;
		break;
	case HALMAC_DMA_MAPPING_LOW:
		*cmd53_addr = HALMAC_SDIO_CMD_ADDR_TXFF_LOW;
		break;
	case HALMAC_DMA_MAPPING_EXTRA:
		*cmd53_addr = HALMAC_SDIO_CMD_ADDR_TXFF_EXTRA;
		break;
	default:
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]DmaMapping is out of range\n");
		return HALMAC_RET_DMA_MAP_INCORRECT;
	}

	*cmd53_addr = (*cmd53_addr << 13) | (len_unit4 & HALMAC_SDIO_4BYTE_LEN_MASK);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

enum halmac_ret_status
halmac_check_qsel_8821c(
	IN struct halmac_adapter *adapter,
	IN u8 qsel_first,
	IN u8 *pkt,
	INOUT u8 *macid_cnt
)
{
	u8 flag = 0;
	u8 qsel_now;
	u8 macid;

	macid = (u8)GET_TX_DESC_MACID(pkt);
	qsel_now = (u8)GET_TX_DESC_QSEL(pkt);
	if (qsel_first == qsel_now) {
		if (*(adapter->sdio_free_space.macid_map + macid) == 0) {
			*(adapter->sdio_free_space.macid_map + macid) = 1;
			(*macid_cnt)++;
		}
	} else {
		switch ((enum halmac_queue_select)qsel_now) {
		case HALMAC_QUEUE_SELECT_VO:
			if ((enum halmac_queue_select)qsel_first != HALMAC_QUEUE_SELECT_VO_V2)
				flag = 1;
			break;
		case HALMAC_QUEUE_SELECT_VO_V2:
			if ((enum halmac_queue_select)qsel_first != HALMAC_QUEUE_SELECT_VO)
				flag = 1;
			break;
		case HALMAC_QUEUE_SELECT_VI:
			if ((enum halmac_queue_select)qsel_first != HALMAC_QUEUE_SELECT_VI_V2)
				flag = 1;
			break;
		case HALMAC_QUEUE_SELECT_VI_V2:
			if ((enum halmac_queue_select)qsel_first != HALMAC_QUEUE_SELECT_VI)
				flag = 1;
			break;
		case HALMAC_QUEUE_SELECT_BE:
			if ((enum halmac_queue_select)qsel_first != HALMAC_QUEUE_SELECT_BE_V2)
				flag = 1;
			break;
		case HALMAC_QUEUE_SELECT_BE_V2:
			if ((enum halmac_queue_select)qsel_first != HALMAC_QUEUE_SELECT_BE)
				flag = 1;
			break;
		case HALMAC_QUEUE_SELECT_BK:
			if ((enum halmac_queue_select)qsel_first != HALMAC_QUEUE_SELECT_BK_V2)
				flag = 1;
			break;
		case HALMAC_QUEUE_SELECT_BK_V2:
			if ((enum halmac_queue_select)qsel_first != HALMAC_QUEUE_SELECT_BK)
				flag = 1;
			break;
		case HALMAC_QUEUE_SELECT_MGNT:
		case HALMAC_QUEUE_SELECT_HIGH:
		case HALMAC_QUEUE_SELECT_BCN:
		case HALMAC_QUEUE_SELECT_CMD:
			flag = 1;
			break;
		default:
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Qsel is out of range: %d\n", qsel_first);
			return HALMAC_RET_QSEL_INCORRECT;
		}
		if (flag == 1) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Multi-Qsel in a bus agg is not allowed, qsel = %d, %d\n", qsel_first, qsel_now);
			return HALMAC_RET_QSEL_INCORRECT;
		}
		if (*(adapter->sdio_free_space.macid_map + macid + HALMAC_MACID_MAX_8821C) == 0) {
			*(adapter->sdio_free_space.macid_map + macid + HALMAC_MACID_MAX_8821C) = 1;
			(*macid_cnt)++;
		}
	}

	return HALMAC_RET_SUCCESS;
}

enum halmac_ret_status
halmac_check_dma_mapping_8821c(
	IN struct halmac_adapter *adapter,
	INOUT u16 *cur_free_space,
	IN u8 qsel_first
)
{
	enum halmac_dma_mapping dma_mapping;

	switch ((enum halmac_queue_select)qsel_first) {
	case HALMAC_QUEUE_SELECT_VO:
	case HALMAC_QUEUE_SELECT_VO_V2:
		dma_mapping = adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_VO];
		break;
	case HALMAC_QUEUE_SELECT_VI:
	case HALMAC_QUEUE_SELECT_VI_V2:
		dma_mapping = adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_VI];
		break;
	case HALMAC_QUEUE_SELECT_BE:
	case HALMAC_QUEUE_SELECT_BE_V2:
		dma_mapping = adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_BE];
		break;
	case HALMAC_QUEUE_SELECT_BK:
	case HALMAC_QUEUE_SELECT_BK_V2:
		dma_mapping = adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_BK];
		break;
	case HALMAC_QUEUE_SELECT_MGNT:
		dma_mapping = adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_MG];
		break;
	case HALMAC_QUEUE_SELECT_HIGH:
		dma_mapping = adapter->halmac_ptcl_queue[HALMAC_PTCL_QUEUE_HI];
		break;
	case HALMAC_QUEUE_SELECT_BCN:
	case HALMAC_QUEUE_SELECT_CMD:
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[ERR]QSEL = %d. BCN/CMD always return HALMAC_RET_SUCCESS\n", qsel_first);
		return HALMAC_RET_SUCCESS;
	default:
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Qsel is out of range: %d\n", qsel_first);
		return HALMAC_RET_QSEL_INCORRECT;
	}

	switch (dma_mapping) {
	case HALMAC_DMA_MAPPING_HIGH:
		cur_free_space = &adapter->sdio_free_space.high_queue_number;
		break;
	case HALMAC_DMA_MAPPING_NORMAL:
		cur_free_space = &adapter->sdio_free_space.normal_queue_number;
		break;
	case HALMAC_DMA_MAPPING_LOW:
		cur_free_space = &adapter->sdio_free_space.low_queue_number;
		break;
	case HALMAC_DMA_MAPPING_EXTRA:
		cur_free_space = &adapter->sdio_free_space.extra_queue_number;
		break;
	default:
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]DmaMapping is out of range\n");
		return HALMAC_RET_DMA_MAP_INCORRECT;
	}

	return HALMAC_RET_SUCCESS;
}

enum halmac_ret_status
halmac_check_required_page_num_8821c(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	INOUT u32 *rqd_pg_num,
	INOUT u16 *cur_free_space,
	INOUT u8 *macid_cnt,
	IN u32 tx_agg_num
)
{
	u8 *pkt;
	u8 qsel_first;
	u32 i, pkt_size;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	pkt = buf;

	qsel_first = (u8)GET_TX_DESC_QSEL(pkt);

	status = halmac_check_dma_mapping_8821c(adapter, cur_free_space, qsel_first);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	for (i = 0; i < tx_agg_num; i++) {
		/*QSEL parser*/
		status = halmac_check_qsel_8821c(adapter, qsel_first, pkt, macid_cnt);
		if (status != HALMAC_RET_SUCCESS)
			return status;

		/*Page number parser*/
		pkt_size = GET_TX_DESC_TXPKTSIZE(pkt) + GET_TX_DESC_OFFSET(pkt);
		*rqd_pg_num += (pkt_size >> adapter->hw_cfg_info.page_size_2_power) + ((pkt_size & (adapter->hw_cfg_info.page_size - 1)) ? 1 : 0);

		pkt += HALMAC_ALIGN(GET_TX_DESC_TXPKTSIZE(pkt) + (GET_TX_DESC_PKT_OFFSET(pkt) << 3) + adapter->hw_cfg_info.txdesc_size, 8);
	}

	return HALMAC_RET_SUCCESS;
}

#endif /* HALMAC_8821C_SUPPORT */
