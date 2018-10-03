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

#include "../mp_precomp.h"
#include "../phydm_precomp.h"



/*---------------------------Define Local Constant---------------------------*/
/* 2010/04/25 MH Define the max tx power tracking tx agc power. */
#define		ODM_TXPWRTRACK_MAX_IDX8812A		6

/*---------------------------Define Local Constant---------------------------*/


/* 3============================================================
 * 3 Tx Power Tracking
 * 3============================================================ */

#if 0



/* new element A = element D x X */

/* new element C = element D x Y */


void do_iqk_8812a(
	void		*p_dm_void,
	u8		delta_thermal_index,
	u8		thermal_value,
	u8		threshold
)
{
	struct PHY_DM_STRUCT	*p_dm_odm = (struct PHY_DM_STRUCT *)p_dm_void;
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct _ADAPTER		*adapter = p_dm_odm->adapter;
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(adapter);
#endif

	odm_reset_iqk_result(p_dm_odm);

#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
#if (DEV_BUS_TYPE == RT_PCI_INTERFACE)
#if USE_WORKITEM
	platform_acquire_mutex(&p_hal_data->mx_chnl_bw_control);
#else
	platform_acquire_spin_lock(adapter, RT_CHANNEL_AND_BANDWIDTH_SPINLOCK);
#endif
#elif ((DEV_BUS_TYPE == RT_USB_INTERFACE) || (DEV_BUS_TYPE == RT_SDIO_INTERFACE))
	platform_acquire_mutex(&p_hal_data->mx_chnl_bw_control);
#endif
#endif


	p_dm_odm->rf_calibrate_info.thermal_value_iqk = thermal_value;
	phy_iq_calibrate_8812a(adapter, false);


#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
#if (DEV_BUS_TYPE == RT_PCI_INTERFACE)
#if USE_WORKITEM
	platform_release_mutex(&p_hal_data->mx_chnl_bw_control);
#else
	platform_release_spin_lock(adapter, RT_CHANNEL_AND_BANDWIDTH_SPINLOCK);
#endif
#elif ((DEV_BUS_TYPE == RT_USB_INTERFACE) || (DEV_BUS_TYPE == RT_SDIO_INTERFACE))
	platform_release_mutex(&p_hal_data->mx_chnl_bw_control);
#endif
#endif
}

/*-----------------------------------------------------------------------------
 * Function:	odm_TxPwrTrackSetPwr88E()
 *
 * Overview:	88E change all channel tx power accordign to flag.
 *				OFDM & CCK are all different.
 *
 * Input:		NONE
 *
 * Output:		NONE
 *
 * Return:		NONE
 *
 * Revised History:
 *	When		Who		Remark
 *	04/23/2012	MHC		Create version 0.
 *
 *---------------------------------------------------------------------------*/
void
odm_tx_pwr_track_set_pwr8812a(
	struct PHY_DM_STRUCT			*p_dm_odm,
	enum pwrtrack_method	method,
	u8				rf_path,
	u8				channel_mapped_index
)
{
	if (method == TXAGC) {
		u8	cck_power_level[MAX_TX_COUNT], ofdm_power_level[MAX_TX_COUNT];
		u8	bw20_power_level[MAX_TX_COUNT], bw40_power_level[MAX_TX_COUNT];
		u8	rf = 0;
		u32	pwr = 0, tx_agc = 0;
		struct _ADAPTER *adapter = p_dm_odm->adapter;

		ODM_RT_TRACE(p_dm_odm, ODM_COMP_TX_PWR_TRACK, ODM_DBG_LOUD, ("odm_TxPwrTrackSetPwr88E CH=%d\n", *(p_dm_odm->p_channel)));
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))

		pwr = phy_query_bb_reg(adapter, REG_TX_AGC_A_RATE18_06, 0xFF);
		pwr += (p_dm_odm->bb_swing_idx_cck - p_dm_odm->bb_swing_idx_cck_base);
		tx_agc = (pwr << 16) | (pwr << 8) | (pwr);
		phy_set_bb_reg(adapter, REG_TX_AGC_A_CCK_1_MCS32, MASKBYTE1, tx_agc);
		phy_set_bb_reg(adapter, REG_TX_AGC_B_CCK_11_A_CCK_2_11, 0xffffff00, tx_agc);
		RTPRINT(FPHY, PHY_TXPWR, ("odm_tx_pwr_track_set_pwr88_e: CCK Tx-rf(A) Power = 0x%x\n", tx_agc));

		pwr = phy_query_bb_reg(adapter, REG_TX_AGC_A_RATE18_06, 0xFF);
		pwr += (p_dm_odm->bb_swing_idx_ofdm[RF_PATH_A] - p_dm_odm->bb_swing_idx_ofdm_base);
		tx_agc |= ((pwr << 24) | (pwr << 16) | (pwr << 8) | pwr);
		phy_set_bb_reg(adapter, REG_TX_AGC_A_RATE18_06, MASKDWORD, tx_agc);
		phy_set_bb_reg(adapter, REG_TX_AGC_A_RATE54_24, MASKDWORD, tx_agc);
		phy_set_bb_reg(adapter, REG_TX_AGC_A_MCS03_MCS00, MASKDWORD, tx_agc);
		phy_set_bb_reg(adapter, REG_TX_AGC_A_MCS07_MCS04, MASKDWORD, tx_agc);
		phy_set_bb_reg(adapter, REG_TX_AGC_A_MCS11_MCS08, MASKDWORD, tx_agc);
		phy_set_bb_reg(adapter, REG_TX_AGC_A_MCS15_MCS12, MASKDWORD, tx_agc);
		RTPRINT(FPHY, PHY_TXPWR, ("odm_tx_pwr_track_set_pwr88_e: OFDM Tx-rf(A) Power = 0x%x\n", tx_agc));
#endif
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
		phy_rf6052_set_cck_tx_power(p_dm_odm->priv, *(p_dm_odm->p_channel));
		phy_rf6052_set_ofdm_tx_power(p_dm_odm->priv, *(p_dm_odm->p_channel));
#endif

	} else if (method == BBSWING) {
		/* Adjust BB swing by OFDM IQ matrix */
		if (rf_path == RF_PATH_A)
			odm_set_bb_reg(p_dm_odm, REG_A_TX_SCALE_JAGUAR, MASKDWORD, p_dm_odm->bb_swing_idx_ofdm[RF_PATH_A]);
		else if (rf_path == RF_PATH_B)
			odm_set_bb_reg(p_dm_odm, REG_B_TX_SCALE_JAGUAR, MASKDWORD, p_dm_odm->bb_swing_idx_ofdm[RF_PATH_B]);
	} else
		return;
}	/* odm_TxPwrTrackSetPwr88E */

void configure_txpower_track_8812a(
	struct _TXPWRTRACK_CFG	*p_config
)
{
	p_config->swing_table_size_cck = CCK_TABLE_SIZE;
	p_config->swing_table_size_ofdm = OFDM_TABLE_SIZE;
	p_config->threshold_iqk = 8;
	p_config->average_thermal_num = AVG_THERMAL_NUM_8812A;
	p_config->rf_path_count = 2;
	p_config->thermal_reg_addr = RF_T_METER_8812A;

	p_config->odm_tx_pwr_track_set_pwr = odm_tx_pwr_track_set_pwr8812a;
	p_config->do_iqk = do_iqk_8812a;
	p_config->phy_lc_calibrate = phy_lc_calibrate_8812a;
}

#endif

/* 1 7.	IQK */
#define MAX_TOLERANCE		5
#define IQK_DELAY_TIME		1		/* ms */

u8			/* bit0 = 1 => Tx OK, bit1 = 1 => Rx OK */
phy_path_a_iqk_8812a(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct PHY_DM_STRUCT		*p_dm_odm,
#else
	struct _ADAPTER	*p_adapter,
#endif
	boolean		config_path_b
)
{
	u32 reg_eac, reg_e94, reg_e9c, reg_ea4;
	u8 result = 0x00;
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->odmpriv;
#endif
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->DM_OutSrc;
#endif
#endif
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("path A IQK!\n"));

	/* 1 Tx IQK */
	/* path-A IQK setting */
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("path-A IQK setting!\n"));
	odm_set_bb_reg(p_dm_odm, REG_TX_IQK_TONE_A, MASKDWORD, 0x10008c1c);
	odm_set_bb_reg(p_dm_odm, REG_RX_IQK_TONE_A, MASKDWORD, 0x30008c1c);
	odm_set_bb_reg(p_dm_odm, REG_TX_IQK_PI_A, MASKDWORD, 0x8214032a);
	odm_set_bb_reg(p_dm_odm, REG_RX_IQK_PI_A, MASKDWORD, 0x28160000);

	/* LO calibration setting */
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("LO calibration setting!\n"));
	odm_set_bb_reg(p_dm_odm, REG_IQK_AGC_RSP, MASKDWORD, 0x00462911);

	/* One shot, path A LOK & IQK */
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("One shot, path A LOK & IQK!\n"));
	odm_set_bb_reg(p_dm_odm, REG_IQK_AGC_PTS, MASKDWORD, 0xf9000000);
	odm_set_bb_reg(p_dm_odm, REG_IQK_AGC_PTS, MASKDWORD, 0xf8000000);

	/* delay x ms */
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("delay %d ms for One shot, path A LOK & IQK.\n", IQK_DELAY_TIME_8812A));
	/* platform_stall_execution(IQK_DELAY_TIME_8812A*1000); */
	ODM_delay_ms(IQK_DELAY_TIME_8812A);

	/* Check failed */
	reg_eac = odm_get_bb_reg(p_dm_odm, REG_RX_POWER_AFTER_IQK_A_2, MASKDWORD);
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xeac = 0x%x\n", reg_eac));
	reg_e94 = odm_get_bb_reg(p_dm_odm, REG_TX_POWER_BEFORE_IQK_A, MASKDWORD);
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xe94 = 0x%x\n", reg_e94));
	reg_e9c = odm_get_bb_reg(p_dm_odm, REG_TX_POWER_AFTER_IQK_A, MASKDWORD);
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xe9c = 0x%x\n", reg_e9c));
	reg_ea4 = odm_get_bb_reg(p_dm_odm, REG_RX_POWER_BEFORE_IQK_A_2, MASKDWORD);
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xea4 = 0x%x\n", reg_ea4));

	if (!(reg_eac & BIT(28)) &&
	    (((reg_e94 & 0x03FF0000) >> 16) != 0x142) &&
	    (((reg_e9c & 0x03FF0000) >> 16) != 0x42))
		result |= 0x01;
	else							/* if Tx not OK, ignore Rx */
		return result;

#if 0
	if (!(reg_eac & BIT(27)) &&		/* if Tx is OK, check whether Rx is OK */
	    (((reg_ea4 & 0x03FF0000) >> 16) != 0x132) &&
	    (((reg_eac & 0x03FF0000) >> 16) != 0x36))
		result |= 0x02;
	else
		RTPRINT(FINIT, INIT_IQK, ("path A Rx IQK fail!!\n"));
#endif

	return result;


}

u8			/* bit0 = 1 => Tx OK, bit1 = 1 => Rx OK */
phy_path_a_rx_iqk_8812a(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct PHY_DM_STRUCT		*p_dm_odm,
#else
	struct _ADAPTER	*p_adapter,
#endif
	boolean		config_path_b
)
{
	u32 reg_eac, reg_e94, reg_e9c, reg_ea4, u4tmp;
	u8 result = 0x00;
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->odmpriv;
#endif
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->DM_OutSrc;
#endif
#endif
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("path A Rx IQK!\n"));

	/* 1 Get TXIMR setting */
	/* modify RXIQK mode table */
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("path-A Rx IQK modify RXIQK mode table!\n"));
	odm_set_bb_reg(p_dm_odm, REG_FPGA0_IQK, 0xffffff00, 0);
	odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, RF_WE_LUT, RFREGOFFSETMASK, 0x800a0);
	odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, RF_RCK_OS, RFREGOFFSETMASK, 0x30000);
	odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, RF_TXPA_G1, RFREGOFFSETMASK, 0x0000f);
	odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, RF_TXPA_G2, RFREGOFFSETMASK, 0xf117B);
	odm_set_bb_reg(p_dm_odm, REG_FPGA0_IQK, 0xffffff00, 0x808000);

	/* IQK setting */
	odm_set_bb_reg(p_dm_odm, REG_TX_IQK, MASKDWORD, 0x01007c00);
	odm_set_bb_reg(p_dm_odm, REG_RX_IQK, MASKDWORD, 0x81004800);

	/* path-A IQK setting */
	odm_set_bb_reg(p_dm_odm, REG_TX_IQK_TONE_A, MASKDWORD, 0x10008c1c);
	odm_set_bb_reg(p_dm_odm, REG_RX_IQK_TONE_A, MASKDWORD, 0x30008c1c);
	odm_set_bb_reg(p_dm_odm, REG_TX_IQK_PI_A, MASKDWORD, 0x82160804);
	odm_set_bb_reg(p_dm_odm, REG_RX_IQK_PI_A, MASKDWORD, 0x28160000);

	/* LO calibration setting */
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("LO calibration setting!\n"));
	odm_set_bb_reg(p_dm_odm, REG_IQK_AGC_RSP, MASKDWORD, 0x0046a911);

	/* One shot, path A LOK & IQK */
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("One shot, path A LOK & IQK!\n"));
	odm_set_bb_reg(p_dm_odm, REG_IQK_AGC_PTS, MASKDWORD, 0xf9000000);
	odm_set_bb_reg(p_dm_odm, REG_IQK_AGC_PTS, MASKDWORD, 0xf8000000);

	/* delay x ms */
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("delay %d ms for One shot, path A LOK & IQK.\n", IQK_DELAY_TIME_8812A));
	/* platform_stall_execution(IQK_DELAY_TIME_8812A*1000); */
	ODM_delay_ms(IQK_DELAY_TIME_8812A);


	/* Check failed */
	reg_eac = odm_get_bb_reg(p_dm_odm, REG_RX_POWER_AFTER_IQK_A_2, MASKDWORD);
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xeac = 0x%x\n", reg_eac));
	reg_e94 = odm_get_bb_reg(p_dm_odm, REG_TX_POWER_BEFORE_IQK_A, MASKDWORD);
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xe94 = 0x%x\n", reg_e94));
	reg_e9c = odm_get_bb_reg(p_dm_odm, REG_TX_POWER_AFTER_IQK_A, MASKDWORD);
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xe9c = 0x%x\n", reg_e9c));

	if (!(reg_eac & BIT(28)) &&
	    (((reg_e94 & 0x03FF0000) >> 16) != 0x142) &&
	    (((reg_e9c & 0x03FF0000) >> 16) != 0x42))
		result |= 0x01;
	else							/* if Tx not OK, ignore Rx */
		return result;

	u4tmp = 0x80007C00 | (reg_e94 & 0x3FF0000)  | ((reg_e9c & 0x3FF0000) >> 16);
	odm_set_bb_reg(p_dm_odm, REG_TX_IQK, MASKDWORD, u4tmp);
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xe40 = 0x%x u4tmp = 0x%x\n", odm_get_bb_reg(p_dm_odm, REG_TX_IQK, MASKDWORD), u4tmp));


	/* 1 RX IQK */
	/* modify RXIQK mode table */
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("path-A Rx IQK modify RXIQK mode table 2!\n"));
	odm_set_bb_reg(p_dm_odm, REG_FPGA0_IQK, 0xffffff00, 0);
	odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, RF_WE_LUT, RFREGOFFSETMASK, 0x800a0);
	odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, RF_RCK_OS, RFREGOFFSETMASK, 0x30000);
	odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, RF_TXPA_G1, RFREGOFFSETMASK, 0x0000f);
	odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, RF_TXPA_G2, RFREGOFFSETMASK, 0xf7ffa);
	odm_set_bb_reg(p_dm_odm, REG_FPGA0_IQK, 0xffffff00, 0x808000);

	/* IQK setting */
	odm_set_bb_reg(p_dm_odm, REG_RX_IQK, MASKDWORD, 0x01004800);

	/* path-A IQK setting */
	odm_set_bb_reg(p_dm_odm, REG_TX_IQK_TONE_A, MASKDWORD, 0x30008c1c);
	odm_set_bb_reg(p_dm_odm, REG_RX_IQK_TONE_A, MASKDWORD, 0x10008c1c);
	odm_set_bb_reg(p_dm_odm, REG_TX_IQK_PI_A, MASKDWORD, 0x82160c05);
	odm_set_bb_reg(p_dm_odm, REG_RX_IQK_PI_A, MASKDWORD, 0x28160c05);

	/* LO calibration setting */
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("LO calibration setting!\n"));
	odm_set_bb_reg(p_dm_odm, REG_IQK_AGC_RSP, MASKDWORD, 0x0046a911);

	/* One shot, path A LOK & IQK */
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("One shot, path A LOK & IQK!\n"));
	odm_set_bb_reg(p_dm_odm, REG_IQK_AGC_PTS, MASKDWORD, 0xf9000000);
	odm_set_bb_reg(p_dm_odm, REG_IQK_AGC_PTS, MASKDWORD, 0xf8000000);

	/* delay x ms */
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("delay %d ms for One shot, path A LOK & IQK.\n", IQK_DELAY_TIME_8812A));
	/* platform_stall_execution(IQK_DELAY_TIME_8812A*1000); */
	ODM_delay_ms(IQK_DELAY_TIME_8812A);

	/* Check failed */
	reg_eac = odm_get_bb_reg(p_dm_odm, REG_RX_POWER_AFTER_IQK_A_2, MASKDWORD);
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xeac = 0x%x\n", reg_eac));
	reg_e94 = odm_get_bb_reg(p_dm_odm, REG_TX_POWER_BEFORE_IQK_A, MASKDWORD);
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xe94 = 0x%x\n", reg_e94));
	reg_e9c = odm_get_bb_reg(p_dm_odm, REG_TX_POWER_AFTER_IQK_A, MASKDWORD);
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xe9c = 0x%x\n", reg_e9c));
	reg_ea4 = odm_get_bb_reg(p_dm_odm, REG_RX_POWER_BEFORE_IQK_A_2, MASKDWORD);
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xea4 = 0x%x\n", reg_ea4));

#if 0
	if (!(reg_eac & BIT(28)) &&
	    (((reg_e94 & 0x03FF0000) >> 16) != 0x142) &&
	    (((reg_e9c & 0x03FF0000) >> 16) != 0x42))
		result |= 0x01;
	else							/* if Tx not OK, ignore Rx */
		return result;
#endif

	if (!(reg_eac & BIT(27)) &&		/* if Tx is OK, check whether Rx is OK */
	    (((reg_ea4 & 0x03FF0000) >> 16) != 0x132) &&
	    (((reg_eac & 0x03FF0000) >> 16) != 0x36))
		result |= 0x02;
	else
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("path A Rx IQK fail!!\n"));

	return result;


}

u8				/* bit0 = 1 => Tx OK, bit1 = 1 => Rx OK */
phy_path_b_iqk_8812a(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct PHY_DM_STRUCT		*p_dm_odm
#else
	struct _ADAPTER	*p_adapter
#endif
)
{
	u32 reg_eac, reg_eb4, reg_ebc, reg_ec4, reg_ecc;
	u8	result = 0x00;
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->odmpriv;
#endif
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->DM_OutSrc;
#endif
#endif
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("path B IQK!\n"));

	/* One shot, path B LOK & IQK */
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("One shot, path A LOK & IQK!\n"));
	odm_set_bb_reg(p_dm_odm, REG_IQK_AGC_CONT, MASKDWORD, 0x00000002);
	odm_set_bb_reg(p_dm_odm, REG_IQK_AGC_CONT, MASKDWORD, 0x00000000);

	/* delay x ms */
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("delay %d ms for One shot, path B LOK & IQK.\n", IQK_DELAY_TIME_8812A));
	/* platform_stall_execution(IQK_DELAY_TIME_8812A*1000); */
	ODM_delay_ms(IQK_DELAY_TIME_8812A);

	/* Check failed */
	reg_eac = odm_get_bb_reg(p_dm_odm, REG_RX_POWER_AFTER_IQK_A_2, MASKDWORD);
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xeac = 0x%x\n", reg_eac));
	reg_eb4 = odm_get_bb_reg(p_dm_odm, REG_TX_POWER_BEFORE_IQK_B, MASKDWORD);
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xeb4 = 0x%x\n", reg_eb4));
	reg_ebc = odm_get_bb_reg(p_dm_odm, REG_TX_POWER_AFTER_IQK_B, MASKDWORD);
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xebc = 0x%x\n", reg_ebc));
	reg_ec4 = odm_get_bb_reg(p_dm_odm, REG_RX_POWER_BEFORE_IQK_B_2, MASKDWORD);
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xec4 = 0x%x\n", reg_ec4));
	reg_ecc = odm_get_bb_reg(p_dm_odm, REG_RX_POWER_AFTER_IQK_B_2, MASKDWORD);
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xecc = 0x%x\n", reg_ecc));

	if (!(reg_eac & BIT(31)) &&
	    (((reg_eb4 & 0x03FF0000) >> 16) != 0x142) &&
	    (((reg_ebc & 0x03FF0000) >> 16) != 0x42))
		result |= 0x01;
	else
		return result;

	if (!(reg_eac & BIT(30)) &&
	    (((reg_ec4 & 0x03FF0000) >> 16) != 0x132) &&
	    (((reg_ecc & 0x03FF0000) >> 16) != 0x36))
		result |= 0x02;
	else
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("path B Rx IQK fail!!\n"));


	return result;

}

void
_phy_path_a_fill_iqk_matrix_8812a(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct PHY_DM_STRUCT		*p_dm_odm,
#else
	struct _ADAPTER	*p_adapter,
#endif
	boolean	is_iqk_ok,
	s32		result[][8],
	u8		final_candidate,
	boolean		is_tx_only
)
{
	u32	oldval_0, X, TX0_A, reg;
	s32	Y, TX0_C;
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->odmpriv;
#endif
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->DM_OutSrc;
#endif
#endif
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("path A IQ Calibration %s !\n", (is_iqk_ok) ? "Success" : "Failed"));

	if (final_candidate == 0xFF)
		return;

	else if (is_iqk_ok) {
		oldval_0 = (odm_get_bb_reg(p_dm_odm, REG_OFDM_0_XA_TX_IQ_IMBALANCE, MASKDWORD) >> 22) & 0x3FF;

		X = result[final_candidate][0];
		if ((X & 0x00000200) != 0)
			X = X | 0xFFFFFC00;
		TX0_A = (X * oldval_0) >> 8;
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("X = 0x%x, TX0_A = 0x%x, oldval_0 0x%x\n", X, TX0_A, oldval_0));
		odm_set_bb_reg(p_dm_odm, REG_OFDM_0_XA_TX_IQ_IMBALANCE, 0x3FF, TX0_A);

		odm_set_bb_reg(p_dm_odm, REG_OFDM_0_ECCA_THRESHOLD, BIT(31), ((X * oldval_0 >> 7) & 0x1));

		Y = result[final_candidate][1];
		if ((Y & 0x00000200) != 0)
			Y = Y | 0xFFFFFC00;


		TX0_C = (Y * oldval_0) >> 8;
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("Y = 0x%x, TX = 0x%x\n", (u32)Y, (u32)TX0_C));
		odm_set_bb_reg(p_dm_odm, REG_OFDM_0_XC_TX_AFE, 0xF0000000, ((TX0_C & 0x3C0) >> 6));
		odm_set_bb_reg(p_dm_odm, REG_OFDM_0_XA_TX_IQ_IMBALANCE, 0x003F0000, (TX0_C & 0x3F));

		odm_set_bb_reg(p_dm_odm, REG_OFDM_0_ECCA_THRESHOLD, BIT(29), ((Y * oldval_0 >> 7) & 0x1));

		if (is_tx_only) {
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("_phy_path_a_fill_iqk_matrix_8812a only Tx OK\n"));
			return;
		}

		reg = result[final_candidate][2];
#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
		if (RTL_ABS(reg, 0x100) >= 16)
			reg = 0x100;
#endif
		odm_set_bb_reg(p_dm_odm, REG_OFDM_0_XA_RX_IQ_IMBALANCE, 0x3FF, reg);

		reg = result[final_candidate][3] & 0x3F;
		odm_set_bb_reg(p_dm_odm, REG_OFDM_0_XA_RX_IQ_IMBALANCE, 0xFC00, reg);

		reg = (result[final_candidate][3] >> 6) & 0xF;
		odm_set_bb_reg(p_dm_odm, REG_OFDM_0_RX_IQ_EXT_ANTA, 0xF0000000, reg);
	}
}

void
_phy_path_b_fill_iqk_matrix_8812a(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct PHY_DM_STRUCT		*p_dm_odm,
#else
	struct _ADAPTER	*p_adapter,
#endif
	boolean	is_iqk_ok,
	s32		result[][8],
	u8		final_candidate,
	boolean		is_tx_only			/* do Tx only */
)
{
	u32	oldval_1, X, TX1_A, reg;
	s32	Y, TX1_C;
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->odmpriv;
#endif
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->DM_OutSrc;
#endif
#endif
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("path B IQ Calibration %s !\n", (is_iqk_ok) ? "Success" : "Failed"));

	if (final_candidate == 0xFF)
		return;

	else if (is_iqk_ok) {
		oldval_1 = (odm_get_bb_reg(p_dm_odm, REG_OFDM_0_XB_TX_IQ_IMBALANCE, MASKDWORD) >> 22) & 0x3FF;

		X = result[final_candidate][4];
		if ((X & 0x00000200) != 0)
			X = X | 0xFFFFFC00;
		TX1_A = (X * oldval_1) >> 8;
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("X = 0x%x, TX1_A = 0x%x\n", X, TX1_A));
		odm_set_bb_reg(p_dm_odm, REG_OFDM_0_XB_TX_IQ_IMBALANCE, 0x3FF, TX1_A);

		odm_set_bb_reg(p_dm_odm, REG_OFDM_0_ECCA_THRESHOLD, BIT(27), ((X * oldval_1 >> 7) & 0x1));

		Y = result[final_candidate][5];
		if ((Y & 0x00000200) != 0)
			Y = Y | 0xFFFFFC00;

		TX1_C = (Y * oldval_1) >> 8;
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("Y = 0x%x, TX1_C = 0x%x\n", (u32)Y, (u32)TX1_C));
		odm_set_bb_reg(p_dm_odm, REG_OFDM_0_XD_TX_AFE, 0xF0000000, ((TX1_C & 0x3C0) >> 6));
		odm_set_bb_reg(p_dm_odm, REG_OFDM_0_XB_TX_IQ_IMBALANCE, 0x003F0000, (TX1_C & 0x3F));

		odm_set_bb_reg(p_dm_odm, REG_OFDM_0_ECCA_THRESHOLD, BIT(25), ((Y * oldval_1 >> 7) & 0x1));

		if (is_tx_only)
			return;

		reg = result[final_candidate][6];
		odm_set_bb_reg(p_dm_odm, REG_OFDM_0_XB_RX_IQ_IMBALANCE, 0x3FF, reg);

		reg = result[final_candidate][7] & 0x3F;
		odm_set_bb_reg(p_dm_odm, REG_OFDM_0_XB_RX_IQ_IMBALANCE, 0xFC00, reg);

		reg = (result[final_candidate][7] >> 6) & 0xF;
		odm_set_bb_reg(p_dm_odm, REG_OFDM_0_AGC_RSSI_TABLE, 0x0000F000, reg);
	}
}

/*
 * 2011/07/26 MH Add an API for testing IQK fail case.
 *
 * MP Already declare in odm.c */
#if 0	/* !(DM_ODM_SUPPORT_TYPE & ODM_WIN) */
boolean
odm_check_power_status(
	struct _ADAPTER		*adapter)
{
#if 0
	/* HAL_DATA_TYPE		*p_hal_data = GET_HAL_DATA(adapter); */
	struct PHY_DM_STRUCT			*p_dm_odm = &p_hal_data->DM_OutSrc;
	RT_RF_POWER_STATE	rt_state;
	PMGNT_INFO			p_mgnt_info	= &(adapter->MgntInfo);

	/*  2011/07/27 MH We are not testing ready~~!! We may fail to get correct value when init sequence. */
	if (p_mgnt_info->init_adpt_in_progress == true) {
		ODM_RT_TRACE(p_dm_odm, COMP_INIT, DBG_LOUD, ("odm_check_power_status Return true, due to initadapter"));
		return	true;
	}

	/* */
	/*	2011/07/19 MH We can not execute tx pwoer tracking/ LLC calibrate or IQK. */
	/* */
	phydm_get_hw_reg_interface(p_dm_odm, HW_VAR_RF_STATE, (u8 *)(&rt_state));
	if (adapter->is_driver_stopped || adapter->is_driver_is_going_to_pnp_set_power_sleep || rt_state == eRfOff) {
		ODM_RT_TRACE(p_dm_odm, COMP_INIT, DBG_LOUD, ("odm_check_power_status Return false, due to %d/%d/%d\n",
			adapter->is_driver_stopped, adapter->is_driver_is_going_to_pnp_set_power_sleep, rt_state));
		return	false;
	}
#endif
	return	true;
}
#endif

void
_phy_save_adda_registers_8812a(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct PHY_DM_STRUCT		*p_dm_odm,
#else
	struct _ADAPTER	*p_adapter,
#endif
	u32		*adda_reg,
	u32		*adda_backup,
	u32		register_num
)
{
	u32	i;
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->odmpriv;
#endif
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->DM_OutSrc;
#endif

	if (odm_check_power_status(p_adapter) == false)
		return;
#endif

	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("Save ADDA parameters.\n"));
	for (i = 0 ; i < register_num ; i++)
		adda_backup[i] = odm_get_bb_reg(p_dm_odm, adda_reg[i], MASKDWORD);
}


void
_phy_save_mac_registers_8812a(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct PHY_DM_STRUCT		*p_dm_odm,
#else
	struct _ADAPTER	*p_adapter,
#endif
	u32		*mac_reg,
	u32		*mac_backup
)
{
	u32	i;
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->odmpriv;
#endif
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->DM_OutSrc;
#endif
#endif
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("Save MAC parameters.\n"));
	for (i = 0 ; i < (IQK_MAC_REG_NUM - 1); i++)
		mac_backup[i] = odm_read_1byte(p_dm_odm, mac_reg[i]);
	mac_backup[i] = odm_read_4byte(p_dm_odm, mac_reg[i]);

}


void
_phy_reload_adda_registers_8812a(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct PHY_DM_STRUCT		*p_dm_odm,
#else
	struct _ADAPTER	*p_adapter,
#endif
	u32		*adda_reg,
	u32		*adda_backup,
	u32		regiester_num
)
{
	u32	i;
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->odmpriv;
#endif
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->DM_OutSrc;
#endif
#endif

	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("Reload ADDA power saving parameters !\n"));
	for (i = 0 ; i < regiester_num; i++)
		odm_set_bb_reg(p_dm_odm, adda_reg[i], MASKDWORD, adda_backup[i]);
}

void
_phy_reload_mac_registers_8812a(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct PHY_DM_STRUCT		*p_dm_odm,
#else
	struct _ADAPTER	*p_adapter,
#endif
	u32		*mac_reg,
	u32		*mac_backup
)
{
	u32	i;
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->odmpriv;
#endif
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->DM_OutSrc;
#endif
#endif
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("Reload MAC parameters !\n"));
	for (i = 0 ; i < (IQK_MAC_REG_NUM - 1); i++)
		odm_write_1byte(p_dm_odm, mac_reg[i], (u8)mac_backup[i]);
	odm_write_4byte(p_dm_odm, mac_reg[i], mac_backup[i]);
}


void
_phy_path_adda_on_8812a(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct PHY_DM_STRUCT		*p_dm_odm,
#else
	struct _ADAPTER	*p_adapter,
#endif
	u32		*adda_reg,
	boolean		is_path_a_on,
	boolean		is2T
)
{
	u32	path_on;
	u32	i;
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->odmpriv;
#endif
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->DM_OutSrc;
#endif
#endif
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("ADDA ON.\n"));

	path_on = is_path_a_on ? 0x04db25a4 : 0x0b1b25a4;
	if (false == is2T) {
		path_on = 0x0bdb25a0;
		odm_set_bb_reg(p_dm_odm, adda_reg[0], MASKDWORD, 0x0b1b25a0);
	} else
		odm_set_bb_reg(p_dm_odm, adda_reg[0], MASKDWORD, path_on);

	for (i = 1 ; i < IQK_ADDA_REG_NUM ; i++)
		odm_set_bb_reg(p_dm_odm, adda_reg[i], MASKDWORD, path_on);

}

void
_phy_mac_setting_calibration_8812a(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct PHY_DM_STRUCT		*p_dm_odm,
#else
	struct _ADAPTER	*p_adapter,
#endif
	u32		*mac_reg,
	u32		*mac_backup
)
{
	u32	i = 0;
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->odmpriv;
#endif
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->DM_OutSrc;
#endif
#endif
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("MAC settings for Calibration.\n"));

	odm_write_1byte(p_dm_odm, mac_reg[i], 0x3F);

	for (i = 1 ; i < (IQK_MAC_REG_NUM - 1); i++)
		odm_write_1byte(p_dm_odm, mac_reg[i], (u8)(mac_backup[i] & (~BIT(3))));
	odm_write_1byte(p_dm_odm, mac_reg[i], (u8)(mac_backup[i] & (~BIT(5))));

}

void
_phy_path_a_stand_by_8812a(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct PHY_DM_STRUCT		*p_dm_odm
#else
	struct _ADAPTER	*p_adapter
#endif
)
{
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->odmpriv;
#endif
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->DM_OutSrc;
#endif
#endif
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("path-A standby mode!\n"));

	odm_set_bb_reg(p_dm_odm, REG_FPGA0_IQK, 0xffffff00, 0x0);
	odm_set_bb_reg(p_dm_odm, 0x840, MASKDWORD, 0x00010000);
	odm_set_bb_reg(p_dm_odm, REG_FPGA0_IQK, 0xffffff00, 0x808000);
}

void
_phy_pi_mode_switch_8812a(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct PHY_DM_STRUCT		*p_dm_odm,
#else
	struct _ADAPTER	*p_adapter,
#endif
	boolean		pi_mode
)
{
	u32	mode;
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->odmpriv;
#endif
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->DM_OutSrc;
#endif
#endif
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("BB Switch to %s mode!\n", (pi_mode ? "PI" : "SI")));

	mode = pi_mode ? 0x01000100 : 0x01000000;
	odm_set_bb_reg(p_dm_odm, REG_FPGA0_XA_HSSI_PARAMETER1, MASKDWORD, mode);
	odm_set_bb_reg(p_dm_odm, REG_FPGA0_XB_HSSI_PARAMETER1, MASKDWORD, mode);
}

boolean
phy_simularity_compare_8812a(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct PHY_DM_STRUCT		*p_dm_odm,
#else
	struct _ADAPTER	*p_adapter,
#endif
	s32		result[][8],
	u8		 c1,
	u8		 c2
)
{
	u32		i, j, diff, simularity_bit_map, bound = 0;
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->odmpriv;
#endif
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->DM_OutSrc;
#endif
#endif
	u8		final_candidate[2] = {0xFF, 0xFF};	/* for path A and path B */
	boolean		is_result = true;
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	boolean		is2T = IS_92C_SERIAL(p_hal_data->version_id);
#else
	boolean		is2T = 0;
#endif

	if (is2T)
		bound = 8;
	else
		bound = 4;

	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("===> IQK:phy_simularity_compare_8812a c1 %d c2 %d!!!\n", c1, c2));


	simularity_bit_map = 0;

	for (i = 0; i < bound; i++) {
		diff = (result[c1][i] > result[c2][i]) ? (result[c1][i] - result[c2][i]) : (result[c2][i] - result[c1][i]);
		if (diff > MAX_TOLERANCE) {
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("IQK:phy_simularity_compare_8812a differnece overflow index %d compare1 0x%x compare2 0x%x!!!\n",  i, (u32)result[c1][i], (u32)result[c2][i]));

			if ((i == 2 || i == 6) && !simularity_bit_map) {
				if (result[c1][i] + result[c1][i + 1] == 0)
					final_candidate[(i / 4)] = c2;
				else if (result[c2][i] + result[c2][i + 1] == 0)
					final_candidate[(i / 4)] = c1;
				else
					simularity_bit_map = simularity_bit_map | (1 << i);
			} else
				simularity_bit_map = simularity_bit_map | (1 << i);
		}
	}

	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("IQK:phy_simularity_compare_8812a simularity_bit_map   %d !!!\n", simularity_bit_map));

	if (simularity_bit_map == 0) {
		for (i = 0; i < (bound / 4); i++) {
			if (final_candidate[i] != 0xFF) {
				for (j = i * 4; j < (i + 1) * 4 - 2; j++)
					result[3][j] = result[final_candidate[i]][j];
				is_result = false;
			}
		}
		return is_result;
	} else if (!(simularity_bit_map & 0x0F)) {		/* path A OK */
		for (i = 0; i < 4; i++)
			result[3][i] = result[c1][i];
		return false;
	} else if (!(simularity_bit_map & 0xF0) && is2T) {	/* path B OK */
		for (i = 4; i < 8; i++)
			result[3][i] = result[c1][i];
		return false;
	} else
		return false;

}
#if 0
	#define BW_20M	0
	#define	BW_40M  1
	#define	BW_80M	2
#endif

void _iqk_rx_fill_iqc_8812a(
	struct PHY_DM_STRUCT			*p_dm_odm,
	enum odm_rf_radio_path_e	path,
	unsigned int			RX_X,
	unsigned int			RX_Y
)
{
	switch (path) {
	case ODM_RF_PATH_A:
	{
		odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
		odm_set_bb_reg(p_dm_odm, 0xc10, 0x000003ff, RX_X >> 1);
		odm_set_bb_reg(p_dm_odm, 0xc10, 0x03ff0000, RX_Y >> 1);
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RX_X = %x;;RX_Y = %x ====>fill to IQC\n", RX_X >> 1 & 0x000003ff, RX_Y >> 1 & 0x000003ff));
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xc10 = %x ====>fill to IQC\n", odm_read_4byte(p_dm_odm, 0xc10)));
	}
	break;
	case ODM_RF_PATH_B:
	{
		odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
		odm_set_bb_reg(p_dm_odm, 0xe10, 0x000003ff, RX_X >> 1);
		odm_set_bb_reg(p_dm_odm, 0xe10, 0x03ff0000, RX_Y >> 1);
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RX_X = %x;;RX_Y = %x====>fill to IQC\n ", RX_X >> 1 & 0x000003ff, RX_Y >> 1 & 0x000003ff));
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xe10 = %x====>fill to IQC\n", odm_read_4byte(p_dm_odm, 0xe10)));
	}
	break;
	default:
		break;
	};
}

void _iqk_tx_fill_iqc_8812a(
	struct PHY_DM_STRUCT			*p_dm_odm,
	enum odm_rf_radio_path_e	path,
	unsigned int			TX_X,
	unsigned int			TX_Y
)
{
	switch (path) {
	case ODM_RF_PATH_A:
	{
		odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1); /* [31] = 1 --> Page C1 */
		odm_write_4byte(p_dm_odm, 0xc90, 0x00000080);
		odm_write_4byte(p_dm_odm, 0xcc4, 0x20040000);
		odm_write_4byte(p_dm_odm, 0xcc8, 0x20000000);
		odm_set_bb_reg(p_dm_odm, 0xccc, 0x000007ff, TX_Y);
		odm_set_bb_reg(p_dm_odm, 0xcd4, 0x000007ff, TX_X);
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("TX_X = %x;;TX_Y = %x =====> fill to IQC\n", TX_X & 0x000007ff, TX_Y & 0x000007ff));
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xcd4 = %x;;0xccc = %x ====>fill to IQC\n", odm_get_bb_reg(p_dm_odm, 0xcd4, 0x000007ff), odm_get_bb_reg(p_dm_odm, 0xccc, 0x000007ff)));
	}
	break;
	case ODM_RF_PATH_B:
	{
		odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1); /* [31] = 1 --> Page C1 */
		odm_write_4byte(p_dm_odm, 0xe90, 0x00000080);
		odm_write_4byte(p_dm_odm, 0xec4, 0x20040000);
		odm_write_4byte(p_dm_odm, 0xec8, 0x20000000);
		odm_set_bb_reg(p_dm_odm, 0xecc, 0x000007ff, TX_Y);
		odm_set_bb_reg(p_dm_odm, 0xed4, 0x000007ff, TX_X);
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("TX_X = %x;;TX_Y = %x =====> fill to IQC\n", TX_X & 0x000007ff, TX_Y & 0x000007ff));
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xed4 = %x;;0xecc = %x ====>fill to IQC\n", odm_get_bb_reg(p_dm_odm, 0xed4, 0x000007ff), odm_get_bb_reg(p_dm_odm, 0xecc, 0x000007ff)));
	}
	break;
	default:
		break;
	};
}

void _iqk_backup_mac_bb_8812a(
	struct PHY_DM_STRUCT		*p_dm_odm,
	u32		*MACBB_backup,
	u32		*backup_macbb_reg,
	u32		MACBB_NUM
)
{
	u32 i;
	odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	/* save MACBB default value */
	for (i = 0; i < MACBB_NUM; i++)
		MACBB_backup[i] = odm_read_4byte(p_dm_odm, backup_macbb_reg[i]);

	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("BackupMacBB Success!!!!\n"));
}
void _iqk_backup_rf_8812a(
	struct PHY_DM_STRUCT		*p_dm_odm,
	u32		*RFA_backup,
	u32		*RFB_backup,
	u32		*backup_rf_reg,
	u32		RF_NUM
)
{

	u32 i;
	odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	/* Save RF Parameters */
	for (i = 0; i < RF_NUM; i++) {
		RFA_backup[i] = odm_get_rf_reg(p_dm_odm, ODM_RF_PATH_A, backup_rf_reg[i], MASKDWORD);
		RFB_backup[i] = odm_get_rf_reg(p_dm_odm, ODM_RF_PATH_B, backup_rf_reg[i], MASKDWORD);
	}
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("BackupRF Success!!!!\n"));
}
void _iqk_backup_afe_8812a(
	struct PHY_DM_STRUCT		*p_dm_odm,
	u32		*AFE_backup,
	u32		*backup_afe_reg,
	u32		AFE_NUM
)
{
	u32 i;
	odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	/* Save AFE Parameters */
	for (i = 0; i < AFE_NUM; i++)
		AFE_backup[i] = odm_read_4byte(p_dm_odm, backup_afe_reg[i]);
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("BackupAFE Success!!!!\n"));
}
void _iqk_restore_mac_bb_8812a(
	struct PHY_DM_STRUCT		*p_dm_odm,
	u32		*MACBB_backup,
	u32		*backup_macbb_reg,
	u32		MACBB_NUM
)
{
	u32 i;
	odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	/* Reload MacBB Parameters */
	for (i = 0; i < MACBB_NUM; i++)
		odm_write_4byte(p_dm_odm, backup_macbb_reg[i], MACBB_backup[i]);
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RestoreMacBB Success!!!!\n"));
}
void _iqk_restore_rf_8812a(
	struct PHY_DM_STRUCT			*p_dm_odm,
	enum odm_rf_radio_path_e	path,
	u32			*backup_rf_reg,
	u32			*RF_backup,
	u32			RF_REG_NUM
)
{
	u32 i;

	odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	for (i = 0; i < RF_REG_NUM; i++)
		odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, backup_rf_reg[i], RFREGOFFSETMASK, RF_backup[i]);
	odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0xef, RFREGOFFSETMASK, 0x0);

	switch (path) {
	case ODM_RF_PATH_A:
	{
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RestoreRF path A Success!!!!\n"));
	}
	break;
	case ODM_RF_PATH_B:
	{
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RestoreRF path B Success!!!!\n"));
	}
	break;
	default:
		break;
	}
}
void _iqk_restore_afe_8812a(
	struct PHY_DM_STRUCT		*p_dm_odm,
	u32		*AFE_backup,
	u32		*backup_afe_reg,
	u32		AFE_NUM
)
{
	u32 i;
	odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	/* Reload AFE Parameters */
	for (i = 0; i < AFE_NUM; i++)
		odm_write_4byte(p_dm_odm, backup_afe_reg[i], AFE_backup[i]);
	odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1); /* [31] = 1 --> Page C1 */
	odm_write_4byte(p_dm_odm, 0xc80, 0x0);
	odm_write_4byte(p_dm_odm, 0xc84, 0x0);
	odm_write_4byte(p_dm_odm, 0xc88, 0x0);
	odm_write_4byte(p_dm_odm, 0xc8c, 0x3c000000);
	odm_write_4byte(p_dm_odm, 0xcb8, 0x0);
	odm_write_4byte(p_dm_odm, 0xe80, 0x0);
	odm_write_4byte(p_dm_odm, 0xe84, 0x0);
	odm_write_4byte(p_dm_odm, 0xe88, 0x0);
	odm_write_4byte(p_dm_odm, 0xe8c, 0x3c000000);
	odm_write_4byte(p_dm_odm, 0xeb8, 0x0);
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RestoreAFE Success!!!!\n"));
}


void _iqk_configure_mac_8812a(
	struct PHY_DM_STRUCT		*p_dm_odm
)
{
	/* ========MAC register setting======== */
	odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	odm_write_1byte(p_dm_odm, 0x522, 0x3f);
	odm_set_bb_reg(p_dm_odm, 0x550, BIT(11) | BIT(3), 0x0);
	odm_set_bb_reg(p_dm_odm, 0x808, BIT(28), 0x0);	/*		CCK Off */
	odm_write_1byte(p_dm_odm, 0x808, 0x00);		/*		RX ante off */
	odm_set_bb_reg(p_dm_odm, 0x838, 0xf, 0xc);		/*		CCA off */
}

#define cal_num 3

void _iqk_tx_8812a(
	struct PHY_DM_STRUCT		*p_dm_odm,
	enum odm_rf_radio_path_e path,
	u8 chnl_idx
)
{
	u32		TX_fail, RX_fail, delay_count, IQK_ready, cal_retry, cal = 0, temp_reg65;
	int			TX_X = 0, TX_Y = 0, RX_X = 0, RX_Y = 0, tx_average = 0, rx_average = 0;
	int			TX_X0[cal_num], TX_Y0[cal_num], RX_X0[cal_num], RX_Y0[cal_num];
	boolean	TX0IQKOK = false, RX0IQKOK = false;
	int			TX_X1[cal_num], TX_Y1[cal_num], RX_X1[cal_num], RX_Y1[cal_num];
	boolean	TX1IQKOK = false, RX1IQKOK = false, VDF_enable = false;
	int			i, k, VDF_Y[3], VDF_X[3], tx_dt[3], rx_dt[3], ii, dx = 0, dy = 0, TX_finish = 0, RX_finish = 0;
	struct odm_rf_calibration_structure  *p_rf_calibrate_info = &(p_dm_odm->rf_calibrate_info);
	struct rtl8192cd_priv		*priv = p_dm_odm->priv;

	p_dm_odm->priv->pshare->IQK_total_cnt++;

	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("band_width = %d ext_pa = %d pBand = %d\n", *p_dm_odm->p_band_width, p_dm_odm->ext_pa, *p_dm_odm->p_band_type));

	if (*p_dm_odm->p_band_width == 2)
		VDF_enable = true;

	temp_reg65 = odm_get_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x65, MASKDWORD);

	switch (path) {
	case ODM_RF_PATH_A:
		odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0);

		/* Port 0 DAC/ADC on*/
		odm_write_4byte(p_dm_odm, 0xc60, 0x77777777);
		odm_write_4byte(p_dm_odm, 0xc64, 0x77777777);

		/*Port 1 DAC/ADC off*/
		odm_write_4byte(p_dm_odm, 0xe60, 0x00000000);
		odm_write_4byte(p_dm_odm, 0xe64, 0x00000000);

		odm_write_4byte(p_dm_odm, 0xc68, 0x19791979);
		odm_set_bb_reg(p_dm_odm, 0xc00, 0xf, 0x4);

		/*DAC/ADC sampling rate (160 MHz)*/
		odm_set_bb_reg(p_dm_odm, 0xc5c, BIT(26) | BIT(25) | BIT(24), 0x7);

		odm_set_bb_reg(p_dm_odm, 0xcb0, 0x00ff0000, 0x77);
		odm_set_bb_reg(p_dm_odm, 0xcb4, 0x03000000, 0x0);
		break;
	case ODM_RF_PATH_B:
		odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0);
		/*Port 0 DAC/ADC off*/
		odm_write_4byte(p_dm_odm, 0xc60, 0x00000000);
		odm_write_4byte(p_dm_odm, 0xc64, 0x00000000);

		/*Port 1 DAC/ADC on*/
		odm_write_4byte(p_dm_odm, 0xe60, 0x77777777);
		odm_write_4byte(p_dm_odm, 0xe64, 0x77777777);

		odm_write_4byte(p_dm_odm, 0xe68, 0x19791979);

		odm_set_bb_reg(p_dm_odm, 0xe00, 0xf, 0x4);

		/*DAC/ADC sampling rate (160 MHz)*/
		odm_set_bb_reg(p_dm_odm, 0xe5c, BIT(26) | BIT(25) | BIT(24), 0x7);
		odm_set_bb_reg(p_dm_odm, 0xeb0, 0x00ff0000, 0x77);
		odm_set_bb_reg(p_dm_odm, 0xeb4, 0x03000000, 0x0);
		break;
	default:
		break;
	}

	while (cal < cal_num) {
		switch (path) {
		case ODM_RF_PATH_A:
		{
			/*======pathA TX IQK ======*/
			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0xef, RFREGOFFSETMASK, 0x80002);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x30, RFREGOFFSETMASK, 0x20000);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x31, RFREGOFFSETMASK, 0x3fffd);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x32, RFREGOFFSETMASK, 0xfe83f);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x65, RFREGOFFSETMASK, 0x931d5);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x8f, RFREGOFFSETMASK, 0x8a001);
			odm_write_4byte(p_dm_odm, 0x90c, 0x00008000);
			odm_write_4byte(p_dm_odm, 0xb00, 0x03000100);
			odm_set_bb_reg(p_dm_odm, 0xc94, BIT(0), 0x1);
			odm_write_4byte(p_dm_odm, 0x978, 0x29002000);/*TX (X,Y)*/
			odm_write_4byte(p_dm_odm, 0x97c, 0xa9002000);/*RX (X,Y)*/
			odm_write_4byte(p_dm_odm, 0x984, 0x00462910);/*[0]:AGC_en, [15]:idac_K_Mask*/

			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1);

			if (p_dm_odm->ext_pa)
				odm_write_4byte(p_dm_odm, 0xc88, 0x821403e3);
			else
				odm_write_4byte(p_dm_odm, 0xc88, 0x821403f1);

			if (*p_dm_odm->p_band_type == ODM_BAND_5G)
				odm_write_4byte(p_dm_odm, 0xc8c, 0x68163e96);
			else
				odm_write_4byte(p_dm_odm, 0xc8c, 0x28163e96);

			if (VDF_enable == 1) {
				/*====== pathA VDF TX IQK ======*/
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("TXVDF Start\n"));
				for (k = 0; k <= 2; k++) {
					switch (k) {
					case 0:
						odm_write_4byte(p_dm_odm, 0xc80, 0x18008c38);/*TX_Tone_idx[9:0], TxK_Mask[29] TX_Tone = 16*/
						odm_write_4byte(p_dm_odm, 0xc84, 0x38008c38);/*RX_Tone_idx[9:0], RxK_Mask[29]*/
						odm_write_4byte(p_dm_odm, 0x984, 0x00462910);/*[0]:AGC_en, [15]:idac_K_Mask*/
						odm_set_bb_reg(p_dm_odm, 0xce8, BIT(31), 0x0);
						break;
					case 1:
						odm_set_bb_reg(p_dm_odm, 0xc80, BIT(28), 0x0);
						odm_set_bb_reg(p_dm_odm, 0xc84, BIT(28), 0x0);
						odm_write_4byte(p_dm_odm, 0x984, 0x0046a910);/*[0]:AGC_en, [15]:idac_K_Mask*/
						break;
					case 2:
						ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("VDF_Y[1] = %x;;;VDF_Y[0] = %x\n", VDF_Y[1] >> 21 & 0x00007ff, VDF_Y[0] >> 21 & 0x00007ff));
						ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("VDF_X[1] = %x;;;VDF_X[0] = %x\n", VDF_X[1] >> 21 & 0x00007ff, VDF_X[0] >> 21 & 0x00007ff));
						tx_dt[cal] = (VDF_Y[1] >> 20) - (VDF_Y[0] >> 20);
						ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("tx_dt = %d\n", tx_dt[cal]));
						tx_dt[cal] = ((16 * tx_dt[cal]) * 10000 / 15708);
						tx_dt[cal] = (tx_dt[cal] >> 1) + (tx_dt[cal] & BIT(0));
						odm_write_4byte(p_dm_odm, 0xc80, 0x18008c20);
						odm_write_4byte(p_dm_odm, 0xc84, 0x38008c20);
						odm_set_bb_reg(p_dm_odm, 0xce8, BIT(31), 0x1);
						odm_set_bb_reg(p_dm_odm, 0xce8, 0x3fff0000, tx_dt[cal] & 0x00003fff);
						break;
					default:
						break;
					}

					odm_write_4byte(p_dm_odm, 0xcb8, 0x00100000);/*cb8[20] SI/PI*/
					cal_retry = 0;
					while (1) {
						/*one shot*/
						odm_write_4byte(p_dm_odm, 0x980, 0xfa000000);
						odm_write_4byte(p_dm_odm, 0x980, 0xf8000000);
						delay_ms(10);
						odm_write_4byte(p_dm_odm, 0xcb8, 0x00000000);
						delay_count = 0;
						while (1) {
							IQK_ready = odm_get_bb_reg(p_dm_odm, 0xd00, BIT(10));
							if (IQK_ready || (delay_count > 20))
								break;
							delay_ms(1);
							delay_count++;
						}

						if (delay_count < 20) {
							/*============pathA VDF TXIQK Check==============*/
							TX_fail = odm_get_bb_reg(p_dm_odm, 0xd00, BIT(12));
							if (~TX_fail) {
								odm_write_4byte(p_dm_odm, 0xcb8, 0x02000000);
								VDF_X[k] = odm_get_bb_reg(p_dm_odm, 0xd00, 0x07ff0000) << 21;
								odm_write_4byte(p_dm_odm, 0xcb8, 0x04000000);
								VDF_Y[k] = odm_get_bb_reg(p_dm_odm, 0xd00, 0x07ff0000) << 21;
								TX0IQKOK = true;
								break;
							} else {
								TX0IQKOK = false;
								cal_retry++;
								if (cal_retry == 10)
									break;
							}
						} else {
							TX0IQKOK = false;
							cal_retry++;
							if (cal_retry == 10)
								break;
						}
					}
				}
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("TXA_VDF_cal_retry = %d\n", cal_retry));
				TX_X0[cal] = VDF_X[k - 1];
				TX_Y0[cal] = VDF_Y[k - 1];
			} else {

				/*====== pathA TX IQK ======*/
				odm_write_4byte(p_dm_odm, 0xc80, 0x18008c10);/*TX_Tone_idx[9:0], TxK_Mask[29] TX_Tone = 16*/
				odm_write_4byte(p_dm_odm, 0xc84, 0x38008c10);/*RX_Tone_idx[9:0], RxK_Mask[29]*/
				odm_write_4byte(p_dm_odm, 0xce8, 0x00000000);
				odm_write_4byte(p_dm_odm, 0xcb8, 0x00100000);
				cal_retry = 0;
				while (1) {
					odm_write_4byte(p_dm_odm, 0x980, 0xfa000000);
					odm_write_4byte(p_dm_odm, 0x980, 0xf8000000);
					delay_ms(10); /* delay 25ms */
					odm_write_4byte(p_dm_odm, 0xcb8, 0x00000000);
					delay_count = 0;
					while (1) {
						IQK_ready = odm_get_bb_reg(p_dm_odm, 0xd00, BIT(10));
						if (IQK_ready || (delay_count > 20))
							break;
						delay_ms(1);
						delay_count++;
					}

					if (delay_count < 20) {
						/*============pathA TXIQK Check==============*/
						TX_fail = odm_get_bb_reg(p_dm_odm, 0xd00, BIT(12));
						if (~TX_fail) {
							odm_write_4byte(p_dm_odm, 0xcb8, 0x02000000);
							TX_X0[cal] = odm_get_bb_reg(p_dm_odm, 0xd00, 0x07ff0000) << 21;
							odm_write_4byte(p_dm_odm, 0xcb8, 0x04000000);
							TX_Y0[cal] = odm_get_bb_reg(p_dm_odm, 0xd00, 0x07ff0000) << 21;
							TX0IQKOK = true;
#if 0
							odm_write_4byte(p_dm_odm, 0xcb8, 0x01000000);
							reg1 = odm_get_bb_reg(p_dm_odm, 0xd00, 0xffffffff);
							odm_write_4byte(p_dm_odm, 0xcb8, 0x02000000);
							reg2 = odm_get_bb_reg(p_dm_odm, 0xd00, 0x0000001f);
							image_power = (reg2 << 32) + reg1;
							dbg_print("Before PW = %d\n", image_power);
							odm_write_4byte(p_dm_odm, 0xcb8, 0x03000000);
							reg1 = odm_get_bb_reg(p_dm_odm, 0xd00, 0xffffffff);
							odm_write_4byte(p_dm_odm, 0xcb8, 0x04000000);
							reg2 = odm_get_bb_reg(p_dm_odm, 0xd00, 0x0000001f);
							image_power = (reg2 << 32) + reg1;
							dbg_print("After PW = %d\n", image_power);
#endif
							break;
						} else {
							TX0IQKOK = false;
							cal_retry++;
							if (cal_retry == 10)
								break;
						}
					} else {
						TX0IQKOK = false;
						cal_retry++;
						if (cal_retry == 10)
							break;
					}
				}

				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("TXA_cal_retry = %d\n", cal_retry));
			}

			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x58, 0x7fe00, odm_get_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x8, 0xffc00));
			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1);
			if (TX0IQKOK == false)
				break;


			/*======pathA VDF RX IQK ======*/
			if (VDF_enable == 1) {
				odm_set_bb_reg(p_dm_odm, 0xce8, BIT(31), 0x0);/*TX VDF Disable*/
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RXVDF Start\n"));

				odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0xef, RFREGOFFSETMASK, 0x80000);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x30, RFREGOFFSETMASK, 0x30000);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x31, RFREGOFFSETMASK, 0x3f7ff);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x32, RFREGOFFSETMASK, 0xfe7bf);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x8f, RFREGOFFSETMASK, 0x88001);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x65, RFREGOFFSETMASK, 0x931d0);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0xef, RFREGOFFSETMASK, 0x00000);
				odm_set_bb_reg(p_dm_odm, 0x978, BIT(31), 0x1);
				odm_set_bb_reg(p_dm_odm, 0x97c, BIT(31), 0x0);
				odm_write_4byte(p_dm_odm, 0x984, 0x0046a911);

				odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1);
				odm_write_4byte(p_dm_odm, 0xc88, 0x02140119);
				odm_write_4byte(p_dm_odm, 0xc8c, 0x28161420);

				for (k = 0; k <= 2; k++) {
					odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0);
					odm_set_bb_reg(p_dm_odm, 0x978, 0x03FF8000, (VDF_X[k]) >> 21 & 0x000007ff);
					odm_set_bb_reg(p_dm_odm, 0x978, 0x000007FF, (VDF_Y[k]) >> 21 & 0x000007ff);

					odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1);
					switch (k) {
					case 0:
						odm_write_4byte(p_dm_odm, 0xc80, 0x38008c38);/*TX_Tone_idx[9:0], TxK_Mask[29] TX_Tone = 16*/
						odm_write_4byte(p_dm_odm, 0xc84, 0x18008c38);/*RX_Tone_idx[9:0], RxK_Mask[29]*/
						odm_set_bb_reg(p_dm_odm, 0xce8, BIT(30), 0x0);
						break;
					case 1:
						odm_write_4byte(p_dm_odm, 0xc80, 0x28008c38);
						odm_write_4byte(p_dm_odm, 0xc84, 0x08008c38);
						break;
					case 2:
						ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("VDF_Y[1] = %x;;;VDF_Y[0] = %x\n", VDF_Y[1] >> 21 & 0x00007ff, VDF_Y[0] >> 21 & 0x00007ff));
						ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("VDF_X[1] = %x;;;VDF_X[0] = %x\n", VDF_X[1] >> 21 & 0x00007ff, VDF_X[0] >> 21 & 0x00007ff));
						rx_dt[cal] = (VDF_Y[1] >> 20) - (VDF_Y[0] >> 20);
						ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("rx_dt = %d\n", rx_dt[cal]));
						rx_dt[cal] = ((16 * rx_dt[cal]) * 10000 / 13823);
						rx_dt[cal] = (rx_dt[cal] >> 1) + (rx_dt[cal] & BIT(0));
						odm_write_4byte(p_dm_odm, 0xc80, 0x38008c20);
						odm_write_4byte(p_dm_odm, 0xc84, 0x18008c20);
						odm_set_bb_reg(p_dm_odm, 0xce8, 0x00003fff, rx_dt[cal] & 0x00003fff);
						break;
					default:
						break;
					}

					if (k == 2)
						odm_set_bb_reg(p_dm_odm, 0xce8, BIT(30), 0x1);  /*RX VDF Enable*/

					odm_write_4byte(p_dm_odm, 0xcb8, 0x00100000);/*cb8[20] N SI/PI*/
					cal_retry = 0;
					while (1) {
						/*one shot*/
						odm_write_4byte(p_dm_odm, 0x980, 0xfa000000);
						odm_write_4byte(p_dm_odm, 0x980, 0xf8000000);
						delay_ms(10);
						odm_write_4byte(p_dm_odm, 0xcb8, 0x00000000);
						delay_count = 0;
						while (1) {
							IQK_ready = odm_get_bb_reg(p_dm_odm, 0xd00, BIT(10));
							if (IQK_ready || (delay_count > 20))
								break;
							delay_ms(1);
							delay_count++;
						}

						odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0);
						ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("==== A VDF: path A RF0 = 0x%x ====\n",
							odm_get_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)0, 0x0, RFREGOFFSETMASK)));
						ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("==== A VDF: path B RF0 = 0x%x ====\n",
							odm_get_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)1, 0x0, RFREGOFFSETMASK)));
						odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1);

						if (delay_count < 20) {
							/*============pathA VDF RXIQK Check==============*/
							RX_fail = odm_get_bb_reg(p_dm_odm, 0xd00, BIT(11));
							if (RX_fail == 0) {
								odm_write_4byte(p_dm_odm, 0xcb8, 0x06000000);
								VDF_X[k] = odm_get_bb_reg(p_dm_odm, 0xd00, 0x07ff0000) << 21;
								odm_write_4byte(p_dm_odm, 0xcb8, 0x08000000);
								VDF_Y[k] = odm_get_bb_reg(p_dm_odm, 0xd00, 0x07ff0000) << 21;
								RX0IQKOK = true;
								break;
							} else {
								odm_set_bb_reg(p_dm_odm, 0xc10, 0x000003ff, 0x200 >> 1);
								odm_set_bb_reg(p_dm_odm, 0xc10, 0x03ff0000, 0x0 >> 1);
								RX0IQKOK = false;
								cal_retry++;
								if (cal_retry == 10)
									break;
							}
						} else {
							RX0IQKOK = false;
							cal_retry++;
							if (cal_retry == 10)
								break;
						}
					}
				}
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RXA_VDF_cal_retry = %d\n", cal_retry));
				RX_X0[cal] = VDF_X[k - 1] ;
				RX_Y0[cal] = VDF_Y[k - 1];
				odm_set_bb_reg(p_dm_odm, 0xce8, BIT(31), 0x1);    /*TX VDF Enable*/
			} else {
				/*====== pathA RX IQK ======*/
				odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0xef, RFREGOFFSETMASK, 0x80000);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x30, RFREGOFFSETMASK, 0x30000);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x31, RFREGOFFSETMASK, 0x3f7ff);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x32, RFREGOFFSETMASK, 0xfe7bf);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x8f, RFREGOFFSETMASK, 0x88001);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x65, RFREGOFFSETMASK, 0x931d0);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0xef, RFREGOFFSETMASK, 0x00000);
				odm_set_bb_reg(p_dm_odm, 0x978, 0x03FF8000, (TX_X0[cal]) >> 21 & 0x000007ff);
				odm_set_bb_reg(p_dm_odm, 0x978, 0x000007FF, (TX_Y0[cal]) >> 21 & 0x000007ff);
				odm_set_bb_reg(p_dm_odm, 0x978, BIT(31), 0x1);
				odm_set_bb_reg(p_dm_odm, 0x97c, BIT(31), 0x0);
				odm_write_4byte(p_dm_odm, 0x90c, 0x00008000);
				odm_write_4byte(p_dm_odm, 0x984, 0x0046a911);

				odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1);
				odm_write_4byte(p_dm_odm, 0xc80, 0x38008c10);
				odm_write_4byte(p_dm_odm, 0xc84, 0x18008c10);
				odm_write_4byte(p_dm_odm, 0xc88, 0x02140119);
				odm_write_4byte(p_dm_odm, 0xc8c, 0x28161420);
				odm_write_4byte(p_dm_odm, 0xcb8, 0x00100000);
				cal_retry = 0;
				while (1) {
					/*one shot*/
					odm_write_4byte(p_dm_odm, 0x980, 0xfa000000);
					odm_write_4byte(p_dm_odm, 0x980, 0xf8000000);
					delay_ms(10);
					odm_write_4byte(p_dm_odm, 0xcb8, 0x00000000);
					delay_count = 0;
					while (1) {
						IQK_ready = odm_get_bb_reg(p_dm_odm, 0xd00, BIT(10));
						if (IQK_ready || (delay_count > 20))
							break;
						else {
							delay_ms(1);
							delay_count++;
						}
					}

					odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0);
					ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("==== A: path A RF0 = 0x%x ====\n",
						odm_get_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)0, 0x0, RFREGOFFSETMASK)));
					ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("==== A: path B RF0 = 0x%x ====\n",
						odm_get_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)1, 0x0, RFREGOFFSETMASK)));
					odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1);

					if (delay_count < 20) {
						/*============pathA RXIQK Check==============*/
						RX_fail = odm_get_bb_reg(p_dm_odm, 0xd00, BIT(11));
						if (RX_fail == 0) {
							odm_write_4byte(p_dm_odm, 0xcb8, 0x06000000);
							RX_X0[cal] = odm_get_bb_reg(p_dm_odm, 0xd00, 0x07ff0000) << 21;
							odm_write_4byte(p_dm_odm, 0xcb8, 0x08000000);
							RX_Y0[cal] = odm_get_bb_reg(p_dm_odm, 0xd00, 0x07ff0000) << 21;
							RX0IQKOK = true;
#if 0
							odm_write_4byte(p_dm_odm, 0xcb8, 0x05000000);
							reg1 = odm_get_bb_reg(p_dm_odm, 0xd00, 0xffffffff);
							odm_write_4byte(p_dm_odm, 0xcb8, 0x06000000);
							reg2 = odm_get_bb_reg(p_dm_odm, 0xd00, 0x0000001f);
							image_power = (reg2 << 32) + reg1;
							ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("Before PW = %d\n", image_power));
							odm_write_4byte(p_dm_odm, 0xcb8, 0x07000000);
							reg1 = odm_get_bb_reg(p_dm_odm, 0xd00, 0xffffffff);
							odm_write_4byte(p_dm_odm, 0xcb8, 0x08000000);
							reg2 = odm_get_bb_reg(p_dm_odm, 0xd00, 0x0000001f);
							image_power = (reg2 << 32) + reg1;
							ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("After PW = %d\n", image_power));
#endif
							break;
						} else {
							odm_set_bb_reg(p_dm_odm, 0xc10, 0x000003ff, 0x200 >> 1);
							odm_set_bb_reg(p_dm_odm, 0xc10, 0x03ff0000, 0x0 >> 1);
							RX0IQKOK = false;
							cal_retry++;
							if (cal_retry == 10)
								break;
						}
					} else {
						RX0IQKOK = false;
						cal_retry++;
						if (cal_retry == 10)
							break;
					}
				}
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RXA_cal_retry = %d\n", cal_retry));
			}
			if (TX0IQKOK)
				tx_average++;
			if (RX0IQKOK)
				rx_average++;
		}
		break;

		case ODM_RF_PATH_B:
		{
			/*path-B TX/RX IQK*/
			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0xef, RFREGOFFSETMASK, 0x80002);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x30, RFREGOFFSETMASK, 0x20000);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x31, RFREGOFFSETMASK, 0x3fffd);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x32, RFREGOFFSETMASK, 0xfe83f);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x65, RFREGOFFSETMASK, 0x931d5);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x8f, RFREGOFFSETMASK, 0x8a001);
			odm_write_4byte(p_dm_odm, 0x90c, 0x00008000);
			odm_write_4byte(p_dm_odm, 0xb00, 0x03000100);
			odm_set_bb_reg(p_dm_odm, 0xe94, BIT(0), 0x1);
			odm_write_4byte(p_dm_odm, 0x978, 0x29002000);/*TX (X,Y)*/
			odm_write_4byte(p_dm_odm, 0x97c, 0xa9002000);/*RX (X,Y)*/
			odm_write_4byte(p_dm_odm, 0x984, 0x00462910);/*[0]:AGC_en, [15]:idac_K_Mask*/

			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1);
			if (p_dm_odm->ext_pa)
				odm_write_4byte(p_dm_odm, 0xe88, 0x821403e3);
			else
				odm_write_4byte(p_dm_odm, 0xe88, 0x821403f1);

			if (*p_dm_odm->p_band_type == ODM_BAND_5G)
				odm_write_4byte(p_dm_odm, 0xe8c, 0x68163e96);
			else
				odm_write_4byte(p_dm_odm, 0xe8c, 0x28163e96);

			if (VDF_enable == 1) {
				/*============pathB VDF TXIQK==============*/
				for (k = 0; k <= 2; k++) {
					switch (k) {
					case 0:
						/*one shot*/
						odm_write_4byte(p_dm_odm, 0xe80, 0x18008c38);/*TX_Tone_idx[9:0], TxK_Mask[29] TX_Tone = 16*/
						odm_write_4byte(p_dm_odm, 0xe84, 0x38008c38);/*RX_Tone_idx[9:0], RxK_Mask[29]*/
						odm_write_4byte(p_dm_odm, 0x984, 0x00462910);
						odm_set_bb_reg(p_dm_odm, 0xee8, BIT(31), 0x0);
						break;
					case 1:
						odm_set_bb_reg(p_dm_odm, 0xe80, BIT(28), 0x0);
						odm_set_bb_reg(p_dm_odm, 0xe84, BIT(28), 0x0);
						odm_write_4byte(p_dm_odm, 0x984, 0x0046a910);
						odm_set_bb_reg(p_dm_odm, 0xee8, BIT(31), 0x0);
						break;
					case 2:
						ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("VDF_Y[1] = %x;;;VDF_Y[0] = %x\n", VDF_Y[1] >> 21 & 0x00007ff, VDF_Y[0] >> 21 & 0x00007ff));
						ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("VDF_X[1] = %x;;;VDF_X[0] = %x\n", VDF_X[1] >> 21 & 0x00007ff, VDF_X[0] >> 21 & 0x00007ff));
						tx_dt[cal] = (VDF_Y[1] >> 20) - (VDF_Y[0] >> 20);
						ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("tx_dt = %d\n", tx_dt[cal]));
						tx_dt[cal] = ((16 * tx_dt[cal]) * 10000 / 15708);
						tx_dt[cal] = (tx_dt[cal] >> 1) + (tx_dt[cal] & BIT(0));
						odm_write_4byte(p_dm_odm, 0xe80, 0x18008c20);
						odm_write_4byte(p_dm_odm, 0xe84, 0x38008c20);
						odm_set_bb_reg(p_dm_odm, 0xee8, BIT(31), 0x1);
						odm_set_bb_reg(p_dm_odm, 0xee8, 0x3fff0000, tx_dt[cal] & 0x00003fff);
						break;
					default:
						break;
					}

					odm_write_4byte(p_dm_odm, 0xeb8, 0x00100000);
					cal_retry = 0;
					while (1) {
						/*one shot*/
						odm_write_4byte(p_dm_odm, 0x980, 0xfa000000);
						odm_write_4byte(p_dm_odm, 0x980, 0xf8000000);
						delay_ms(10);
						odm_write_4byte(p_dm_odm, 0xeb8, 0x00000000);
						delay_count = 0;
						while (1) {
							IQK_ready = odm_get_bb_reg(p_dm_odm, 0xd40, BIT(10));
							if (IQK_ready || (delay_count > 20))
								break;
							else {
								delay_ms(1);
								delay_count++;
							}
						}

						if (delay_count < 20) {
							/*============pathB VDF TXIQK Check==============*/
							TX_fail = odm_get_bb_reg(p_dm_odm, 0xd40, BIT(12));

							if (~TX_fail) {
								odm_write_4byte(p_dm_odm, 0xeb8, 0x02000000);
								VDF_X[k] = odm_get_bb_reg(p_dm_odm, 0xd40, 0x07ff0000) << 21;
								odm_write_4byte(p_dm_odm, 0xeb8, 0x04000000);
								VDF_Y[k] = odm_get_bb_reg(p_dm_odm, 0xd40, 0x07ff0000) << 21;
								TX1IQKOK = true;
								break;
							} else {
								TX1IQKOK = false;
								cal_retry++;
								if (cal_retry == 10)
									break;
							}
						} else {
							TX1IQKOK = false;
							cal_retry++;
							if (cal_retry == 10)
								break;
						}
					}
				}
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("TXB_VDF_cal_retry = %d\n", cal_retry));
				TX_X1[cal] = VDF_X[k - 1] ;
				TX_Y1[cal] = VDF_Y[k - 1];
			} else {
				/*============pathB TXIQK==============*/
				odm_write_4byte(p_dm_odm, 0xe80, 0x18008c10);
				odm_write_4byte(p_dm_odm, 0xe84, 0x38008c10);
				odm_write_4byte(p_dm_odm, 0xee8, 0x00000000);
				odm_write_4byte(p_dm_odm, 0xeb8, 0x00100000);
				cal_retry = 0;
				while (1) {
					/*one shot*/
					odm_write_4byte(p_dm_odm, 0x980, 0xfa000000);
					odm_write_4byte(p_dm_odm, 0x980, 0xf8000000);
					delay_ms(10);
					odm_write_4byte(p_dm_odm, 0xeb8, 0x00000000);
					delay_count = 0;
					while (1) {
						IQK_ready = odm_get_bb_reg(p_dm_odm, 0xd40, BIT(10));
						if (IQK_ready || (delay_count > 20))
							break;
						delay_ms(1);
						delay_count++;
					}

					if (delay_count < 20) {
						/*============pathB TXIQK Check==============*/
						TX_fail = odm_get_bb_reg(p_dm_odm, 0xd40, BIT(12));
						if (~TX_fail) {
							odm_write_4byte(p_dm_odm, 0xeb8, 0x02000000);
							TX_X1[cal] = odm_get_bb_reg(p_dm_odm, 0xd40, 0x07ff0000) << 21;
							odm_write_4byte(p_dm_odm, 0xeb8, 0x04000000);
							TX_Y1[cal] = odm_get_bb_reg(p_dm_odm, 0xd40, 0x07ff0000) << 21;
							TX1IQKOK = true;
#if 0
							int			reg1 = 0, reg2 = 0, image_power = 0;
							odm_write_4byte(p_dm_odm, 0xeb8, 0x01000000);
							reg1 = odm_get_bb_reg(p_dm_odm, 0xd40, 0xffffffff);
							odm_write_4byte(p_dm_odm, 0xeb8, 0x02000000);
							reg2 = odm_get_bb_reg(p_dm_odm, 0xd40, 0x0000001f);
							image_power = (reg2 << 32) + reg1;
							dbg_print("Before PW = %d\n", image_power);
							odm_write_4byte(p_dm_odm, 0xeb8, 0x03000000);
							reg1 = odm_get_bb_reg(p_dm_odm, 0xd40, 0xffffffff);
							odm_write_4byte(p_dm_odm, 0xeb8, 0x04000000);
							reg2 = odm_get_bb_reg(p_dm_odm, 0xd40, 0x0000001f);
							image_power = (reg2 << 32) + reg1;
							dbg_print("After PW = %d\n", image_power);
#endif
							break;
						} else {
							TX1IQKOK = false;
							cal_retry++;
							if (cal_retry == 10)
								break;
						}
					} else {
						TX1IQKOK = false;
						cal_retry++;
						if (cal_retry == 10)
							break;
					}
				}
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("TXB_cal_retry = %d\n", cal_retry));
			}

			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x58, 0x7fe00, odm_get_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x8, 0xffc00));
			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1);

			if (TX1IQKOK == false)
				break;


			/*======pathB VDF RX IQK ======*/
			if (VDF_enable == 1) {
				odm_set_bb_reg(p_dm_odm, 0xee8, BIT(31), 0x0);/*TX VDF Disable*/
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RXVDF Start\n"));

				odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0xef, RFREGOFFSETMASK, 0x80000);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x30, RFREGOFFSETMASK, 0x30000);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x31, RFREGOFFSETMASK, 0x3f7ff);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x32, RFREGOFFSETMASK, 0xfe7bf);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x8f, RFREGOFFSETMASK, 0x88001);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x65, RFREGOFFSETMASK, 0x931d0);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0xef, RFREGOFFSETMASK, 0x00000);

				odm_set_bb_reg(p_dm_odm, 0x978, BIT(31), 0x1);
				odm_set_bb_reg(p_dm_odm, 0x97c, BIT(31), 0x0);
				odm_write_4byte(p_dm_odm, 0x984, 0x0046a911);
				odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1);
				odm_write_4byte(p_dm_odm, 0xe88, 0x02140119);
				odm_write_4byte(p_dm_odm, 0xe8c, 0x28161420);

				for (k = 0; k <= 2; k++) {
					odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0);
					odm_set_bb_reg(p_dm_odm, 0x978, 0x03FF8000, (VDF_X[k]) >> 21 & 0x000007ff);
					odm_set_bb_reg(p_dm_odm, 0x978, 0x000007FF, (VDF_Y[k]) >> 21 & 0x000007ff);
					odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1);

					switch (k) {
					case 0:
						odm_write_4byte(p_dm_odm, 0xe80, 0x38008c38);/*TX_Tone_idx[9:0], TxK_Mask[29] TX_Tone = 16*/
						odm_write_4byte(p_dm_odm, 0xe84, 0x18008c38);/*RX_Tone_idx[9:0], RxK_Mask[29]*/
						odm_set_bb_reg(p_dm_odm, 0xee8, BIT(30), 0x0);
						break;
					case 1:
						odm_write_4byte(p_dm_odm, 0xe80, 0x28008c38);
						odm_write_4byte(p_dm_odm, 0xe84, 0x08008c38);
						odm_set_bb_reg(p_dm_odm, 0xee8, BIT(30), 0x0);
						break;
					case 2:
						ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("VDF_Y[1] = %x;;;VDF_Y[0] = %x\n", VDF_Y[1] >> 21 & 0x00007ff, VDF_Y[0] >> 21 & 0x00007ff));
						ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("VDF_X[1] = %x;;;VDF_X[0] = %x\n", VDF_X[1] >> 21 & 0x00007ff, VDF_X[0] >> 21 & 0x00007ff));
						rx_dt[cal] = (VDF_Y[1] >> 20) - (VDF_Y[0] >> 20);
						ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("rx_dt = %d\n", rx_dt[cal]));
						rx_dt[cal] = ((16 * rx_dt[cal]) * 10000 / 13823);
						rx_dt[cal] = (rx_dt[cal] >> 1) + (rx_dt[cal] & BIT(0));
						odm_write_4byte(p_dm_odm, 0xe80, 0x38008c20);
						odm_write_4byte(p_dm_odm, 0xe84, 0x18008c20);
						odm_set_bb_reg(p_dm_odm, 0xee8, 0x00003fff, rx_dt[cal] & 0x00003fff);
						break;
					default:
						break;
					}


					if (k == 2)
						odm_set_bb_reg(p_dm_odm, 0xee8, BIT(30), 0x1);  /*RX VDF Enable*/

					odm_write_4byte(p_dm_odm, 0xeb8, 0x00100000);

					cal_retry = 0;
					while (1) {
						/*one shot*/
						odm_write_4byte(p_dm_odm, 0x980, 0xfa000000);
						odm_write_4byte(p_dm_odm, 0x980, 0xf8000000);
						delay_ms(10);
						odm_write_4byte(p_dm_odm, 0xeb8, 0x00000000);
						delay_count = 0;
						while (1) {
							IQK_ready = odm_get_bb_reg(p_dm_odm, 0xd40, BIT(10));
							if (IQK_ready || (delay_count > 20))
								break;
							delay_ms(1);
							delay_count++;
						}

						odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0);
						ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("==== B VDF: path A RF0 = 0x%x ====\n",
							odm_get_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)0, 0x0, RFREGOFFSETMASK)));
						ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("==== B VDF: path B RF0 = 0x%x ====\n",
							odm_get_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)1, 0x0, RFREGOFFSETMASK)));
						odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1);

						if (delay_count < 20) {
							/*============pathB VDF RXIQK Check==============*/
							RX_fail = odm_get_bb_reg(p_dm_odm, 0xd40, BIT(11));
							if (RX_fail == 0) {
								odm_write_4byte(p_dm_odm, 0xeb8, 0x06000000);
								VDF_X[k] = odm_get_bb_reg(p_dm_odm, 0xd40, 0x07ff0000) << 21;
								odm_write_4byte(p_dm_odm, 0xeb8, 0x08000000);
								VDF_Y[k] = odm_get_bb_reg(p_dm_odm, 0xd40, 0x07ff0000) << 21;
								RX1IQKOK = true;
								break;
							} else {
								odm_set_bb_reg(p_dm_odm, 0xe10, 0x000003ff, 0x200 >> 1);
								odm_set_bb_reg(p_dm_odm, 0xe10, 0x03ff0000, 0x0 >> 1);
								RX1IQKOK = false;
								cal_retry++;
								if (cal_retry == 10)
									break;
							}
						} else {
							RX1IQKOK = false;
							cal_retry++;
							if (cal_retry == 10)
								break;
						}
					}
				}
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RXB_VDF_cal_retry = %d\n", cal_retry));
				RX_X1[cal] = VDF_X[k - 1] ;
				RX_Y1[cal] = VDF_Y[k - 1];
				odm_set_bb_reg(p_dm_odm, 0xee8, BIT(31), 0x1);    /*TX VDF Enable*/
			} else {
				/*============pathB RXIQK==============*/
				odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0xef, RFREGOFFSETMASK, 0x80000);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x30, RFREGOFFSETMASK, 0x30000);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x31, RFREGOFFSETMASK, 0x3f7ff);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x32, RFREGOFFSETMASK, 0xfe7bf);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x8f, RFREGOFFSETMASK, 0x88001);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x65, RFREGOFFSETMASK, 0x931d0);
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0xef, RFREGOFFSETMASK, 0x00000);

				odm_set_bb_reg(p_dm_odm, 0x978, 0x03FF8000, (TX_X1[cal]) >> 21 & 0x000007ff);
				odm_set_bb_reg(p_dm_odm, 0x978, 0x000007FF, (TX_Y1[cal]) >> 21 & 0x000007ff);
				odm_set_bb_reg(p_dm_odm, 0x978, BIT(31), 0x1);
				odm_set_bb_reg(p_dm_odm, 0x97c, BIT(31), 0x0);
				odm_write_4byte(p_dm_odm, 0x90c, 0x00008000);
				odm_write_4byte(p_dm_odm, 0x984, 0x0046a911);

				odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1);
				odm_write_4byte(p_dm_odm, 0xe80, 0x38008c15);
				odm_write_4byte(p_dm_odm, 0xe84, 0x18008c15);
				odm_write_4byte(p_dm_odm, 0xe88, 0x02140119);
				odm_write_4byte(p_dm_odm, 0xe8c, 0x28161420);
				odm_write_4byte(p_dm_odm, 0xeb8, 0x00100000);

				cal_retry = 0;
				while (1) {
					/*one shot*/
					odm_write_4byte(p_dm_odm, 0x980, 0xfa000000);
					odm_write_4byte(p_dm_odm, 0x980, 0xf8000000);
					delay_ms(10);
					odm_write_4byte(p_dm_odm, 0xeb8, 0x00000000);
					delay_count = 0;
					while (1) {
						IQK_ready = odm_get_bb_reg(p_dm_odm, 0xd40, BIT(10));
						if (IQK_ready || (delay_count > 20))
							break;
						delay_ms(1);
						delay_count++;
					}

					odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0);
					ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("==== B: path A RF0 = 0x%x ====\n",
						odm_get_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)0, 0x0, RFREGOFFSETMASK)));
					ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("==== B: path B RF0 = 0x%x ====\n",
						odm_get_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)1, 0x0, RFREGOFFSETMASK)));
					odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1);

					if (delay_count < 20) {
						/*============pathB RXIQK Check==============*/
						RX_fail = odm_get_bb_reg(p_dm_odm, 0xd40, BIT(11));
						if (RX_fail == 0) {
							odm_write_4byte(p_dm_odm, 0xeb8, 0x06000000);
							RX_X1[cal] = odm_get_bb_reg(p_dm_odm, 0xd40, 0x07ff0000) << 21;
							odm_write_4byte(p_dm_odm, 0xeb8, 0x08000000);
							RX_Y1[cal] = odm_get_bb_reg(p_dm_odm, 0xd40, 0x07ff0000) << 21;
							RX1IQKOK = true;
							break;
						} else {
							odm_set_bb_reg(p_dm_odm, 0xe10, 0x000003ff, 0x200 >> 1);
							odm_set_bb_reg(p_dm_odm, 0xe10, 0x03ff0000, 0x0 >> 1);
							RX1IQKOK = false;
							cal_retry++;
							if (cal_retry == 10)
								break;
						}
					} else {
						RX1IQKOK = false;
						cal_retry++;
						if (cal_retry == 10)
							break;
					}
				}
#if 0
				odm_write_4byte(p_dm_odm, 0xeb8, 0x05000000);
				reg1 = odm_get_bb_reg(p_dm_odm, 0xd40, 0xffffffff);
				odm_write_4byte(p_dm_odm, 0xeb8, 0x06000000);
				reg2 = odm_get_bb_reg(p_dm_odm, 0xd40, 0x0000001f);
				image_power = (reg2 << 32) + reg1;
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("Before PW = %d\n", image_power));

				odm_write_4byte(p_dm_odm, 0xeb8, 0x07000000);
				reg1 = odm_get_bb_reg(p_dm_odm, 0xd40, 0xffffffff);
				odm_write_4byte(p_dm_odm, 0xeb8, 0x08000000);
				reg2 = odm_get_bb_reg(p_dm_odm, 0xd40, 0x0000001f);
				image_power = (reg2 << 32) + reg1;
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("After PW = %d\n", image_power));
#endif

				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RXB_cal_retry = %d\n", cal_retry));
			}
			if (RX1IQKOK)
				rx_average++;
			if (TX1IQKOK)
				tx_average++;
		}
		break;
		default:
			break;
		}
		cal++;
	}
	/*FillIQK Result*/
	switch (path) {
	case ODM_RF_PATH_A:
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("========Path_A =======\n"));
		if (tx_average == 0) {
			_iqk_tx_fill_iqc_8812a(p_dm_odm, path, 0x200, 0x0);
			break;
		}

		for (i = 0; i < tx_average; i++)
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("TX_X0[%d] = %x ;; TX_Y0[%d] = %x\n", i, (TX_X0[i]) >> 21 & 0x000007ff, i, (TX_Y0[i]) >> 21 & 0x000007ff));

		for (i = 0; i < tx_average; i++) {
			for (ii = i + 1; ii < tx_average; ii++) {
				dx = (TX_X0[i] >> 21) - (TX_X0[ii] >> 21);
				if (dx < 4 && dx > -4) {
					dy = (TX_Y0[i] >> 21) - (TX_Y0[ii] >> 21);
					if (dy < 4 && dy > -4) {
						TX_X = ((TX_X0[i] >> 21) + (TX_X0[ii] >> 21)) / 2;
						TX_Y = ((TX_Y0[i] >> 21) + (TX_Y0[ii] >> 21)) / 2;
						if (*p_dm_odm->p_band_width == 2)
							tx_dt[0] = (tx_dt[i] + tx_dt[ii]) / 2;
						TX_finish = 1;
						break;
					}
				}
			}
			if (TX_finish == 1)
				break;
		}

		if (*p_dm_odm->p_band_width == 2) {
			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1);
			odm_set_bb_reg(p_dm_odm, 0xce8, 0x3fff0000, tx_dt[0] & 0x00003fff);
			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0);
		}
		if (TX_finish == 1)
			_iqk_tx_fill_iqc_8812a(p_dm_odm, path, TX_X, TX_Y);
		else
			_iqk_tx_fill_iqc_8812a(p_dm_odm, path, 0x200, 0x0);

		if (rx_average == 0) {
			_iqk_rx_fill_iqc_8812a(p_dm_odm, path, 0x200, 0x0);
			break;
		}

		for (i = 0; i < rx_average; i++)
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RX_X0[%d] = %x ;; RX_Y0[%d] = %x\n", i, (RX_X0[i]) >> 21 & 0x000007ff, i, (RX_Y0[i]) >> 21 & 0x000007ff));

		for (i = 0; i < rx_average; i++) {
			for (ii = i + 1; ii < rx_average; ii++) {
				dx = (RX_X0[i] >> 21) - (RX_X0[ii] >> 21);
				if (dx < 4 && dx > -4) {
					dy = (RX_Y0[i] >> 21) - (RX_Y0[ii] >> 21);
					if (dy < 4 && dy > -4) {
						RX_X = ((RX_X0[i] >> 21) + (RX_X0[ii] >> 21)) / 2;
						RX_Y = ((RX_Y0[i] >> 21) + (RX_Y0[ii] >> 21)) / 2;
						if (*p_dm_odm->p_band_width == 2)
							rx_dt[0] = (rx_dt[i] + rx_dt[ii]) / 2;
						RX_finish = 1;
						break;
					}
				}
			}
			if (RX_finish == 1)
				break;
		}

		if (*p_dm_odm->p_band_width == 2) {
			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1);
			odm_set_bb_reg(p_dm_odm, 0xce8, 0x00003fff, rx_dt[0] & 0x00003fff);
			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0);
		}

		if (RX_finish == 1)
			_iqk_rx_fill_iqc_8812a(p_dm_odm, path, RX_X, RX_Y);
		else
			_iqk_rx_fill_iqc_8812a(p_dm_odm, path, 0x200, 0x0);

		if (TX_finish && RX_finish) {
			p_rf_calibrate_info->is_need_iqk = false;
			p_rf_calibrate_info->iqk_matrix_reg_setting[chnl_idx].value[*p_dm_odm->p_band_width][0] = ((TX_X & 0x000007ff) << 16) + (TX_Y & 0x000007ff);	/* path A TX */
			p_rf_calibrate_info->iqk_matrix_reg_setting[chnl_idx].value[*p_dm_odm->p_band_width][1] = ((RX_X & 0x000007ff) << 16) + (RX_Y & 0x000007ff);	/* path A RX */

			if (*p_dm_odm->p_band_width == 2) {
				odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1);
				p_rf_calibrate_info->iqk_matrix_reg_setting[chnl_idx].value[*p_dm_odm->p_band_width][4] = odm_read_4byte(p_dm_odm, 0xce8);	/* path B VDF */
				odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0);
			}
		}
		break;
	case ODM_RF_PATH_B:
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("========Path_B =======\n"));
		if (tx_average == 0) {
			_iqk_tx_fill_iqc_8812a(p_dm_odm, path, 0x200, 0x0);
			break;
		}

		for (i = 0; i < tx_average; i++)
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("TX_X1[%d] = %x ;; TX_Y1[%d] = %x\n", i, (TX_X1[i]) >> 21 & 0x000007ff, i, (TX_Y1[i]) >> 21 & 0x000007ff));

		for (i = 0; i < tx_average; i++) {
			for (ii = i + 1; ii < tx_average; ii++) {
				dx = (TX_X1[i] >> 21) - (TX_X1[ii] >> 21);
				if (dx < 4 && dx > -4) {
					dy = (TX_Y1[i] >> 21) - (TX_Y1[ii] >> 21);
					if (dy < 4 && dy > -4) {
						TX_X = ((TX_X1[i] >> 21) + (TX_X1[ii] >> 21)) / 2;
						TX_Y = ((TX_Y1[i] >> 21) + (TX_Y1[ii] >> 21)) / 2;
						if (*p_dm_odm->p_band_width == 2)
							tx_dt[0] = (tx_dt[i] + tx_dt[ii]) / 2;
						TX_finish = 1;
						break;
					}
				}
			}
			if (TX_finish == 1)
				break;
		}

		if (*p_dm_odm->p_band_width == 2) {
			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1);
			odm_set_bb_reg(p_dm_odm, 0xee8, 0x3fff0000, tx_dt[0] & 0x00003fff);
			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0);
		}

		if (TX_finish == 1)
			_iqk_tx_fill_iqc_8812a(p_dm_odm, path, TX_X, TX_Y);
		else
			_iqk_tx_fill_iqc_8812a(p_dm_odm, path, 0x200, 0x0);

		if (rx_average == 0) {
			_iqk_rx_fill_iqc_8812a(p_dm_odm, path, 0x200, 0x0);
			break;
		}

		for (i = 0; i < rx_average; i++)
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RX_X1[%d] = %x ;; RX_Y1[%d] = %x\n", i, (RX_X1[i]) >> 21 & 0x000007ff, i, (RX_Y1[i]) >> 21 & 0x000007ff));

		for (i = 0; i < rx_average; i++) {
			for (ii = i + 1; ii < rx_average; ii++) {
				dx = (RX_X1[i] >> 21) - (RX_X1[ii] >> 21);
				if (dx < 4 && dx > -4) {
					dy = (RX_Y1[i] >> 21) - (RX_Y1[ii] >> 21);
					if (dy < 4 && dy > -4) {
						RX_X = ((RX_X1[i] >> 21) + (RX_X1[ii] >> 21)) / 2;
						RX_Y = ((RX_Y1[i] >> 21) + (RX_Y1[ii] >> 21)) / 2;
						if (*p_dm_odm->p_band_width == 2)
							rx_dt[0] = (rx_dt[i] + rx_dt[ii]) / 2;
						RX_finish = 1;
						break;
					}
				}
			}
			if (RX_finish == 1)
				break;
		}

		if (*p_dm_odm->p_band_width == 2) {
			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1);
			odm_set_bb_reg(p_dm_odm, 0xee8, 0x00003fff, rx_dt[0] & 0x00003fff);
			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0);
		}

		if (RX_finish == 1)
			_iqk_rx_fill_iqc_8812a(p_dm_odm, path, RX_X, RX_Y);
		else
			_iqk_rx_fill_iqc_8812a(p_dm_odm, path, 0x200, 0x0);

		if (TX_finish && RX_finish) {
			p_rf_calibrate_info->is_need_iqk = false;
			p_rf_calibrate_info->iqk_matrix_reg_setting[chnl_idx].value[*p_dm_odm->p_band_width][2] = ((TX_X & 0x000007ff) << 16) + (TX_Y & 0x000007ff);	/* path B TX */
			p_rf_calibrate_info->iqk_matrix_reg_setting[chnl_idx].value[*p_dm_odm->p_band_width][3] = ((RX_X & 0x000007ff) << 16) + (RX_Y & 0x000007ff);	/* path B RX */

			if (*p_dm_odm->p_band_width == 2) {
				odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1);
				p_rf_calibrate_info->iqk_matrix_reg_setting[chnl_idx].value[*p_dm_odm->p_band_width][5] = odm_read_4byte(p_dm_odm, 0xee8);	/* path B VDF */
				odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0);
			}
		}
		break;
	default:
		break;
	}

	if (!TX_finish && !RX_finish)
		priv->pshare->IQK_fail_cnt++;

#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	if (!TX0IQKOK)
		panic_printk("[IQK] please check S0 TXIQK\n");
	if (!RX0IQKOK)
		panic_printk("[IQK] please check S0 RXIQK\n");
	if (!TX1IQKOK)
		panic_printk("[IQK] please check S1 TXIQK\n");
	if (!RX1IQKOK)
		panic_printk("[IQK] please check S1 RXIQK\n");
#endif
}

#define MACBB_REG_NUM 9
#define AFE_REG_NUM 14
#define RF_REG_NUM 3

/*IQK: v1.1*/
/*1.remove 0x8c4 setting*/
/*2.add IQK debug message*/
void
_phy_iq_calibrate_8812a(
	struct PHY_DM_STRUCT		*p_dm_odm,
	u8		channel
)
{
	u32	MACBB_backup[MACBB_REG_NUM], AFE_backup[AFE_REG_NUM], RFA_backup[RF_REG_NUM], RFB_backup[RF_REG_NUM];
	u32	backup_macbb_reg[MACBB_REG_NUM] = {0x520, 0x550, 0x808, 0x838, 0x90c, 0xb00, 0xc00, 0xe00, 0x82c};
	u32	backup_afe_reg[AFE_REG_NUM] = {0xc5c, 0xc60, 0xc64, 0xc68, 0xcb8, 0xcb0, 0xcb4, 0xe5c, 0xe60, 0xe64,
					       0xe68, 0xeb8, 0xeb0, 0xeb4
					  };
	u32	backup_rf_reg[RF_REG_NUM] = {0x65, 0x8f, 0x0};
	u8	chnl_idx = odm_get_right_chnl_place_for_iqk(channel);

	_iqk_backup_mac_bb_8812a(p_dm_odm, MACBB_backup, backup_macbb_reg, MACBB_REG_NUM);
	_iqk_backup_afe_8812a(p_dm_odm, AFE_backup, backup_afe_reg, AFE_REG_NUM);
	_iqk_backup_rf_8812a(p_dm_odm, RFA_backup, RFB_backup, backup_rf_reg, RF_REG_NUM);

	_iqk_configure_mac_8812a(p_dm_odm);
	_iqk_tx_8812a(p_dm_odm, ODM_RF_PATH_A, chnl_idx);
	_iqk_restore_rf_8812a(p_dm_odm, ODM_RF_PATH_A, backup_rf_reg, RFA_backup, RF_REG_NUM);

	_iqk_tx_8812a(p_dm_odm, ODM_RF_PATH_B, chnl_idx);
	_iqk_restore_rf_8812a(p_dm_odm, ODM_RF_PATH_B, backup_rf_reg, RFB_backup, RF_REG_NUM);

	_iqk_restore_afe_8812a(p_dm_odm, AFE_backup, backup_afe_reg, AFE_REG_NUM);
	_iqk_restore_mac_bb_8812a(p_dm_odm, MACBB_backup, backup_macbb_reg, MACBB_REG_NUM);

	/* _IQK_Exit_8812A(p_dm_odm); */
	/* _IQK_TX_CheckResult_8812A */

}


void
_phy_lc_calibrate_8812a(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct PHY_DM_STRUCT		*p_dm_odm,
#else
	struct _ADAPTER	*p_adapter,
#endif
	boolean		is2T
)
{
	u8	tmp_reg;
	u32	rf_amode = 0, rf_bmode = 0, lc_cal;
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->odmpriv;
#endif
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->DM_OutSrc;
#endif
#endif
	/* Check continuous TX and Packet TX */
	tmp_reg = odm_read_1byte(p_dm_odm, 0xd03);

	if ((tmp_reg & 0x70) != 0)			/* Deal with contisuous TX case */
		odm_write_1byte(p_dm_odm, 0xd03, tmp_reg & 0x8F);	/* disable all continuous TX */
	else							/* Deal with Packet TX case */
		odm_write_1byte(p_dm_odm, REG_TXPAUSE, 0xFF);			/* block all queues */

	if ((tmp_reg & 0x70) != 0) {
		/* 1. Read original RF mode */
		/* path-A */
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
		rf_amode = phy_query_rf_reg(p_adapter, ODM_RF_PATH_A, RF_AC, MASK12BITS);

		/* path-B */
		if (is2T)
			rf_bmode = phy_query_rf_reg(p_adapter, ODM_RF_PATH_B, RF_AC, MASK12BITS);
#else
		rf_amode = odm_get_rf_reg(p_dm_odm, ODM_RF_PATH_A, RF_AC, MASK12BITS);

		/* path-B */
		if (is2T)
			rf_bmode = odm_get_rf_reg(p_dm_odm, ODM_RF_PATH_B, RF_AC, MASK12BITS);
#endif

		/* 2. Set RF mode = standby mode */
		/* path-A */
		odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, RF_AC, MASK12BITS, (rf_amode & 0x8FFFF) | 0x10000);

		/* path-B */
		if (is2T)
			odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_B, RF_AC, MASK12BITS, (rf_bmode & 0x8FFFF) | 0x10000);
	}

	/* 3. Read RF reg18 */
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	lc_cal = phy_query_rf_reg(p_adapter, ODM_RF_PATH_A, RF_CHNLBW, MASK12BITS);
#else
	lc_cal = odm_get_rf_reg(p_dm_odm, ODM_RF_PATH_A, RF_CHNLBW, MASK12BITS);
#endif

	/* 4. Set LC calibration begin	bit15 */
	odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, RF_CHNLBW, MASK12BITS, lc_cal | 0x08000);

	ODM_delay_ms(100);


	/* Restore original situation */
	if ((tmp_reg & 0x70) != 0) {	/* Deal with contisuous TX case */
		/* path-A */
		odm_write_1byte(p_dm_odm, 0xd03, tmp_reg);
		odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, RF_AC, MASK12BITS, rf_amode);

		/* path-B */
		if (is2T)
			odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_B, RF_AC, MASK12BITS, rf_bmode);
	} else /* Deal with Packet TX case */
		odm_write_1byte(p_dm_odm, REG_TXPAUSE, 0x00);
}

/* Analog Pre-distortion calibration */
#define		APK_BB_REG_NUM	8
#define		APK_CURVE_REG_NUM 4
#define		PATH_NUM		2

void
_phy_ap_calibrate_8812a(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct PHY_DM_STRUCT		*p_dm_odm,
#else
	struct _ADAPTER	*p_adapter,
#endif
	s8		delta,
	boolean		is2T
)
{
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->odmpriv;
#endif
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->DM_OutSrc;
#endif
#endif
	u32			reg_d[PATH_NUM];
	u32			tmp_reg, index, offset,  apkbound;
	u8			path, i, pathbound = PATH_NUM;
	u32			BB_backup[APK_BB_REG_NUM];
	u32			BB_REG[APK_BB_REG_NUM] = {
		REG_FPGA1_TX_BLOCK,	REG_OFDM_0_TRX_PATH_ENABLE,
		REG_FPGA0_RFMOD,	REG_OFDM_0_TR_MUX_PAR,
		REG_FPGA0_XCD_RF_INTERFACE_SW,	REG_FPGA0_XAB_RF_INTERFACE_SW,
		REG_FPGA0_XA_RF_INTERFACE_OE,	REG_FPGA0_XB_RF_INTERFACE_OE
	};
	u32			BB_AP_MODE[APK_BB_REG_NUM] = {
		0x00000020, 0x00a05430, 0x02040000,
		0x000800e4, 0x00204000
	};
	u32			BB_normal_AP_MODE[APK_BB_REG_NUM] = {
		0x00000020, 0x00a05430, 0x02040000,
		0x000800e4, 0x22204000
	};

	u32			AFE_backup[IQK_ADDA_REG_NUM];
	u32			AFE_REG[IQK_ADDA_REG_NUM] = {
		REG_FPGA0_XCD_SWITCH_CONTROL,	REG_BLUE_TOOTH,
		REG_RX_WAIT_CCA,		REG_TX_CCK_RFON,
		REG_TX_CCK_BBON,	REG_TX_OFDM_RFON,
		REG_TX_OFDM_BBON,	REG_TX_TO_RX,
		REG_TX_TO_TX,		REG_RX_CCK,
		REG_RX_OFDM,		REG_RX_WAIT_RIFS,
		REG_RX_TO_RX,		REG_STANDBY,
		REG_SLEEP,			REG_PMPD_ANAEN
	};

	u32			MAC_backup[IQK_MAC_REG_NUM];
	u32			MAC_REG[IQK_MAC_REG_NUM] = {
		REG_TXPAUSE,		REG_BCN_CTRL,
		REG_BCN_CTRL_1,	REG_GPIO_MUXCFG
	};

	u32			APK_RF_init_value[PATH_NUM][APK_BB_REG_NUM] = {
		{0x0852c, 0x1852c, 0x5852c, 0x1852c, 0x5852c},
		{0x2852e, 0x0852e, 0x3852e, 0x0852e, 0x0852e}
	};

	u32			APK_normal_RF_init_value[PATH_NUM][APK_BB_REG_NUM] = {
		{0x0852c, 0x0a52c, 0x3a52c, 0x5a52c, 0x5a52c},	/* path settings equal to path b settings */
		{0x0852c, 0x0a52c, 0x5a52c, 0x5a52c, 0x5a52c}
	};

	u32			APK_RF_value_0[PATH_NUM][APK_BB_REG_NUM] = {
		{0x52019, 0x52014, 0x52013, 0x5200f, 0x5208d},
		{0x5201a, 0x52019, 0x52016, 0x52033, 0x52050}
	};

	u32			APK_normal_RF_value_0[PATH_NUM][APK_BB_REG_NUM] = {
		{0x52019, 0x52017, 0x52010, 0x5200d, 0x5206a},	/* path settings equal to path b settings */
		{0x52019, 0x52017, 0x52010, 0x5200d, 0x5206a}
	};

	u32			AFE_on_off[PATH_NUM] = {
		0x04db25a4, 0x0b1b25a4
	};	/* path A on path B off / path A off path B on */

	u32			APK_offset[PATH_NUM] = {
		REG_CONFIG_ANT_A, REG_CONFIG_ANT_B
	};

	u32			APK_normal_offset[PATH_NUM] = {
		REG_CONFIG_PMPD_ANT_A, REG_CONFIG_PMPD_ANT_B
	};

	u32			APK_value[PATH_NUM] = {
		0x92fc0000, 0x12fc0000
	};

	u32			APK_normal_value[PATH_NUM] = {
		0x92680000, 0x12680000
	};

	s8			APK_delta_mapping[APK_BB_REG_NUM][13] = {
		{-4, -3, -2, -2, -1, -1, 0, 1, 2, 3, 4, 5, 6},
		{-4, -3, -2, -2, -1, -1, 0, 1, 2, 3, 4, 5, 6},
		{-6, -4, -2, -2, -1, -1, 0, 1, 2, 3, 4, 5, 6},
		{-1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6},
		{-11, -9, -7, -5, -3, -1, 0, 0, 0, 0, 0, 0, 0}
	};

	u32			APK_normal_setting_value_1[13] = {
		0x01017018, 0xf7ed8f84, 0x1b1a1816, 0x2522201e, 0x322e2b28,
		0x433f3a36, 0x5b544e49, 0x7b726a62, 0xa69a8f84, 0xdfcfc0b3,
		0x12680000, 0x00880000, 0x00880000
	};

	u32			APK_normal_setting_value_2[16] = {
		0x01c7021d, 0x01670183, 0x01000123, 0x00bf00e2, 0x008d00a3,
		0x0068007b, 0x004d0059, 0x003a0042, 0x002b0031, 0x001f0025,
		0x0017001b, 0x00110014, 0x000c000f, 0x0009000b, 0x00070008,
		0x00050006
	};

	u32			APK_result[PATH_NUM][APK_BB_REG_NUM];	/* val_1_1a, val_1_2a, val_2a, val_3a, val_4a */
	/*	u32			AP_curve[PATH_NUM][APK_CURVE_REG_NUM]; */

	s32			BB_offset, delta_V, delta_offset;

#if defined(MP_DRIVER) && (MP_DRIVER == 1)
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	PMPT_CONTEXT	p_mpt_ctx = &(p_adapter->mppriv.mpt_ctx);
#else
	PMPT_CONTEXT	p_mpt_ctx = &(p_adapter->mpt_ctx);
#endif
	p_mpt_ctx->APK_bound[0] = 45;
	p_mpt_ctx->APK_bound[1] = 52;

#endif

	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("==>_phy_ap_calibrate_8812a() delta %d\n", delta));
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("AP Calibration for %s\n", (is2T ? "2T2R" : "1T1R")));
	if (!is2T)
		pathbound = 1;

	/* 2 FOR NORMAL CHIP SETTINGS */

	/* Temporarily do not allow normal driver to do the following settings because these offset */
	/* and value will cause RF internal PA to be unpredictably disabled by HW, such that RF Tx signal */
	/* will disappear after disable/enable card many times on 88CU. RF SD and DD have not find the */
	/* root cause, so we remove these actions temporarily. Added by tynli and SD3 Allen. 2010.05.31. */
#if !defined(MP_DRIVER) || (MP_DRIVER != 1)
	return;
#endif
	/* settings adjust for normal chip */
	for (index = 0; index < PATH_NUM; index++) {
		APK_offset[index] = APK_normal_offset[index];
		APK_value[index] = APK_normal_value[index];
		AFE_on_off[index] = 0x6fdb25a4;
	}

	for (index = 0; index < APK_BB_REG_NUM; index++) {
		for (path = 0; path < pathbound; path++) {
			APK_RF_init_value[path][index] = APK_normal_RF_init_value[path][index];
			APK_RF_value_0[path][index] = APK_normal_RF_value_0[path][index];
		}
		BB_AP_MODE[index] = BB_normal_AP_MODE[index];
	}

	apkbound = 6;

	/* save BB default value */
	for (index = 0; index < APK_BB_REG_NUM ; index++) {
		if (index == 0)		/* skip */
			continue;
		BB_backup[index] = odm_get_bb_reg(p_dm_odm, BB_REG[index], MASKDWORD);
	}

	/* save MAC default value */
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	_phy_save_mac_registers_8812a(p_adapter, MAC_REG, MAC_backup);

	/* save AFE default value */
	_phy_save_adda_registers_8812a(p_adapter, AFE_REG, AFE_backup, IQK_ADDA_REG_NUM);
#else
	_phy_save_mac_registers_8812a(p_dm_odm, MAC_REG, MAC_backup);

	/* save AFE default value */
	_phy_save_adda_registers_8812a(p_dm_odm, AFE_REG, AFE_backup, IQK_ADDA_REG_NUM);
#endif

	for (path = 0; path < pathbound; path++) {


		if (path == ODM_RF_PATH_A) {
			/* path A APK */
			/* load APK setting */
			/* path-A */
			offset = REG_PDP_ANT_A;
			for (index = 0; index < 11; index++) {
				odm_set_bb_reg(p_dm_odm, offset, MASKDWORD, APK_normal_setting_value_1[index]);
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("_phy_ap_calibrate_8812a() offset 0x%x value 0x%x\n", offset, odm_get_bb_reg(p_dm_odm, offset, MASKDWORD)));

				offset += 0x04;
			}

			odm_set_bb_reg(p_dm_odm, REG_CONFIG_PMPD_ANT_B, MASKDWORD, 0x12680000);

			offset = REG_CONFIG_ANT_A;
			for (; index < 13; index++) {
				odm_set_bb_reg(p_dm_odm, offset, MASKDWORD, APK_normal_setting_value_1[index]);
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("_phy_ap_calibrate_8812a() offset 0x%x value 0x%x\n", offset, odm_get_bb_reg(p_dm_odm, offset, MASKDWORD)));

				offset += 0x04;
			}

			/* page-B1 */
			odm_set_bb_reg(p_dm_odm, REG_FPGA0_IQK, 0xffffff00, 0x400000);

			/* path A */
			offset = REG_PDP_ANT_A;
			for (index = 0; index < 16; index++) {
				odm_set_bb_reg(p_dm_odm, offset, MASKDWORD, APK_normal_setting_value_2[index]);
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("_phy_ap_calibrate_8812a() offset 0x%x value 0x%x\n", offset, odm_get_bb_reg(p_dm_odm, offset, MASKDWORD)));

				offset += 0x04;
			}
			odm_set_bb_reg(p_dm_odm,  REG_FPGA0_IQK, 0xffffff00, 0);
		} else if (path == ODM_RF_PATH_B) {
			/* path B APK */
			/* load APK setting */
			/* path-B */
			offset = REG_PDP_ANT_B;
			for (index = 0; index < 10; index++) {
				odm_set_bb_reg(p_dm_odm, offset, MASKDWORD, APK_normal_setting_value_1[index]);
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("_phy_ap_calibrate_8812a() offset 0x%x value 0x%x\n", offset, odm_get_bb_reg(p_dm_odm, offset, MASKDWORD)));

				offset += 0x04;
			}
			odm_set_bb_reg(p_dm_odm, REG_CONFIG_PMPD_ANT_A, MASKDWORD, 0x12680000);
			odm_set_bb_reg(p_dm_odm, REG_CONFIG_PMPD_ANT_B, MASKDWORD, 0x12680000);

			offset = REG_CONFIG_ANT_A;
			index = 11;
			for (; index < 13; index++) { /* offset 0xb68, 0xb6c */
				odm_set_bb_reg(p_dm_odm, offset, MASKDWORD, APK_normal_setting_value_1[index]);
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("_phy_ap_calibrate_8812a() offset 0x%x value 0x%x\n", offset, odm_get_bb_reg(p_dm_odm, offset, MASKDWORD)));

				offset += 0x04;
			}

			/* page-B1 */
			odm_set_bb_reg(p_dm_odm, REG_FPGA0_IQK, 0xffffff00, 0x400000);

			/* path B */
			offset = 0xb60;
			for (index = 0; index < 16; index++) {
				odm_set_bb_reg(p_dm_odm, offset, MASKDWORD, APK_normal_setting_value_2[index]);
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("_phy_ap_calibrate_8812a() offset 0x%x value 0x%x\n", offset, odm_get_bb_reg(p_dm_odm, offset, MASKDWORD)));

				offset += 0x04;
			}
			odm_set_bb_reg(p_dm_odm, REG_FPGA0_IQK, 0xffffff00, 0);
		}

		/* save RF default value */
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
		reg_d[path] = phy_query_rf_reg(p_adapter, path, RF_TXBIAS_A, MASKDWORD);
#else
		reg_d[path] = odm_get_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, RF_TXBIAS_A, MASKDWORD);
#endif

		/* path A AFE all on, path B AFE All off or vise versa */
		for (index = 0; index < IQK_ADDA_REG_NUM ; index++)
			odm_set_bb_reg(p_dm_odm, AFE_REG[index], MASKDWORD, AFE_on_off[path]);
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("_phy_ap_calibrate_8812a() offset 0xe70 %x\n", odm_get_bb_reg(p_dm_odm, REG_RX_WAIT_CCA, MASKDWORD)));

		/* BB to AP mode */
		if (path == 0) {
			for (index = 0; index < APK_BB_REG_NUM ; index++) {

				if (index == 0)		/* skip */
					continue;
				else if (index < 5)
					odm_set_bb_reg(p_dm_odm, BB_REG[index], MASKDWORD, BB_AP_MODE[index]);
				else if (BB_REG[index] == 0x870)
					odm_set_bb_reg(p_dm_odm, BB_REG[index], MASKDWORD, BB_backup[index] | BIT(10) | BIT(26));
				else
					odm_set_bb_reg(p_dm_odm, BB_REG[index], BIT(10), 0x0);
			}

			odm_set_bb_reg(p_dm_odm, REG_TX_IQK_TONE_A, MASKDWORD, 0x01008c00);
			odm_set_bb_reg(p_dm_odm, REG_RX_IQK_TONE_A, MASKDWORD, 0x01008c00);
		} else {	/* path B */
			odm_set_bb_reg(p_dm_odm, REG_TX_IQK_TONE_B, MASKDWORD, 0x01008c00);
			odm_set_bb_reg(p_dm_odm, REG_RX_IQK_TONE_B, MASKDWORD, 0x01008c00);

		}

		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("_phy_ap_calibrate_8812a() offset 0x800 %x\n", odm_get_bb_reg(p_dm_odm, 0x800, MASKDWORD)));

		/* MAC settings */
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
		_phy_mac_setting_calibration_8812a(p_adapter, MAC_REG, MAC_backup);
#else
		_phy_mac_setting_calibration_8812a(p_dm_odm, MAC_REG, MAC_backup);
#endif

		if (path == ODM_RF_PATH_A)
			odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_B, RF_AC, MASKDWORD, 0x10000);
		else {
			odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, RF_AC, MASKDWORD, 0x10000);
			odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, RF_MODE1, MASKDWORD, 0x1000f);
			odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, RF_MODE2, MASKDWORD, 0x20103);
		}

		delta_offset = ((delta + 14) / 2);
		if (delta_offset < 0)
			delta_offset = 0;
		else if (delta_offset > 12)
			delta_offset = 12;

		/* AP calibration */
		for (index = 0; index < APK_BB_REG_NUM; index++) {
			if (index != 1)	/* only DO PA11+PAD01001, AP RF setting */
				continue;

			tmp_reg = APK_RF_init_value[path][index];
#if 1
			if (!p_dm_odm->rf_calibrate_info.is_apk_thermal_meter_ignore) {
				BB_offset = (tmp_reg & 0xF0000) >> 16;

				if (!(tmp_reg & BIT(15))) /* sign bit 0 */
					BB_offset = -BB_offset;

				delta_V = APK_delta_mapping[index][delta_offset];

				BB_offset += delta_V;

				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("_phy_ap_calibrate_8812a() APK index %d tmp_reg 0x%x delta_V %d delta_offset %d\n", index, tmp_reg, (int)delta_V, (int)delta_offset));

				if (BB_offset < 0) {
					tmp_reg = tmp_reg & (~BIT(15));
					BB_offset = -BB_offset;
				} else
					tmp_reg = tmp_reg | BIT(15);
				tmp_reg = (tmp_reg & 0xFFF0FFFF) | (BB_offset << 16);
			}
#endif

			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, RF_IPA_A, MASKDWORD, 0x8992e);
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("_phy_ap_calibrate_8812a() offset 0xc %x\n", phy_query_rf_reg(p_adapter, path, RF_IPA_A, MASKDWORD)));
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, RF_AC, MASKDWORD, APK_RF_value_0[path][index]);
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("_phy_ap_calibrate_8812a() offset 0x0 %x\n", phy_query_rf_reg(p_adapter, path, RF_AC, MASKDWORD)));
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, RF_TXBIAS_A, MASKDWORD, tmp_reg);
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("_phy_ap_calibrate_8812a() offset 0xd %x\n", phy_query_rf_reg(p_adapter, path, RF_TXBIAS_A, MASKDWORD)));
#else
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("_phy_ap_calibrate_8812a() offset 0xc %x\n", odm_get_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, RF_IPA_A, MASKDWORD)));
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, RF_AC, MASKDWORD, APK_RF_value_0[path][index]);
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("_phy_ap_calibrate_8812a() offset 0x0 %x\n", odm_get_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, RF_AC, MASKDWORD)));
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, RF_TXBIAS_A, MASKDWORD, tmp_reg);
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("_phy_ap_calibrate_8812a() offset 0xd %x\n", odm_get_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, RF_TXBIAS_A, MASKDWORD)));
#endif

			/* PA11+PAD01111, one shot */
			i = 0;
			do {
				odm_set_bb_reg(p_dm_odm, REG_FPGA0_IQK, 0xffffff00, 0x800000);
				{
					odm_set_bb_reg(p_dm_odm, APK_offset[path], MASKDWORD, APK_value[0]);
					ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("_phy_ap_calibrate_8812a() offset 0x%x value 0x%x\n", APK_offset[path], odm_get_bb_reg(p_dm_odm, APK_offset[path], MASKDWORD)));
					ODM_delay_ms(3);
					odm_set_bb_reg(p_dm_odm, APK_offset[path], MASKDWORD, APK_value[1]);
					ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("_phy_ap_calibrate_8812a() offset 0x%x value 0x%x\n", APK_offset[path], odm_get_bb_reg(p_dm_odm, APK_offset[path], MASKDWORD)));

					ODM_delay_ms(20);
				}
				odm_set_bb_reg(p_dm_odm, REG_FPGA0_IQK, 0xffffff00, 0);

				if (path == ODM_RF_PATH_A)
					tmp_reg = odm_get_bb_reg(p_dm_odm, REG_APK, 0x03E00000);
				else
					tmp_reg = odm_get_bb_reg(p_dm_odm, REG_APK, 0xF8000000);
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("_phy_ap_calibrate_8812a() offset 0xbd8[25:21] %x\n", tmp_reg));


				i++;
			} while (tmp_reg > apkbound && i < 4);

			APK_result[path][index] = tmp_reg;
		}
	}

	/* reload MAC default value */
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	_phy_reload_mac_registers_8812a(p_adapter, MAC_REG, MAC_backup);
#else
	_phy_reload_mac_registers_8812a(p_dm_odm, MAC_REG, MAC_backup);
#endif

	/* reload BB default value */
	for (index = 0; index < APK_BB_REG_NUM ; index++) {

		if (index == 0)		/* skip */
			continue;
		odm_set_bb_reg(p_dm_odm, BB_REG[index], MASKDWORD, BB_backup[index]);
	}

	/* reload AFE default value */
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	_phy_reload_adda_registers_8812a(p_adapter, AFE_REG, AFE_backup, IQK_ADDA_REG_NUM);
#else
	_phy_reload_adda_registers_8812a(p_dm_odm, AFE_REG, AFE_backup, IQK_ADDA_REG_NUM);
#endif

	/* reload RF path default value */
	for (path = 0; path < pathbound; path++) {
		odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0xd, MASKDWORD, reg_d[path]);
		if (path == ODM_RF_PATH_B) {
			odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, RF_MODE1, MASKDWORD, 0x1000f);
			odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, RF_MODE2, MASKDWORD, 0x20101);
		}

		/* note no index == 0 */
		if (APK_result[path][1] > 6)
			APK_result[path][1] = 6;
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("apk path %d result %d 0x%x \t", path, 1, APK_result[path][1]));
	}

	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("\n"));


	for (path = 0; path < pathbound; path++) {
		odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x3, MASKDWORD,
			((APK_result[path][1] << 15) | (APK_result[path][1] << 10) | (APK_result[path][1] << 5) | APK_result[path][1]));
		if (path == ODM_RF_PATH_A)
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x4, MASKDWORD,
				((APK_result[path][1] << 15) | (APK_result[path][1] << 10) | (0x00 << 5) | 0x05));
		else
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x4, MASKDWORD,
				((APK_result[path][1] << 15) | (APK_result[path][1] << 10) | (0x02 << 5) | 0x05));
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
		odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, RF_BS_PA_APSET_G9_G11, MASKDWORD,
			((0x08 << 15) | (0x08 << 10) | (0x08 << 5) | 0x08));
#endif
	}

	p_dm_odm->rf_calibrate_info.is_ap_kdone = true;

	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("<==_phy_ap_calibrate_8812a()\n"));
}



#define		DP_BB_REG_NUM		7
#define		DP_RF_REG_NUM		1
#define		DP_RETRY_LIMIT		10
#define		DP_PATH_NUM		2
#define		DP_DPK_NUM			3
#define		DP_DPK_VALUE_NUM	2


#if 0 /* FOR_8812_IQK */
void
phy_iq_calibrate_8812a(
	struct _ADAPTER	*p_adapter,
	boolean	is_recovery
)
{

#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->DM_OutSrc;
#else  /* (DM_ODM_SUPPORT_TYPE == ODM_CE) */
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->odmpriv;
#endif
#endif
#if (MP_DRIVER == 1)
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PMPT_CONTEXT	p_mpt_ctx = &(p_adapter->mpt_ctx);
#else/* (DM_ODM_SUPPORT_TYPE == ODM_CE) */
	PMPT_CONTEXT	p_mpt_ctx = &(p_adapter->mppriv.mpt_ctx);
#endif
#endif/* (MP_DRIVER == 1) */

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
	if (odm_check_power_status(p_adapter) == false)
		return;
#endif

#if MP_DRIVER == 1
	if (!(p_mpt_ctx->is_single_tone || p_mpt_ctx->is_carrier_suppression))
#endif
		_phy_iq_calibrate_8812a(p_dm_odm);


}
#endif

void
phy_lc_calibrate_8812a(
	struct PHY_DM_STRUCT		*p_dm_odm
)
{
#if 0
	boolean		is_start_cont_tx = false, is_single_tone = false, is_carrier_suppression = false;
	u32			timeout = 2000, timecount = 0;

#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->DM_OutSrc;
#else  /* (DM_ODM_SUPPORT_TYPE == ODM_CE) */
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->odmpriv;
#endif

#if (MP_DRIVER == 1)
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PMPT_CONTEXT	p_mpt_ctx = &(p_adapter->mpt_ctx);
#else/* (DM_ODM_SUPPORT_TYPE == ODM_CE) */
	PMPT_CONTEXT	p_mpt_ctx = &(p_adapter->mppriv.mpt_ctx);
#endif
#endif/* (MP_DRIVER == 1) */
#endif




#if MP_DRIVER == 1
	is_start_cont_tx = p_mpt_ctx->is_start_cont_tx;
	is_single_tone = p_mpt_ctx->is_single_tone;
	is_carrier_suppression = p_mpt_ctx->is_carrier_suppression;
#endif


#ifdef DISABLE_BB_RF
	return;
#endif

#if (DM_ODM_SUPPORT_TYPE & (ODM_CE | ODM_AP))
	if (!(p_dm_odm->support_ability & ODM_RF_CALIBRATION))
		return;
#endif
	/* 20120213<Kordan> Turn on when continuous Tx to pass lab testing. (required by Edlu) */
	if (is_single_tone || is_carrier_suppression)
		return;

	while (*(p_dm_odm->p_is_scan_in_process) && timecount < timeout) {
		ODM_delay_ms(50);
		timecount += 50;
	}

	p_dm_odm->rf_calibrate_info.is_lck_in_progress = true;

	/* ODM_RT_TRACE(p_dm_odm,ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("LCK:Start!!!interface %d currentband %x delay %d ms\n", p_dm_odm->interface_index, p_hal_data->CurrentBandType92D, timecount)); */
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)

	if (IS_2T2R(p_hal_data->version_id))
		_phy_lc_calibrate_8812a(p_adapter, true);
	else
#endif
	{
		/* For 88C 1T1R */
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
		_phy_lc_calibrate_8812a(p_adapter, false);
#else
		_phy_lc_calibrate_8812a(p_dm_odm, false);
#endif
	}

	p_dm_odm->rf_calibrate_info.is_lck_in_progress = false;

	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("LCK:Finish!!!interface %d\n", p_dm_odm->interface_index));
#endif
}

void
phy_ap_calibrate_8812a(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct PHY_DM_STRUCT		*p_dm_odm,
#else
	struct _ADAPTER	*p_adapter,
#endif
	s8		delta
)
{
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->odmpriv;
#endif
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->DM_OutSrc;
#endif
#endif
#ifdef DISABLE_BB_RF
	return;
#endif

	return;
#if (DM_ODM_SUPPORT_TYPE & (ODM_CE | ODM_AP))
	if (!(p_dm_odm->support_ability & ODM_RF_CALIBRATION))
		return;
#endif

#if defined(FOR_BRAZIL_PRETEST) && (FOR_BRAZIL_PRETEST != 1)
	if (p_dm_odm->rf_calibrate_info.is_ap_kdone)
#endif
		return;

#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	if (IS_92C_SERIAL(p_hal_data->version_id))
		_phy_ap_calibrate_8812a(p_adapter, delta, true);
	else
#endif
	{
		/* For 88C 1T1R */
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
		_phy_ap_calibrate_8812a(p_adapter, delta, false);
#else
		_phy_ap_calibrate_8812a(p_dm_odm, delta, false);
#endif
	}
}
void _phy_set_rf_path_switch_8812a(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct PHY_DM_STRUCT		*p_dm_odm,
#else
	struct _ADAPTER	*p_adapter,
#endif
	boolean		is_main,
	boolean		is2T
)
{
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->odmpriv;
#elif (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->DM_OutSrc;
#endif

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	if (!p_adapter->is_hw_init_ready)
#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
	if (p_adapter->hw_init_completed == _FALSE)
#endif
	{
		u8	u1b_tmp;
		u1b_tmp = odm_read_1byte(p_dm_odm, REG_LEDCFG2) | BIT(7);
		odm_write_1byte(p_dm_odm, REG_LEDCFG2, u1b_tmp);
		/* odm_set_bb_reg(p_dm_odm, REG_LEDCFG0, BIT23, 0x01); */
		odm_set_bb_reg(p_dm_odm, REG_FPGA0_XAB_RF_PARAMETER, BIT(13), 0x01);
	}

#endif

	if (is2T) {	/* 92C */
		if (is_main)
			odm_set_bb_reg(p_dm_odm, REG_FPGA0_XB_RF_INTERFACE_OE, BIT(5) | BIT6, 0x1);	/* 92C_Path_A */
		else
			odm_set_bb_reg(p_dm_odm, REG_FPGA0_XB_RF_INTERFACE_OE, BIT(5) | BIT6, 0x2);	/* BT */
	} else {		/* 88C */

		if (is_main)
			odm_set_bb_reg(p_dm_odm, REG_FPGA0_XA_RF_INTERFACE_OE, BIT(8) | BIT9, 0x2);	/* Main */
		else
			odm_set_bb_reg(p_dm_odm, REG_FPGA0_XA_RF_INTERFACE_OE, BIT(8) | BIT9, 0x1);	/* Aux */
	}
}
void phy_set_rf_path_switch_8812a(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct PHY_DM_STRUCT		*p_dm_odm,
#else
	struct _ADAPTER	*p_adapter,
#endif
	boolean		is_main
)
{
	/* HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter); */

#ifdef DISABLE_BB_RF
	return;
#endif

#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	if (IS_92C_SERIAL(p_hal_data->version_id))
		_phy_set_rf_path_switch_8812a(p_adapter, is_main, true);
	else
#endif
	{
		/* For 88C 1T1R */
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
		_phy_set_rf_path_switch_8812a(p_adapter, is_main, false);
#else
		_phy_set_rf_path_switch_8812a(p_dm_odm, is_main, false);
#endif
	}
}

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
/* digital predistortion */
void
_phy_digital_predistortion_8812a(
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct _ADAPTER	*p_adapter,
#else
	struct PHY_DM_STRUCT	*p_dm_odm,
#endif
	boolean		is2T
)
{
#if (RT_PLATFORM == PLATFORM_WINDOWS)
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->odmpriv;
#endif
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->DM_OutSrc;
#endif
#endif

	u32			tmp_reg, tmp_reg2, index,  i;
	u8			path, pathbound = PATH_NUM;
	u32			AFE_backup[IQK_ADDA_REG_NUM];
	u32			AFE_REG[IQK_ADDA_REG_NUM] = {
		REG_FPGA0_XCD_SWITCH_CONTROL,	REG_BLUE_TOOTH,
		REG_RX_WAIT_CCA,		REG_TX_CCK_RFON,
		REG_TX_CCK_BBON,	REG_TX_OFDM_RFON,
		REG_TX_OFDM_BBON,	REG_TX_TO_RX,
		REG_TX_TO_TX,		REG_RX_CCK,
		REG_RX_OFDM,		REG_RX_WAIT_RIFS,
		REG_RX_TO_RX,		REG_STANDBY,
		REG_SLEEP,			REG_PMPD_ANAEN
	};

	u32			BB_backup[DP_BB_REG_NUM];
	u32			BB_REG[DP_BB_REG_NUM] = {
		REG_OFDM_0_TRX_PATH_ENABLE, REG_FPGA0_RFMOD,
		REG_OFDM_0_TR_MUX_PAR,	REG_FPGA0_XCD_RF_INTERFACE_SW,
		REG_FPGA0_XAB_RF_INTERFACE_SW, REG_FPGA0_XA_RF_INTERFACE_OE,
		REG_FPGA0_XB_RF_INTERFACE_OE
	};
	u32			BB_settings[DP_BB_REG_NUM] = {
		0x00a05430, 0x02040000, 0x000800e4, 0x22208000,
		0x0, 0x0, 0x0
	};

	u32			RF_backup[DP_PATH_NUM][DP_RF_REG_NUM];
	u32			RF_REG[DP_RF_REG_NUM] = {
		RF_TXBIAS_A
	};

	u32			MAC_backup[IQK_MAC_REG_NUM];
	u32			MAC_REG[IQK_MAC_REG_NUM] = {
		REG_TXPAUSE,		REG_BCN_CTRL,
		REG_BCN_CTRL_1,	REG_GPIO_MUXCFG
	};

	u32			tx_agc[DP_DPK_NUM][DP_DPK_VALUE_NUM] = {
		{0x1e1e1e1e, 0x03901e1e},
		{0x18181818, 0x03901818},
		{0x0e0e0e0e, 0x03900e0e}
	};

	u32			AFE_on_off[PATH_NUM] = {
		0x04db25a4, 0x0b1b25a4
	};	/* path A on path B off / path A off path B on */

	u8			retry_count = 0;


	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("==>_phy_digital_predistortion_8812a()\n"));

	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("_phy_digital_predistortion_8812a for %s %s\n", (is2T ? "2T2R" : "1T1R")));

	/* save BB default value */
	for (index = 0; index < DP_BB_REG_NUM; index++)
		BB_backup[index] = odm_get_bb_reg(p_dm_odm, BB_REG[index], MASKDWORD);

	/* save MAC default value */
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	_phy_save_mac_registers_8812a(p_adapter, BB_REG, MAC_backup);
#else
	_phy_save_mac_registers_8812a(p_dm_odm, BB_REG, MAC_backup);
#endif

	/* save RF default value */
	for (path = 0; path < DP_PATH_NUM; path++) {
		for (index = 0; index < DP_RF_REG_NUM; index++)
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
			RF_backup[path][index] = phy_query_rf_reg(p_adapter, path, RF_REG[index], MASKDWORD);
#else
			RF_backup[path][index] = odm_get_rf_reg(p_adapter, (enum odm_rf_radio_path_e)path, RF_REG[index], MASKDWORD);
#endif
	}

	/* save AFE default value */
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	_phy_save_adda_registers_8812a(p_adapter, AFE_REG, AFE_backup, IQK_ADDA_REG_NUM);
#else
	RF_backup[path][index] = odm_get_rf_reg(p_adapter, (enum odm_rf_radio_path_e)path, RF_REG[index], MASKDWORD);
#endif

	/* path A/B AFE all on */
	for (index = 0; index < IQK_ADDA_REG_NUM ; index++)
		odm_set_bb_reg(p_dm_odm, AFE_REG[index], MASKDWORD, 0x6fdb25a4);

	/* BB register setting */
	for (index = 0; index < DP_BB_REG_NUM; index++) {
		if (index < 4)
			odm_set_bb_reg(p_dm_odm, BB_REG[index], MASKDWORD, BB_settings[index]);
		else if (index == 4)
			odm_set_bb_reg(p_dm_odm, BB_REG[index], MASKDWORD, BB_backup[index] | BIT(10) | BIT(26));
		else
			odm_set_bb_reg(p_dm_odm, BB_REG[index], BIT(10), 0x00);
	}

	/* MAC register setting */
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	_phy_mac_setting_calibration_8812a(p_adapter, MAC_REG, MAC_backup);
#else
	_phy_mac_setting_calibration_8812a(p_dm_odm, MAC_REG, MAC_backup);
#endif

	/* PAGE-E IQC setting */
	odm_set_bb_reg(p_dm_odm, REG_TX_IQK_TONE_A, MASKDWORD, 0x01008c00);
	odm_set_bb_reg(p_dm_odm, REG_RX_IQK_TONE_A, MASKDWORD, 0x01008c00);
	odm_set_bb_reg(p_dm_odm, REG_TX_IQK_TONE_B, MASKDWORD, 0x01008c00);
	odm_set_bb_reg(p_dm_odm, REG_RX_IQK_TONE_B, MASKDWORD, 0x01008c00);

	/* path_A DPK */
	/* path B to standby mode */
	odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_B, RF_AC, MASKDWORD, 0x10000);

	/* PA gain = 11 & PAD1 => tx_agc 1f ~11 */
	/* PA gain = 11 & PAD2 => tx_agc 10~0e */
	/* PA gain = 01 => tx_agc 0b~0d */
	/* PA gain = 00 => tx_agc 0a~00 */
	odm_set_bb_reg(p_dm_odm, REG_FPGA0_IQK, 0xffffff00, 0x400000);
	odm_set_bb_reg(p_dm_odm, 0xbc0, MASKDWORD, 0x0005361f);
	odm_set_bb_reg(p_dm_odm, REG_FPGA0_IQK, 0xffffff00, 0);

	/* do inner loopback DPK 3 times */
	for (i = 0; i < 3; i++) {
		/* PA gain = 11 & PAD2 => tx_agc = 0x0f/0x0c/0x07 */
		for (index = 0; index < 3; index++)
			odm_set_bb_reg(p_dm_odm, 0xe00 + index * 4, MASKDWORD, tx_agc[i][0]);
		odm_set_bb_reg(p_dm_odm, 0xe00 + index * 4, MASKDWORD, tx_agc[i][1]);
		for (index = 0; index < 4; index++)
			odm_set_bb_reg(p_dm_odm, 0xe10 + index * 4, MASKDWORD, tx_agc[i][0]);

		/* PAGE_B for path-A inner loopback DPK setting */
		odm_set_bb_reg(p_dm_odm, REG_PDP_ANT_A, MASKDWORD, 0x02097098);
		odm_set_bb_reg(p_dm_odm, REG_PDP_ANT_A_4, MASKDWORD, 0xf76d9f84);
		odm_set_bb_reg(p_dm_odm, REG_CONFIG_PMPD_ANT_A, MASKDWORD, 0x0004ab87);
		odm_set_bb_reg(p_dm_odm, REG_CONFIG_ANT_A, MASKDWORD, 0x00880000);

		/* ----send one shot signal---- */
		/* path A */
		odm_set_bb_reg(p_dm_odm, REG_CONFIG_PMPD_ANT_A, MASKDWORD, 0x80047788);
		ODM_delay_ms(1);
		odm_set_bb_reg(p_dm_odm, REG_CONFIG_PMPD_ANT_A, MASKDWORD, 0x00047788);
		ODM_delay_ms(50);
	}

	/* PA gain = 11 => tx_agc = 1a */
	for (index = 0; index < 3; index++)
		odm_set_bb_reg(p_dm_odm, 0xe00 + index * 4, MASKDWORD, 0x34343434);
	odm_set_bb_reg(p_dm_odm, 0xe08 + index * 4, MASKDWORD, 0x03903434);
	for (index = 0; index < 4; index++)
		odm_set_bb_reg(p_dm_odm, 0xe10 + index * 4, MASKDWORD, 0x34343434);

	/* ==================================== */
	/* PAGE_B for path-A DPK setting */
	/* ==================================== */
	/* open inner loopback @ b00[19]:10 od 0xb00 0x01097018 */
	odm_set_bb_reg(p_dm_odm, REG_PDP_ANT_A, MASKDWORD, 0x02017098);
	odm_set_bb_reg(p_dm_odm, REG_PDP_ANT_A_4, MASKDWORD, 0xf76d9f84);
	odm_set_bb_reg(p_dm_odm, REG_CONFIG_PMPD_ANT_A, MASKDWORD, 0x0004ab87);
	odm_set_bb_reg(p_dm_odm, REG_CONFIG_ANT_A, MASKDWORD, 0x00880000);

	/* rf_lpbk_setup */
	/* 1.rf 00:5205a, rf 0d:0e52c */
	odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, 0x0c, MASKDWORD, 0x8992b);
	odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, 0x0d, MASKDWORD, 0x0e52c);
	odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, 0x00, MASKDWORD, 0x5205a);

	/* ----send one shot signal---- */
	/* path A */
	odm_set_bb_reg(p_dm_odm, REG_CONFIG_PMPD_ANT_A, MASKDWORD, 0x800477c0);
	ODM_delay_ms(1);
	odm_set_bb_reg(p_dm_odm, REG_CONFIG_PMPD_ANT_A, MASKDWORD, 0x000477c0);
	ODM_delay_ms(50);

	while (retry_count < DP_RETRY_LIMIT && !p_dm_odm->rf_calibrate_info.is_dp_path_aok) {
		/* ----read back measurement results---- */
		odm_set_bb_reg(p_dm_odm, REG_PDP_ANT_A, MASKDWORD, 0x0c297018);
		tmp_reg = odm_get_bb_reg(p_dm_odm, 0xbe0, MASKDWORD);
		ODM_delay_ms(10);
		odm_set_bb_reg(p_dm_odm, REG_PDP_ANT_A, MASKDWORD, 0x0c29701f);
		tmp_reg2 = odm_get_bb_reg(p_dm_odm, 0xbe8, MASKDWORD);
		ODM_delay_ms(10);

		tmp_reg = (tmp_reg & MASKHWORD) >> 16;
		tmp_reg2 = (tmp_reg2 & MASKHWORD) >> 16;
		if (tmp_reg < 0xf0 || tmp_reg > 0x105 || tmp_reg2 > 0xff) {
			odm_set_bb_reg(p_dm_odm, REG_PDP_ANT_A, MASKDWORD, 0x02017098);

			odm_set_bb_reg(p_dm_odm, REG_FPGA0_IQK, 0xffffff00, 0x800000);
			odm_set_bb_reg(p_dm_odm, REG_FPGA0_IQK, 0xffffff00, 0);
			ODM_delay_ms(1);
			odm_set_bb_reg(p_dm_odm, REG_CONFIG_PMPD_ANT_A, MASKDWORD, 0x800477c0);
			ODM_delay_ms(1);
			odm_set_bb_reg(p_dm_odm, REG_CONFIG_PMPD_ANT_A, MASKDWORD, 0x000477c0);
			ODM_delay_ms(50);
			retry_count++;
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("path A DPK retry_count %d 0xbe0[31:16] %x 0xbe8[31:16] %x\n", retry_count, tmp_reg, tmp_reg2));
		} else {
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("path A DPK Sucess\n"));
			p_dm_odm->rf_calibrate_info.is_dp_path_aok = true;
			break;
		}
	}
	retry_count = 0;

	/* DPP path A */
	if (p_dm_odm->rf_calibrate_info.is_dp_path_aok) {
		/* DP settings */
		odm_set_bb_reg(p_dm_odm, REG_PDP_ANT_A, MASKDWORD, 0x01017098);
		odm_set_bb_reg(p_dm_odm, REG_PDP_ANT_A_4, MASKDWORD, 0x776d9f84);
		odm_set_bb_reg(p_dm_odm, REG_CONFIG_PMPD_ANT_A, MASKDWORD, 0x0004ab87);
		odm_set_bb_reg(p_dm_odm, REG_CONFIG_ANT_A, MASKDWORD, 0x00880000);
		odm_set_bb_reg(p_dm_odm, REG_FPGA0_IQK, 0xffffff00, 0x400000);

		for (i = REG_PDP_ANT_A; i <= 0xb3c; i += 4) {
			odm_set_bb_reg(p_dm_odm, i, MASKDWORD, 0x40004000);
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("path A ofsset = 0x%x\n", i));
		}

		/* pwsf */
		odm_set_bb_reg(p_dm_odm, 0xb40, MASKDWORD, 0x40404040);
		odm_set_bb_reg(p_dm_odm, 0xb44, MASKDWORD, 0x28324040);
		odm_set_bb_reg(p_dm_odm, 0xb48, MASKDWORD, 0x10141920);

		for (i = 0xb4c; i <= 0xb5c; i += 4)
			odm_set_bb_reg(p_dm_odm, i, MASKDWORD, 0x0c0c0c0c);

		/* TX_AGC boundary */
		odm_set_bb_reg(p_dm_odm, 0xbc0, MASKDWORD, 0x0005361f);
		odm_set_bb_reg(p_dm_odm, REG_FPGA0_IQK, 0xffffff00, 0);
	} else {
		odm_set_bb_reg(p_dm_odm, REG_PDP_ANT_A, MASKDWORD, 0x00000000);
		odm_set_bb_reg(p_dm_odm, REG_PDP_ANT_A_4, MASKDWORD, 0x00000000);
	}

	/* DPK path B */
	if (is2T) {
		/* path A to standby mode */
		odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, RF_AC, MASKDWORD, 0x10000);

		/* LUTs => tx_agc */
		/* PA gain = 11 & PAD1, => tx_agc 1f ~11 */
		/* PA gain = 11 & PAD2, => tx_agc 10 ~0e */
		/* PA gain = 01 => tx_agc 0b ~0d */
		/* PA gain = 00 => tx_agc 0a ~00 */
		odm_set_bb_reg(p_dm_odm, REG_FPGA0_IQK, 0xffffff00, 0x400000);
		odm_set_bb_reg(p_dm_odm, 0xbc4, MASKDWORD, 0x0005361f);
		odm_set_bb_reg(p_dm_odm, REG_FPGA0_IQK, 0xffffff00, 0);

		/* do inner loopback DPK 3 times */
		for (i = 0; i < 3; i++) {
			/* PA gain = 11 & PAD2 => tx_agc = 0x0f/0x0c/0x07 */
			for (index = 0; index < 4; index++)
				odm_set_bb_reg(p_dm_odm, 0x830 + index * 4, MASKDWORD, tx_agc[i][0]);
			for (index = 0; index < 2; index++)
				odm_set_bb_reg(p_dm_odm, 0x848 + index * 4, MASKDWORD, tx_agc[i][0]);
			for (index = 0; index < 2; index++)
				odm_set_bb_reg(p_dm_odm, 0x868 + index * 4, MASKDWORD, tx_agc[i][0]);

			/* PAGE_B for path-A inner loopback DPK setting */
			odm_set_bb_reg(p_dm_odm, REG_PDP_ANT_B, MASKDWORD, 0x02097098);
			odm_set_bb_reg(p_dm_odm, REG_PDP_ANT_B_4, MASKDWORD, 0xf76d9f84);
			odm_set_bb_reg(p_dm_odm, REG_CONFIG_PMPD_ANT_B, MASKDWORD, 0x0004ab87);
			odm_set_bb_reg(p_dm_odm, REG_CONFIG_ANT_B, MASKDWORD, 0x00880000);

			/* ----send one shot signal---- */
			/* path B */
			odm_set_bb_reg(p_dm_odm, REG_CONFIG_PMPD_ANT_B, MASKDWORD, 0x80047788);
			ODM_delay_ms(1);
			odm_set_bb_reg(p_dm_odm, REG_CONFIG_PMPD_ANT_B, MASKDWORD, 0x00047788);
			ODM_delay_ms(50);
		}

		/* PA gain = 11 => tx_agc = 1a */
		for (index = 0; index < 4; index++)
			odm_set_bb_reg(p_dm_odm, 0x830 + index * 4, MASKDWORD, 0x34343434);
		for (index = 0; index < 2; index++)
			odm_set_bb_reg(p_dm_odm, 0x848 + index * 4, MASKDWORD, 0x34343434);
		for (index = 0; index < 2; index++)
			odm_set_bb_reg(p_dm_odm, 0x868 + index * 4, MASKDWORD, 0x34343434);

		/* PAGE_B for path-B DPK setting */
		odm_set_bb_reg(p_dm_odm, REG_PDP_ANT_B, MASKDWORD, 0x02017098);
		odm_set_bb_reg(p_dm_odm, REG_PDP_ANT_B_4, MASKDWORD, 0xf76d9f84);
		odm_set_bb_reg(p_dm_odm, REG_CONFIG_PMPD_ANT_B, MASKDWORD, 0x0004ab87);
		odm_set_bb_reg(p_dm_odm, REG_CONFIG_ANT_B, MASKDWORD, 0x00880000);

		/* RF lpbk switches on */
		odm_set_bb_reg(p_dm_odm, 0x840, MASKDWORD, 0x0101000f);
		odm_set_bb_reg(p_dm_odm, 0x840, MASKDWORD, 0x01120103);

		/* path-B RF lpbk */
		odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_B, 0x0c, MASKDWORD, 0x8992b);
		odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_B, 0x0d, MASKDWORD, 0x0e52c);
		odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_B, RF_AC, MASKDWORD, 0x5205a);

		/* ----send one shot signal---- */
		odm_set_bb_reg(p_dm_odm, REG_CONFIG_PMPD_ANT_B, MASKDWORD, 0x800477c0);
		ODM_delay_ms(1);
		odm_set_bb_reg(p_dm_odm, REG_CONFIG_PMPD_ANT_B, MASKDWORD, 0x000477c0);
		ODM_delay_ms(50);

		while (retry_count < DP_RETRY_LIMIT && !p_dm_odm->rf_calibrate_info.is_dp_path_bok) {
			/* ----read back measurement results---- */
			odm_set_bb_reg(p_dm_odm, REG_PDP_ANT_B, MASKDWORD, 0x0c297018);
			tmp_reg = odm_get_bb_reg(p_dm_odm, 0xbf0, MASKDWORD);
			odm_set_bb_reg(p_dm_odm, REG_PDP_ANT_B, MASKDWORD, 0x0c29701f);
			tmp_reg2 = odm_get_bb_reg(p_dm_odm, 0xbf8, MASKDWORD);

			tmp_reg = (tmp_reg & MASKHWORD) >> 16;
			tmp_reg2 = (tmp_reg2 & MASKHWORD) >> 16;

			if (tmp_reg < 0xf0 || tmp_reg > 0x105 || tmp_reg2 > 0xff) {
				odm_set_bb_reg(p_dm_odm, REG_PDP_ANT_B, MASKDWORD, 0x02017098);

				odm_set_bb_reg(p_dm_odm, REG_FPGA0_IQK, 0xffffff00, 0x800000);
				odm_set_bb_reg(p_dm_odm, REG_FPGA0_IQK, 0xffffff00, 0);
				ODM_delay_ms(1);
				odm_set_bb_reg(p_dm_odm, REG_CONFIG_PMPD_ANT_B, MASKDWORD, 0x800477c0);
				ODM_delay_ms(1);
				odm_set_bb_reg(p_dm_odm, REG_CONFIG_PMPD_ANT_B, MASKDWORD, 0x000477c0);
				ODM_delay_ms(50);
				retry_count++;
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("path B DPK retry_count %d 0xbf0[31:16] %x, 0xbf8[31:16] %x\n", retry_count, tmp_reg, tmp_reg2));
			} else {
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("path B DPK Success\n"));
				p_dm_odm->rf_calibrate_info.is_dp_path_bok = true;
				break;
			}
		}

		/* DPP path B */
		if (p_dm_odm->rf_calibrate_info.is_dp_path_bok) {
			/* DP setting */
			/* LUT by SRAM */
			odm_set_bb_reg(p_dm_odm, REG_PDP_ANT_B, MASKDWORD, 0x01017098);
			odm_set_bb_reg(p_dm_odm, REG_PDP_ANT_B_4, MASKDWORD, 0x776d9f84);
			odm_set_bb_reg(p_dm_odm, REG_CONFIG_PMPD_ANT_B, MASKDWORD, 0x0004ab87);
			odm_set_bb_reg(p_dm_odm, REG_CONFIG_ANT_B, MASKDWORD, 0x00880000);

			odm_set_bb_reg(p_dm_odm, REG_FPGA0_IQK, 0xffffff00, 0x400000);
			for (i = 0xb60; i <= 0xb9c; i += 4) {
				odm_set_bb_reg(p_dm_odm, i, MASKDWORD, 0x40004000);
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("path B ofsset = 0x%x\n", i));
			}

			/* PWSF */
			odm_set_bb_reg(p_dm_odm, 0xba0, MASKDWORD, 0x40404040);
			odm_set_bb_reg(p_dm_odm, 0xba4, MASKDWORD, 0x28324050);
			odm_set_bb_reg(p_dm_odm, 0xba8, MASKDWORD, 0x0c141920);

			for (i = 0xbac; i <= 0xbbc; i += 4)
				odm_set_bb_reg(p_dm_odm, i, MASKDWORD, 0x0c0c0c0c);

			/* tx_agc boundary */
			odm_set_bb_reg(p_dm_odm, 0xbc4, MASKDWORD, 0x0005361f);
			odm_set_bb_reg(p_dm_odm, REG_FPGA0_IQK, 0xffffff00, 0);

		} else {
			odm_set_bb_reg(p_dm_odm, REG_PDP_ANT_B, MASKDWORD, 0x00000000);
			odm_set_bb_reg(p_dm_odm, REG_PDP_ANT_B_4, MASKDWORD, 0x00000000);
		}
	}

	/* reload BB default value */
	for (index = 0; index < DP_BB_REG_NUM; index++)
		odm_set_bb_reg(p_dm_odm, BB_REG[index], MASKDWORD, BB_backup[index]);

	/* reload RF default value */
	for (path = 0; path < DP_PATH_NUM; path++) {
		for (i = 0 ; i < DP_RF_REG_NUM ; i++)
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, RF_REG[i], MASKDWORD, RF_backup[path][i]);
	}
	odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, RF_MODE1, MASKDWORD, 0x1000f);
	odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, RF_MODE2, MASKDWORD, 0x20101);

	/* reload AFE default value */
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	_phy_reload_adda_registers_8812a(p_adapter, AFE_REG, AFE_backup, IQK_ADDA_REG_NUM);

	/* reload MAC default value */
	_phy_reload_mac_registers_8812a(p_adapter, MAC_REG, MAC_backup);
#else
	_phy_reload_adda_registers_8812a(p_dm_odm, AFE_REG, AFE_backup, IQK_ADDA_REG_NUM);

	/* reload MAC default value */
	_phy_reload_mac_registers_8812a(p_dm_odm, MAC_REG, MAC_backup);
#endif

	p_dm_odm->rf_calibrate_info.is_dp_done = true;
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("<==_phy_digital_predistortion_8812a()\n"));
#endif
}

void
_phy_digital_predistortion_8812a_8812a(
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct _ADAPTER	*p_adapter
#else
	struct PHY_DM_STRUCT	*p_dm_odm
#endif
)
{
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->odmpriv;
#endif
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->DM_OutSrc;
#endif
#endif
#ifdef DISABLE_BB_RF
	return;
#endif

	return;

	if (p_dm_odm->rf_calibrate_info.is_dp_done)
		return;
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)

	if (IS_92C_SERIAL(p_hal_data->version_id))
		_phy_digital_predistortion_8812a(p_adapter, true);
	else
#endif
	{
		/* For 88C 1T1R */
		_phy_digital_predistortion_8812a(p_adapter, false);
	}
}



/* return value true => Main; false => Aux */

boolean _phy_query_rf_path_switch_8812a(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct PHY_DM_STRUCT		*p_dm_odm,
#else
	struct _ADAPTER	*p_adapter,
#endif
	boolean		is2T
)
{
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->odmpriv;
#endif
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm_odm = &p_hal_data->DM_OutSrc;
#endif
#endif
	if (!p_adapter->is_hw_init_ready) {
		u8	u1b_tmp;
		u1b_tmp = odm_read_1byte(p_dm_odm, REG_LEDCFG2) | BIT(7);
		odm_write_1byte(p_dm_odm, REG_LEDCFG2, u1b_tmp);
		/* odm_set_bb_reg(p_dm_odm, REG_LEDCFG0, BIT23, 0x01); */
		odm_set_bb_reg(p_dm_odm, REG_FPGA0_XAB_RF_PARAMETER, BIT(13), 0x01);
	}

	if (is2T) {
		if (odm_get_bb_reg(p_dm_odm, REG_FPGA0_XB_RF_INTERFACE_OE, BIT(5) | BIT(6)) == 0x01)
			return true;
		else
			return false;
	} else {
		if ((odm_get_bb_reg(p_dm_odm, REG_FPGA0_XB_RF_INTERFACE_OE, BIT(5) | BIT(4) | BIT(3)) == 0x0) ||
		    (odm_get_bb_reg(p_dm_odm, REG_CONFIG_RAM64X16, BIT(31)) == 0x0))
			return true;
		else
			return false;
	}
}



/* return value true => Main; false => Aux */
boolean phy_query_rf_path_switch_8812a(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct PHY_DM_STRUCT		*p_dm_odm
#else
	struct _ADAPTER	*p_adapter
#endif
)
{
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);

#ifdef DISABLE_BB_RF
	return true;
#endif
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)

	/* if(IS_92C_SERIAL( p_hal_data->version_id)) { */
	if (IS_2T2R(p_hal_data->version_id))
		return _phy_query_rf_path_switch_8812a(p_adapter, true);
	else
#endif
	{
		/* For 88C 1T1R */
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
		return _phy_query_rf_path_switch_8812a(p_adapter, false);
#else
		return _phy_query_rf_path_switch_8812a(p_dm_odm, false);
#endif
	}
}
#endif
