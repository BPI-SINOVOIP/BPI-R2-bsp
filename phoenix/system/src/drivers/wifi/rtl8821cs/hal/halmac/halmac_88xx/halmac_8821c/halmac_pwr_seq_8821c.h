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

#ifndef HALMAC_POWER_SEQUENCE_8821C
#define HALMAC_POWER_SEQUENCE_8821C

#include "../../halmac_pwr_seq_cmd.h"
#include "../../halmac_hw_cfg.h"

#if HALMAC_8821C_SUPPORT

#define HALMAC_8821C_PWR_SEQ_VER  "V17"

extern PHALMAC_WLAN_PWR_CFG halmac_8821c_card_disable_flow[];
extern PHALMAC_WLAN_PWR_CFG halmac_8821c_card_enable_flow[];
extern PHALMAC_WLAN_PWR_CFG halmac_8821c_suspend_flow[];
extern PHALMAC_WLAN_PWR_CFG halmac_8821c_resume_flow[];
extern PHALMAC_WLAN_PWR_CFG halmac_8821c_hwpdn_flow[];
extern PHALMAC_WLAN_PWR_CFG halmac_8821c_enter_lps_flow[];
extern PHALMAC_WLAN_PWR_CFG halmac_8821c_enter_deep_lps_flow[];
extern PHALMAC_WLAN_PWR_CFG halmac_8821c_leave_lps_flow[];

#endif /* HALMAC_8821C_SUPPORT */

#endif
