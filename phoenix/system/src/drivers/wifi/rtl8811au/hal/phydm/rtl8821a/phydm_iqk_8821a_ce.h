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

#ifndef __PHYDM_IQK_8821A_H__
#define __PHYDM_IQK_8821A_H__

/*--------------------------Define Parameters-------------------------------*/


/*---------------------------End Define Parameters-------------------------------*/
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
void
do_iqk_8821a(
	void		*p_dm_void,
	u8		delta_thermal_index,
	u8		thermal_value,
	u8		threshold
);
void
phy_iq_calibrate_8821a(
	struct PHY_DM_STRUCT	*p_dm_odm,
	boolean	is_recovery
);
#else
void
_phy_iq_calibrate_8821a(
	struct PHY_DM_STRUCT		*p_dm_odm
);
#endif
#endif	/*  #ifndef __PHYDM_IQK_8821A_H__ */
