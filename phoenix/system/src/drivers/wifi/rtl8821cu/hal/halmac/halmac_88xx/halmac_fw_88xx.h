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

#ifndef _HALMAC_FW_88XX_H_
#define _HALMAC_FW_88XX_H_

#include "../halmac_api.h"

#if HALMAC_88XX_SUPPORT

#define HALMC_DDMA_POLLING_COUNT		1000

#endif /* HALMAC_88XX_SUPPORT */

enum halmac_ret_status
halmac_download_firmware_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *fw_bin,
	IN u32 size
);

enum halmac_ret_status
halmac_free_download_firmware_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_dlfw_mem mem_sel,
	IN u8 *fw_bin,
	IN u32 size
);

enum halmac_ret_status
halmac_get_fw_version_88xx(
	IN struct halmac_adapter *adapter,
	OUT struct halmac_fw_version *ver
);

enum halmac_ret_status
halmac_check_fw_status_88xx(
	IN struct halmac_adapter *adapter,
	OUT u8 *fw_status
);

enum halmac_ret_status
halmac_dump_fw_dmem_88xx(
	IN struct halmac_adapter *adapter,
	INOUT u8 *dmem,
	INOUT u32 *size
);

enum halmac_ret_status
halmac_cfg_max_dl_size_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 size
);

enum halmac_ret_status
halmac_enter_cpu_sleep_mode_88xx(
	IN struct halmac_adapter *adapter
);

enum halmac_ret_status
halmac_get_cpu_mode_88xx(
	IN struct halmac_adapter *adapter,
	OUT enum halmac_wlcpu_mode *mode
);

#endif/* _HALMAC_FW_88XX_H_ */
