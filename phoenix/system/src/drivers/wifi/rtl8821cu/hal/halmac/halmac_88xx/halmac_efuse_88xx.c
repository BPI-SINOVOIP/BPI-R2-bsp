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

#include "halmac_efuse_88xx.h"
#include "halmac_88xx_cfg.h"
#include "halmac_common_88xx.h"
#include "halmac_init_88xx.h"

#if HALMAC_88XX_SUPPORT

static enum halmac_efuse_cmd_construct_state
halmac_efuse_cmd_state_88xx(
	IN struct halmac_adapter *adapter
);

static enum halmac_ret_status
halmac_dump_efuse_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_efuse_read_cfg cfg
);

static enum halmac_ret_status
halmac_read_hw_efuse_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u32 size,
	OUT u8 *map
);

static enum halmac_ret_status
halmac_eeprom_parser_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *phy_map,
	OUT u8 *log_map
);

static enum halmac_ret_status
halmac_read_logical_efuse_map_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *pMap
);

static enum halmac_ret_status
halmac_func_pg_efuse_by_map_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_pg_efuse_info *info,
	IN enum halmac_efuse_read_cfg cfg
);

static enum halmac_ret_status
halmac_dump_efuse_fw_88xx(
	IN struct halmac_adapter *adapter
);

static enum halmac_ret_status
halmac_dump_efuse_drv_88xx(
	IN struct halmac_adapter *adapter
);

static enum halmac_ret_status
halmac_func_write_logical_efuse_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u8 value
);

static enum halmac_ret_status
halmac_update_eeprom_mask_88xx(
	IN struct halmac_adapter *adapter,
	INOUT struct halmac_pg_efuse_info *info,
	OUT u8 *updated_mask
);

static enum halmac_ret_status
halmac_check_efuse_enough_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_pg_efuse_info *info,
	IN u8 *updated_mask
);

static enum halmac_ret_status
halmac_func_pg_extend_efuse_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_pg_efuse_info *info,
	IN u8 word_en,
	IN u8 pre_word_en,
	IN u32 eeprom_offset
);

static enum halmac_ret_status
halmac_func_pg_efuse_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_pg_efuse_info *info,
	IN u8 word_en,
	IN u8 pre_word_en,
	IN u32 eeprom_offset
);

static enum halmac_ret_status
halmac_program_efuse_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_pg_efuse_info *info,
	IN u8 *updated_mask
);

static void
halmac_mask_eeprom_88xx(
	IN struct halmac_adapter *adapter,
	INOUT struct halmac_pg_efuse_info *info
);

/**
 * halmac_dump_efuse_map_88xx() - dump "physical" efuse map
 * @adapter : the adapter of halmac
 * @cfg : dump efuse method
 * Author : Ivan Lin/KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_dump_efuse_map_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_efuse_read_cfg cfg
)
{
	u8 *map = NULL;
	u32 efuse_size = adapter->hw_cfg_info.efuse_size;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	enum halmac_cmd_process_status *proc_status =
		&adapter->halmac_state.efuse_state_set.process_status;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (cfg == HALMAC_EFUSE_R_FW &&
	    halmac_fw_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_NO_DLFW;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]cfg = %d\n", cfg);

	if (*proc_status == HALMAC_CMD_PROCESS_SENDING) {
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]Wait event(efuse)\n");
		return HALMAC_RET_BUSY_STATE;
	}

	if (halmac_efuse_cmd_state_88xx(adapter) != HALMAC_EFUSE_CMD_IDLE) {
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]Not idle(efuse)\n");
		return HALMAC_RET_ERROR_STATE;
	}

	if (adapter->halmac_state.mac_power == HALMAC_MAC_POWER_OFF)
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Dump efuse in suspend\n");

	*proc_status = HALMAC_CMD_PROCESS_IDLE;
	adapter->event_trigger.physical_efuse_map = 1;

	status = halmac_switch_efuse_bank_88xx(adapter, HALMAC_EFUSE_BANK_WIFI);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]switch efuse bank!!\n");
		return status;
	}

	status = halmac_dump_efuse_88xx(adapter, cfg);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]dump efuse!!\n");
		return status;
	}

	if (adapter->hal_efuse_map_valid == _TRUE) {
		*proc_status = HALMAC_CMD_PROCESS_DONE;

		map = (u8 *)PLTFM_MALLOC(efuse_size);
		if (!map) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]malloc!!\n");
			return HALMAC_RET_MALLOC_FAIL;
		}
		PLTFM_MEMSET(map, 0xFF, efuse_size);
		PLTFM_MUTEX_LOCK(&adapter->EfuseMutex);
		PLTFM_MEMCPY(map, adapter->efuse_map,
			     efuse_size - HALMAC_PROTECTED_EFUSE_SIZE_88XX);
		PLTFM_MEMCPY(
			map + efuse_size -
			HALMAC_PROTECTED_EFUSE_SIZE_88XX +
			HALMAC_RSVD_CS_EFUSE_SIZE_88XX,
			adapter->efuse_map + efuse_size -
			HALMAC_PROTECTED_EFUSE_SIZE_88XX +
			HALMAC_RSVD_CS_EFUSE_SIZE_88XX,
			HALMAC_PROTECTED_EFUSE_SIZE_88XX -
			HALMAC_RSVD_EFUSE_SIZE_88XX -
			HALMAC_RSVD_CS_EFUSE_SIZE_88XX);
		PLTFM_MUTEX_UNLOCK(&adapter->EfuseMutex);

		PLTFM_EVENT_INDICATION(HALMAC_FEATURE_DUMP_PHYSICAL_EFUSE,
				       *proc_status, map, efuse_size);
		adapter->event_trigger.physical_efuse_map = 0;

		PLTFM_FREE(map, efuse_size);
	}

	if (halmac_transform_efuse_state_88xx(adapter, HALMAC_EFUSE_CMD_IDLE) !=
	    HALMAC_RET_SUCCESS)
		return HALMAC_RET_ERROR_STATE;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_dump_efuse_map_bt_88xx() - dump "BT physical" efuse map
 * @adapter : the adapter of halmac
 * @bank : bt efuse bank
 * @size : bt efuse map size. get from halmac_get_efuse_size API
 * @map : bt efuse map
 * Author : Soar / Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_dump_efuse_map_bt_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_efuse_bank bank,
	IN u32 size,
	OUT u8 *map
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	enum halmac_cmd_process_status *proc_status =
		&adapter->halmac_state.efuse_state_set.process_status;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (adapter->hw_cfg_info.bt_efuse_size != size)
		return HALMAC_RET_EFUSE_SIZE_INCORRECT;

	if ((bank >= HALMAC_EFUSE_BANK_MAX) ||
	    (bank == HALMAC_EFUSE_BANK_WIFI)) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Undefined BT bank\n");
		return HALMAC_RET_EFUSE_BANK_INCORRECT;
	}

	if (*proc_status == HALMAC_CMD_PROCESS_SENDING) {
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]Wait event(efuse)\n");
		return HALMAC_RET_BUSY_STATE;
	}

	if (halmac_efuse_cmd_state_88xx(adapter) != HALMAC_EFUSE_CMD_IDLE) {
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]Not idle(efuse)\n");
		return HALMAC_RET_ERROR_STATE;
	}

	status = halmac_switch_efuse_bank_88xx(adapter, bank);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]switch efuse bank!!\n");
		return status;
	}

	status = halmac_read_hw_efuse_88xx(adapter, 0, size, map);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]read hw efuse\n");
		return status;
	}

	if (halmac_transform_efuse_state_88xx(adapter, HALMAC_EFUSE_CMD_IDLE) !=
	    HALMAC_RET_SUCCESS)
		return HALMAC_RET_ERROR_STATE;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_write_efuse_bt_88xx() - write "BT physical" efuse offset
 * @adapter : the adapter of halmac
 * @offset : offset
 * @value : Write value
 * @map : bt efuse map
 * Author : Soar
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_write_efuse_bt_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u8 value,
	IN enum halmac_efuse_bank bank
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	enum halmac_cmd_process_status *proc_status =
		&adapter->halmac_state.efuse_state_set.process_status;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (*proc_status == HALMAC_CMD_PROCESS_SENDING) {
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]Wait event(efuse)\n");
		return HALMAC_RET_BUSY_STATE;
	}

	if (halmac_efuse_cmd_state_88xx(adapter) != HALMAC_EFUSE_CMD_IDLE) {
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]Not idle(efuse)\n");
		return HALMAC_RET_ERROR_STATE;
	}

	if (offset >= adapter->hw_cfg_info.efuse_size) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Offset is too large\n");
		return HALMAC_RET_EFUSE_SIZE_INCORRECT;
	}

	if ((bank > HALMAC_EFUSE_BANK_MAX) ||
	    (bank == HALMAC_EFUSE_BANK_WIFI)) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Undefined BT bank\n");
		return HALMAC_RET_EFUSE_BANK_INCORRECT;
	}

	status = halmac_switch_efuse_bank_88xx(adapter, bank);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]switch efuse bank!!\n");
		return status;
	}

	status = halmac_func_write_efuse_88xx(adapter, offset, value);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]write efuse\n");
		return status;
	}

	if (halmac_transform_efuse_state_88xx(adapter, HALMAC_EFUSE_CMD_IDLE) !=
	    HALMAC_RET_SUCCESS)
		return HALMAC_RET_ERROR_STATE;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_read_efuse_bt_88xx() - read "BT physical" efuse offset
 * @adapter : the adapter of halmac
 * @offset : offset
 * @value : 1 byte efuse value
 * @bank : efuse bank
 * Author : Soar
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_read_efuse_bt_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	OUT u8 *value,
	IN enum halmac_efuse_bank bank
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	enum halmac_cmd_process_status *proc_status =
		&adapter->halmac_state.efuse_state_set.process_status;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (*proc_status == HALMAC_CMD_PROCESS_SENDING) {
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]Wait event(efuse)\n");
		return HALMAC_RET_BUSY_STATE;
	}

	if (halmac_efuse_cmd_state_88xx(adapter) != HALMAC_EFUSE_CMD_IDLE) {
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]Not idle(efuse)\n");
		return HALMAC_RET_ERROR_STATE;
	}

	if (offset >= adapter->hw_cfg_info.efuse_size) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Offset is too large\n");
		return HALMAC_RET_EFUSE_SIZE_INCORRECT;
	}

	if ((bank > HALMAC_EFUSE_BANK_MAX) ||
	    (bank == HALMAC_EFUSE_BANK_WIFI)) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Undefined BT bank\n");
		return HALMAC_RET_EFUSE_BANK_INCORRECT;
	}

	status = halmac_switch_efuse_bank_88xx(adapter, bank);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]switch efuse bank\n");
		return status;
	}

	status = halmac_func_read_efuse_88xx(adapter, offset, 1, value);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]read efuse\n");
		return status;
	}

	if (halmac_transform_efuse_state_88xx(adapter, HALMAC_EFUSE_CMD_IDLE) !=
	    HALMAC_RET_SUCCESS)
		return HALMAC_RET_ERROR_STATE;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_cfg_efuse_auto_check_88xx() - check efuse after writing it
 * @adapter : the adapter of halmac
 * @enable : 1, enable efuse auto check. others, disable
 * Author : Soar
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_cfg_efuse_auto_check_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 enable
)
{
	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	adapter->efuse_auto_check_en = enable;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_get_efuse_available_size_88xx() - get efuse available size
 * @adapter : the adapter of halmac
 * @size : physical efuse available size
 * Author : Soar
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_get_efuse_available_size_88xx(
	IN struct halmac_adapter *adapter,
	OUT u32 *size
)
{
	enum halmac_ret_status status;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	status = halmac_dump_logical_efuse_map_88xx(adapter,
						    HALMAC_EFUSE_R_DRV);

	if (status != HALMAC_RET_SUCCESS)
		return status;

	*size = adapter->hw_cfg_info.efuse_size -
		HALMAC_PROTECTED_EFUSE_SIZE_88XX - adapter->efuse_end;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_get_efuse_size_88xx() - get "physical" efuse size
 * @adapter : the adapter of halmac
 * @size : physical efuse size
 * Author : Ivan Lin/KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_get_efuse_size_88xx(
	IN struct halmac_adapter *adapter,
	OUT u32 *size
)
{
	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	*size = adapter->hw_cfg_info.efuse_size;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_get_logical_efuse_size_88xx() - get "logical" efuse size
 * @adapter : the adapter of halmac
 * @size : logical efuse size
 * Author : Ivan Lin/KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_get_logical_efuse_size_88xx(
	IN struct halmac_adapter *adapter,
	OUT u32 *size
)
{
	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	*size = adapter->hw_cfg_info.eeprom_size;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_dump_logical_efuse_map_88xx() - dump "logical" efuse map
 * @adapter : the adapter of halmac
 * @cfg : dump efuse method
 * Author : Soar
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_dump_logical_efuse_map_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_efuse_read_cfg cfg
)
{
	u8 *map = NULL;
	u32 size = adapter->hw_cfg_info.eeprom_size;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	enum halmac_cmd_process_status *proc_status =
		&adapter->halmac_state.efuse_state_set.process_status;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (cfg == HALMAC_EFUSE_R_FW &&
	    halmac_fw_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_NO_DLFW;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]cfg = %d\n", cfg);

	if (*proc_status == HALMAC_CMD_PROCESS_SENDING) {
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]Wait event(efuse)\n");
		return HALMAC_RET_BUSY_STATE;
	}

	if (halmac_efuse_cmd_state_88xx(adapter) != HALMAC_EFUSE_CMD_IDLE) {
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]Not idle(efuse)\n");
		return HALMAC_RET_ERROR_STATE;
	}

	if (adapter->halmac_state.mac_power == HALMAC_MAC_POWER_OFF)
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Dump efuse in suspend\n");

	*proc_status = HALMAC_CMD_PROCESS_IDLE;
	adapter->event_trigger.logical_efuse_map = 1;

	status = halmac_switch_efuse_bank_88xx(adapter, HALMAC_EFUSE_BANK_WIFI);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]switch efuse bank\n");
		return status;
	}

	status = halmac_dump_efuse_88xx(adapter, cfg);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]dump efuse\n");
		return status;
	}

	if (adapter->hal_efuse_map_valid == _TRUE) {
		*proc_status = HALMAC_CMD_PROCESS_DONE;

		map = (u8 *)PLTFM_MALLOC(size);
		if (!map) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]malloc map\n");
			return HALMAC_RET_MALLOC_FAIL;
		}
		PLTFM_MEMSET(map, 0xFF, size);

		if (halmac_eeprom_parser_88xx(
			adapter, adapter->efuse_map, map) !=
			HALMAC_RET_SUCCESS) {
			PLTFM_FREE(map, size);
			return HALMAC_RET_EEPROM_PARSING_FAIL;
		}

		PLTFM_EVENT_INDICATION(HALMAC_FEATURE_DUMP_LOGICAL_EFUSE,
				       *proc_status, map, size);
		adapter->event_trigger.logical_efuse_map = 0;

		PLTFM_FREE(map, size);
	}

	if (halmac_transform_efuse_state_88xx(adapter, HALMAC_EFUSE_CMD_IDLE) !=
	    HALMAC_RET_SUCCESS)
		return HALMAC_RET_ERROR_STATE;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_read_logical_efuse_88xx() - read logical efuse map 1 byte
 * @adapter : the adapter of halmac
 * @offset : offset
 * @value : 1 byte efuse value
 * Author : Soar
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_read_logical_efuse_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	OUT u8 *value
)
{
	u8 *map = NULL;
	u32 size = adapter->hw_cfg_info.eeprom_size;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	enum halmac_cmd_process_status *proc_status =
		&adapter->halmac_state.efuse_state_set.process_status;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (offset >= size) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Offset is too large\n");
		return HALMAC_RET_EFUSE_SIZE_INCORRECT;
	}

	if (*proc_status == HALMAC_CMD_PROCESS_SENDING) {
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]Wait event(efuse)\n");
		return HALMAC_RET_BUSY_STATE;
	}
	if (halmac_efuse_cmd_state_88xx(adapter) != HALMAC_EFUSE_CMD_IDLE) {
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]Not idle(efuse)\n");
		return HALMAC_RET_ERROR_STATE;
	}

	status = halmac_switch_efuse_bank_88xx(adapter, HALMAC_EFUSE_BANK_WIFI);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]switch efuse bank\n");
		return status;
	}

	map = (u8 *)PLTFM_MALLOC(size);
	if (!map) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]malloc map\n");
		return HALMAC_RET_MALLOC_FAIL;
	}
	PLTFM_MEMSET(map, 0xFF, size);

	status = halmac_read_logical_efuse_map_88xx(adapter, map);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]read logical efuse\n");
		PLTFM_FREE(map, size);
		return status;
	}

	*value = *(map + offset);

	if (halmac_transform_efuse_state_88xx(adapter, HALMAC_EFUSE_CMD_IDLE) !=
	    HALMAC_RET_SUCCESS) {
		PLTFM_FREE(map, size);
		return HALMAC_RET_ERROR_STATE;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	PLTFM_FREE(map, size);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_write_logical_efuse_88xx() - write "logical" efuse offset
 * @adapter : the adapter of halmac
 * @offset : offset
 * @value : value
 * Author : Soar
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_write_logical_efuse_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u8 value
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	enum halmac_cmd_process_status *proc_status =
		&adapter->halmac_state.efuse_state_set.process_status;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (offset >= adapter->hw_cfg_info.eeprom_size) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Offset is too large\n");
		return HALMAC_RET_EFUSE_SIZE_INCORRECT;
	}

	if (*proc_status == HALMAC_CMD_PROCESS_SENDING) {
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]Wait event(efuse)\n");
		return HALMAC_RET_BUSY_STATE;
	}

	if (halmac_efuse_cmd_state_88xx(adapter) != HALMAC_EFUSE_CMD_IDLE) {
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]Not idle(efuse)\n");
		return HALMAC_RET_ERROR_STATE;
	}

	status = halmac_switch_efuse_bank_88xx(adapter, HALMAC_EFUSE_BANK_WIFI);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]switch efuse bank\n");
		return status;
	}

	status = halmac_func_write_logical_efuse_88xx(adapter, offset, value);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]write logical efuse\n");
		return status;
	}

	if (halmac_transform_efuse_state_88xx(adapter, HALMAC_EFUSE_CMD_IDLE) !=
	    HALMAC_RET_SUCCESS)
		return HALMAC_RET_ERROR_STATE;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_pg_efuse_by_map_88xx() - pg logical efuse by map
 * @adapter : the adapter of halmac
 * @info : efuse map information
 * @cfg : dump efuse method
 * Author : Soar
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_pg_efuse_by_map_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_pg_efuse_info *info,
	IN enum halmac_efuse_read_cfg cfg
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	enum halmac_cmd_process_status *proc_status =
		&adapter->halmac_state.efuse_state_set.process_status;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (info->efuse_map_size != adapter->hw_cfg_info.eeprom_size) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]map size error\n");
		return HALMAC_RET_EFUSE_SIZE_INCORRECT;
	}

	if ((info->efuse_map_size & 0xF) > 0) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]not multiple of 16\n");
		return HALMAC_RET_EFUSE_SIZE_INCORRECT;
	}

	if (info->efuse_mask_size != info->efuse_map_size >> 4) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]mask size error\n");
		return HALMAC_RET_EFUSE_SIZE_INCORRECT;
	}

	if (!info->pEfuse_map) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]map is NULL\n");
		return HALMAC_RET_NULL_POINTER;
	}

	if (!info->pEfuse_mask) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]mask is NULL\n");
		return HALMAC_RET_NULL_POINTER;
	}

	if (*proc_status == HALMAC_CMD_PROCESS_SENDING) {
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]Wait event(efuse)\n");
		return HALMAC_RET_BUSY_STATE;
	}

	if (halmac_efuse_cmd_state_88xx(adapter) != HALMAC_EFUSE_CMD_IDLE) {
		PLTFM_MSG_PRINT(HALMAC_DBG_WARN, "[WARN]Not idle(efuse)\n");
		return HALMAC_RET_ERROR_STATE;
	}

	status = halmac_switch_efuse_bank_88xx(adapter, HALMAC_EFUSE_BANK_WIFI);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]switch efuse bank\n");
		return status;
	}

	status = halmac_func_pg_efuse_by_map_88xx(adapter, info, cfg);

	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]pg efuse\n");
		return status;
	}

	if (halmac_transform_efuse_state_88xx(adapter, HALMAC_EFUSE_CMD_IDLE) !=
	    HALMAC_RET_SUCCESS)
		return HALMAC_RET_ERROR_STATE;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_mask_logical_efuse_88xx() - mask logical efuse
 * @adapter : the adapter of halmac
 * @info : efuse map information
 * Author : Soar
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_mask_logical_efuse_88xx(
	IN struct halmac_adapter *adapter,
	INOUT struct halmac_pg_efuse_info *info
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	enum halmac_cmd_process_status *proc_status =
		&adapter->halmac_state.efuse_state_set.process_status;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	if (info->efuse_map_size != adapter->hw_cfg_info.eeprom_size) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]map size error\n");
		return HALMAC_RET_EFUSE_SIZE_INCORRECT;
	}

	if ((info->efuse_map_size & 0xF) > 0) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]not multiple of 16\n");
		return HALMAC_RET_EFUSE_SIZE_INCORRECT;
	}

	if (info->efuse_mask_size != info->efuse_map_size >> 4) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]mask size error\n");
		return HALMAC_RET_EFUSE_SIZE_INCORRECT;
	}

	if (!info->pEfuse_map) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]map is NULL\n");
		return HALMAC_RET_NULL_POINTER;
	}

	if (!info->pEfuse_mask) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]mask is NULL\n");
		return HALMAC_RET_NULL_POINTER;
	}

	halmac_mask_eeprom_88xx(adapter, info);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_efuse_cmd_construct_state
halmac_efuse_cmd_state_88xx(
	IN struct halmac_adapter *adapter
)
{
	return adapter->halmac_state.efuse_state_set.efuse_cmd_construct_state;
}

enum halmac_ret_status
halmac_switch_efuse_bank_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_efuse_bank bank
)
{
	u8 reg_value;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_transform_efuse_state_88xx(adapter, HALMAC_EFUSE_CMD_BUSY) !=
	    HALMAC_RET_SUCCESS)
		return HALMAC_RET_ERROR_STATE;

	reg_value = HALMAC_REG_R8(REG_LDO_EFUSE_CTRL + 1);

	if (bank == (reg_value & (BIT(0) | BIT(1))))
		return HALMAC_RET_SUCCESS;

	reg_value &= ~(BIT(0) | BIT(1));
	reg_value |= bank;
	HALMAC_REG_W8(REG_LDO_EFUSE_CTRL + 1, reg_value);

	if ((HALMAC_REG_R8(REG_LDO_EFUSE_CTRL + 1) & (BIT(0) | BIT(1))) !=
	    bank)
		return HALMAC_RET_SWITCH_EFUSE_BANK_FAIL;

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_dump_efuse_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_efuse_read_cfg cfg
)
{
	u32 h2c_init;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	enum halmac_cmd_process_status *proc_status =
		&adapter->halmac_state.efuse_state_set.process_status;

	*proc_status = HALMAC_CMD_PROCESS_SENDING;

	if (halmac_transform_efuse_state_88xx(
		adapter, HALMAC_EFUSE_CMD_H2C_SENT) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_ERROR_STATE;

	if (cfg == HALMAC_EFUSE_R_AUTO) {
		h2c_init = HALMAC_REG_R32(REG_H2C_PKT_READADDR);
		if (adapter->halmac_state.dlfw_state == HALMAC_DLFW_NONE ||
		    h2c_init == 0)
			status = halmac_dump_efuse_drv_88xx(adapter);
		else
			status = halmac_dump_efuse_fw_88xx(adapter);
	} else if (cfg == HALMAC_EFUSE_R_FW) {
		status = halmac_dump_efuse_fw_88xx(adapter);
	} else {
		status = halmac_dump_efuse_drv_88xx(adapter);
	}

	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]dump efsue drv/fw\n");
		return status;
	}

	return status;
}

enum halmac_ret_status
halmac_transform_efuse_state_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_efuse_cmd_construct_state dest_state
)
{
	struct halmac_efuse_state_set *state = &adapter->halmac_state.efuse_state_set;

	if ((state->efuse_cmd_construct_state != HALMAC_EFUSE_CMD_IDLE) &&
	    (state->efuse_cmd_construct_state != HALMAC_EFUSE_CMD_BUSY) &&
	    (state->efuse_cmd_construct_state != HALMAC_EFUSE_CMD_H2C_SENT))
		return HALMAC_RET_ERROR_STATE;

	if (state->efuse_cmd_construct_state == dest_state)
		return HALMAC_RET_ERROR_STATE;

	if (dest_state == HALMAC_EFUSE_CMD_BUSY) {
		if (state->efuse_cmd_construct_state ==
		    HALMAC_EFUSE_CMD_H2C_SENT)
			return HALMAC_RET_ERROR_STATE;
	} else if (dest_state == HALMAC_EFUSE_CMD_H2C_SENT) {
		if (state->efuse_cmd_construct_state == HALMAC_EFUSE_CMD_IDLE)
			return HALMAC_RET_ERROR_STATE;
	}

	state->efuse_cmd_construct_state = dest_state;

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_read_hw_efuse_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u32 size,
	OUT u8 *map
)
{
	u8 value8;
	u32 value32;
	u32 addr;
	u32 tmp32;
	u32 cnt;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	/* Read efuse no need 2.5V LDO */
	value8 = HALMAC_REG_R8(REG_LDO_EFUSE_CTRL + 3);
	if (value8 & BIT(7))
		HALMAC_REG_W8(REG_LDO_EFUSE_CTRL + 3, (u8)(value8 & ~BIT(7)));

	value32 = HALMAC_REG_R32(REG_EFUSE_CTRL);

	for (addr = offset; addr < offset + size; addr++) {
		value32 &= ~(BIT_MASK_EF_DATA | BITS_EF_ADDR);
		value32 |= ((addr & BIT_MASK_EF_ADDR) << BIT_SHIFT_EF_ADDR);
		HALMAC_REG_W32(REG_EFUSE_CTRL, value32 & (~BIT_EF_FLAG));

		cnt = 1000000;
		do {
			PLTFM_DELAY_US(1);
			tmp32 = HALMAC_REG_R32(REG_EFUSE_CTRL);
			cnt--;
			if (cnt == 0) {
				PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]read\n");
				return HALMAC_RET_EFUSE_R_FAIL;
			}
		} while ((tmp32 & BIT_EF_FLAG) == 0);

		*(map + addr - offset) = (u8)(tmp32 & BIT_MASK_EF_DATA);
	}

	return HALMAC_RET_SUCCESS;
}

enum halmac_ret_status
halmac_func_write_efuse_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u8 value
)
{
	const u8 unlock_code = 0x69;
	u8 value_read = 0;
	u32 value32;
	u32 tmp32;
	u32 cnt;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	PLTFM_MUTEX_LOCK(&adapter->EfuseMutex);
	adapter->hal_efuse_map_valid = _FALSE;
	PLTFM_MUTEX_UNLOCK(&adapter->EfuseMutex);

	HALMAC_REG_W8(REG_PMC_DBG_CTRL2 + 3, unlock_code);

	/* Enable 2.5V LDO */
	HALMAC_REG_W8(REG_LDO_EFUSE_CTRL + 3,
		      (u8)(HALMAC_REG_R8(REG_LDO_EFUSE_CTRL + 3) | BIT(7)));

	value32 = HALMAC_REG_R32(REG_EFUSE_CTRL);
	value32 &= ~(BIT_MASK_EF_DATA | BITS_EF_ADDR);
	value32 = value32 | ((offset & BIT_MASK_EF_ADDR) << BIT_SHIFT_EF_ADDR) |
			(value & BIT_MASK_EF_DATA);
	HALMAC_REG_W32(REG_EFUSE_CTRL, value32 | BIT_EF_FLAG);

	cnt = 1000000;
	do {
		PLTFM_DELAY_US(1);
		tmp32 = HALMAC_REG_R32(REG_EFUSE_CTRL);
		cnt--;
		if (cnt == 0) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]write!!\n");
			return HALMAC_RET_EFUSE_W_FAIL;
		}
	} while (BIT_EF_FLAG == (tmp32 & BIT_EF_FLAG));

	HALMAC_REG_W8(REG_PMC_DBG_CTRL2 + 3, 0x00);

	/* Disable 2.5V LDO */
	HALMAC_REG_W8(REG_LDO_EFUSE_CTRL + 3,
		      (u8)(HALMAC_REG_R8(REG_LDO_EFUSE_CTRL + 3) & ~BIT(7)));

	if (adapter->efuse_auto_check_en == 1) {
		if (halmac_read_hw_efuse_88xx(
			adapter, offset, 1, &value_read) != HALMAC_RET_SUCCESS)
			return HALMAC_RET_EFUSE_R_FAIL;
		if (value_read != value) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]efuse compare\n");
			return HALMAC_RET_EFUSE_W_FAIL;
		}
	}
	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_eeprom_parser_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *phy_map,
	OUT u8 *log_map
)
{
	u8 i;
	u8 value8;
	u8 blk_idx;
	u8 word_en;
	u8 valid;
	u8 read_hdr;
	u8 read_hdr2 = 0;
	u32 eeprom_idx;
	u32 efuse_idx = 0;
	struct halmac_hw_config_info *hw_info = &adapter->hw_cfg_info;

	PLTFM_MEMSET(log_map, 0xFF, hw_info->eeprom_size);

	do {
		value8 = *(phy_map + efuse_idx);
		read_hdr = value8;

		if ((read_hdr & 0x1f) == 0x0f) {
			efuse_idx++;
			value8 = *(phy_map + efuse_idx);
			read_hdr2 = value8;
			if (read_hdr2 == 0xff)
				break;
			blk_idx = ((read_hdr2 & 0xF0) >> 1) |
					((read_hdr >> 5) & 0x07);
			word_en = read_hdr2 & 0x0F;
		} else {
			blk_idx = (read_hdr & 0xF0) >> 4;
			word_en = read_hdr & 0x0F;
		}

		if (read_hdr == 0xff)
			break;

		efuse_idx++;

		if (efuse_idx >=
		    hw_info->efuse_size - HALMAC_PROTECTED_EFUSE_SIZE_88XX - 1)
			return HALMAC_RET_EEPROM_PARSING_FAIL;

		for (i = 0; i < 4; i++) {
			valid = (u8)((~(word_en >> i)) & BIT(0));
			if (valid == 1) {
				eeprom_idx = (blk_idx << 3) + (i << 1);

				if ((eeprom_idx + 1) > hw_info->eeprom_size) {
					PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
							"[ERR]efuse idx:0x%X\n",
							efuse_idx - 1);

					PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
							"[ERR]read hdr:0x%X\n",
							read_hdr);

					PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
							"[ERR]rad hdr2:0x%X\n",
							read_hdr2);

					return HALMAC_RET_EEPROM_PARSING_FAIL;
				}

				value8 = *(phy_map + efuse_idx);
				*(log_map + eeprom_idx) = value8;

				eeprom_idx++;
				efuse_idx++;

				if (efuse_idx > hw_info->efuse_size -
				    HALMAC_PROTECTED_EFUSE_SIZE_88XX - 1)
					return HALMAC_RET_EEPROM_PARSING_FAIL;

				value8 = *(phy_map + efuse_idx);
				*(log_map + eeprom_idx) = value8;

				efuse_idx++;

				if (efuse_idx > hw_info->efuse_size -
				    HALMAC_PROTECTED_EFUSE_SIZE_88XX)
					return HALMAC_RET_EEPROM_PARSING_FAIL;
			}
		}
	} while (1);

	adapter->efuse_end = efuse_idx;

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_read_logical_efuse_map_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *map
)
{
	u8 *local_map = NULL;
	u32 efuse_size;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	if (adapter->hal_efuse_map_valid == _FALSE) {
		efuse_size = adapter->hw_cfg_info.efuse_size;

		local_map = (u8 *)PLTFM_MALLOC(efuse_size);
		if (!local_map) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]local map\n");
			return HALMAC_RET_MALLOC_FAIL;
		}

		status = halmac_func_read_efuse_88xx(
					adapter, 0, efuse_size, local_map);
		if (status != HALMAC_RET_SUCCESS) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]read efuse\n");
			PLTFM_FREE(local_map, efuse_size);
			return status;
		}

		if (!adapter->efuse_map) {
			adapter->efuse_map = (u8 *)PLTFM_MALLOC(efuse_size);
			if (!adapter->efuse_map) {
				PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
						"[ERR]malloc adapter map\n");
				PLTFM_FREE(local_map, efuse_size);
				return HALMAC_RET_MALLOC_FAIL;
			}
		}

		PLTFM_MUTEX_LOCK(&adapter->EfuseMutex);
		PLTFM_MEMCPY(adapter->efuse_map, local_map, efuse_size);
		adapter->hal_efuse_map_valid = _TRUE;
		PLTFM_MUTEX_UNLOCK(&adapter->EfuseMutex);

		PLTFM_FREE(local_map, efuse_size);
	}

	if (halmac_eeprom_parser_88xx(adapter, adapter->efuse_map, map) !=
	    HALMAC_RET_SUCCESS)
		return HALMAC_RET_EEPROM_PARSING_FAIL;

	return status;
}

static enum halmac_ret_status
halmac_func_pg_efuse_by_map_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_pg_efuse_info *info,
	IN enum halmac_efuse_read_cfg cfg
)
{
	u8 *updated_mask = NULL;
	u32 mask_size = adapter->hw_cfg_info.eeprom_size >> 4;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	updated_mask = (u8 *)PLTFM_MALLOC(mask_size);
	if (!updated_mask) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]malloc updated mask\n");
		return HALMAC_RET_MALLOC_FAIL;
	}
	PLTFM_MEMSET(updated_mask, 0x00, mask_size);

	status = halmac_update_eeprom_mask_88xx(adapter, info, updated_mask);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]update eeprom mask\n");
		PLTFM_FREE(updated_mask, mask_size);
		return status;
	}

	status = halmac_check_efuse_enough_88xx(adapter, info, updated_mask);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]chk efuse enough\n");
		PLTFM_FREE(updated_mask, mask_size);
		return status;
	}

	status = halmac_program_efuse_88xx(adapter, info, updated_mask);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]pg efuse\n");
		PLTFM_FREE(updated_mask, mask_size);
		return status;
	}

	PLTFM_FREE(updated_mask, mask_size);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_dump_efuse_drv_88xx(
	IN struct halmac_adapter *adapter
)
{
	u8 *map = NULL;
	u32 efuse_size = adapter->hw_cfg_info.efuse_size;

	if (!adapter->efuse_map) {
		adapter->efuse_map = (u8 *)PLTFM_MALLOC(efuse_size);
		if (!adapter->efuse_map) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
					"[ERR]malloc adapter map!!\n");
			halmac_reset_feature_88xx(
				adapter, HALMAC_FEATURE_DUMP_PHYSICAL_EFUSE);
			return HALMAC_RET_MALLOC_FAIL;
		}
	}

	if (adapter->hal_efuse_map_valid == _FALSE) {
		map = (u8 *)PLTFM_MALLOC(efuse_size);
		if (!map) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]malloc map\n");
			return HALMAC_RET_MALLOC_FAIL;
		}

		if (halmac_read_hw_efuse_88xx(adapter, 0, efuse_size, map) !=
		    HALMAC_RET_SUCCESS) {
			PLTFM_FREE(map, efuse_size);
			return HALMAC_RET_EFUSE_R_FAIL;
		}

		PLTFM_MUTEX_LOCK(&adapter->EfuseMutex);
		PLTFM_MEMCPY(adapter->efuse_map, map, efuse_size);
		adapter->hal_efuse_map_valid = _TRUE;
		PLTFM_MUTEX_UNLOCK(&adapter->EfuseMutex);

		PLTFM_FREE(map, efuse_size);
	}

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_dump_efuse_fw_88xx(
	IN struct halmac_adapter *adapter
)
{
	u8 h2c_buf[HALMAC_H2C_CMD_SIZE_88XX] = { 0 };
	u16 seq_num = 0;
	u32 efuse_size = adapter->hw_cfg_info.efuse_size;
	struct halmac_h2c_header_info hdr_info;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	hdr_info.sub_cmd_id = SUB_CMD_ID_DUMP_PHYSICAL_EFUSE;
	hdr_info.content_size = 0;
	hdr_info.ack = _TRUE;
	halmac_set_h2c_pkt_hdr_88xx(adapter, h2c_buf, &hdr_info, &seq_num);

	adapter->halmac_state.efuse_state_set.seq_num = seq_num;

	if (!adapter->efuse_map) {
		adapter->efuse_map = (u8 *)PLTFM_MALLOC(efuse_size);
		if (!adapter->efuse_map) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
					"[ERR]malloc adapter map\n");
			halmac_reset_feature_88xx(
				adapter, HALMAC_FEATURE_DUMP_PHYSICAL_EFUSE);
			return HALMAC_RET_MALLOC_FAIL;
		}
	}

	if (adapter->hal_efuse_map_valid == _FALSE) {
		status = halmac_send_h2c_pkt_88xx(adapter, h2c_buf,
						  HALMAC_H2C_CMD_SIZE_88XX);
		if (status != HALMAC_RET_SUCCESS) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]send h2c pkt\n");
			halmac_reset_feature_88xx(
				adapter, HALMAC_FEATURE_DUMP_PHYSICAL_EFUSE);
			return status;
		}
	}

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_func_write_logical_efuse_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u8 value
)
{
	u8 efuse_byte1;
	u8 efuse_byte2;
	u8 blk;
	u8 blk_idx;
	u8 efuse_hdr;
	u8 efuse_hdr2;
	u8 *map = NULL;
	u32 eeprom_size = adapter->hw_cfg_info.eeprom_size;
	u32 efuse_end;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	map = (u8 *)PLTFM_MALLOC(eeprom_size);
	if (!map) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]malloc map\n");
		return HALMAC_RET_MALLOC_FAIL;
	}
	PLTFM_MEMSET(map, 0xFF, eeprom_size);

	status = halmac_read_logical_efuse_map_88xx(adapter, map);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]read logical efuse\n");
		PLTFM_FREE(map, eeprom_size);
		return status;
	}

	if (*(map + offset) != value) {
		efuse_end = adapter->efuse_end;
		blk = (u8)(offset >> 3);
		blk_idx = (u8)((offset & (8 - 1)) >> 1);

		if (offset > 0x7f) {
			efuse_hdr = (((blk & 0x07) << 5) & 0xE0) | 0x0F;
			efuse_hdr2 = (u8)(((blk & 0x78) << 1) +
						((0x1 << blk_idx) ^ 0x0F));
		} else {
			efuse_hdr = (u8)((blk << 4) +
						((0x01 << blk_idx) ^ 0x0F));
		}

		if ((offset & 1) == 0) {
			efuse_byte1 = value;
			efuse_byte2 = *(map + offset + 1);
		} else {
			efuse_byte1 = *(map + offset - 1);
			efuse_byte2 = value;
		}

		if (offset > 0x7f) {
			if (adapter->hw_cfg_info.efuse_size <=
			    4 + HALMAC_PROTECTED_EFUSE_SIZE_88XX + efuse_end) {
				PLTFM_FREE(map, eeprom_size);
				return HALMAC_RET_EFUSE_NOT_ENOUGH;
			}

			status = halmac_func_write_efuse_88xx(
					adapter, efuse_end, efuse_hdr);
			if (status != HALMAC_RET_SUCCESS) {
				PLTFM_FREE(map, eeprom_size);
				return status;
			}

			status = halmac_func_write_efuse_88xx(
					adapter, efuse_end + 1, efuse_hdr2);
			if (status != HALMAC_RET_SUCCESS) {
				PLTFM_FREE(map, eeprom_size);
				return status;
			}

			status = halmac_func_write_efuse_88xx(
					adapter, efuse_end + 2, efuse_byte1);
			if (status != HALMAC_RET_SUCCESS) {
				PLTFM_FREE(map, eeprom_size);
				return status;
			}

			status = halmac_func_write_efuse_88xx(
					adapter, efuse_end + 3, efuse_byte2);
			if (status != HALMAC_RET_SUCCESS) {
				PLTFM_FREE(map, eeprom_size);
				return status;
			}
		} else {
			if (adapter->hw_cfg_info.efuse_size <=
			    3 + HALMAC_PROTECTED_EFUSE_SIZE_88XX + efuse_end) {
				PLTFM_FREE(map, eeprom_size);
				return HALMAC_RET_EFUSE_NOT_ENOUGH;
			}

			status = halmac_func_write_efuse_88xx(
					adapter, efuse_end, efuse_hdr);
			if (status != HALMAC_RET_SUCCESS) {
				PLTFM_FREE(map, eeprom_size);
				return status;
			}

			status = halmac_func_write_efuse_88xx(
					adapter, efuse_end + 1, efuse_byte1);
			if (status != HALMAC_RET_SUCCESS) {
				PLTFM_FREE(map, eeprom_size);
				return status;
			}

			status = halmac_func_write_efuse_88xx(
					adapter, efuse_end + 2, efuse_byte2);
			if (status != HALMAC_RET_SUCCESS) {
				PLTFM_FREE(map, eeprom_size);
				return status;
			}
		}
	}

	PLTFM_FREE(map, eeprom_size);

	return HALMAC_RET_SUCCESS;
}

enum halmac_ret_status
halmac_func_read_efuse_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u32 size,
	OUT u8 *map
)
{
	if (!map) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]malloc map\n");
		return HALMAC_RET_NULL_POINTER;
	}

	if (adapter->hal_efuse_map_valid == _TRUE) {
		PLTFM_MEMCPY(map, adapter->efuse_map + offset, size);
	} else {
		if (halmac_read_hw_efuse_88xx(adapter, offset, size, map) !=
		    HALMAC_RET_SUCCESS)
			return HALMAC_RET_EFUSE_R_FAIL;
	}

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_update_eeprom_mask_88xx(
	IN struct halmac_adapter *adapter,
	INOUT struct halmac_pg_efuse_info *info,
	OUT u8 *updated_mask
)
{
	u8 *map = NULL;
	u8 clr_bit = 0;
	u32 eeprom_size = adapter->hw_cfg_info.eeprom_size;
	u8 *map_pg;
	u8 *efuse_mask;
	u16 i;
	u16 j;
	u16 map_offset;
	u16 mask_offset;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	map = (u8 *)PLTFM_MALLOC(eeprom_size);
	if (!map) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]malloc map\n");
		return HALMAC_RET_MALLOC_FAIL;
	}
	PLTFM_MEMSET(map, 0xFF, eeprom_size);

	PLTFM_MEMSET(updated_mask, 0x00, info->efuse_mask_size);

	status = halmac_read_logical_efuse_map_88xx(adapter, map);

	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_FREE(map, eeprom_size);
		return status;
	}

	map_pg = info->pEfuse_map;
	efuse_mask = info->pEfuse_mask;

	for (i = 0; i < info->efuse_mask_size; i++)
		*(updated_mask + i) = *(efuse_mask + i);

	for (i = 0; i < info->efuse_map_size; i += 16) {
		for (j = 0; j < 16; j += 2) {
			map_offset = i + j;
			mask_offset = i >> 4;
			if (*(u16 *)(map_pg + map_offset) ==
			    *(u16 *)(map + map_offset)) {
				switch (j) {
				case 0:
					clr_bit = BIT(4);
					break;
				case 2:
					clr_bit = BIT(5);
					break;
				case 4:
					clr_bit = BIT(6);
					break;
				case 6:
					clr_bit = BIT(7);
					break;
				case 8:
					clr_bit = BIT(0);
					break;
				case 10:
					clr_bit = BIT(1);
					break;
				case 12:
					clr_bit = BIT(2);
					break;
				case 14:
					clr_bit = BIT(3);
					break;
				default:
					break;
				}
				*(updated_mask + mask_offset) &= ~clr_bit;
			}
		}
	}

	PLTFM_FREE(map, eeprom_size);

	return status;
}

static enum halmac_ret_status
halmac_check_efuse_enough_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_pg_efuse_info *info,
	IN u8 *updated_mask
)
{
	u8 pre_word_en, word_en;
	u8 efuse_hdr, efuse_hdr2;
	u8 blk;
	u16 i, j;
	u32 efuse_end;
	u32 eeprom_offset, pg_num = 0;

	efuse_end = adapter->efuse_end;

	for (i = 0; i < info->efuse_map_size; i = i + 8) {
		eeprom_offset = i;

		if ((eeprom_offset & 7) > 0) {
			pre_word_en = (*(updated_mask + (i >> 4)) & 0x0F);
			word_en = pre_word_en ^ 0x0F;
		} else {
			pre_word_en = (*(updated_mask + (i >> 4)) >> 4);
			word_en = pre_word_en ^ 0x0F;
		}

		blk = (u8)(eeprom_offset >> 3);

		if (pre_word_en > 0) {
			if (eeprom_offset > 0x7f) {
				efuse_hdr = (((blk & 0x07) << 5) & 0xE0) |
									0x0F;
				efuse_hdr2 = (u8)((blk & 0x78) << 1) + word_en;
			} else {
				efuse_hdr = (u8)((blk << 4) + word_en);
			}

			if (eeprom_offset > 0x7f) {
				pg_num++;
				pg_num++;
				efuse_end = efuse_end + 2;
				for (j = 0; j < 4; j++) {
					if (((pre_word_en >> j) & 0x1) > 0) {
						pg_num++;
						pg_num++;
						efuse_end = efuse_end + 2;
					}
				}
			} else {
				pg_num++;
				efuse_end = efuse_end + 1;
				for (j = 0; j < 4; j++) {
					if (((pre_word_en >> j) & 0x1) > 0) {
						pg_num++;
						pg_num++;
						efuse_end = efuse_end + 2;
					}
				}
			}
		}
	}

	if (adapter->hw_cfg_info.efuse_size <=
	    (pg_num + HALMAC_PROTECTED_EFUSE_SIZE_88XX + adapter->efuse_end))
		return HALMAC_RET_EFUSE_NOT_ENOUGH;

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_func_pg_extend_efuse_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_pg_efuse_info *info,
	IN u8 word_en,
	IN u8 pre_word_en,
	IN u32 eeprom_offset
)
{
	u8 blk;
	u8 efuse_hdr;
	u8 efuse_hdr2;
	u16 i;
	u32 efuse_end;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	efuse_end = adapter->efuse_end;

	blk = (u8)(eeprom_offset >> 3);
	efuse_hdr = (((blk & 0x07) << 5) & 0xE0) | 0x0F;
	efuse_hdr2 = (u8)(((blk & 0x78) << 1) + word_en);

	status = halmac_func_write_efuse_88xx(adapter, efuse_end, efuse_hdr);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]write efuse\n");
		return status;
	}

	status = halmac_func_write_efuse_88xx(
					adapter, efuse_end + 1, efuse_hdr2);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]write efuse(+1)\n");
		return status;
	}

	efuse_end = efuse_end + 2;
	for (i = 0; i < 4; i++) {
		if (((pre_word_en >> i) & 0x1) > 0) {
			status = halmac_func_write_efuse_88xx(
					adapter, efuse_end,
					*(info->pEfuse_map +
					eeprom_offset + (i << 1)));
			if (status != HALMAC_RET_SUCCESS) {
				PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
						"[ERR]write efuse(<<1)\n");
				return status;
			}

			status = halmac_func_write_efuse_88xx(
					adapter, efuse_end + 1,
					*(info->pEfuse_map +
					eeprom_offset + (i << 1) + 1));
			if (status != HALMAC_RET_SUCCESS) {
				PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
						"[ERR]write efuse(<<1)+1\n");
				return status;
			}
			efuse_end = efuse_end + 2;
		}
	}
	adapter->efuse_end = efuse_end;
	return status;
}

static enum halmac_ret_status
halmac_func_pg_efuse_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_pg_efuse_info *info,
	IN u8 word_en,
	IN u8 pre_word_en,
	IN u32 eeprom_offset
)
{
	u8 blk;
	u8 efuse_hdr;
	u16 i;
	u32 efuse_end;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	efuse_end = adapter->efuse_end;

	blk = (u8)(eeprom_offset >> 3);
	efuse_hdr = (u8)((blk << 4) + word_en);

	status = halmac_func_write_efuse_88xx(adapter, efuse_end, efuse_hdr);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]write efuse\n");
		return status;
	}
	efuse_end = efuse_end + 1;
	for (i = 0; i < 4; i++) {
		if (((pre_word_en >> i) & 0x1) > 0) {
			status = halmac_func_write_efuse_88xx(
					adapter, efuse_end,
					*(info->pEfuse_map +
					eeprom_offset + (i << 1)));
			if (status != HALMAC_RET_SUCCESS) {
				PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
						"[ERR]write efuse(<<1)\n");
				return status;
			}
			status = halmac_func_write_efuse_88xx(
					adapter, efuse_end + 1,
					*(info->pEfuse_map +
					eeprom_offset + (i << 1) + 1));
			if (status != HALMAC_RET_SUCCESS) {
				PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
						"[ERR]write efuse(<<1)+1\n");
				return status;
			}
			efuse_end = efuse_end + 2;
		}
	}
	adapter->efuse_end = efuse_end;
	return status;
}

static enum halmac_ret_status
halmac_program_efuse_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_pg_efuse_info *info,
	IN u8 *updated_mask
)
{
	u8 pre_word_en;
	u8 word_en;
	u16 i;
	u32 eeprom_offset;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	for (i = 0; i < info->efuse_map_size; i = i + 8) {
		eeprom_offset = i;

		if (((eeprom_offset >> 3) & 1) > 0) {
			pre_word_en = (*(updated_mask + (i >> 4)) & 0x0F);
			word_en = pre_word_en ^ 0x0F;
		} else {
			pre_word_en = (*(updated_mask + (i >> 4)) >> 4);
			word_en = pre_word_en ^ 0x0F;
		}

		if (pre_word_en > 0) {
			if (eeprom_offset > 0x7f) {
				status = halmac_func_pg_extend_efuse_88xx(
						adapter, info, word_en,
						pre_word_en, eeprom_offset);
				if (status != HALMAC_RET_SUCCESS) {
					PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
							"[ERR]extend efuse\n");
					return status;
				}
			} else {
				status = halmac_func_pg_efuse_88xx(
						adapter, info, word_en,
						pre_word_en, eeprom_offset);
				if (status != HALMAC_RET_SUCCESS) {
					PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
							"[ERR]extend efuse");
					return status;
				}
			}
		}
	}

	return status;
}

static void
halmac_mask_eeprom_88xx(
	IN struct halmac_adapter *adapter,
	INOUT struct halmac_pg_efuse_info *info
)
{
	u8 pre_word_en;
	u8 *updated_mask;
	u16 i;
	u16 j;
	u32 eeprom_offset;

	updated_mask = info->pEfuse_mask;

	for (i = 0; i < info->efuse_map_size; i = i + 8) {
		eeprom_offset = i;

		if (((eeprom_offset >> 3) & 1) > 0)
			pre_word_en = (*(updated_mask + (i >> 4)) & 0x0F);
		else
			pre_word_en = (*(updated_mask + (i >> 4)) >> 4);

		for (j = 0; j < 4; j++) {
			if (((pre_word_en >> j) & 0x1) == 0) {
				*(info->pEfuse_map + eeprom_offset +
							(j << 1)) = 0xFF;
				*(info->pEfuse_map + eeprom_offset +
							(j << 1) + 1) = 0xFF;
			}
		}
	}
}

enum halmac_ret_status
halmac_get_efuse_data_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
)
{
	u8 seg_id;
	u8 seg_size;
	u8 seq_num;
	u8 fw_rc;
	u8 *map = NULL;
	u32 eeprom_size = adapter->hw_cfg_info.eeprom_size;
	struct halmac_efuse_state_set *state =
		&adapter->halmac_state.efuse_state_set;
	enum halmac_cmd_process_status proc_status;

	seq_num = (u8)EFUSE_DATA_GET_H2C_SEQ(buf);
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

	seg_id = (u8)EFUSE_DATA_GET_SEGMENT_ID(buf);
	seg_size = (u8)EFUSE_DATA_GET_SEGMENT_SIZE(buf);
	if (seg_id == 0)
		adapter->efuse_segment_size = seg_size;

	map = (u8 *)PLTFM_MALLOC(eeprom_size);
	if (!map) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]malloc map\n");
		return HALMAC_RET_MALLOC_FAIL;
	}
	PLTFM_MEMSET(map, 0xFF, eeprom_size);

	PLTFM_MUTEX_LOCK(&adapter->EfuseMutex);
	PLTFM_MEMCPY(adapter->efuse_map + seg_id * adapter->efuse_segment_size,
		     buf + HALMAC_C2H_DATA_OFFSET_88XX, seg_size);
	PLTFM_MUTEX_UNLOCK(&adapter->EfuseMutex);

	if (EFUSE_DATA_GET_END_SEGMENT(buf) == _FALSE) {
		PLTFM_FREE(map, eeprom_size);
		return HALMAC_RET_SUCCESS;
	}

	fw_rc = state->fw_return_code;

	if ((enum halmac_h2c_return_code)fw_rc == HALMAC_H2C_RETURN_SUCCESS) {
		proc_status = HALMAC_CMD_PROCESS_DONE;
		state->process_status = proc_status;

		PLTFM_MUTEX_LOCK(&adapter->EfuseMutex);
		adapter->hal_efuse_map_valid = _TRUE;
		PLTFM_MUTEX_UNLOCK(&adapter->EfuseMutex);

		if (adapter->event_trigger.physical_efuse_map == 1) {
			PLTFM_EVENT_INDICATION(
				HALMAC_FEATURE_DUMP_PHYSICAL_EFUSE,
				proc_status, adapter->efuse_map,
				adapter->hw_cfg_info.efuse_size);
			adapter->event_trigger.physical_efuse_map = 0;
		}

		if (adapter->event_trigger.logical_efuse_map == 1) {
			if (halmac_eeprom_parser_88xx(
					adapter, adapter->efuse_map, map) !=
					HALMAC_RET_SUCCESS) {
				PLTFM_FREE(map, eeprom_size);
				return HALMAC_RET_EEPROM_PARSING_FAIL;
			}
			PLTFM_EVENT_INDICATION(
				HALMAC_FEATURE_DUMP_LOGICAL_EFUSE,
				proc_status, map, eeprom_size);
			adapter->event_trigger.logical_efuse_map = 0;
		}
	} else {
		proc_status = HALMAC_CMD_PROCESS_ERROR;
		state->process_status = proc_status;

		if (adapter->event_trigger.physical_efuse_map == 1) {
			PLTFM_EVENT_INDICATION(
				HALMAC_FEATURE_DUMP_PHYSICAL_EFUSE,
				proc_status, &state->fw_return_code, 1);
			adapter->event_trigger.physical_efuse_map = 0;
		}

		if (adapter->event_trigger.logical_efuse_map == 1) {
			if (halmac_eeprom_parser_88xx(
					adapter, adapter->efuse_map, map) !=
					HALMAC_RET_SUCCESS) {
				PLTFM_FREE(map, eeprom_size);
				return HALMAC_RET_EEPROM_PARSING_FAIL;
			}
			PLTFM_EVENT_INDICATION(
				HALMAC_FEATURE_DUMP_LOGICAL_EFUSE,
				proc_status, &state->fw_return_code, 1);
			adapter->event_trigger.logical_efuse_map = 0;
		}
	}

	PLTFM_FREE(map, eeprom_size);

	return HALMAC_RET_SUCCESS;
}

enum halmac_ret_status
halmac_query_dump_physical_efuse_status_88xx(
	IN struct halmac_adapter *adapter,
	OUT enum halmac_cmd_process_status *proc_status,
	INOUT u8 *data,
	INOUT u32 *size
)
{
	u8 *map = NULL;
	u32 efuse_size = adapter->hw_cfg_info.efuse_size;
	struct halmac_efuse_state_set *state =
			&adapter->halmac_state.efuse_state_set;

	*proc_status = state->process_status;

	if (!data)
		return HALMAC_RET_NULL_POINTER;

	if (!size)
		return HALMAC_RET_NULL_POINTER;

	if (*proc_status == HALMAC_CMD_PROCESS_DONE) {
		if (*size < efuse_size) {
			*size = efuse_size;
			return HALMAC_RET_BUFFER_TOO_SMALL;
		}

		*size = efuse_size;

		map = (u8 *)PLTFM_MALLOC(efuse_size);
		if (!map) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]malloc map\n");
			return HALMAC_RET_MALLOC_FAIL;
		}
		PLTFM_MEMSET(map, 0xFF, efuse_size);
		PLTFM_MUTEX_LOCK(&adapter->EfuseMutex);
		PLTFM_MEMCPY(map, adapter->efuse_map,
			     efuse_size - HALMAC_PROTECTED_EFUSE_SIZE_88XX);
		PLTFM_MEMCPY(
			map + efuse_size -
			HALMAC_PROTECTED_EFUSE_SIZE_88XX +
			HALMAC_RSVD_CS_EFUSE_SIZE_88XX,
			adapter->efuse_map + efuse_size -
			HALMAC_PROTECTED_EFUSE_SIZE_88XX +
			HALMAC_RSVD_CS_EFUSE_SIZE_88XX,
			HALMAC_PROTECTED_EFUSE_SIZE_88XX -
			HALMAC_RSVD_EFUSE_SIZE_88XX -
			HALMAC_RSVD_CS_EFUSE_SIZE_88XX);
		PLTFM_MUTEX_UNLOCK(&adapter->EfuseMutex);

		PLTFM_MEMCPY(data, map, *size);

		PLTFM_FREE(map, efuse_size);
	}

	return HALMAC_RET_SUCCESS;
}

enum halmac_ret_status
halmac_query_dump_logical_efuse_status_88xx(
	IN struct halmac_adapter *adapter,
	OUT enum halmac_cmd_process_status *proc_status,
	INOUT u8 *data,
	INOUT u32 *size
)
{
	u8 *map = NULL;
	u32 eeprom_size = adapter->hw_cfg_info.eeprom_size;
	struct halmac_efuse_state_set *state =
			&adapter->halmac_state.efuse_state_set;

	*proc_status = state->process_status;

	if (!data)
		return HALMAC_RET_NULL_POINTER;

	if (!size)
		return HALMAC_RET_NULL_POINTER;

	if (*proc_status == HALMAC_CMD_PROCESS_DONE) {
		if (*size < eeprom_size) {
			*size = eeprom_size;
			return HALMAC_RET_BUFFER_TOO_SMALL;
		}

		*size = eeprom_size;

		map = (u8 *)PLTFM_MALLOC(eeprom_size);
		if (!map) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]malloc map\n");
			return HALMAC_RET_MALLOC_FAIL;
		}
		PLTFM_MEMSET(map, 0xFF, eeprom_size);

		if (halmac_eeprom_parser_88xx(
				adapter, adapter->efuse_map, map) !=
				HALMAC_RET_SUCCESS) {
			PLTFM_FREE(map, eeprom_size);
			return HALMAC_RET_EEPROM_PARSING_FAIL;
		}

		PLTFM_MEMCPY(data, map, *size);

		PLTFM_FREE(map, eeprom_size);
	}

	return HALMAC_RET_SUCCESS;
}

enum halmac_ret_status
halmac_get_h2c_ack_phy_efuse_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
)
{
	u8 seq_num = 0;
	u8 fw_rc;
	struct halmac_efuse_state_set *state =
		&adapter->halmac_state.efuse_state_set;

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

	return HALMAC_RET_SUCCESS;
}

#endif /* HALMAC_88XX_SUPPORT */
