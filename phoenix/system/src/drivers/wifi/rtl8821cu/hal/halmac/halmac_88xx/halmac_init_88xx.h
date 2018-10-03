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

#ifndef _HALMAC_INIT_88XX_H_
#define _HALMAC_INIT_88XX_H_

#include "../halmac_api.h"

#if HALMAC_88XX_SUPPORT

enum halmac_ret_status
halmac_register_api_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_api_registry *registry
);

void
halmac_init_adapter_para_88xx(
	IN struct halmac_adapter *adapter
);

void
halmac_init_adapter_dynamic_para_88xx(
	IN struct halmac_adapter *adapter
);

enum halmac_ret_status
halmac_mount_api_88xx(
	IN struct halmac_adapter *adapter
);

enum halmac_ret_status
halmac_pre_init_system_cfg_88xx(
	IN struct halmac_adapter *adapter
);

enum halmac_ret_status
halmac_init_system_cfg_88xx(
	IN struct halmac_adapter *adapter
);

enum halmac_ret_status
halmac_init_edca_cfg_88xx(
	IN struct halmac_adapter *adapter
);

enum halmac_ret_status
halmac_init_wmac_cfg_88xx(
	IN struct halmac_adapter *adapter
);

enum halmac_ret_status
halmac_init_mac_cfg_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_trx_mode mode
);

enum halmac_ret_status
halmac_reset_feature_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_feature_id feature_id
);

enum halmac_ret_status
halmac_verify_platform_api_88xx(
	IN struct halmac_adapter *adapter
);

void
halmac_tx_desc_checksum_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 enable
);

enum halmac_ret_status
halmac_pg_num_parser_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_trx_mode mode,
	IN struct halmac_pg_num *tbl
);

enum halmac_ret_status
halmac_rqpn_parser_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_trx_mode mode,
	IN struct halmac_rqpn *tbl
);

void
halmac_init_offload_feature_state_machine_88xx(
	IN struct halmac_adapter *adapter
);

#endif /* HALMAC_88XX_SUPPORT */

#endif/* _HALMAC_INIT_88XX_H_ */
