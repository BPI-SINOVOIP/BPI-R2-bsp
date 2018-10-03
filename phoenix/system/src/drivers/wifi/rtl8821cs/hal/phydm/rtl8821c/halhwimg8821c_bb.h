/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
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
 *****************************************************************************/

/*Image2HeaderVersion: R2 1.3.2*/
#if (RTL8821C_SUPPORT == 1)
#ifndef __INC_MP_BB_HW_IMG_8821C_H
#define __INC_MP_BB_HW_IMG_8821C_H


/******************************************************************************
*                           agc_tab.TXT
******************************************************************************/

void
odm_read_and_config_mp_8821c_agc_tab(/* tc: Test Chip, mp: mp Chip*/
	struct	PHY_DM_STRUCT *p_dm
);
u32	odm_get_version_mp_8821c_agc_tab(void);

/******************************************************************************
*                           agc_tab_diff.TXT
******************************************************************************/

extern u32	array_mp_8821c_agc_tab_diff_wlg[780];
extern u32	array_mp_8821c_agc_tab_diff_btg[780];
void
odm_read_and_config_mp_8821c_agc_tab_diff(
	struct PHY_DM_STRUCT *p_dm,
	u32	array[],
	u32	array_len
);
u32	odm_get_version_mp_8821c_agc_tab_diff(void);

/******************************************************************************
*                           phy_reg.TXT
******************************************************************************/

void
odm_read_and_config_mp_8821c_phy_reg(/* tc: Test Chip, mp: mp Chip*/
	struct	PHY_DM_STRUCT *p_dm
);
u32	odm_get_version_mp_8821c_phy_reg(void);

/******************************************************************************
*                           phy_reg_mp.TXT
******************************************************************************/

void
odm_read_and_config_mp_8821c_phy_reg_mp(/* tc: Test Chip, mp: mp Chip*/
	struct	PHY_DM_STRUCT *p_dm
);
u32	odm_get_version_mp_8821c_phy_reg_mp(void);

/******************************************************************************
*                           phy_reg_pg.TXT
******************************************************************************/

void
odm_read_and_config_mp_8821c_phy_reg_pg(/* tc: Test Chip, mp: mp Chip*/
	struct	PHY_DM_STRUCT *p_dm
);
u32	odm_get_version_mp_8821c_phy_reg_pg(void);

/******************************************************************************
*                           phy_reg_pg_type0x28.TXT
******************************************************************************/

void
odm_read_and_config_mp_8821c_phy_reg_pg_type0x28(/* tc: Test Chip, mp: mp Chip*/
	struct	PHY_DM_STRUCT *p_dm
);
u32	odm_get_version_mp_8821c_phy_reg_pg_type0x28(void);

#endif
#endif /* end of HWIMG_SUPPORT*/

