/******************************************************************************
*
* Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
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
* You should have received a copy of the GNU General Public License along with
* this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
*
*
******************************************************************************/

/*Image2HeaderVersion: 2.18*/
#if (RTL8812A_SUPPORT == 1)
#ifndef __INC_MP_BB_HW_IMG_8812A_H
#define __INC_MP_BB_HW_IMG_8812A_H


/******************************************************************************
*                           AGC_TAB.TXT
******************************************************************************/

void
odm_read_and_config_mp_8812a_agc_tab(/* TC: Test Chip, MP: MP Chip*/
	struct PHY_DM_STRUCT  *p_dm_odm
);
u32 odm_get_version_mp_8812a_agc_tab(void);

/******************************************************************************
*                           AGC_TAB_DIFF.TXT
******************************************************************************/

extern u32 array_mp_8812a_agc_tab_diff_lb[60];
extern u32 array_mp_8812a_agc_tab_diff_hb[60];
void
odm_read_and_config_mp_8812a_agc_tab_diff(
	struct PHY_DM_STRUCT    *p_dm_odm,
	u32	   array[],
	u32	   array_len
);
u32 odm_get_version_mp_8812a_agc_tab_diff(void);

/******************************************************************************
*                           PHY_REG.TXT
******************************************************************************/

void
odm_read_and_config_mp_8812a_phy_reg(/* TC: Test Chip, MP: MP Chip*/
	struct PHY_DM_STRUCT  *p_dm_odm
);
u32 odm_get_version_mp_8812a_phy_reg(void);

/******************************************************************************
*                           PHY_REG_MP.TXT
******************************************************************************/

void
odm_read_and_config_mp_8812a_phy_reg_mp(/* TC: Test Chip, MP: MP Chip*/
	struct PHY_DM_STRUCT  *p_dm_odm
);
u32 odm_get_version_mp_8812a_phy_reg_mp(void);

/******************************************************************************
*                           PHY_REG_PG.TXT
******************************************************************************/

void
odm_read_and_config_mp_8812a_phy_reg_pg(/* TC: Test Chip, MP: MP Chip*/
	struct PHY_DM_STRUCT  *p_dm_odm
);
u32 odm_get_version_mp_8812a_phy_reg_pg(void);

/******************************************************************************
*                           PHY_REG_PG_ASUS.TXT
******************************************************************************/

void
odm_read_and_config_mp_8812a_phy_reg_pg_asus(/* TC: Test Chip, MP: MP Chip*/
	struct PHY_DM_STRUCT  *p_dm_odm
);
u32 odm_get_version_mp_8812a_phy_reg_pg_asus(void);

/******************************************************************************
*                           PHY_REG_PG_DNI.TXT
******************************************************************************/

void
odm_read_and_config_mp_8812a_phy_reg_pg_dni(/* TC: Test Chip, MP: MP Chip*/
	struct PHY_DM_STRUCT  *p_dm_odm
);
u32 odm_get_version_mp_8812a_phy_reg_pg_dni(void);

/******************************************************************************
*                           PHY_REG_PG_NEC.TXT
******************************************************************************/

void
odm_read_and_config_mp_8812a_phy_reg_pg_nec(/* TC: Test Chip, MP: MP Chip*/
	struct PHY_DM_STRUCT  *p_dm_odm
);
u32 odm_get_version_mp_8812a_phy_reg_pg_nec(void);

/******************************************************************************
*                           PHY_REG_PG_TPLINK.TXT
******************************************************************************/

void
odm_read_and_config_mp_8812a_phy_reg_pg_tplink(/* TC: Test Chip, MP: MP Chip*/
	struct PHY_DM_STRUCT  *p_dm_odm
);
u32 odm_get_version_mp_8812a_phy_reg_pg_tplink(void);

#endif
#endif /* end of HWIMG_SUPPORT*/
