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

#include "halmac_sdio_88xx.h"
#include "halmac_88xx_cfg.h"

#if HALMAC_88XX_SUPPORT

/**
 * halmac_init_sdio_cfg_88xx() - init SDIO
 * @adapter : the adapter of halmac
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_init_sdio_cfg_88xx(
	IN struct halmac_adapter *adapter
)
{
	u32 value32;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (adapter->halmac_interface != HALMAC_INTERFACE_SDIO)
		return HALMAC_RET_WRONG_INTF;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	HALMAC_REG_R32(REG_SDIO_FREE_TXPG);

	value32 = HALMAC_REG_R32(REG_SDIO_TX_CTRL) & 0xFFFF;
	value32 &= ~(BIT_CMD_ERR_STOP_INT_EN | BIT_EN_MASK_TIMER |
							BIT_EN_RXDMA_MASK_INT);
	HALMAC_REG_W32(REG_SDIO_TX_CTRL, value32);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_deinit_sdio_cfg_88xx() - deinit SDIO
 * @adapter : the adapter of halmac
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_deinit_sdio_cfg_88xx(
	IN struct halmac_adapter *adapter
)
{
	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (adapter->halmac_interface != HALMAC_INTERFACE_SDIO)
		return HALMAC_RET_WRONG_INTF;

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_cfg_rx_aggregation_88xx_sdio() - config rx aggregation
 * @adapter : the adapter of halmac
 * @halmac_rx_agg_mode
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_cfg_rx_aggregation_88xx_sdio(
	IN struct halmac_adapter *adapter,
	IN struct halmac_rxagg_cfg *cfg
)
{
	u8 value8;
	u8 size;
	u8 timeout;
	u8 agg_enable;
	u32 value32;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	agg_enable = HALMAC_REG_R8(REG_TXDMA_PQ_MAP);

	switch (cfg->mode) {
	case HALMAC_RX_AGG_MODE_NONE:
		agg_enable &= ~(BIT_RXDMA_AGG_EN);
		break;
	case HALMAC_RX_AGG_MODE_DMA:
	case HALMAC_RX_AGG_MODE_USB:
		agg_enable |= BIT_RXDMA_AGG_EN;
		break;
	default:
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]unsupported mode\n");
		agg_enable &= ~BIT_RXDMA_AGG_EN;
		break;
	}

	if (cfg->threshold.drv_define == _FALSE) {
		size = 0xFF;
		timeout = 0x01;
	} else {
		size = cfg->threshold.size;
		timeout = cfg->threshold.timeout;
	}

	value32 = HALMAC_REG_R32(REG_RXDMA_AGG_PG_TH);
	if (cfg->threshold.size_limit_en == _FALSE)
		HALMAC_REG_W32(REG_RXDMA_AGG_PG_TH, value32 & ~BIT_EN_PRE_CALC);
	else
		HALMAC_REG_W32(REG_RXDMA_AGG_PG_TH, value32 | BIT_EN_PRE_CALC);

	HALMAC_REG_W8(REG_TXDMA_PQ_MAP, agg_enable);
	HALMAC_REG_W16(REG_RXDMA_AGG_PG_TH,
		       (u16)(size | (timeout << BIT_SHIFT_DMA_AGG_TO_V1)));

	value8 = HALMAC_REG_R8(REG_RXDMA_MODE);
	if (0 != (agg_enable & BIT_RXDMA_AGG_EN))
		HALMAC_REG_W8(REG_RXDMA_MODE, value8 | BIT_DMA_MODE);
	else
		HALMAC_REG_W8(REG_RXDMA_MODE, value8 & ~(BIT_DMA_MODE));

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_reg_read_8_sdio_88xx() - read 1byte register
 * @adapter : the adapter of halmac
 * @offset : register offset
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
u8
halmac_reg_read_8_sdio_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset
)
{
	u8 value8;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (0 == (offset & 0xFFFF0000))
		offset |= WLAN_IOREG_OFFSET;

	status = halmac_convert_to_sdio_bus_offset_88xx(adapter, &offset);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]convert offset\n");
		return status;
	}

	value8 = PLTFM_SDIO_CMD52_R(offset);

	return value8;
}

/**
 * halmac_reg_write_8_sdio_88xx() - write 1byte register
 * @adapter : the adapter of halmac
 * @offset : register offset
 * @value : register value
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_reg_write_8_sdio_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u8 value
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (0 == (offset & 0xFFFF0000))
		offset |= WLAN_IOREG_OFFSET;

	status = halmac_convert_to_sdio_bus_offset_88xx(adapter, &offset);

	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]convert offset\n");
		return status;
	}

	PLTFM_SDIO_CMD52_W(offset, value);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_reg_read_16_sdio_88xx() - read 2byte register
 * @adapter : the adapter of halmac
 * @offset : register offset
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
u16
halmac_reg_read_16_sdio_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset
)
{
	u32 offset_old = 0;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	union {
		u16 word;
		u8 byte[2];
	} value16 = { 0x0000 };

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	offset_old = offset;

	if ((offset & 0xFFFF0000) == 0)
		offset |= WLAN_IOREG_OFFSET;

	status = halmac_convert_to_sdio_bus_offset_88xx(adapter, &offset);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]convert offset\n");
		return status;
	}

	if ((adapter->halmac_state.mac_power == HALMAC_MAC_POWER_OFF) ||
	    ((offset & (2 - 1)) != 0) ||
	    (adapter->sdio_cmd53_4byte == HALMAC_SDIO_CMD53_4BYTE_MODE_RW) ||
	    (adapter->sdio_cmd53_4byte == HALMAC_SDIO_CMD53_4BYTE_MODE_R)) {
		value16.byte[0] = PLTFM_SDIO_CMD52_R(offset);
		value16.byte[1] = PLTFM_SDIO_CMD52_R(offset + 1);
		value16.word = rtk_le16_to_cpu(value16.word);
	} else {
		if ((adapter->sdio_hw_info.io_hi_speed_flag != 0) &&
		    (adapter->sdio_hw_info.io_wait_ready_flag == 0)) {
			if ((offset_old & 0xffffef00) == 0x00000000) {
				value16.byte[0] = PLTFM_SDIO_CMD52_R(offset);
				value16.byte[1] = PLTFM_SDIO_CMD52_R(offset + 1);
				value16.word = rtk_le16_to_cpu(value16.word);
			} else {
				value16.word = PLTFM_SDIO_CMD53_R16(offset);
			}
		} else {
			value16.word = PLTFM_SDIO_CMD53_R16(offset);
		}
	}

	return value16.word;
}

/**
 * halmac_reg_write_16_sdio_88xx() - write 2byte register
 * @adapter : the adapter of halmac
 * @offset : register offset
 * @value : register value
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_reg_write_16_sdio_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u16 value
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (0 == (offset & 0xFFFF0000))
		offset |= WLAN_IOREG_OFFSET;

	status = halmac_convert_to_sdio_bus_offset_88xx(adapter, &offset);

	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]convert offset\n");
		return status;
	}

	if ((adapter->halmac_state.mac_power == HALMAC_MAC_POWER_OFF) ||
	    ((offset & (2 - 1)) != 0) ||
	    (adapter->sdio_cmd53_4byte == HALMAC_SDIO_CMD53_4BYTE_MODE_RW) ||
	    (adapter->sdio_cmd53_4byte == HALMAC_SDIO_CMD53_4BYTE_MODE_W)) {
		PLTFM_SDIO_CMD52_W(offset, (u8)(value & 0xFF));
		PLTFM_SDIO_CMD52_W(offset + 1, (u8)((value & 0xFF00) >> 8));
	} else {
		PLTFM_SDIO_CMD53_W16(offset, value);
	}

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_reg_read_32_sdio_88xx() - read 4byte register
 * @adapter : the adapter of halmac
 * @offset : register offset
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
u32
halmac_reg_read_32_sdio_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	u32 offset_old = 0;
	union {
		u32 dword;
		u8 byte[4];
	} value32 = { 0x00000000 };

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	offset_old = offset;

	if (0 == (offset & 0xFFFF0000))
		offset |= WLAN_IOREG_OFFSET;

	status = halmac_convert_to_sdio_bus_offset_88xx(adapter, &offset);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]convert offset\n");
		return status;
	}

	if (adapter->halmac_state.mac_power == HALMAC_MAC_POWER_OFF ||
	    (offset & (4 - 1)) != 0) {
		value32.byte[0] = PLTFM_SDIO_CMD52_R(offset);
		value32.byte[1] = PLTFM_SDIO_CMD52_R(offset + 1);
		value32.byte[2] = PLTFM_SDIO_CMD52_R(offset + 2);
		value32.byte[3] = PLTFM_SDIO_CMD52_R(offset + 3);
		value32.dword = rtk_le32_to_cpu(value32.dword);
	} else {
		if ((adapter->sdio_hw_info.io_hi_speed_flag != 0) &&
		    (adapter->sdio_hw_info.io_wait_ready_flag == 0)) {
			if ((offset_old & 0xffffef00) == 0x00000000) {
				value32.byte[0] = PLTFM_SDIO_CMD52_R(offset);
				value32.byte[1] = PLTFM_SDIO_CMD52_R(offset + 1);
				value32.byte[2] = PLTFM_SDIO_CMD52_R(offset + 2);
				value32.byte[3] = PLTFM_SDIO_CMD52_R(offset + 3);
				value32.dword = rtk_le32_to_cpu(value32.dword);
			} else {
				value32.dword = PLTFM_SDIO_CMD53_R32(offset);
			}
		} else {
			value32.dword = PLTFM_SDIO_CMD53_R32(offset);
		}
	}

	return value32.dword;
}

/**
 * halmac_reg_write_32_sdio_88xx() - write 4byte register
 * @adapter : the adapter of halmac
 * @offset : register offset
 * @value : register value
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_reg_write_32_sdio_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u32 value
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if ((offset & 0xFFFF0000) == 0)
		offset |= WLAN_IOREG_OFFSET;

	status = halmac_convert_to_sdio_bus_offset_88xx(adapter, &offset);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]convert offset\n");
		return status;
	}

	if (adapter->halmac_state.mac_power == HALMAC_MAC_POWER_OFF ||
	    (offset & (4 - 1)) !=  0) {
		PLTFM_SDIO_CMD52_W(offset, (u8)(value & 0xFF));
		PLTFM_SDIO_CMD52_W(offset + 1, (u8)((value & 0xFF00) >> 8));
		PLTFM_SDIO_CMD52_W(offset + 2, (u8)((value & 0xFF0000) >> 16));
		PLTFM_SDIO_CMD52_W(offset + 3, (u8)((value & 0xFF000000) >> 24));
	} else {
		PLTFM_SDIO_CMD53_W32(offset, value);
	}

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_reg_read_nbyte_sdio_88xx() - read n byte register
 * @adapter : the adapter of halmac
 * @offset : register offset
 * @halmac_size : register value size
 * @value : register value
 * Author : Soar
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_reg_read_nbyte_sdio_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u32 size,
	OUT u8 *value
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (0 == (offset & 0xFFFF0000)) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]offset 0x%x\n", offset);
		return HALMAC_RET_FAIL;
	}

	status = halmac_convert_to_sdio_bus_offset_88xx(adapter, &offset);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]convert offset\n");
		return status;
	}

	if (adapter->halmac_state.mac_power == HALMAC_MAC_POWER_OFF) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]power off\n");
		return HALMAC_RET_FAIL;
	}

	PLTFM_SDIO_CMD53_RN(offset, size, value);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_cfg_tx_agg_align_sdio_88xx() -config sdio bus tx agg alignment
 * @adapter : the adapter of halmac
 * @enable : function enable(1)/disable(0)
 * @align_size : sdio bus tx agg alignment size (2^n, n = 3~11)
 * Author : Soar Tu
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_cfg_tx_agg_align_sdio_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 enable,
	IN u16 align_size
)
{
	u8 i;
	u8 flag = 0;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (adapter->chip_id == HALMAC_CHIP_ID_8822B)
		return HALMAC_RET_NOT_SUPPORT;

	if ((align_size & 0xF000) != 0) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]out of range\n");
		return HALMAC_RET_FAIL;
	}

	for (i = 3; i <= 11; i++) {
		if (align_size == 1 << i) {
			flag = 1;
			break;
		}
	}

	if (flag == 0) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]not 2^3 ~ 2^11\n");
		return HALMAC_RET_FAIL;
	}

	adapter->hw_cfg_info.tx_align_size = align_size;

	if (enable)
		HALMAC_REG_W16(REG_RQPN_CTRL_2, 0x8000 | align_size);
	else
		HALMAC_REG_W16(REG_RQPN_CTRL_2, align_size);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_reg_read_indirect_32_sdio_88xx() - read MAC reg by SDIO reg
 * @adapter : the adapter of halmac
 * @offset : register offset
 * Author : Soar
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
u32
halmac_reg_read_indirect_32_sdio_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset
)
{
	u8 rtemp;
	u32 cnt = 1000;
	union {
		u32 dword;
		u8 byte[4];
	} value32 = { 0x00000000 };

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_SDIO_CMD53_W32(
		(HALMAC_SDIO_CMD_ADDR_SDIO_REG << 13) |
		(REG_SDIO_INDIRECT_REG_CFG & HALMAC_SDIO_LOCAL_MSK),
		offset | BIT(19) | BIT(17));

	do {
		rtemp = PLTFM_SDIO_CMD52_R(
			(HALMAC_SDIO_CMD_ADDR_SDIO_REG << 13) |
			((REG_SDIO_INDIRECT_REG_CFG + 2) &
			HALMAC_SDIO_LOCAL_MSK));
		cnt--;
	} while (((rtemp & BIT(4)) != 0) && (cnt > 0));

	value32.dword = PLTFM_SDIO_CMD53_R32(
			(HALMAC_SDIO_CMD_ADDR_SDIO_REG << 13) |
			(REG_SDIO_INDIRECT_REG_DATA & HALMAC_SDIO_LOCAL_MSK));

	return value32.dword;
}

/**
 * halmac_set_bulkout_num_sdio_88xx() - inform bulk-out num
 * @adapter : the adapter of halmac
 * @bulkout_num : usb bulk-out number
 * Author : KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_set_bulkout_num_sdio_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 num
)
{
	return HALMAC_RET_NOT_SUPPORT;
}

/**
 * halmac_get_usb_bulkout_id_sdio_88xx() - get bulk out id for the TX packet
 * @adapter : the adapter of halmac
 * @halmac_buf : tx packet, include txdesc
 * @halmac_size : tx packet size
 * @bulkout_id : usb bulk-out id
 * Author : KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_get_usb_bulkout_id_sdio_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size,
	OUT u8 *id
)
{
	return HALMAC_RET_NOT_SUPPORT;
}

/**
 * halmac_sdio_cmd53_4byte_88xx() - cmd53 only for 4byte len register IO
 * @adapter : the adapter of halmac
 * @enable : 1->CMD53 only use in 4byte reg, 0 : No limitation
 * Author : Ivan Lin/KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_sdio_cmd53_4byte_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_sdio_cmd53_4byte_mode mode
)
{
	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (adapter->halmac_interface != HALMAC_INTERFACE_SDIO)
		return HALMAC_RET_WRONG_INTF;

	if (adapter->api_registry.sdio_cmd53_4byte_en == 0)
		return HALMAC_RET_NOT_SUPPORT;

	adapter->sdio_cmd53_4byte = mode;

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_sdio_hw_info_88xx() - info sdio hw info
 * @adapter : the adapter of halmac
 * @HALMAC_SDIO_CMD53_4BYTE_MODE :
 * clock_speed : sdio bus clock. Unit -> MHz
 * spec_ver : sdio spec version
 * Author : Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_sdio_hw_info_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_sdio_hw_info *info
)
{
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (adapter->halmac_interface != HALMAC_INTERFACE_SDIO)
		return HALMAC_RET_WRONG_INTF;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE,
			"[TRACE]SDIO clock:%d, spec:%d\n",
			info->clock_speed, info->spec_ver);

	if (info->clock_speed > HALMAC_SDIO_CLOCK_SPEED_MAX_88XX)
		return HALMAC_RET_SDIO_CLOCK_ERR;

	if (info->clock_speed > HALMAC_SDIO_CLK_THRESHOLD_88XX)
		adapter->sdio_hw_info.io_hi_speed_flag = 1;

	adapter->sdio_hw_info.clock_speed = info->clock_speed;
	adapter->sdio_hw_info.spec_ver = info->spec_ver;
	adapter->sdio_hw_info.block_size = info->block_size;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

void
halmac_config_sdio_tx_page_threshold_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_tx_page_threshold_info *info
)
{
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	u32 threshold = info->threshold;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (info->enable == 1) {
		threshold = BIT(31) | threshold;
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]enable\n");
	} else {
		threshold = ~(BIT(31)) & threshold;
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]disable\n");
	}

	switch (info->dma_queue_sel) {
	case HALMAC_MAP2_HQ:
		HALMAC_REG_W32(REG_TQPNT1, threshold);
		break;
	case HALMAC_MAP2_NQ:
		HALMAC_REG_W32(REG_TQPNT2, threshold);
		break;
	case HALMAC_MAP2_LQ:
		HALMAC_REG_W32(REG_TQPNT3, threshold);
		break;
	case HALMAC_MAP2_EXQ:
		HALMAC_REG_W32(REG_TQPNT4, threshold);
		break;
	default:
		break;
	}
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);
}

enum halmac_ret_status
halmac_convert_to_sdio_bus_offset_88xx(
	IN struct halmac_adapter *adapter,
	INOUT u32 *offset
)
{
	switch ((*offset) & 0xFFFF0000) {
	case WLAN_IOREG_OFFSET:
		*offset = (HALMAC_SDIO_CMD_ADDR_MAC_REG << 13) |
					(*offset & HALMAC_WLAN_MAC_REG_MSK);
		break;
	case SDIO_LOCAL_OFFSET:
		*offset = (HALMAC_SDIO_CMD_ADDR_SDIO_REG << 13) |
					(*offset & HALMAC_SDIO_LOCAL_MSK);
		break;
	default:
		*offset = 0xFFFFFFFF;
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]base address!!\n");
		return HALMAC_RET_CONVERT_SDIO_OFFSET_FAIL;
	}

	return HALMAC_RET_SUCCESS;
}

#endif /* HALMAC_88XX_SUPPORT */
