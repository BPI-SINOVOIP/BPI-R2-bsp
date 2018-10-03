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

#ifndef __HAL_PHY_RF_8821A_H__
#define __HAL_PHY_RF_8821A_H__

/*--------------------------Define Parameters-------------------------------*/
#define	IQK_DELAY_TIME_8821A		10		/* ms */
#define	index_mapping_NUM_8821A	15
#define AVG_THERMAL_NUM_8821A	4
#define RF_T_METER_8821A		0x42


void configure_txpower_track_8821a(
	struct _TXPWRTRACK_CFG	*p_config
);

void do_iqk_8821a(
	void *p_dm_void,
	u8		delta_thermal_index,
	u8		thermal_value,
	u8		threshold
);

void
odm_tx_pwr_track_set_pwr8821a(
	void *p_dm_void,
	enum pwrtrack_method	method,
	u8				rf_path,
	u8				channel_mapped_index
);

/* 1 7.	IQK */

void
phy_iq_calibrate_8821a(
	struct _ADAPTER	*p_adapter,
	boolean	is_recovery
);

void
phy_lc_calibrate_8821a(
	IN void *p_dm_void
);

void
get_delta_swing_table_8821a(
	void *p_dm_void,
	u8 **temperature_up_a,
	u8 **temperature_down_a,
	u8 **temperature_up_b,
	u8 **temperature_down_b
);
#endif	/*  #ifndef __HAL_PHY_RF_8821A_H__ */
