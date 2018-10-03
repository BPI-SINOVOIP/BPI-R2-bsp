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

#ifndef	__ODM_RTL8812A_H__
#define __ODM_RTL8812A_H__
#if (defined(CONFIG_PATH_DIVERSITY))

void
odm_path_statistics_8812a(
	struct PHY_DM_STRUCT		*p_dm_odm,
	u32			mac_id,
	u32			RSSI_A,
	u32			RSSI_B
);

void
odm_path_diversity_init_8812a(struct PHY_DM_STRUCT	*p_dm_odm);

void
odm_path_diversity_8812a(struct PHY_DM_STRUCT	*p_dm_odm);

void
odm_set_tx_path_by_tx_info_8812a(
	struct PHY_DM_STRUCT		*p_dm_odm,
	u8			*p_desc,
	u8			mac_id
);
#endif
#endif
