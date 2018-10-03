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
#ifndef __INC_MP_RF_HW_IMG_8812A_H
#define __INC_MP_RF_HW_IMG_8812A_H


/******************************************************************************
*                           RadioA.TXT
******************************************************************************/

void
odm_read_and_config_mp_8812a_radioa(/* TC: Test Chip, MP: MP Chip*/
	struct PHY_DM_STRUCT  *p_dm_odm
);
u32 odm_get_version_mp_8812a_radioa(void);

/******************************************************************************
*                           RadioB.TXT
******************************************************************************/

void
odm_read_and_config_mp_8812a_radiob(/* TC: Test Chip, MP: MP Chip*/
	struct PHY_DM_STRUCT  *p_dm_odm
);
u32 odm_get_version_mp_8812a_radiob(void);

/******************************************************************************
*                           TxPowerTrack_AP.TXT
******************************************************************************/

void
odm_read_and_config_mp_8812a_txpowertrack_ap(/* TC: Test Chip, MP: MP Chip*/
	struct PHY_DM_STRUCT  *p_dm_odm
);
u32 odm_get_version_mp_8812a_txpowertrack_ap(void);

/******************************************************************************
*                           TxPowerTrack_PCIE.TXT
******************************************************************************/

void
odm_read_and_config_mp_8812a_txpowertrack_pcie(/* TC: Test Chip, MP: MP Chip*/
	struct PHY_DM_STRUCT  *p_dm_odm
);
u32 odm_get_version_mp_8812a_txpowertrack_pcie(void);

/******************************************************************************
*                           TxPowerTrack_RFE3.TXT
******************************************************************************/

void
odm_read_and_config_mp_8812a_txpowertrack_rfe3(/* TC: Test Chip, MP: MP Chip*/
	struct PHY_DM_STRUCT  *p_dm_odm
);
u32 odm_get_version_mp_8812a_txpowertrack_rfe3(void);

/******************************************************************************
*                           TxPowerTrack_RFE4.TXT
******************************************************************************/

void
odm_read_and_config_mp_8812a_txpowertrack_rfe4(/* TC: Test Chip, MP: MP Chip*/
	struct PHY_DM_STRUCT  *p_dm_odm
);
u32 odm_get_version_mp_8812a_txpowertrack_rfe4(void);

/******************************************************************************
*                           TxPowerTrack_USB.TXT
******************************************************************************/

void
odm_read_and_config_mp_8812a_txpowertrack_usb(/* TC: Test Chip, MP: MP Chip*/
	struct PHY_DM_STRUCT  *p_dm_odm
);
u32 odm_get_version_mp_8812a_txpowertrack_usb(void);

/******************************************************************************
*                           TXPWR_LMT.TXT
******************************************************************************/

void
odm_read_and_config_mp_8812a_txpwr_lmt(/* TC: Test Chip, MP: MP Chip*/
	struct PHY_DM_STRUCT  *p_dm_odm
);
u32 odm_get_version_mp_8812a_txpwr_lmt(void);

/******************************************************************************
*                           TXPWR_LMT_HM812A03.TXT
******************************************************************************/

void
odm_read_and_config_mp_8812a_txpwr_lmt_hm812a03(/* TC: Test Chip, MP: MP Chip*/
	struct PHY_DM_STRUCT  *p_dm_odm
);
u32 odm_get_version_mp_8812a_txpwr_lmt_hm812a03(void);

/******************************************************************************
*                           TXPWR_LMT_NFA812A00.TXT
******************************************************************************/

void
odm_read_and_config_mp_8812a_txpwr_lmt_nfa812a00(/* TC: Test Chip, MP: MP Chip*/
	struct PHY_DM_STRUCT  *p_dm_odm
);
u32 odm_get_version_mp_8812a_txpwr_lmt_nfa812a00(void);

/******************************************************************************
*                           TXPWR_LMT_TPLINK.TXT
******************************************************************************/

void
odm_read_and_config_mp_8812a_txpwr_lmt_tplink(/* TC: Test Chip, MP: MP Chip*/
	struct PHY_DM_STRUCT  *p_dm_odm
);
u32 odm_get_version_mp_8812a_txpwr_lmt_tplink(void);

#endif
#endif /* end of HWIMG_SUPPORT*/
