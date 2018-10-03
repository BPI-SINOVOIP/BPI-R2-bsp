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

#include "halmac_bb_rf_88xx.h"
#include "halmac_88xx_cfg.h"
#include "halmac_common_88xx.h"
#include "halmac_init_88xx.h"

#if HALMAC_88XX_SUPPORT

/**
 * halmac_start_iqk_88xx() -trigger FW IQK
 * @adapter : the adapter of halmac
 * @param : IQK parameter
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_start_iqk_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_iqk_para *param
)
{
	u8 h2c_buf[HALMAC_H2C_CMD_SIZE_88XX] = { 0 };
	u16 seq_num = 0;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	struct halmac_h2c_header_info hdr_info;
	enum halmac_cmd_process_status *proc_status =
		&adapter->halmac_state.iqk_set.process_status;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (halmac_fw_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_NO_DLFW;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (*proc_status == HALMAC_CMD_PROCESS_SENDING) {
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Wait event(iqk)\n");
		return HALMAC_RET_BUSY_STATE;
	}

	*proc_status = HALMAC_CMD_PROCESS_SENDING;

	IQK_SET_CLEAR(h2c_buf, param->clear);
	IQK_SET_SEGMENT_IQK(h2c_buf, param->segment_iqk);

	hdr_info.sub_cmd_id = SUB_CMD_ID_IQK;
	hdr_info.content_size = 1;
	hdr_info.ack = _TRUE;
	halmac_set_h2c_pkt_hdr_88xx(adapter, h2c_buf, &hdr_info, &seq_num);

	adapter->halmac_state.iqk_set.seq_num = seq_num;

	status = halmac_send_h2c_pkt_88xx(adapter, h2c_buf,
					  HALMAC_H2C_CMD_SIZE_88XX);

	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]send h2c pkt fail!!\n");
		halmac_reset_feature_88xx(adapter, HALMAC_FEATURE_IQK);
		return status;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_ctrl_pwr_tracking_88xx() -trigger FW power tracking
 * @adapter : the adapter of halmac
 * @opt : power tracking option
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_ctrl_pwr_tracking_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_pwr_tracking_option *opt
)
{
	u8 h2c_buf[HALMAC_H2C_CMD_SIZE_88XX] = { 0 };
	u16 seq_num = 0;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	struct halmac_h2c_header_info hdr_info;
	struct halmac_pwr_tracking_para *param;
	enum halmac_cmd_process_status *proc_status =
		&adapter->halmac_state.power_tracking_set.process_status;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (halmac_fw_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_NO_DLFW;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (*proc_status == HALMAC_CMD_PROCESS_SENDING) {
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE,
				"[TRACE]Wait event(pwr tracking)...\n");
		return HALMAC_RET_BUSY_STATE;
	}

	*proc_status = HALMAC_CMD_PROCESS_SENDING;

	POWER_TRACKING_SET_TYPE(h2c_buf, opt->type);
	POWER_TRACKING_SET_BBSWING_INDEX(h2c_buf, opt->bbswing_index);

	param = &opt->pwr_tracking_para[HALMAC_RF_PATH_A];
	POWER_TRACKING_SET_ENABLE_A(h2c_buf, param->enable);
	POWER_TRACKING_SET_TX_PWR_INDEX_A(h2c_buf, param->tx_pwr_index);
	POWER_TRACKING_SET_TSSI_VALUE_A(h2c_buf, param->tssi_value);
	POWER_TRACKING_SET_PWR_TRACKING_OFFSET_VALUE_A(
		h2c_buf, param->pwr_tracking_offset_value);

	POWER_TRACKING_SET_ENABLE_B(h2c_buf, param->enable);
	POWER_TRACKING_SET_TX_PWR_INDEX_B(h2c_buf, param->tx_pwr_index);
	POWER_TRACKING_SET_TSSI_VALUE_B(h2c_buf, param->tssi_value);
	POWER_TRACKING_SET_PWR_TRACKING_OFFSET_VALUE_B(
		h2c_buf, param->pwr_tracking_offset_value);

	POWER_TRACKING_SET_ENABLE_C(h2c_buf, param->enable);
	POWER_TRACKING_SET_TX_PWR_INDEX_C(h2c_buf, param->tx_pwr_index);
	POWER_TRACKING_SET_TSSI_VALUE_C(h2c_buf, param->tssi_value);
	POWER_TRACKING_SET_PWR_TRACKING_OFFSET_VALUE_C(
		h2c_buf, param->pwr_tracking_offset_value);

	POWER_TRACKING_SET_ENABLE_D(h2c_buf, param->enable);
	POWER_TRACKING_SET_TX_PWR_INDEX_D(h2c_buf, param->tx_pwr_index);
	POWER_TRACKING_SET_TSSI_VALUE_D(h2c_buf, param->tssi_value);
	POWER_TRACKING_SET_PWR_TRACKING_OFFSET_VALUE_D(
		h2c_buf, param->pwr_tracking_offset_value);

	hdr_info.sub_cmd_id = SUB_CMD_ID_POWER_TRACKING;
	hdr_info.content_size = 20;
	hdr_info.ack = _TRUE;
	halmac_set_h2c_pkt_hdr_88xx(adapter, h2c_buf, &hdr_info, &seq_num);

	adapter->halmac_state.power_tracking_set.seq_num = seq_num;

	status = halmac_send_h2c_pkt_88xx(adapter, h2c_buf,
					  HALMAC_H2C_CMD_SIZE_88XX);

	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]send h2c pkt fail!!\n");
		halmac_reset_feature_88xx(adapter,
					  HALMAC_FEATURE_POWER_TRACKING);
		return status;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

enum halmac_ret_status
halmac_query_iqk_status_88xx(
	IN struct halmac_adapter *adapter,
	OUT enum halmac_cmd_process_status *proc_status,
	INOUT u8 *data,
	INOUT u32 *size
)
{
	struct halmac_iqk_state_set *state = &adapter->halmac_state.iqk_set;

	*proc_status = state->process_status;

	return HALMAC_RET_SUCCESS;
}

enum halmac_ret_status
halmac_query_power_tracking_status_88xx(
	IN struct halmac_adapter *adapter,
	OUT enum halmac_cmd_process_status *proc_status,
	INOUT u8 *data,
	INOUT u32 *size
)
{
	struct halmac_power_tracking_state_set *state =
		&adapter->halmac_state.power_tracking_set;

	*proc_status = state->process_status;

	return HALMAC_RET_SUCCESS;
}

enum halmac_ret_status
halmac_query_psd_status_88xx(
	IN struct halmac_adapter *adapter,
	OUT enum halmac_cmd_process_status *proc_status,
	INOUT u8 *data,
	INOUT u32 *size
)
{
	struct halmac_psd_state_set *state = &adapter->halmac_state.psd_set;

	*proc_status = state->process_status;

	if (!data)
		return HALMAC_RET_NULL_POINTER;

	if (!size)
		return HALMAC_RET_NULL_POINTER;

	if (*proc_status == HALMAC_CMD_PROCESS_DONE) {
		if (*size < state->data_size) {
			*size = state->data_size;
			return HALMAC_RET_BUFFER_TOO_SMALL;
		}

		*size = state->data_size;
		PLTFM_MEMCPY(data, state->data, *size);
	}

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_psd_88xx() - trigger fw psd
 * @adapter : the adapter of halmac
 * @start_psd : start PSD
 * @end_psd : end PSD
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_psd_88xx(
	IN struct halmac_adapter *adapter,
	IN u16 start_psd,
	IN u16 end_psd
)
{
	u8 h2c_buf[HALMAC_H2C_CMD_SIZE_88XX] = { 0 };
	u16 seq_num = 0;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	struct halmac_h2c_header_info hdr_info;
	enum halmac_cmd_process_status *proc_status =
		&adapter->halmac_state.psd_set.process_status;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (halmac_fw_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_NO_DLFW;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (*proc_status == HALMAC_CMD_PROCESS_SENDING) {
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Wait event(psd)\n");
		return HALMAC_RET_BUSY_STATE;
	}

	if (adapter->halmac_state.psd_set.data) {
		PLTFM_FREE(adapter->halmac_state.psd_set.data,
			   adapter->halmac_state.psd_set.data_size);
		adapter->halmac_state.psd_set.data = (u8 *)NULL;
	}

	adapter->halmac_state.psd_set.data_size = 0;
	adapter->halmac_state.psd_set.segment_size = 0;

	*proc_status = HALMAC_CMD_PROCESS_SENDING;

	PSD_SET_START_PSD(h2c_buf, start_psd);
	PSD_SET_END_PSD(h2c_buf, end_psd);

	hdr_info.sub_cmd_id = SUB_CMD_ID_PSD;
	hdr_info.content_size = 4;
	hdr_info.ack = _TRUE;
	halmac_set_h2c_pkt_hdr_88xx(adapter, h2c_buf, &hdr_info, &seq_num);

	status = halmac_send_h2c_pkt_88xx(adapter, h2c_buf,
					  HALMAC_H2C_CMD_SIZE_88XX);

	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]send h2c pkt fail!!\n");
		halmac_reset_feature_88xx(adapter, HALMAC_FEATURE_PSD);
		return status;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

enum halmac_ret_status
halmac_get_h2c_ack_iqk_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
)
{
	u8 seq_num;
	u8 fw_rc;
	struct halmac_iqk_state_set *state =
		&adapter->halmac_state.iqk_set;
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
		proc_status = HALMAC_CMD_PROCESS_DONE;
		state->process_status = proc_status;
		PLTFM_EVENT_INDICATION(HALMAC_FEATURE_IQK,
				       proc_status, NULL, 0);
	} else {
		proc_status = HALMAC_CMD_PROCESS_ERROR;
		state->process_status = proc_status;
		PLTFM_EVENT_INDICATION(HALMAC_FEATURE_IQK, proc_status,
				       &state->fw_return_code, 1);
	}

	return HALMAC_RET_SUCCESS;
}

enum halmac_ret_status
halmac_get_h2c_ack_power_tracking_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
)
{
	u8 seq_num;
	u8 fw_rc;
	struct halmac_power_tracking_state_set *state =
		&adapter->halmac_state.power_tracking_set;
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
		proc_status = HALMAC_CMD_PROCESS_DONE;
		state->process_status = proc_status;
		PLTFM_EVENT_INDICATION(HALMAC_FEATURE_POWER_TRACKING,
				       proc_status, NULL, 0);
	} else {
		proc_status = HALMAC_CMD_PROCESS_ERROR;
		state->process_status = proc_status;
		PLTFM_EVENT_INDICATION(HALMAC_FEATURE_POWER_TRACKING,
				       proc_status, &state->fw_return_code, 1);
	}

	return HALMAC_RET_SUCCESS;
}

#endif /* HALMAC_88XX_SUPPORT */
