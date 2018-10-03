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
#ifndef __INC_MP_RF_HW_IMG_8821C_H
#define __INC_MP_RF_HW_IMG_8821C_H


/******************************************************************************
*                           radioa.TXT
******************************************************************************/

void
odm_read_and_config_mp_8821c_radioa(/* tc: Test Chip, mp: mp Chip*/
	struct	PHY_DM_STRUCT *p_dm
);
u32	odm_get_version_mp_8821c_radioa(void);

/******************************************************************************
*                           txpowertrack.TXT
******************************************************************************/

void
odm_read_and_config_mp_8821c_txpowertrack(/* tc: Test Chip, mp: mp Chip*/
	struct	PHY_DM_STRUCT *p_dm
);
u32	odm_get_version_mp_8821c_txpowertrack(void);

/******************************************************************************
*                           txpowertrack_type0x20.TXT
******************************************************************************/

void
odm_read_and_config_mp_8821c_txpowertrack_type0x20(/* tc: Test Chip, mp: mp Chip*/
	struct	PHY_DM_STRUCT *p_dm
);
u32	odm_get_version_mp_8821c_txpowertrack_type0x20(void);

/******************************************************************************
*                           txpowertrack_type0x28.TXT
******************************************************************************/

void
odm_read_and_config_mp_8821c_txpowertrack_type0x28(/* tc: Test Chip, mp: mp Chip*/
	struct	PHY_DM_STRUCT *p_dm
);
u32	odm_get_version_mp_8821c_txpowertrack_type0x28(void);

/******************************************************************************
*                           txpwr_lmt.TXT
******************************************************************************/

void
odm_read_and_config_mp_8821c_txpwr_lmt(/* tc: Test Chip, mp: mp Chip*/
	struct	PHY_DM_STRUCT *p_dm
);
u32	odm_get_version_mp_8821c_txpwr_lmt(void);

/******************************************************************************
*                           txpwr_lmt_fccsar.TXT
******************************************************************************/

void
odm_read_and_config_mp_8821c_txpwr_lmt_fccsar(/* tc: Test Chip, mp: mp Chip*/
	struct	PHY_DM_STRUCT *p_dm
);
u32	odm_get_version_mp_8821c_txpwr_lmt_fccsar(void);

#endif
#endif /* end of HWIMG_SUPPORT*/

