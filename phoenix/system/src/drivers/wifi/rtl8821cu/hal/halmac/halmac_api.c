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

#include "halmac_type.h"
#include "halmac_api.h"

#if (HALMAC_PLATFORM_WINDOWS)

#if HALMAC_8822B_SUPPORT
#include "halmac_88xx/halmac_init_win8822b.h"
#endif

#if HALMAC_8821C_SUPPORT
#include "halmac_88xx/halmac_init_win8821c.h"
#endif

#if HALMAC_8814B_SUPPORT
#include "halmac_88xx_v1/halmac_init_win8814b_v1.h"
#endif

#if HALMAC_8822C_SUPPORT
#include "halmac_88xx/halmac_init_win8822c.h"
#endif

#else

#if HALMAC_88XX_SUPPORT
#include "halmac_88xx/halmac_init_88xx.h"
#endif
#if HALMAC_88XX_V1_SUPPORT
#include "halmac_88xx_v1/halmac_init_88xx_v1.h"
#endif

#endif

enum halmac_chip_id_hw_def {
	HALMAC_CHIP_ID_HW_DEF_8723A = 0x01,
	HALMAC_CHIP_ID_HW_DEF_8188E = 0x02,
	HALMAC_CHIP_ID_HW_DEF_8881A = 0x03,
	HALMAC_CHIP_ID_HW_DEF_8812A = 0x04,
	HALMAC_CHIP_ID_HW_DEF_8821A = 0x05,
	HALMAC_CHIP_ID_HW_DEF_8723B = 0x06,
	HALMAC_CHIP_ID_HW_DEF_8192E = 0x07,
	HALMAC_CHIP_ID_HW_DEF_8814A = 0x08,
	HALMAC_CHIP_ID_HW_DEF_8821C = 0x09,
	HALMAC_CHIP_ID_HW_DEF_8822B = 0x0A,
	HALMAC_CHIP_ID_HW_DEF_8703B = 0x0B,
	HALMAC_CHIP_ID_HW_DEF_8188F = 0x0C,
	HALMAC_CHIP_ID_HW_DEF_8192F = 0x0D,
	HALMAC_CHIP_ID_HW_DEF_8197F = 0x0E,
	HALMAC_CHIP_ID_HW_DEF_8723D = 0x0F,
	HALMAC_CHIP_ID_HW_DEF_8814B = 0x11,
	HALMAC_CHIP_ID_HW_DEF_8822C = 0x13,
	HALMAC_CHIP_ID_HW_DEF_UNDEFINE = 0x7F,
	HALMAC_CHIP_ID_HW_DEF_PS = 0xEA,
};

static enum halmac_ret_status
halmac_check_platform_api(
	IN void *drv_adapter,
	IN enum halmac_interface intf,
	IN struct halmac_platform_api *pltfm_api
);

static enum halmac_ret_status
halmac_get_chip_info(
	IN void	*drv_adapter,
	IN struct halmac_platform_api *pltfm_api,
	IN enum halmac_interface intf,
	IN struct halmac_adapter *adapter
);

static u8
pltfm_reg_r8_sdio(
	IN void	*drv_adapter,
	IN struct halmac_platform_api *pltfm_api,
	IN u32 offset
);

static enum halmac_ret_status
pltfm_reg_w8_sdio(
	IN void	*drv_adapter,
	IN struct halmac_platform_api *pltfm_api,
	IN u32 offset,
	IN u8 data
);

static enum halmac_ret_status
halmac_convert_to_sdio_bus_offset(
	INOUT u32 *halmac_offset
);

/**
 * halmac_init_adapter() - init halmac_adapter
 * @drv_adapter : the adapter of caller
 * @pltfm_api : the platform APIs which is used in halmac
 * @intf : bus interface
 * @halmac_adapter : the adapter of halmac
 * @halmac_api : the function pointer of APIs
 * Author : KaiYuan Chang / Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_init_adapter(
	IN void	*drv_adapter,
	IN struct halmac_platform_api *pltfm_api,
	IN enum halmac_interface intf,
	OUT struct halmac_adapter **halmac_adapter,
	OUT struct halmac_api **halmac_api
)
{
	struct halmac_adapter *adapter = NULL;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	u8 *pBuf = NULL;

	union {
		u32 i;
		u8 x[4];
	} ENDIAN_CHECK = { 0x01000000 };

	status = halmac_check_platform_api(drv_adapter, intf, pltfm_api);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ALWAYS,
			     HALMAC_SVN_VER "\n"
			     "HALMAC_MAJOR_VER = %x\n"
			     "HALMAC_PROTOTYPE_VER = %x\n"
			     "HALMAC_MINOR_VER = %x\n"
			     "HALMAC_PATCH_VER = %x\n",
			     HALMAC_MAJOR_VER, HALMAC_PROTOTYPE_VER,
			     HALMAC_MINOR_VER, HALMAC_PATCH_VER);

	if (ENDIAN_CHECK.x[0] == HALMAC_SYSTEM_ENDIAN) {
		pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
				     HALMAC_DBG_ERR,
				     "[ERR]Endian setting err!!\n");
		return HALMAC_RET_ENDIAN_ERR;
	}

	pBuf = (u8 *)pltfm_api->RTL_MALLOC(drv_adapter, sizeof(*adapter));

	if (!pBuf) {
		pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
				     HALMAC_DBG_ERR,
				     "[ERR]Malloc HAL adapter err!!\n");
		return HALMAC_RET_MALLOC_FAIL;
	}
	pltfm_api->RTL_MEMSET(drv_adapter, pBuf, 0x00, sizeof(*adapter));
	adapter = (struct halmac_adapter *)pBuf;

	*halmac_adapter = adapter;

	adapter->pHalmac_platform_api = pltfm_api;
	adapter->pDriver_adapter = drv_adapter;
	intf = (intf == HALMAC_INTERFACE_AXI) ? HALMAC_INTERFACE_PCIE : intf;
	adapter->halmac_interface = intf;

	PLTFM_MUTEX_INIT(&adapter->EfuseMutex);
	PLTFM_MUTEX_INIT(&adapter->h2c_seq_mutex);

	if (halmac_get_chip_info(drv_adapter, pltfm_api, intf, adapter)
	    != HALMAC_RET_SUCCESS)
		return HALMAC_RET_CHIP_NOT_SUPPORT;

#if (HALMAC_PLATFORM_WINDOWS == 0)

#if HALMAC_88XX_SUPPORT
	if (adapter->chip_id == HALMAC_CHIP_ID_8822B ||
	    adapter->chip_id == HALMAC_CHIP_ID_8821C ||
	    adapter->chip_id == HALMAC_CHIP_ID_8822C) {
		halmac_init_adapter_para_88xx(adapter);
		status = halmac_mount_api_88xx(adapter);
	}
#endif

#if HALMAC_88XX_V1_SUPPORT
	if (adapter->chip_id == HALMAC_CHIP_ID_8814B) {
		halmac_init_adapter_para_88xx_v1(adapter);
		status = halmac_mount_api_88xx_v1(adapter);
	}
#endif

#else

#if HALMAC_8822B_SUPPORT
	if (adapter->chip_id == HALMAC_CHIP_ID_8822B) {
		halmac_init_adapter_para_win8822b(adapter);
		status = halmac_mount_api_win8822b(adapter);
	}
#endif

#if HALMAC_8821C_SUPPORT
	if (adapter->chip_id == HALMAC_CHIP_ID_8821C) {
		halmac_init_adapter_para_win8821c(adapter);
		status = halmac_mount_api_win8821c(adapter);
	}
#endif

#if HALMAC_8814B_SUPPORT
	if (adapter->chip_id == HALMAC_CHIP_ID_8814B) {
		halmac_init_adapter_para_win8814b_v1(adapter);
		status = halmac_mount_api_win8814b_v1(adapter);
	}
#endif

#if HALMAC_8822C_SUPPORT
	if (adapter->chip_id == HALMAC_CHIP_ID_8822C) {
		halmac_init_adapter_para_win8822c(adapter);
		status = halmac_mount_api_win8822c(adapter);
	}
#endif

#endif
	*halmac_api = (struct halmac_api *)adapter->pHalmac_api;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return status;
}

/**
 * halmac_halt_api() - stop halmac_api action
 * @adapter : the adapter of halmac
 * Author : Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_halt_api(
	IN struct halmac_adapter *adapter
)
{
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	adapter->halmac_state.api_state = HALMAC_API_STATE_HALT;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_deinit_adapter() - deinit halmac adapter
 * @adapter : the adapter of halmac
 * Author : KaiYuan Chang / Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_deinit_adapter(
	IN struct halmac_adapter *adapter
)
{
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	PLTFM_MUTEX_DEINIT(&adapter->EfuseMutex);
	PLTFM_MUTEX_DEINIT(&adapter->h2c_seq_mutex);

	if (adapter->efuse_map) {
		PLTFM_FREE(adapter->efuse_map, adapter->hw_cfg_info.efuse_size);
		adapter->efuse_map = (u8 *)NULL;
	}

	if (adapter->sdio_free_space.macid_map) {
		PLTFM_FREE(adapter->sdio_free_space.macid_map,
			   adapter->sdio_free_space.macid_map_size);
		adapter->sdio_free_space.macid_map = (u8 *)NULL;
	}

	if (adapter->halmac_state.psd_set.data) {
		PLTFM_FREE(adapter->halmac_state.psd_set.data,
			   adapter->halmac_state.psd_set.data_size);
		adapter->halmac_state.psd_set.data = (u8 *)NULL;
	}

	if (adapter->pHalmac_api) {
		PLTFM_FREE(adapter->pHalmac_api, sizeof(struct halmac_api));
		adapter->pHalmac_api = NULL;
	}

	adapter->pHalAdapter_backup = NULL;
	PLTFM_FREE(adapter, sizeof(*adapter));

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_check_platform_api(
	IN void *drv_adapter,
	IN enum halmac_interface intf,
	IN struct halmac_platform_api *pltfm_api
)
{
	if (!pltfm_api)
		return HALMAC_RET_PLATFORM_API_NULL;

	if (!pltfm_api->MSG_PRINT)
		return HALMAC_RET_PLATFORM_API_NULL;

	if (intf == HALMAC_INTERFACE_SDIO) {
		if (!pltfm_api->SDIO_CMD52_READ) {
			pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
					     HALMAC_DBG_ERR, "[ERR]sdio-r\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (!pltfm_api->SDIO_CMD53_READ_8) {
			pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
					     HALMAC_DBG_ERR, "[ERR]sdio-r8\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (!pltfm_api->SDIO_CMD53_READ_16) {
			pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
					     HALMAC_DBG_ERR, "[ERR]sdio-r16\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (!pltfm_api->SDIO_CMD53_READ_32) {
			pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
					     HALMAC_DBG_ERR, "[ERR]sdio-r32\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (!pltfm_api->SDIO_CMD53_READ_N) {
			pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
					     HALMAC_DBG_ERR, "[ERR]sdio-rn\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (!pltfm_api->SDIO_CMD52_WRITE) {
			pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
					     HALMAC_DBG_ERR, "[ERR]sdio-w\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (!pltfm_api->SDIO_CMD53_WRITE_8) {
			pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
					     HALMAC_DBG_ERR, "[ERR]sdio-w8\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (!pltfm_api->SDIO_CMD53_WRITE_16) {
			pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
					     HALMAC_DBG_ERR, "[ERR]sdio-w16\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (!pltfm_api->SDIO_CMD53_WRITE_32) {
			pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
					     HALMAC_DBG_ERR, "[ERR]sdio-w32\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (!pltfm_api->SDIO_CMD52_CIA_READ) {
			pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
					     HALMAC_DBG_ERR, "[ERR]sdio-cia\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
	}

	if ((intf == HALMAC_INTERFACE_USB) || (intf == HALMAC_INTERFACE_PCIE)) {
		if (!pltfm_api->REG_READ_8) {
			pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
					     HALMAC_DBG_ERR, "[ERR]reg-r8\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (!pltfm_api->REG_READ_16) {
			pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
					     HALMAC_DBG_ERR, "[ERR]reg-r16\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (!pltfm_api->REG_READ_32) {
			pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
					     HALMAC_DBG_ERR, "[ERR]reg-r32\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (!pltfm_api->REG_WRITE_8) {
			pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
					     HALMAC_DBG_ERR, "[ERR]reg-w8\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (!pltfm_api->REG_WRITE_16) {
			pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
					     HALMAC_DBG_ERR, "[ERR]reg-w16\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (!pltfm_api->REG_WRITE_32) {
			pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
					     HALMAC_DBG_ERR, "[ERR]reg-w32\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
	}

	if (!pltfm_api->RTL_FREE) {
		pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
				     HALMAC_DBG_ERR, "[ERR]mem-free\n");
		return HALMAC_RET_PLATFORM_API_NULL;
	}

	if (!pltfm_api->RTL_MALLOC) {
		pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
				     HALMAC_DBG_ERR, "[ERR]mem-malloc\n");
		return HALMAC_RET_PLATFORM_API_NULL;
	}
	if (!pltfm_api->RTL_MEMCPY) {
		pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
				     HALMAC_DBG_ERR, "[ERR]mem-free\n");
		return HALMAC_RET_PLATFORM_API_NULL;
	}
	if (!pltfm_api->RTL_MEMSET) {
		pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
				     HALMAC_DBG_ERR, "[ERR]mem-set\n");
		return HALMAC_RET_PLATFORM_API_NULL;
	}
	if (!pltfm_api->RTL_DELAY_US) {
		pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
				     HALMAC_DBG_ERR, "[ERR]time-delay\n");
		return HALMAC_RET_PLATFORM_API_NULL;
	}

	if (!pltfm_api->MUTEX_INIT) {
		pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
				     HALMAC_DBG_ERR, "[ERR]mutex-init\n");
		return HALMAC_RET_PLATFORM_API_NULL;
	}
	if (!pltfm_api->MUTEX_DEINIT) {
		pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
				     HALMAC_DBG_ERR, "[ERR]mutex-deinit\n");
		return HALMAC_RET_PLATFORM_API_NULL;
	}
	if (!pltfm_api->MUTEX_LOCK) {
		pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
				     HALMAC_DBG_ERR, "[ERR]mutex-lock\n");
		return HALMAC_RET_PLATFORM_API_NULL;
	}
	if (!pltfm_api->MUTEX_UNLOCK) {
		pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
				     HALMAC_DBG_ERR, "[ERR]mutex-unlock\n");
		return HALMAC_RET_PLATFORM_API_NULL;
	}
	if (!pltfm_api->EVENT_INDICATION) {
		pltfm_api->MSG_PRINT(drv_adapter, HALMAC_MSG_INIT,
				     HALMAC_DBG_ERR, "[ERR]event-indication\n");
		return HALMAC_RET_PLATFORM_API_NULL;
	}

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_get_version() - get HALMAC version
 * @version : return version of major, prototype and minor information
 * Author : KaiYuan Chang / Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_get_version(
	OUT struct halmac_ver *version
)
{
	version->major_ver = (u8)HALMAC_MAJOR_VER;
	version->prototype_ver = (u8)HALMAC_PROTOTYPE_VER;
	version->minor_ver = (u8)HALMAC_MINOR_VER;

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_get_chip_info(
	IN void	*drv_adapter,
	IN struct halmac_platform_api *pltfm_api,
	IN enum halmac_interface intf,
	IN struct halmac_adapter *adapter
)
{
	u8 chip_id;
	u8 chip_ver;
	u32 cnt;

	if (adapter->halmac_interface == HALMAC_INTERFACE_SDIO) {
		pltfm_reg_w8_sdio(drv_adapter, pltfm_api, REG_SDIO_HSUS_CTRL,
				  pltfm_reg_r8_sdio(drv_adapter, pltfm_api,
						    REG_SDIO_HSUS_CTRL) &
						    ~(BIT(0)));

		cnt = 10000;
		while (!(pltfm_reg_r8_sdio(drv_adapter, pltfm_api,
					   REG_SDIO_HSUS_CTRL) & BIT(1))) {
			cnt--;
			if (cnt == 0)
				return HALMAC_RET_SDIO_LEAVE_SUSPEND_FAIL;
		}

		chip_id = pltfm_reg_r8_sdio(drv_adapter, pltfm_api,
					    REG_SYS_CFG2);
		chip_ver =  pltfm_reg_r8_sdio(drv_adapter, pltfm_api,
					      REG_SYS_CFG1 + 1) >> 4;
	} else {
		chip_id = pltfm_api->REG_READ_8(drv_adapter, REG_SYS_CFG2);
		chip_ver = pltfm_api->REG_READ_8(drv_adapter,
						 REG_SYS_CFG1 + 1) >> 4;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Chip id : 0x%X\n", chip_id);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Chip ver : 0x%X\n", chip_ver);

	adapter->chip_version = (enum halmac_chip_ver)chip_ver;

	if (chip_id == HALMAC_CHIP_ID_HW_DEF_8822B) {
		adapter->chip_id = HALMAC_CHIP_ID_8822B;
	} else if (chip_id == HALMAC_CHIP_ID_HW_DEF_8821C) {
		adapter->chip_id = HALMAC_CHIP_ID_8821C;
	} else if (chip_id == HALMAC_CHIP_ID_HW_DEF_8814B) {
		adapter->chip_id = HALMAC_CHIP_ID_8814B;
	} else if (chip_id == HALMAC_CHIP_ID_HW_DEF_8197F) {
		adapter->chip_id = HALMAC_CHIP_ID_8197F;
	} else if (chip_id == HALMAC_CHIP_ID_HW_DEF_8822C) {
		adapter->chip_id = HALMAC_CHIP_ID_8822C;
	} else {
		adapter->chip_id = HALMAC_CHIP_ID_UNDEFINE;
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Chip id is undefined\n");
		return HALMAC_RET_CHIP_NOT_SUPPORT;
	}

	return HALMAC_RET_SUCCESS;
}

static u8
pltfm_reg_r8_sdio(
	IN void	*drv_adapter,
	IN struct halmac_platform_api *pltfm_api,
	IN u32 offset
)
{
	u8 value8;
	u32 halmac_offset = offset;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	if (0 == (halmac_offset & 0xFFFF0000))
		halmac_offset |= WLAN_IOREG_OFFSET;

	status = halmac_convert_to_sdio_bus_offset(&halmac_offset);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	value8 = pltfm_api->SDIO_CMD52_READ(drv_adapter, halmac_offset);

	return value8;
}

static enum halmac_ret_status
pltfm_reg_w8_sdio(
	IN void	*drv_adapter,
	IN struct halmac_platform_api *pltfm_api,
	IN u32 offset,
	IN u8 data
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;
	u32 halmac_offset = offset;

	if (0 == (halmac_offset & 0xFFFF0000))
		halmac_offset |= WLAN_IOREG_OFFSET;

	status = halmac_convert_to_sdio_bus_offset(&halmac_offset);

	if (status != HALMAC_RET_SUCCESS)
		return status;

	pltfm_api->SDIO_CMD52_WRITE(drv_adapter, halmac_offset, data);

	return HALMAC_RET_SUCCESS;
}

/*Note: copy from halmac_convert_to_sdio_bus_offset_88xx*/
static enum halmac_ret_status
halmac_convert_to_sdio_bus_offset(
	INOUT u32 *halmac_offset
)
{
	switch ((*halmac_offset) & 0xFFFF0000) {
	case WLAN_IOREG_OFFSET:
		*halmac_offset = (HALMAC_SDIO_CMD_ADDR_MAC_REG << 13) |
				  (*halmac_offset & HALMAC_WLAN_MAC_REG_MSK);
		break;
	case SDIO_LOCAL_OFFSET:
		*halmac_offset = (HALMAC_SDIO_CMD_ADDR_SDIO_REG << 13) |
				  (*halmac_offset & HALMAC_SDIO_LOCAL_MSK);
		break;
	default:
		*halmac_offset = 0xFFFFFFFF;
		return HALMAC_RET_CONVERT_SDIO_OFFSET_FAIL;
	}

	return HALMAC_RET_SUCCESS;
}

