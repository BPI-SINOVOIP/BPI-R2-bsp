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

#include "halmac_common_8821c.h"
#include "../halmac_common_88xx.h"
#include "halmac_cfg_wmac_8821c.h"

#if HALMAC_8821C_SUPPORT

/**
 * halmac_get_hw_value_8821c() -get hw config value
 * @adapter : the adapter of halmac
 * @hw_id : hw id for driver to query
 * @value : hw value, reference table to get data type
 * Author : KaiYuan Chang / Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_get_hw_value_8821c(
	IN struct halmac_adapter *adapter,
	IN enum halmac_hw_id hw_id,
	OUT void *value
)
{
	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (!value) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]null pointer\n");
		return HALMAC_RET_NULL_POINTER;
	}

	if (halmac_get_hw_value_88xx(adapter, hw_id, value) !=
	    HALMAC_RET_SUCCESS)
		return HALMAC_RET_PARA_NOT_SUPPORT;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_set_hw_value_8821c() -set hw config value
 * @adapter : the adapter of halmac
 * @hw_id : hw id for driver to config
 * @value : hw value, reference table to get data type
 * Author : KaiYuan Chang / Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_set_hw_value_8821c(
	IN struct halmac_adapter *adapter,
	IN enum halmac_hw_id hw_id,
	IN void *value
)
{
	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (!value) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]null pointer\n");
		return HALMAC_RET_NULL_POINTER;
	}

	if (halmac_set_hw_value_88xx(adapter, hw_id, value) !=
	    HALMAC_RET_SUCCESS) {
		switch (hw_id) {
		case HALMAC_HW_SDIO_TX_FORMAT:
			break;
		case HALMAC_HW_RXGCK_FIFO:
			break;
		case HALMAC_HW_RX_IGNORE:
			halmac_cfg_mac_rx_ignore_8821c(
				adapter,
				(struct halmac_mac_rx_ignore_cfg *)value);
			break;
		default:
			return HALMAC_RET_PARA_NOT_SUPPORT;
		}
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_fill_txdesc_check_sum_88xx() -  fill in tx desc check sum
 * @adapter : the adapter of halmac
 * @txdesc : tx desc packet
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_fill_txdesc_check_sum_8821c(
	IN struct halmac_adapter *adapter,
	INOUT u8 *txdesc
)
{
	u16 chksum = 0;
	u16 *data = (u16 *)NULL;
	u32 i;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (!txdesc) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]null pointer\n");
		return HALMAC_RET_NULL_POINTER;
	}

	if (adapter->tx_desc_checksum != _TRUE)
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]chksum disable\n");

	SET_TX_DESC_TXDESC_CHECKSUM(txdesc, 0x0000);

	data = (u16 *)(txdesc);

	/* HW clculates only 32byte */
	for (i = 0; i < 8; i++)
		chksum ^= (*(data + 2 * i) ^ *(data + (2 * i + 1)));

	/* *(data + 2 * i) & *(data + (2 * i + 1) have endain issue*/
	/* Process eniadn issue after checksum calculation */
	chksum = rtk_le16_to_cpu(chksum);

	SET_TX_DESC_TXDESC_CHECKSUM(txdesc, chksum);

	return HALMAC_RET_SUCCESS;
}

#endif /* HALMAC_8821C_SUPPORT */
