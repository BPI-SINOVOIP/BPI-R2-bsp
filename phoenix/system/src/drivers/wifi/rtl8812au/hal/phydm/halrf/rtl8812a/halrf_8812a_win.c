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

#include "mp_precomp.h"

#if RT_PLATFORM==PLATFORM_MACOSX
#include "phydm_precomp.h"
#else
#include "../phydm_precomp.h"
#endif


/*---------------------------Define Local Constant---------------------------*/
/* 2010/04/25 MH Define the max tx power tracking tx agc power. */
#define		ODM_TXPWRTRACK_MAX_IDX8812A		6

/*---------------------------Define Local Constant---------------------------*/

/* 3============================================================
 * 3 Tx Power Tracking
 * 3============================================================ */

void halrf_rf_lna_setting_8812a(
	struct PHY_DM_STRUCT	*p_dm,
	enum phydm_lna_set type
)
{
	/*phydm_disable_lna*/
	if (type == phydm_lna_disable) {
		odm_set_rf_reg(p_dm, RF_PATH_A, 0xef, 0x80000, 0x1);
		odm_set_rf_reg(p_dm, RF_PATH_A, 0x30, 0xfffff, 0x18000);	/*select Rx mode*/
		odm_set_rf_reg(p_dm, RF_PATH_A, 0x31, 0xfffff, 0x3f7ff);
		odm_set_rf_reg(p_dm, RF_PATH_A, 0x32, 0xfffff, 0xc22bf);	/*disable LNA*/
		odm_set_rf_reg(p_dm, RF_PATH_A, 0xef, 0x80000, 0x0);
		if (p_dm->rf_type > RF_1T1R) {
			odm_set_rf_reg(p_dm, RF_PATH_B, 0xef, 0x80000, 0x1);
			odm_set_rf_reg(p_dm, RF_PATH_B, 0x30, 0xfffff, 0x18000);	/*select Rx mode*/
			odm_set_rf_reg(p_dm, RF_PATH_B, 0x31, 0xfffff, 0x3f7ff);
			odm_set_rf_reg(p_dm, RF_PATH_B, 0x32, 0xfffff, 0xc22bf);	/*disable LNA*/
			odm_set_rf_reg(p_dm, RF_PATH_B, 0xef, 0x80000, 0x0);
		}
	} else if (type == phydm_lna_enable) {
		odm_set_rf_reg(p_dm, RF_PATH_A, 0xef, 0x80000, 0x1);
		odm_set_rf_reg(p_dm, RF_PATH_A, 0x30, 0xfffff, 0x18000);	/*select Rx mode*/
		odm_set_rf_reg(p_dm, RF_PATH_A, 0x31, 0xfffff, 0x3f7ff);
		odm_set_rf_reg(p_dm, RF_PATH_A, 0x32, 0xfffff, 0xc26bf);	/*disable LNA*/
		odm_set_rf_reg(p_dm, RF_PATH_A, 0xef, 0x80000, 0x0);
		if (p_dm->rf_type > RF_1T1R) {
			odm_set_rf_reg(p_dm, RF_PATH_B, 0xef, 0x80000, 0x1);
			odm_set_rf_reg(p_dm, RF_PATH_B, 0x30, 0xfffff, 0x18000);	/*select Rx mode*/
			odm_set_rf_reg(p_dm, RF_PATH_B, 0x31, 0xfffff, 0x3f7ff);
			odm_set_rf_reg(p_dm, RF_PATH_B, 0x32, 0xfffff, 0xc26bf);	/*disable LNA*/
			odm_set_rf_reg(p_dm, RF_PATH_B, 0xef, 0x80000, 0x0);
		}
	}
}


void do_iqk_8812a(
	void		*p_dm_void,
	u8		delta_thermal_index,
	u8		thermal_value,
	u8		threshold
)
{
	struct PHY_DM_STRUCT	*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	odm_reset_iqk_result(p_dm);
	p_dm->rf_calibrate_info.thermal_value_iqk = thermal_value;
	halrf_iqk_trigger(p_dm, false);
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
	void		*p_dm_void,
	enum pwrtrack_method	method,
	u8				rf_path,
	u8				channel_mapped_index
)
{
	u32	final_bb_swing_idx[2];
	struct PHY_DM_STRUCT	*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct _ADAPTER		*adapter = p_dm->adapter;
	u8			pwr_tracking_limit = 26; /* +1.0dB */
	u8			tx_rate = 0xFF;
	u8			final_ofdm_swing_index = 0;
	u8			final_cck_swing_index = 0;
	struct odm_rf_calibration_structure	*p_rf_calibrate_info = &(p_dm->rf_calibrate_info);

	if (*(p_dm->p_mp_mode) == true) {
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
#if (MP_DRIVER == 1)
		PMPT_CONTEXT p_mpt_ctx = &(adapter->MptCtx);

		tx_rate = MptToMgntRate(p_mpt_ctx->MptRateIndex);
#endif
#elif (DM_ODM_SUPPORT_TYPE & ODM_CE)
#ifdef CONFIG_MP_INCLUDED
		PMPT_CONTEXT p_mpt_ctx = &(adapter->mppriv.mpt_ctx);

		tx_rate = mpt_to_mgnt_rate(p_mpt_ctx->mpt_rate_index);
#endif
#endif
#endif
	} else {
		u16	rate	 = *(p_dm->p_forced_data_rate);

		if (!rate) { /*auto rate*/
#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
			tx_rate = adapter->HalFunc.GetHwRateFromMRateHandler(p_dm->tx_rate);
#elif (DM_ODM_SUPPORT_TYPE & ODM_CE)
			tx_rate = hw_rate_to_m_rate(p_dm->tx_rate);
#endif
		} else   /*force rate*/
			tx_rate = (u8)rate;
	}

	ODM_RT_TRACE(p_dm, ODM_COMP_TX_PWR_TRACK, ODM_DBG_LOUD, ("Power Tracking tx_rate=0x%X\n", tx_rate));
	ODM_RT_TRACE(p_dm, ODM_COMP_TX_PWR_TRACK, ODM_DBG_LOUD, ("===>odm_tx_pwr_track_set_pwr8812a\n"));

	if (tx_rate != 0xFF) { /* 20130429 Mimic Modify High rate BBSwing Limit. */
		/* 2 CCK */
		if (((tx_rate >= MGN_1M) && (tx_rate <= MGN_5_5M)) || (tx_rate == MGN_11M))
			pwr_tracking_limit = 32; /*+4dB*/
		/*2 OFDM*/
		else if ((tx_rate >= MGN_6M) && (tx_rate <= MGN_48M))
			pwr_tracking_limit = 30; /* +3dB */
		else if (tx_rate == MGN_54M)
			pwr_tracking_limit = 28; /* +2dB */

		/* 2 HT */
		else if ((tx_rate >= MGN_MCS0) && (tx_rate <= MGN_MCS2)) /* QPSK/BPSK */
			pwr_tracking_limit = 34; /* +5dB */
		else if ((tx_rate >= MGN_MCS3) && (tx_rate <= MGN_MCS4)) /* 16QAM */
			pwr_tracking_limit = 30; /* +3dB */
		else if ((tx_rate >= MGN_MCS5) && (tx_rate <= MGN_MCS7)) /* 64QAM */
			pwr_tracking_limit = 28; /* +2dB */

		else if ((tx_rate >= MGN_MCS8) && (tx_rate <= MGN_MCS10)) /* QPSK/BPSK */
			pwr_tracking_limit = 34; /* +5dB */
		else if ((tx_rate >= MGN_MCS11) && (tx_rate <= MGN_MCS12)) /* 16QAM */
			pwr_tracking_limit = 30; /* +3dB */
		else if ((tx_rate >= MGN_MCS13) && (tx_rate <= MGN_MCS15)) /* 64QAM */
			pwr_tracking_limit = 28; /* +2dB */

		/* 2 VHT */
		else if ((tx_rate >= MGN_VHT1SS_MCS0) && (tx_rate <= MGN_VHT1SS_MCS2)) /* QPSK/BPSK */
			pwr_tracking_limit = 34; /* +5dB */
		else if ((tx_rate >= MGN_VHT1SS_MCS3) && (tx_rate <= MGN_VHT1SS_MCS4)) /* 16QAM */
			pwr_tracking_limit = 30; /* +3dB */
		else if ((tx_rate >= MGN_VHT1SS_MCS5) && (tx_rate <= MGN_VHT1SS_MCS6)) /* 64QAM */
			pwr_tracking_limit = 28; /* +2dB */
		else if (tx_rate == MGN_VHT1SS_MCS7) /* 64QAM */
			pwr_tracking_limit = 26; /* +1dB */
		else if (tx_rate == MGN_VHT1SS_MCS8) /* 256QAM */
			pwr_tracking_limit = 24; /* +0dB */
		else if (tx_rate == MGN_VHT1SS_MCS9) /* 256QAM */
			pwr_tracking_limit = 22; /* -1dB */

		else if ((tx_rate >= MGN_VHT2SS_MCS0) && (tx_rate <= MGN_VHT2SS_MCS2)) /* QPSK/BPSK */
			pwr_tracking_limit = 34; /* +5dB */
		else if ((tx_rate >= MGN_VHT2SS_MCS3) && (tx_rate <= MGN_VHT2SS_MCS4)) /* 16QAM */
			pwr_tracking_limit = 30; /* +3dB */
		else if ((tx_rate >= MGN_VHT2SS_MCS5) && (tx_rate <= MGN_VHT2SS_MCS6)) /* 64QAM */
			pwr_tracking_limit = 28; /* +2dB */
		else if (tx_rate == MGN_VHT2SS_MCS7) /* 64QAM */
			pwr_tracking_limit = 26; /* +1dB */
		else if (tx_rate == MGN_VHT2SS_MCS8) /* 256QAM */
			pwr_tracking_limit = 24; /* +0dB */
		else if (tx_rate == MGN_VHT2SS_MCS9) /* 256QAM */
			pwr_tracking_limit = 22; /* -1dB */

		else
			pwr_tracking_limit = 24;
	}
	ODM_RT_TRACE(p_dm, ODM_COMP_TX_PWR_TRACK, ODM_DBG_LOUD, ("tx_rate=0x%x, pwr_tracking_limit=%d\n", tx_rate, pwr_tracking_limit));


	if (method == BBSWING) {
		ODM_RT_TRACE(p_dm, ODM_COMP_TX_PWR_TRACK, ODM_DBG_LOUD, ("===>odm_tx_pwr_track_set_pwr8812a\n"));

		if (rf_path == RF_PATH_A) {
			final_bb_swing_idx[RF_PATH_A] = (p_rf_calibrate_info->OFDM_index[RF_PATH_A] > pwr_tracking_limit) ? pwr_tracking_limit : p_rf_calibrate_info->OFDM_index[RF_PATH_A];
			ODM_RT_TRACE(p_dm, ODM_COMP_TX_PWR_TRACK, ODM_DBG_LOUD, ("p_rf_calibrate_info->OFDM_index[RF_PATH_A]=%d, p_dm->RealBbSwingIdx[RF_PATH_A]=%d\n",
				p_rf_calibrate_info->OFDM_index[RF_PATH_A], final_bb_swing_idx[RF_PATH_A]));

			odm_set_bb_reg(p_dm, REG_A_TX_SCALE_JAGUAR, 0xFFE00000, tx_scaling_table_jaguar[final_bb_swing_idx[RF_PATH_A]]);
		} else {
			final_bb_swing_idx[RF_PATH_B] = (p_rf_calibrate_info->OFDM_index[RF_PATH_B] > pwr_tracking_limit) ? pwr_tracking_limit : p_rf_calibrate_info->OFDM_index[RF_PATH_B];
			ODM_RT_TRACE(p_dm, ODM_COMP_TX_PWR_TRACK, ODM_DBG_LOUD, ("p_rf_calibrate_info->OFDM_index[RF_PATH_B]=%d, p_dm->RealBbSwingIdx[RF_PATH_B]=%d\n",
				p_rf_calibrate_info->OFDM_index[RF_PATH_B], final_bb_swing_idx[RF_PATH_B]));

			odm_set_bb_reg(p_dm, REG_B_TX_SCALE_JAGUAR, 0xFFE00000, tx_scaling_table_jaguar[final_bb_swing_idx[RF_PATH_B]]);
		}
	}

	else if (method == MIX_MODE) {
		ODM_RT_TRACE(p_dm, ODM_COMP_TX_PWR_TRACK, ODM_DBG_LOUD, ("p_rf_calibrate_info->default_ofdm_index=%d, p_rf_calibrate_info->absolute_ofdm_swing_idx[rf_path]=%d, rf_path = %d\n",
			p_rf_calibrate_info->default_ofdm_index, p_rf_calibrate_info->absolute_ofdm_swing_idx[rf_path], rf_path));

		final_cck_swing_index = p_rf_calibrate_info->default_cck_index + p_rf_calibrate_info->absolute_ofdm_swing_idx[rf_path];
		final_ofdm_swing_index = p_rf_calibrate_info->default_ofdm_index + p_rf_calibrate_info->absolute_ofdm_swing_idx[rf_path];

		if (rf_path == RF_PATH_A) {

			if (final_ofdm_swing_index > pwr_tracking_limit) {  /* BBSwing higher then Limit */
				p_rf_calibrate_info->remnant_cck_swing_idx = final_cck_swing_index - pwr_tracking_limit;           /* CCK Follow the same compensate value as path A */
				p_rf_calibrate_info->remnant_ofdm_swing_idx[rf_path] = final_ofdm_swing_index - pwr_tracking_limit;

				odm_set_bb_reg(p_dm, REG_A_TX_SCALE_JAGUAR, 0xFFE00000, tx_scaling_table_jaguar[pwr_tracking_limit]);

				p_rf_calibrate_info->modify_tx_agc_flag_path_a = true;

				PHY_SetTxPowerLevelByPath(adapter, *p_dm->p_channel, RF_PATH_A);

				ODM_RT_TRACE(p_dm, ODM_COMP_TX_PWR_TRACK, ODM_DBG_LOUD, ("******Path_A Over BBSwing Limit, pwr_tracking_limit = %d, Remnant tx_agc value = %d\n", pwr_tracking_limit, p_rf_calibrate_info->remnant_ofdm_swing_idx[rf_path]));
			} else if (final_ofdm_swing_index < 0) {
				p_rf_calibrate_info->remnant_cck_swing_idx = final_cck_swing_index;           /* CCK Follow the same compensate value as path A */
				p_rf_calibrate_info->remnant_ofdm_swing_idx[rf_path] = final_ofdm_swing_index;

				odm_set_bb_reg(p_dm, REG_A_TX_SCALE_JAGUAR, 0xFFE00000, tx_scaling_table_jaguar[0]);

				p_rf_calibrate_info->modify_tx_agc_flag_path_a = true;

				PHY_SetTxPowerLevelByPath(adapter, *p_dm->p_channel, RF_PATH_A);

				ODM_RT_TRACE(p_dm, ODM_COMP_TX_PWR_TRACK, ODM_DBG_LOUD, ("******Path_A Lower then BBSwing lower bound  0, Remnant tx_agc value = %d\n", p_rf_calibrate_info->remnant_ofdm_swing_idx[rf_path]));
			} else {
				odm_set_bb_reg(p_dm, REG_A_TX_SCALE_JAGUAR, 0xFFE00000, tx_scaling_table_jaguar[final_ofdm_swing_index]);

				ODM_RT_TRACE(p_dm, ODM_COMP_TX_PWR_TRACK, ODM_DBG_LOUD, ("******Path_A Compensate with BBSwing, final_ofdm_swing_index = %d\n", final_ofdm_swing_index));

				if (p_rf_calibrate_info->modify_tx_agc_flag_path_a) { /* If tx_agc has changed, reset tx_agc again */
					p_rf_calibrate_info->remnant_cck_swing_idx = 0;
					p_rf_calibrate_info->remnant_ofdm_swing_idx[rf_path] = 0;

					PHY_SetTxPowerLevelByPath(adapter, *p_dm->p_channel, RF_PATH_A);

					p_rf_calibrate_info->modify_tx_agc_flag_path_a = false;

					ODM_RT_TRACE(p_dm, ODM_COMP_TX_PWR_TRACK, ODM_DBG_LOUD, ("******Path_A p_dm->Modify_TxAGC_Flag = false\n"));
				}
			}
		}

		if (rf_path == RF_PATH_B) {
			if (final_ofdm_swing_index > pwr_tracking_limit) {  /* BBSwing higher then Limit */
				p_rf_calibrate_info->remnant_ofdm_swing_idx[rf_path] = final_ofdm_swing_index - pwr_tracking_limit;

				odm_set_bb_reg(p_dm, REG_B_TX_SCALE_JAGUAR, 0xFFE00000, tx_scaling_table_jaguar[pwr_tracking_limit]);

				p_rf_calibrate_info->modify_tx_agc_flag_path_b = true;

				PHY_SetTxPowerLevelByPath(adapter, *p_dm->p_channel, RF_PATH_B);

				ODM_RT_TRACE(p_dm, ODM_COMP_TX_PWR_TRACK, ODM_DBG_LOUD, ("******Path_B Over BBSwing Limit, pwr_tracking_limit = %d, Remnant tx_agc value = %d\n", pwr_tracking_limit, p_rf_calibrate_info->remnant_ofdm_swing_idx[rf_path]));
			} else if (final_ofdm_swing_index < 0) {
				p_rf_calibrate_info->remnant_ofdm_swing_idx[rf_path] = final_ofdm_swing_index;

				odm_set_bb_reg(p_dm, REG_B_TX_SCALE_JAGUAR, 0xFFE00000, tx_scaling_table_jaguar[0]);

				p_rf_calibrate_info->modify_tx_agc_flag_path_b = true;

				PHY_SetTxPowerLevelByPath(adapter, *p_dm->p_channel, RF_PATH_B);

				ODM_RT_TRACE(p_dm, ODM_COMP_TX_PWR_TRACK, ODM_DBG_LOUD, ("******Path_B Lower then BBSwing lower bound  0, Remnant tx_agc value = %d\n", p_rf_calibrate_info->remnant_ofdm_swing_idx[rf_path]));
			} else {
				odm_set_bb_reg(p_dm, REG_B_TX_SCALE_JAGUAR, 0xFFE00000, tx_scaling_table_jaguar[final_ofdm_swing_index]);

				ODM_RT_TRACE(p_dm, ODM_COMP_TX_PWR_TRACK, ODM_DBG_LOUD, ("******Path_B Compensate with BBSwing, final_ofdm_swing_index = %d\n", final_ofdm_swing_index));

				if (p_rf_calibrate_info->modify_tx_agc_flag_path_b) { /* If tx_agc has changed, reset tx_agc again */
					p_rf_calibrate_info->remnant_ofdm_swing_idx[rf_path] = 0;

					PHY_SetTxPowerLevelByPath(adapter, *p_dm->p_channel, RF_PATH_B);

					p_rf_calibrate_info->modify_tx_agc_flag_path_b = false;

					ODM_RT_TRACE(p_dm, ODM_COMP_TX_PWR_TRACK, ODM_DBG_LOUD, ("******Path_B p_dm->Modify_TxAGC_Flag = false\n"));
				}
			}
		}

	} else
		return;
}

void
get_delta_swing_table_8812a(
	void		*p_dm_void,
	u8 **temperature_up_a,
	u8 **temperature_down_a,
	u8 **temperature_up_b,
	u8 **temperature_down_b
)
{
	struct PHY_DM_STRUCT	*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct _ADAPTER		*adapter = p_dm->adapter;
	struct odm_rf_calibration_structure	*p_rf_calibrate_info = &(p_dm->rf_calibrate_info);
	u8		tx_rate			= 0xFF;
	u8	channel		 = *p_dm->p_channel;


	if (*(p_dm->p_mp_mode) == true) {
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
#if (MP_DRIVER == 1)
		PMPT_CONTEXT p_mpt_ctx = &(adapter->MptCtx);

		tx_rate = MptToMgntRate(p_mpt_ctx->MptRateIndex);
#endif
#elif (DM_ODM_SUPPORT_TYPE & ODM_CE)
#ifdef CONFIG_MP_INCLUDED
		PMPT_CONTEXT p_mpt_ctx = &(adapter->mppriv.mpt_ctx);

		tx_rate = mpt_to_mgnt_rate(p_mpt_ctx->mpt_rate_index);
#endif
#endif
#endif
	} else {
		u16	rate	 = *(p_dm->p_forced_data_rate);

		if (!rate) { /*auto rate*/
#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
			tx_rate = adapter->HalFunc.GetHwRateFromMRateHandler(p_dm->tx_rate);
#elif (DM_ODM_SUPPORT_TYPE & ODM_CE)
			tx_rate = hw_rate_to_m_rate(p_dm->tx_rate);
#endif
		} else   /*force rate*/
			tx_rate = (u8)rate;
	}

	ODM_RT_TRACE(p_dm, ODM_COMP_TX_PWR_TRACK, ODM_DBG_LOUD, ("Power Tracking tx_rate=0x%X\n", tx_rate));

	if (1 <= channel && channel <= 14) {
		if (IS_CCK_RATE(tx_rate)) {
			*temperature_up_a   = p_rf_calibrate_info->delta_swing_table_idx_2g_cck_a_p;
			*temperature_down_a = p_rf_calibrate_info->delta_swing_table_idx_2g_cck_a_n;
			*temperature_up_b   = p_rf_calibrate_info->delta_swing_table_idx_2g_cck_b_p;
			*temperature_down_b = p_rf_calibrate_info->delta_swing_table_idx_2g_cck_b_n;
		} else {
			*temperature_up_a   = p_rf_calibrate_info->delta_swing_table_idx_2ga_p;
			*temperature_down_a = p_rf_calibrate_info->delta_swing_table_idx_2ga_n;
			*temperature_up_b   = p_rf_calibrate_info->delta_swing_table_idx_2gb_p;
			*temperature_down_b = p_rf_calibrate_info->delta_swing_table_idx_2gb_n;
		}
	} else if (36 <= channel && channel <= 64) {
		*temperature_up_a   = p_rf_calibrate_info->delta_swing_table_idx_5ga_p[0];
		*temperature_down_a = p_rf_calibrate_info->delta_swing_table_idx_5ga_n[0];
		*temperature_up_b   = p_rf_calibrate_info->delta_swing_table_idx_5gb_p[0];
		*temperature_down_b = p_rf_calibrate_info->delta_swing_table_idx_5gb_n[0];
	} else if (100 <= channel && channel <= 144) {
		*temperature_up_a   = p_rf_calibrate_info->delta_swing_table_idx_5ga_p[1];
		*temperature_down_a = p_rf_calibrate_info->delta_swing_table_idx_5ga_n[1];
		*temperature_up_b   = p_rf_calibrate_info->delta_swing_table_idx_5gb_p[1];
		*temperature_down_b = p_rf_calibrate_info->delta_swing_table_idx_5gb_n[1];
	} else if (149 <= channel && channel <= 177) {
		*temperature_up_a   = p_rf_calibrate_info->delta_swing_table_idx_5ga_p[2];
		*temperature_down_a = p_rf_calibrate_info->delta_swing_table_idx_5ga_n[2];
		*temperature_up_b   = p_rf_calibrate_info->delta_swing_table_idx_5gb_p[2];
		*temperature_down_b = p_rf_calibrate_info->delta_swing_table_idx_5gb_n[2];
	} else {
		*temperature_up_a   = (u8 *)delta_swing_table_idx_2ga_p_8188e;
		*temperature_down_a = (u8 *)delta_swing_table_idx_2ga_n_8188e;
		*temperature_up_b   = (u8 *)delta_swing_table_idx_2ga_p_8188e;
		*temperature_down_b = (u8 *)delta_swing_table_idx_2ga_n_8188e;
	}


	return;
}

void configure_txpower_track_8812a(
	struct _TXPWRTRACK_CFG	*p_config
)
{
	p_config->swing_table_size_cck = TXSCALE_TABLE_SIZE;
	p_config->swing_table_size_ofdm = TXSCALE_TABLE_SIZE;
	p_config->threshold_iqk = IQK_THRESHOLD;
	p_config->threshold_dpk = DPK_THRESHOLD;
	p_config->average_thermal_num = AVG_THERMAL_NUM_8812A;
	p_config->rf_path_count = MAX_PATH_NUM_8812A;
	p_config->thermal_reg_addr = RF_T_METER_8812A;

	p_config->odm_tx_pwr_track_set_pwr = odm_tx_pwr_track_set_pwr8812a;
	p_config->do_iqk = do_iqk_8812a;
	p_config->phy_lc_calibrate = halrf_lck_trigger;
	p_config->get_delta_swing_table = get_delta_swing_table_8812a;
}


#define BW_20M	0
#define	BW_40M  1
#define	BW_80M	2

void _iqk_rx_fill_iqc_8812a(
	struct PHY_DM_STRUCT			*p_dm,
	enum rf_path	path,
	unsigned int			RX_X,
	unsigned int			RX_Y
)
{
	switch (path) {
	case RF_PATH_A:
	{
		odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
		if (RX_X >> 1 >= 0x112 || (RX_Y >> 1 >= 0x12 && RX_Y >> 1 <= 0x3ee)) {
			odm_set_bb_reg(p_dm, 0xc10, 0x000003ff, 0x100);
			odm_set_bb_reg(p_dm, 0xc10, 0x03ff0000, 0);
			ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RX_X = %x;;RX_Y = %x ====>fill to IQC\n", RX_X >> 1 & 0x000003ff, RX_Y >> 1 & 0x000003ff));
		} else {
			odm_set_bb_reg(p_dm, 0xc10, 0x000003ff, RX_X >> 1);
			odm_set_bb_reg(p_dm, 0xc10, 0x03ff0000, RX_Y >> 1);
			ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RX_X = %x;;RX_Y = %x ====>fill to IQC\n", RX_X >> 1 & 0x000003ff, RX_Y >> 1 & 0x000003ff));
			ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xc10 = %x ====>fill to IQC\n", odm_read_4byte(p_dm, 0xc10)));
		}
	}
	break;
	case RF_PATH_B:
	{
		odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
		if (RX_X >> 1 >= 0x112 || (RX_Y >> 1 >= 0x12 && RX_Y >> 1 <= 0x3ee)) {
			odm_set_bb_reg(p_dm, 0xe10, 0x000003ff, 0x100);
			odm_set_bb_reg(p_dm, 0xe10, 0x03ff0000, 0);
			ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RX_X = %x;;RX_Y = %x ====>fill to IQC\n", RX_X >> 1 & 0x000003ff, RX_Y >> 1 & 0x000003ff));
		} else {
			odm_set_bb_reg(p_dm, 0xe10, 0x000003ff, RX_X >> 1);
			odm_set_bb_reg(p_dm, 0xe10, 0x03ff0000, RX_Y >> 1);
			ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RX_X = %x;;RX_Y = %x====>fill to IQC\n ", RX_X >> 1 & 0x000003ff, RX_Y >> 1 & 0x000003ff));
			ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xe10 = %x====>fill to IQC\n", odm_read_4byte(p_dm, 0xe10)));
		}
	}
	break;
	default:
		break;
	};
}

void _iqk_tx_fill_iqc_8812a(
	struct PHY_DM_STRUCT			*p_dm,
	enum rf_path	path,
	unsigned int			TX_X,
	unsigned int			TX_Y
)
{
	struct _hal_rf_		*p_rf = &(p_dm->rf_table);
	
	switch (path) {
	case RF_PATH_A:
	{
		odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x1); /* [31] = 1 --> Page C1 */
		odm_set_bb_reg(p_dm, 0xc90, BIT(7), 0x1);
		odm_set_bb_reg(p_dm, 0xcc4, BIT(18), 0x1);
		if (!p_rf->dpk_done)
			odm_set_bb_reg(p_dm, 0xcc4, BIT(29), 0x1);
		odm_set_bb_reg(p_dm, 0xcc8, BIT(29), 0x1);
		odm_set_bb_reg(p_dm, 0xccc, 0x000007ff, TX_Y);
		odm_set_bb_reg(p_dm, 0xcd4, 0x000007ff, TX_X);
		ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("TX_X = %x;;TX_Y = %x =====> fill to IQC\n", TX_X & 0x000007ff, TX_Y & 0x000007ff));
		ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xcd4 = %x;;0xccc = %x ====>fill to IQC\n", odm_get_bb_reg(p_dm, 0xcd4, 0x000007ff), odm_get_bb_reg(p_dm, 0xccc, 0x000007ff)));
	}
	break;
	case RF_PATH_B:
	{
		odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x1); /* [31] = 1 --> Page C1 */
		odm_set_bb_reg(p_dm, 0xe90, BIT(7), 0x1);
		odm_set_bb_reg(p_dm, 0xec4, BIT(18), 0x1);
		if (!p_rf->dpk_done)
			odm_set_bb_reg(p_dm, 0xec4, BIT(29), 0x1);
		odm_set_bb_reg(p_dm, 0xec8, BIT(29), 0x1);
		odm_set_bb_reg(p_dm, 0xecc, 0x000007ff, TX_Y);
		odm_set_bb_reg(p_dm, 0xed4, 0x000007ff, TX_X);
		ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("TX_X = %x;;TX_Y = %x =====> fill to IQC\n", TX_X & 0x000007ff, TX_Y & 0x000007ff));
		ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xed4 = %x;;0xecc = %x ====>fill to IQC\n", odm_get_bb_reg(p_dm, 0xed4, 0x000007ff), odm_get_bb_reg(p_dm, 0xecc, 0x000007ff)));
	}
	break;
	default:
		break;
	};
}

void _iqk_backup_mac_bb_8812a(
	struct PHY_DM_STRUCT	*p_dm,
	u32		*MACBB_backup,
	u32		*backup_macbb_reg,
	u32		MACBB_NUM
)
{
	u32 i;
	odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	/* save MACBB default value */
	for (i = 0; i < MACBB_NUM; i++)
		MACBB_backup[i] = odm_read_4byte(p_dm, backup_macbb_reg[i]);

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("BackupMacBB Success!!!!\n"));
}
void _iqk_backup_rf_8812a(
	struct PHY_DM_STRUCT	*p_dm,
	u32		*RFA_backup,
	u32		*RFB_backup,
	u32		*backup_rf_reg,
	u32		RF_NUM
)
{

	u32 i;
	odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	/* Save RF Parameters */
	for (i = 0; i < RF_NUM; i++) {
		RFA_backup[i] = odm_get_rf_reg(p_dm, RF_PATH_A, backup_rf_reg[i], MASKDWORD);
		RFB_backup[i] = odm_get_rf_reg(p_dm, RF_PATH_B, backup_rf_reg[i], MASKDWORD);
	}
	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("BackupRF Success!!!!\n"));
}
void _iqk_backup_afe_8812a(
	struct PHY_DM_STRUCT		*p_dm,
	u32		*AFE_backup,
	u32		*backup_afe_reg,
	u32		AFE_NUM
)
{
	u32 i;
	odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	/* Save AFE Parameters */
	for (i = 0; i < AFE_NUM; i++)
		AFE_backup[i] = odm_read_4byte(p_dm, backup_afe_reg[i]);
	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("BackupAFE Success!!!!\n"));
}
void _iqk_restore_mac_bb_8812a(
	struct PHY_DM_STRUCT		*p_dm,
	u32		*MACBB_backup,
	u32		*backup_macbb_reg,
	u32		MACBB_NUM
)
{
	u32 i;
	odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	/* Reload MacBB Parameters */
	for (i = 0; i < MACBB_NUM; i++)
		odm_write_4byte(p_dm, backup_macbb_reg[i], MACBB_backup[i]);
	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RestoreMacBB Success!!!!\n"));
}
void _iqk_restore_rf_8812a(
	struct PHY_DM_STRUCT			*p_dm,
	enum rf_path	path,
	u32			*backup_rf_reg,
	u32			*RF_backup,
	u32			RF_REG_NUM
)
{
	u32 i;

	odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	for (i = 0; i < RF_REG_NUM; i++)
		odm_set_rf_reg(p_dm, (enum rf_path)path, backup_rf_reg[i], RFREGOFFSETMASK, RF_backup[i]);

	odm_set_rf_reg(p_dm, (enum rf_path)path, 0xef, RFREGOFFSETMASK, 0x0);

	switch (path) {
	case RF_PATH_A:
	{
		ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RestoreRF path A Success!!!!\n"));
	}
	break;
	case RF_PATH_B:
	{
		ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RestoreRF path B Success!!!!\n"));
	}
	break;
	default:
		break;
	}
}
void _iqk_restore_afe_8812a(
	struct PHY_DM_STRUCT		*p_dm,
	u32		*AFE_backup,
	u32		*backup_afe_reg,
	u32		AFE_NUM
)
{
	struct _hal_rf_		*p_rf = &(p_dm->rf_table);
	u32 i;
	odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	/* Reload AFE Parameters */
	for (i = 0; i < AFE_NUM; i++)
		odm_write_4byte(p_dm, backup_afe_reg[i], AFE_backup[i]);
	odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x1); /* [31] = 1 --> Page C1 */
	odm_write_4byte(p_dm, 0xc80, 0x0);
	odm_write_4byte(p_dm, 0xc84, 0x0);
	odm_write_4byte(p_dm, 0xc88, 0x0);
	odm_write_4byte(p_dm, 0xc8c, 0x3c000000);
	odm_set_bb_reg(p_dm, 0xc90, BIT(7), 0x1);
	odm_set_bb_reg(p_dm, 0xcc4, BIT(18), 0x1);
	if (!p_rf->dpk_done)
		odm_set_bb_reg(p_dm, 0xcc4, BIT(29), 0x1);
	odm_set_bb_reg(p_dm, 0xcc8, BIT(29), 0x1);
	/* odm_write_4byte(p_dm, 0xcb8, 0x0); */
	odm_write_4byte(p_dm, 0xe80, 0x0);
	odm_write_4byte(p_dm, 0xe84, 0x0);
	odm_write_4byte(p_dm, 0xe88, 0x0);
	odm_write_4byte(p_dm, 0xe8c, 0x3c000000);
	odm_set_bb_reg(p_dm, 0xe90, BIT(7), 0x1);
	odm_set_bb_reg(p_dm, 0xec4, BIT(18), 0x1);
	if (!p_rf->dpk_done)
		odm_set_bb_reg(p_dm, 0xec4, BIT(29), 0x1);
	odm_set_bb_reg(p_dm, 0xec8, BIT(29), 0x1);
	/* odm_write_4byte(p_dm, 0xeb8, 0x0); */
	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RestoreAFE Success!!!!\n"));
}


void _iqk_configure_mac_8812a(
	struct PHY_DM_STRUCT		*p_dm
)
{
	/* ========MAC register setting======== */
	odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	odm_write_1byte(p_dm, 0x522, 0x3f);
	odm_set_bb_reg(p_dm, 0x550, BIT(11) | BIT(3), 0x0);
	odm_write_1byte(p_dm, 0x808, 0x00);		/*		RX ante off */
	odm_set_bb_reg(p_dm, 0x838, 0xf, 0xc);		/*		CCA off */
	odm_write_1byte(p_dm, 0xa07, 0xf);		/*		CCK RX path off */
}

#define cal_num 10

void _iqk_tx_8812a(
	struct PHY_DM_STRUCT		*p_dm,
	u8 chnl_idx
)
{
	u8		delay_count;
	u8		cal0_retry, cal1_retry, tx0_average = 0, tx1_average = 0, rx0_average = 0, rx1_average = 0;
	int			TX_IQC_temp[10][4], TX_IQC[4];		/* TX_IQC = [TX0_X, TX0_Y,TX1_X,TX1_Y]; for 3 times */
	int			RX_IQC_temp[10][4], RX_IQC[4];		/* RX_IQC = [RX0_X, RX0_Y,RX1_X,RX1_Y]; for 3 times */
	boolean	TX0_fail = true, RX0_fail = true, IQK0_ready = false, TX0_finish = false, RX0_finish = false;
	boolean	TX1_fail = true, RX1_fail = true, IQK1_ready = false, TX1_finish = false, RX1_finish = false;
	int			i, ii, dx = 0, dy = 0;

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("band_width = %d, ext_pa_5g = %d, ExtPA2G = %d\n", *p_dm->p_band_width, p_dm->ext_pa_5g, p_dm->ext_pa));
	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("Interface = %d, RFE_Type = %d\n", p_dm->support_interface, p_dm->rfe_type));

	odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	/* ========path-A AFE all on======== */
	/* Port 0 DAC/ADC on */
	odm_write_4byte(p_dm, 0xc60, 0x77777777);
	odm_write_4byte(p_dm, 0xc64, 0x77777777);

	/* Port 1 DAC/ADC on */
	odm_write_4byte(p_dm, 0xe60, 0x77777777);
	odm_write_4byte(p_dm, 0xe64, 0x77777777);

	odm_write_4byte(p_dm, 0xc68, 0x19791979);
	odm_write_4byte(p_dm, 0xe68, 0x19791979);
	odm_set_bb_reg(p_dm, 0xc00, 0xf, 0x4);/*	hardware 3-wire off */
	odm_set_bb_reg(p_dm, 0xe00, 0xf, 0x4);/*	hardware 3-wire off */

	/* DAC/ADC sampling rate (160 MHz) */
	odm_set_bb_reg(p_dm, 0xc5c, BIT(26) | BIT(25) | BIT(24), 0x7);
	odm_set_bb_reg(p_dm, 0xe5c, BIT(26) | BIT(25) | BIT(24), 0x7);

	/* ====== path A TX IQK RF setting ====== */
	odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	odm_set_rf_reg(p_dm, RF_PATH_A, 0xef, RFREGOFFSETMASK, 0x80002);
	odm_set_rf_reg(p_dm, RF_PATH_A, 0x30, RFREGOFFSETMASK, 0x20000);
	odm_set_rf_reg(p_dm, RF_PATH_A, 0x31, RFREGOFFSETMASK, 0x3fffd);
	odm_set_rf_reg(p_dm, RF_PATH_A, 0x32, RFREGOFFSETMASK, 0xfe83f);
	odm_set_rf_reg(p_dm, RF_PATH_A, 0x65, RFREGOFFSETMASK, 0x931d5);
	odm_set_rf_reg(p_dm, RF_PATH_A, 0x8f, RFREGOFFSETMASK, 0x8a001);
	/* ====== path B TX IQK RF setting ====== */
	odm_set_rf_reg(p_dm, RF_PATH_B, 0xef, RFREGOFFSETMASK, 0x80002);
	odm_set_rf_reg(p_dm, RF_PATH_B, 0x30, RFREGOFFSETMASK, 0x20000);
	odm_set_rf_reg(p_dm, RF_PATH_B, 0x31, RFREGOFFSETMASK, 0x3fffd);
	odm_set_rf_reg(p_dm, RF_PATH_B, 0x32, RFREGOFFSETMASK, 0xfe83f);
	odm_set_rf_reg(p_dm, RF_PATH_B, 0x65, RFREGOFFSETMASK, 0x931d5);
	odm_set_rf_reg(p_dm, RF_PATH_B, 0x8f, RFREGOFFSETMASK, 0x8a001);
	odm_write_4byte(p_dm, 0x90c, 0x00008000);
	odm_set_bb_reg(p_dm, 0xc94, BIT(0), 0x1);
	odm_set_bb_reg(p_dm, 0xe94, BIT(0), 0x1);
	odm_write_4byte(p_dm, 0x978, 0x29002000);/* TX (X,Y) */
	odm_write_4byte(p_dm, 0x97c, 0xa9002000);/* RX (X,Y) */
	odm_write_4byte(p_dm, 0x984, 0x00462910);/* [0]:AGC_en, [15]:idac_K_Mask */
	odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x1); /* [31] = 1 --> Page C1 */

	if (p_dm->ext_pa_5g) {
		if (p_dm->rfe_type == 1) {
			odm_write_4byte(p_dm, 0xc88, 0x821403e3);
			odm_write_4byte(p_dm, 0xe88, 0x821403e3);
		} else {
			odm_write_4byte(p_dm, 0xc88, 0x821403f7);
			odm_write_4byte(p_dm, 0xe88, 0x821403f7);
		}
	} else {
		odm_write_4byte(p_dm, 0xc88, 0x821403f1);
		odm_write_4byte(p_dm, 0xe88, 0x821403f1);
	}
	if (*p_dm->p_band_type == ODM_BAND_5G) {
		odm_write_4byte(p_dm, 0xc8c, 0x68163e96);
		odm_write_4byte(p_dm, 0xe8c, 0x68163e96);
	} else {
		odm_write_4byte(p_dm, 0xc8c, 0x28163e96);
		odm_write_4byte(p_dm, 0xe8c, 0x28163e96);
		if (p_dm->rfe_type == 3) {
			if (p_dm->ext_pa)
				odm_write_4byte(p_dm, 0xc88, 0x821403e3);
			else
				odm_write_4byte(p_dm, 0xc88, 0x821403f7);
		}
	}


	odm_write_4byte(p_dm, 0xc80, 0x18008c10);/* TX_Tone_idx[9:0], TxK_Mask[29] TX_Tone = 16 */
	odm_write_4byte(p_dm, 0xc84, 0x38008c10);/* RX_Tone_idx[9:0], RxK_Mask[29] */
	odm_write_4byte(p_dm, 0xce8, 0x00000000);
	odm_write_4byte(p_dm, 0xe80, 0x18008c10);/* TX_Tone_idx[9:0], TxK_Mask[29] TX_Tone = 16 */
	odm_write_4byte(p_dm, 0xe84, 0x38008c10);/* RX_Tone_idx[9:0], RxK_Mask[29] */
	odm_write_4byte(p_dm, 0xee8, 0x00000000);

	cal0_retry = 0;
	cal1_retry = 0;
	while (1) {
		/* one shot */
		odm_write_4byte(p_dm, 0xcb8, 0x00100000);/* cb8[20] �N SI/PI �ϥ��v���� iqk_dpk module */
		odm_write_4byte(p_dm, 0xeb8, 0x00100000);/* cb8[20] �N SI/PI �ϥ��v���� iqk_dpk module */
		odm_write_4byte(p_dm, 0x980, 0xfa000000);
		odm_write_4byte(p_dm, 0x980, 0xf8000000);

		ODM_delay_ms(10); /* delay 10ms */
		odm_write_4byte(p_dm, 0xcb8, 0x00000000);
		odm_write_4byte(p_dm, 0xeb8, 0x00000000);
		delay_count = 0;
		while (1) {
			if (!TX0_finish)
				IQK0_ready = (boolean) odm_get_bb_reg(p_dm, 0xd00, BIT(10));
			if (!TX1_finish)
				IQK1_ready = (boolean) odm_get_bb_reg(p_dm, 0xd40, BIT(10));
			if ((IQK0_ready && IQK1_ready) || (delay_count > 20))
				break;
			else {
				ODM_delay_ms(1);
				delay_count++;
			}
		}
		ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("TX delay_count = %d\n", delay_count));
		if (delay_count < 20) {							/* If 20ms No Result, then cal_retry++ */
			/* ============TXIQK Check============== */
			TX0_fail = (boolean) odm_get_bb_reg(p_dm, 0xd00, BIT(12));
			TX1_fail = (boolean) odm_get_bb_reg(p_dm, 0xd40, BIT(12));
			if (!(TX0_fail || TX0_finish)) {
				odm_write_4byte(p_dm, 0xcb8, 0x02000000);
				TX_IQC_temp[tx0_average][0] = odm_get_bb_reg(p_dm, 0xd00, 0x07ff0000) << 21;
				odm_write_4byte(p_dm, 0xcb8, 0x04000000);
				TX_IQC_temp[tx0_average][1] = odm_get_bb_reg(p_dm, 0xd00, 0x07ff0000) << 21;
				ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("TX_X0[%d] = %x ;; TX_Y0[%d] = %x\n", tx0_average, (TX_IQC_temp[tx0_average][0]) >> 21 & 0x000007ff, tx0_average, (TX_IQC_temp[tx0_average][1]) >> 21 & 0x000007ff));
				/*

				odm_write_4byte(p_dm, 0xcb8, 0x01000000);
				reg1 = odm_get_bb_reg(p_dm, 0xd00, 0xffffffff);
				odm_write_4byte(p_dm, 0xcb8, 0x02000000);
				reg2 = odm_get_bb_reg(p_dm, 0xd00, 0x0000001f);
				image_power = (reg2<<32)+reg1;
				dbg_print("Before PW = %d\n", image_power);
				odm_write_4byte(p_dm, 0xcb8, 0x03000000);
				reg1 = odm_get_bb_reg(p_dm, 0xd00, 0xffffffff);
				odm_write_4byte(p_dm, 0xcb8, 0x04000000);
				reg2 = odm_get_bb_reg(p_dm, 0xd00, 0x0000001f);
				image_power = (reg2<<32)+reg1;
				dbg_print("After PW = %d\n", image_power);
				*/
				tx0_average++;
			} else {
				cal0_retry++;
				if (cal0_retry == 10)
					break;
			}
			if (!(TX1_fail || TX1_finish)) {
				odm_write_4byte(p_dm, 0xeb8, 0x02000000);
				TX_IQC_temp[tx1_average][2] = odm_get_bb_reg(p_dm, 0xd40, 0x07ff0000) << 21;
				odm_write_4byte(p_dm, 0xeb8, 0x04000000);
				TX_IQC_temp[tx1_average][3] = odm_get_bb_reg(p_dm, 0xd40, 0x07ff0000) << 21;
				ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("TX_X1[%d] = %x ;; TX_Y1[%d] = %x\n", tx1_average, (TX_IQC_temp[tx1_average][2]) >> 21 & 0x000007ff, tx1_average, (TX_IQC_temp[tx1_average][3]) >> 21 & 0x000007ff));
				/*
				int			reg1 = 0, reg2 = 0, image_power = 0;
				odm_write_4byte(p_dm, 0xeb8, 0x01000000);
				reg1 = odm_get_bb_reg(p_dm, 0xd40, 0xffffffff);
				odm_write_4byte(p_dm, 0xeb8, 0x02000000);
				reg2 = odm_get_bb_reg(p_dm, 0xd40, 0x0000001f);
				image_power = (reg2<<32)+reg1;
				dbg_print("Before PW = %d\n", image_power);
				odm_write_4byte(p_dm, 0xeb8, 0x03000000);
				reg1 = odm_get_bb_reg(p_dm, 0xd40, 0xffffffff);
				odm_write_4byte(p_dm, 0xeb8, 0x04000000);
				reg2 = odm_get_bb_reg(p_dm, 0xd40, 0x0000001f);
				image_power = (reg2<<32)+reg1;
				dbg_print("After PW = %d\n", image_power);
				*/
				tx1_average++;
			} else {
				cal1_retry++;
				if (cal1_retry == 10)
					break;
			}
		} else {
			cal0_retry++;
			cal1_retry++;
			ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("delay 20ms TX IQK Not Ready!!!!!\n"));
			if (cal0_retry == 10)
				break;
		}
		if (tx0_average >= 2) {
			for (i = 0; i < tx0_average; i++) {
				for (ii = i + 1; ii < tx0_average; ii++) {
					dx = (TX_IQC_temp[i][0] >> 21) - (TX_IQC_temp[ii][0] >> 21);
					if (dx < 4 && dx > -4) {
						dy = (TX_IQC_temp[i][1] >> 21) - (TX_IQC_temp[ii][1] >> 21);
						if (dy < 4 && dy > -4) {
							TX_IQC[0] = ((TX_IQC_temp[i][0] >> 21) + (TX_IQC_temp[ii][0] >> 21)) / 2;
							TX_IQC[1] = ((TX_IQC_temp[i][1] >> 21) + (TX_IQC_temp[ii][1] >> 21)) / 2;
							ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("TXA_X = %x;;TXA_Y = %x\n", TX_IQC[0] & 0x000007ff, TX_IQC[1] & 0x000007ff));
							TX0_finish = true;
						}
					}
				}
			}
		}
		if (tx1_average >= 2) {
			for (i = 0; i < tx1_average; i++) {
				for (ii = i + 1; ii < tx1_average; ii++) {
					dx = (TX_IQC_temp[i][2] >> 21) - (TX_IQC_temp[ii][2] >> 21);
					if (dx < 4 && dx > -4) {
						dy = (TX_IQC_temp[i][3] >> 21) - (TX_IQC_temp[ii][3] >> 21);
						if (dy < 4 && dy > -4) {
							TX_IQC[2] = ((TX_IQC_temp[i][2] >> 21) + (TX_IQC_temp[ii][2] >> 21)) / 2;
							TX_IQC[3] = ((TX_IQC_temp[i][3] >> 21) + (TX_IQC_temp[ii][3] >> 21)) / 2;
							ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("TXB_X = %x;;TXB_Y = %x\n", TX_IQC[2] & 0x000007ff, TX_IQC[3] & 0x000007ff));
							TX1_finish = true;
						}
					}
				}
			}
		}
		ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("tx0_average = %d, tx1_average = %d\n", tx0_average, tx1_average));
		ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("TX0_finish = %d, TX1_finish = %d\n", TX0_finish, TX1_finish));
		if (TX0_finish && TX1_finish)
			break;
		if ((cal0_retry + tx0_average) >= 10 || (cal1_retry + tx1_average) >= 10)
			break;
	}
	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("TXA_cal_retry = %d\n", cal0_retry));
	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("TXB_cal_retry = %d\n", cal1_retry));



	odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	odm_set_rf_reg(p_dm, RF_PATH_A, 0x58, 0x7fe00, odm_get_rf_reg(p_dm, RF_PATH_A, 0x8, 0xffc00)); /* Load LOK */
	odm_set_rf_reg(p_dm, RF_PATH_B, 0x58, 0x7fe00, odm_get_rf_reg(p_dm, RF_PATH_B, 0x8, 0xffc00)); /* Load LOK */
	odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x1); /* [31] = 1 --> Page C1 */

	odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	if (TX0_finish) {
		/* ====== path A RX IQK RF setting====== */
		odm_set_rf_reg(p_dm, RF_PATH_A, 0xef, RFREGOFFSETMASK, 0x80000);
		odm_set_rf_reg(p_dm, RF_PATH_A, 0x30, RFREGOFFSETMASK, 0x30000);
		odm_set_rf_reg(p_dm, RF_PATH_A, 0x31, RFREGOFFSETMASK, 0x3f7ff);
		odm_set_rf_reg(p_dm, RF_PATH_A, 0x32, RFREGOFFSETMASK, 0xfe7bf);
		odm_set_rf_reg(p_dm, RF_PATH_A, 0x8f, RFREGOFFSETMASK, 0x88001);
		odm_set_rf_reg(p_dm, RF_PATH_A, 0x65, RFREGOFFSETMASK, 0x931d1);
		odm_set_rf_reg(p_dm, RF_PATH_A, 0xef, RFREGOFFSETMASK, 0x00000);
	}
	if (TX1_finish) {
		/* ====== path B RX IQK RF setting====== */
		odm_set_rf_reg(p_dm, RF_PATH_B, 0xef, RFREGOFFSETMASK, 0x80000);
		odm_set_rf_reg(p_dm, RF_PATH_B, 0x30, RFREGOFFSETMASK, 0x30000);
		odm_set_rf_reg(p_dm, RF_PATH_B, 0x31, RFREGOFFSETMASK, 0x3f7ff);
		odm_set_rf_reg(p_dm, RF_PATH_B, 0x32, RFREGOFFSETMASK, 0xfe7bf);
		odm_set_rf_reg(p_dm, RF_PATH_B, 0x8f, RFREGOFFSETMASK, 0x88001);
		odm_set_rf_reg(p_dm, RF_PATH_B, 0x65, RFREGOFFSETMASK, 0x931d1);
		odm_set_rf_reg(p_dm, RF_PATH_B, 0xef, RFREGOFFSETMASK, 0x00000);
	}
	odm_set_bb_reg(p_dm, 0x978, BIT(31), 0x1);
	odm_set_bb_reg(p_dm, 0x97c, BIT(31), 0x0);
	odm_write_4byte(p_dm, 0x90c, 0x00008000);
	if (p_dm->support_interface == ODM_ITRF_PCIE)
		odm_write_4byte(p_dm, 0x984, 0x0046a911);
	else
		odm_write_4byte(p_dm, 0x984, 0x0046a890);
	/* odm_write_4byte(p_dm, 0x984, 0x0046a890); */
	if (p_dm->rfe_type == 1) {
		odm_write_4byte(p_dm, 0xcb0, 0x77777717);
		odm_write_4byte(p_dm, 0xcb4, 0x00000077);
		odm_write_4byte(p_dm, 0xeb0, 0x77777717);
		odm_write_4byte(p_dm, 0xeb4, 0x00000077);
	} else {
		odm_write_4byte(p_dm, 0xcb0, 0x77777717);
		odm_write_4byte(p_dm, 0xcb4, 0x02000077);
		odm_write_4byte(p_dm, 0xeb0, 0x77777717);
		odm_write_4byte(p_dm, 0xeb4, 0x02000077);
	}


	odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x1); /* [31] = 1 --> Page C1 */
	if (TX0_finish) {
		odm_write_4byte(p_dm, 0xc80, 0x38008c10);/* TX_Tone_idx[9:0], TxK_Mask[29] TX_Tone = 16 */
		odm_write_4byte(p_dm, 0xc84, 0x18008c10);/* RX_Tone_idx[9:0], RxK_Mask[29] */
		odm_write_4byte(p_dm, 0xc88, 0x82140119);
	}
	if (TX1_finish) {
		odm_write_4byte(p_dm, 0xe80, 0x38008c10);/* TX_Tone_idx[9:0], TxK_Mask[29] TX_Tone = 16 */
		odm_write_4byte(p_dm, 0xe84, 0x18008c10);/* RX_Tone_idx[9:0], RxK_Mask[29] */
		odm_write_4byte(p_dm, 0xe88, 0x82140119);
	}
	cal0_retry = 0;
	cal1_retry = 0;
	while (1) {
		/* one shot */
		odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
		if (TX0_finish) {
			odm_set_bb_reg(p_dm, 0x978, 0x03FF8000, (TX_IQC[0]) & 0x000007ff);
			odm_set_bb_reg(p_dm, 0x978, 0x000007FF, (TX_IQC[1]) & 0x000007ff);
			odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x1); /* [31] = 1 --> Page C1 */
			if (p_dm->rfe_type == 1)
				odm_write_4byte(p_dm, 0xc8c, 0x28161500);
			else
				odm_write_4byte(p_dm, 0xc8c, 0x28160cc0);
			odm_write_4byte(p_dm, 0xcb8, 0x00300000);/* cb8[20] �N SI/PI �ϥ��v���� iqk_dpk module */
			odm_write_4byte(p_dm, 0xcb8, 0x00100000);
			ODM_delay_ms(5); /* delay 5ms */
			odm_write_4byte(p_dm, 0xc8c, 0x3c000000);
			odm_write_4byte(p_dm, 0xcb8, 0x00000000);
		}
		if (TX1_finish) {
			odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
			odm_set_bb_reg(p_dm, 0x978, 0x03FF8000, (TX_IQC[2]) & 0x000007ff);
			odm_set_bb_reg(p_dm, 0x978, 0x000007FF, (TX_IQC[3]) & 0x000007ff);
			odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x1); /* [31] = 1 --> Page C1 */
			if (p_dm->rfe_type == 1)
				odm_write_4byte(p_dm, 0xe8c, 0x28161500);
			else
				odm_write_4byte(p_dm, 0xe8c, 0x28160ca0);
			odm_write_4byte(p_dm, 0xeb8, 0x00300000);/* cb8[20] �N SI/PI �ϥ��v���� iqk_dpk module */
			odm_write_4byte(p_dm, 0xeb8, 0x00100000);/* cb8[20] �N SI/PI �ϥ��v���� iqk_dpk module */
			ODM_delay_ms(5); /* delay 5ms */
			odm_write_4byte(p_dm, 0xe8c, 0x3c000000);
			odm_write_4byte(p_dm, 0xeb8, 0x00000000);
		}
		delay_count = 0;
		while (1) {
			if (!RX0_finish && TX0_finish)
				IQK0_ready = (boolean) odm_get_bb_reg(p_dm, 0xd00, BIT(10));
			if (!RX1_finish && TX1_finish)
				IQK1_ready = (boolean) odm_get_bb_reg(p_dm, 0xd40, BIT(10));
			if ((IQK0_ready && IQK1_ready) || (delay_count > 20))
				break;
			else {
				ODM_delay_ms(1);
				delay_count++;
			}
		}
		ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RX delay_count = %d\n", delay_count));
		if (delay_count < 20) {	/* If 20ms No Result, then cal_retry++ */
			/* ============RXIQK Check============== */
			RX0_fail = (boolean) odm_get_bb_reg(p_dm, 0xd00, BIT(11));
			RX1_fail = (boolean) odm_get_bb_reg(p_dm, 0xd40, BIT(11));
			if (!(RX0_fail || RX0_finish) && TX0_finish) {
				odm_write_4byte(p_dm, 0xcb8, 0x06000000);
				RX_IQC_temp[rx0_average][0] = odm_get_bb_reg(p_dm, 0xd00, 0x07ff0000) << 21;
				odm_write_4byte(p_dm, 0xcb8, 0x08000000);
				RX_IQC_temp[rx0_average][1] = odm_get_bb_reg(p_dm, 0xd00, 0x07ff0000) << 21;
				ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RX_X0[%d] = %x ;; RX_Y0[%d] = %x\n", rx0_average, (RX_IQC_temp[rx0_average][0]) >> 21 & 0x000007ff, rx0_average, (RX_IQC_temp[rx0_average][1]) >> 21 & 0x000007ff));
				/*
									odm_write_4byte(p_dm, 0xcb8, 0x05000000);
									reg1 = odm_get_bb_reg(p_dm, 0xd00, 0xffffffff);
									odm_write_4byte(p_dm, 0xcb8, 0x06000000);
									reg2 = odm_get_bb_reg(p_dm, 0xd00, 0x0000001f);
									dbg_print("reg1 = %d, reg2 = %d", reg1, reg2);
									image_power = (reg2<<32)+reg1;
									dbg_print("Before PW = %d\n", image_power);
									odm_write_4byte(p_dm, 0xcb8, 0x07000000);
									reg1 = odm_get_bb_reg(p_dm, 0xd00, 0xffffffff);
									odm_write_4byte(p_dm, 0xcb8, 0x08000000);
									reg2 = odm_get_bb_reg(p_dm, 0xd00, 0x0000001f);
									image_power = (reg2<<32)+reg1;
									dbg_print("After PW = %d\n", image_power);
				*/
				rx0_average++;
			} else {
				ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("1. RXA_cal_retry = %d\n", cal0_retry));
				cal0_retry++;
				if (cal0_retry == 10)
					break;
			}
			if (!(RX1_fail || RX1_finish) && TX1_finish) {
				odm_write_4byte(p_dm, 0xeb8, 0x06000000);
				RX_IQC_temp[rx1_average][2] = odm_get_bb_reg(p_dm, 0xd40, 0x07ff0000) << 21;
				odm_write_4byte(p_dm, 0xeb8, 0x08000000);
				RX_IQC_temp[rx1_average][3] = odm_get_bb_reg(p_dm, 0xd40, 0x07ff0000) << 21;
				ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RX_X1[%d] = %x ;; RX_Y1[%d] = %x\n", rx1_average, (RX_IQC_temp[rx1_average][2]) >> 21 & 0x000007ff, rx1_average, (RX_IQC_temp[rx1_average][3]) >> 21 & 0x000007ff));
				/*
									odm_write_4byte(p_dm, 0xeb8, 0x05000000);
									reg1 = odm_get_bb_reg(p_dm, 0xd40, 0xffffffff);
									odm_write_4byte(p_dm, 0xeb8, 0x06000000);
									reg2 = odm_get_bb_reg(p_dm, 0xd40, 0x0000001f);
									dbg_print("reg1 = %d, reg2 = %d", reg1, reg2);
									image_power = (reg2<<32)+reg1;
									dbg_print("Before PW = %d\n", image_power);
									odm_write_4byte(p_dm, 0xeb8, 0x07000000);
									reg1 = odm_get_bb_reg(p_dm, 0xd40, 0xffffffff);
									odm_write_4byte(p_dm, 0xeb8, 0x08000000);
									reg2 = odm_get_bb_reg(p_dm, 0xd40, 0x0000001f);
									image_power = (reg2<<32)+reg1;
									dbg_print("After PW = %d\n", image_power);
				*/
				rx1_average++;
			} else {
				cal1_retry++;
				if (cal1_retry == 10)
					break;
			}

		} else {
			ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("2. RXA_cal_retry = %d\n", cal0_retry));
			cal0_retry++;
			cal1_retry++;
			ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("delay 20ms RX IQK Not Ready!!!!!\n"));
			if (cal0_retry == 10)
				break;
		}
		ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("3. RXA_cal_retry = %d\n", cal0_retry));
		if (rx0_average >= 2) {
			for (i = 0; i < rx0_average; i++) {
				for (ii = i + 1; ii < rx0_average; ii++) {
					dx = (RX_IQC_temp[i][0] >> 21) - (RX_IQC_temp[ii][0] >> 21);
					if (dx < 4 && dx > -4) {
						dy = (RX_IQC_temp[i][1] >> 21) - (RX_IQC_temp[ii][1] >> 21);
						if (dy < 4 && dy > -4) {
							RX_IQC[0] = ((RX_IQC_temp[i][0] >> 21) + (RX_IQC_temp[ii][0] >> 21)) / 2;
							RX_IQC[1] = ((RX_IQC_temp[i][1] >> 21) + (RX_IQC_temp[ii][1] >> 21)) / 2;
							RX0_finish = true;
							break;
						}
					}
				}
			}
		}
		if (rx1_average >= 2) {
			for (i = 0; i < rx1_average; i++) {
				for (ii = i + 1; ii < rx1_average; ii++) {
					dx = (RX_IQC_temp[i][2] >> 21) - (RX_IQC_temp[ii][2] >> 21);
					if (dx < 4 && dx > -4) {
						dy = (RX_IQC_temp[i][3] >> 21) - (RX_IQC_temp[ii][3] >> 21);
						if (dy < 4 && dy > -4) {
							RX_IQC[2] = ((RX_IQC_temp[i][2] >> 21) + (RX_IQC_temp[ii][2] >> 21)) / 2;
							RX_IQC[3] = ((RX_IQC_temp[i][3] >> 21) + (RX_IQC_temp[ii][3] >> 21)) / 2;
							RX1_finish = true;
							break;
						}
					}
				}
			}
		}
		ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("rx0_average = %d, rx1_average = %d\n", rx0_average, rx1_average));
		ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RX0_finish = %d, RX1_finish = %d\n", RX0_finish, RX1_finish));
		if ((RX0_finish || !TX0_finish) && (RX1_finish || !TX1_finish))
			break;
		if ((cal0_retry + rx0_average) >= 10 || (cal1_retry + rx1_average) >= 10 || rx0_average == 3 || rx1_average == 3)
			break;
	}
	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RXA_cal_retry = %d\n", cal0_retry));
	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RXB_cal_retry = %d\n", cal1_retry));


	/* FillIQK Result */
	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("========Path_A =======\n"));

	if (TX0_finish)
		_iqk_tx_fill_iqc_8812a(p_dm, RF_PATH_A, TX_IQC[0], TX_IQC[1]);
	else
		_iqk_tx_fill_iqc_8812a(p_dm, RF_PATH_A, 0x200, 0x0);



	if (RX0_finish)
		_iqk_rx_fill_iqc_8812a(p_dm, RF_PATH_A, RX_IQC[0], RX_IQC[1]);
	else
		_iqk_rx_fill_iqc_8812a(p_dm, RF_PATH_A, 0x200, 0x0);

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("========Path_B =======\n"));

	if (TX1_finish)
		_iqk_tx_fill_iqc_8812a(p_dm, RF_PATH_B, TX_IQC[2], TX_IQC[3]);
	else
		_iqk_tx_fill_iqc_8812a(p_dm, RF_PATH_B, 0x200, 0x0);



	if (RX1_finish)
		_iqk_rx_fill_iqc_8812a(p_dm, RF_PATH_B, RX_IQC[2], RX_IQC[3]);
	else
		_iqk_rx_fill_iqc_8812a(p_dm, RF_PATH_B, 0x200, 0x0);



}

#define TARGET_CHNL_NUM_2G_5G	59

u8 get_right_chnl_place_for_iqk(u8 chnl)
{
	u8	channel_all[TARGET_CHNL_NUM_2G_5G] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 100,
		102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126, 128, 130, 132, 134, 136, 138, 140, 149, 151, 153, 155, 157, 159, 161, 163, 165
						};
	u8	place = chnl;


	if (chnl > 14) {
		for (place = 14; place < sizeof(channel_all); place++) {
			if (channel_all[place] == chnl)
				return place - 13;
		}
	}

	return 0;
}

#define MACBB_REG_NUM 9
#define AFE_REG_NUM 12
#define RF_REG_NUM 3

/* Maintained by BB James. */
void
_phy_iq_calibrate_8812a(
	struct PHY_DM_STRUCT		*p_dm,
	u8		channel
)
{
	u32	MACBB_backup[MACBB_REG_NUM], AFE_backup[AFE_REG_NUM], RFA_backup[RF_REG_NUM], RFB_backup[RF_REG_NUM];
	u32	backup_macbb_reg[MACBB_REG_NUM] = {0x520, 0x550, 0x808, 0xa04, 0x90c, 0xc00, 0xe00, 0x838,  0x82c};
	u32	backup_afe_reg[AFE_REG_NUM] = {0xc5c, 0xc60, 0xc64, 0xc68, 0xcb0, 0xcb4,
				       0xe5c, 0xe60, 0xe64, 0xe68, 0xeb0, 0xeb4
					  };
	u32	reg_c1b8, reg_e1b8;
	u32	backup_rf_reg[RF_REG_NUM] = {0x65, 0x8f, 0x0};
	u8	chnl_idx = get_right_chnl_place_for_iqk(channel);

	_iqk_backup_mac_bb_8812a(p_dm, MACBB_backup, backup_macbb_reg, MACBB_REG_NUM);
	odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x1);
	reg_c1b8 = odm_read_4byte(p_dm, 0xcb8);
	reg_e1b8 = odm_read_4byte(p_dm, 0xeb8);
	odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x0);
	_iqk_backup_afe_8812a(p_dm, AFE_backup, backup_afe_reg, AFE_REG_NUM);
	_iqk_backup_rf_8812a(p_dm, RFA_backup, RFB_backup, backup_rf_reg, RF_REG_NUM);

	_iqk_configure_mac_8812a(p_dm);
	_iqk_tx_8812a(p_dm, chnl_idx);
	_iqk_restore_rf_8812a(p_dm, RF_PATH_A, backup_rf_reg, RFA_backup, RF_REG_NUM);
	_iqk_restore_rf_8812a(p_dm, RF_PATH_B, backup_rf_reg, RFB_backup, RF_REG_NUM);

	_iqk_restore_afe_8812a(p_dm, AFE_backup, backup_afe_reg, AFE_REG_NUM);
	odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x1);
	odm_write_4byte(p_dm, 0xcb8, reg_c1b8);
	odm_write_4byte(p_dm, 0xeb8, reg_e1b8);
	odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x0);
	_iqk_restore_mac_bb_8812a(p_dm, MACBB_backup, backup_macbb_reg, MACBB_REG_NUM);


}

void
_phy_lc_calibrate_8812a(
	struct PHY_DM_STRUCT	*p_dm,
	boolean		is2T
)
{
	u32	/*rf_amode=0, rf_bmode=0,*/ lc_cal = 0, tmp = 0;

	/* Check continuous TX and Packet TX */
	u32	reg0x914 = odm_read_4byte(p_dm, REG_SINGLE_TONE_CONT_TX_JAGUAR);;

	/* Backup RF reg18. */
	lc_cal = odm_get_rf_reg(p_dm, RF_PATH_A, RF_CHNLBW, RFREGOFFSETMASK);

	if ((reg0x914 & 0x70000) != 0)	/* If contTx, disable all continuous TX. 0x914[18:16] */
		/* <20121121, Kordan> A workaround: If we set 0x914[18:16] as zero, BB turns off ContTx */
		/* until another packet comes in. To avoid ContTx being turned off, we skip this step. */
		;/* odm_write_4byte(p_dm, REG_SINGLE_TONE_CONT_TX_JAGUAR, reg0x914 & (~0x70000)); */
	else							/* If packet Tx-ing, pause Tx. */
		odm_write_1byte(p_dm, REG_TXPAUSE_8812A, 0xFF);


#if 0
	/* 3 1. Read original RF mode */
	rf_amode = odm_get_rf_reg(p_dm, RF_PATH_A, RF_AC, RFREGOFFSETMASK);
	if (is2T)
		rf_bmode = odm_get_rf_reg(p_dm, RF_PATH_B, RF_AC, RFREGOFFSETMASK);


	/* 3 2. Set RF mode = standby mode */
	odm_set_rf_reg(p_dm, RF_PATH_A, RF_AC, RFREGOFFSETMASK, (rf_amode & 0x8FFFF) | 0x10000);
	if (is2T)
		odm_set_rf_reg(p_dm, RF_PATH_B, RF_AC, RFREGOFFSETMASK, (rf_bmode & 0x8FFFF) | 0x10000);
#endif

	/* Enter LCK mode */
	tmp = odm_get_rf_reg(p_dm, RF_PATH_A, RF_LCK, RFREGOFFSETMASK);
	odm_set_rf_reg(p_dm, RF_PATH_A, RF_LCK, RFREGOFFSETMASK, tmp | BIT(14));

	/* 3 3. Read RF reg18 */
	lc_cal = odm_get_rf_reg(p_dm, RF_PATH_A, RF_CHNLBW, RFREGOFFSETMASK);

	/* 3 4. Set LC calibration begin bit15 */
	odm_set_rf_reg(p_dm, RF_PATH_A, RF_CHNLBW, RFREGOFFSETMASK, lc_cal | 0x08000);

	ODM_delay_ms(150);		/* suggest by RFSI Binson */

	/* Leave LCK mode */
	tmp = odm_get_rf_reg(p_dm, RF_PATH_A, RF_LCK, RFREGOFFSETMASK);
	odm_set_rf_reg(p_dm, RF_PATH_A, RF_LCK, RFREGOFFSETMASK, tmp & ~BIT(14));

	/* 3 Restore original situation */
	if ((reg0x914 & 70000) != 0) {	/* Deal with contisuous TX case, 0x914[18:16] */
		/* <20121121, Kordan> A workaround: If we set 0x914[18:16] as zero, BB turns off ContTx */
		/* until another packet comes in. To avoid ContTx being turned off, we skip this step. */
		/* odm_write_4byte(p_dm, REG_SINGLE_TONE_CONT_TX_JAGUAR, reg0x914); */
	} else /* Deal with Packet TX case */
		odm_write_1byte(p_dm, REG_TXPAUSE_8812A, 0x00);

	/* Recover channel number */
	odm_set_rf_reg(p_dm, RF_PATH_A, RF_CHNLBW, RFREGOFFSETMASK, lc_cal);

	/*
	odm_set_rf_reg(p_dm, RF_PATH_A, RF_AC, RFREGOFFSETMASK, rf_amode);
	if(is2T)
		odm_set_rf_reg(p_dm, RF_PATH_B, RF_AC, RFREGOFFSETMASK, rf_bmode);
		*/

}

void
phy_reload_iqk_setting_8812a(
	struct PHY_DM_STRUCT	*p_dm,
	u8		channel
)
{
	struct odm_rf_calibration_structure  *p_rf_calibrate_info = &(p_dm->rf_calibrate_info);

	u8 chnl_idx = get_right_chnl_place_for_iqk(channel);
	odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x1); /* [31] = 1 --> Page C1 */
	odm_set_bb_reg(p_dm, 0xccc, 0x000007ff, p_rf_calibrate_info->iqk_matrix_reg_setting[chnl_idx].value[*p_dm->p_band_width][0] & 0x7ff);
	odm_set_bb_reg(p_dm, 0xcd4, 0x000007ff, (p_rf_calibrate_info->iqk_matrix_reg_setting[chnl_idx].value[*p_dm->p_band_width][0] & 0x7ff0000) >> 16);
	odm_set_bb_reg(p_dm, 0xecc, 0x000007ff, p_rf_calibrate_info->iqk_matrix_reg_setting[chnl_idx].value[*p_dm->p_band_width][2] & 0x7ff);
	odm_set_bb_reg(p_dm, 0xed4, 0x000007ff, (p_rf_calibrate_info->iqk_matrix_reg_setting[chnl_idx].value[*p_dm->p_band_width][2] & 0x7ff0000) >> 16);

	if (*p_dm->p_band_width != 2) {
		odm_write_4byte(p_dm, 0xce8, 0x0);
		odm_write_4byte(p_dm, 0xee8, 0x0);
	} else {
		odm_write_4byte(p_dm, 0xce8, p_rf_calibrate_info->iqk_matrix_reg_setting[chnl_idx].value[*p_dm->p_band_width][4]);
		odm_write_4byte(p_dm, 0xee8, p_rf_calibrate_info->iqk_matrix_reg_setting[chnl_idx].value[*p_dm->p_band_width][5]);
	}
	odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	odm_set_bb_reg(p_dm, 0xc10, 0x000003ff, (p_rf_calibrate_info->iqk_matrix_reg_setting[chnl_idx].value[*p_dm->p_band_width][1] & 0x7ff0000) >> 17);
	odm_set_bb_reg(p_dm, 0xc10, 0x03ff0000, (p_rf_calibrate_info->iqk_matrix_reg_setting[chnl_idx].value[*p_dm->p_band_width][1] & 0x7ff) >> 1);
	odm_set_bb_reg(p_dm, 0xe10, 0x000003ff, (p_rf_calibrate_info->iqk_matrix_reg_setting[chnl_idx].value[*p_dm->p_band_width][3] & 0x7ff0000) >> 17);
	odm_set_bb_reg(p_dm, 0xe10, 0x03ff0000, (p_rf_calibrate_info->iqk_matrix_reg_setting[chnl_idx].value[*p_dm->p_band_width][3] & 0x7ff) >> 1);


}

void
phy_reset_iqk_result_8812a(
	struct PHY_DM_STRUCT	*p_dm
)
{
	odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x1); /* [31] = 1 --> Page C1 */
	odm_set_bb_reg(p_dm, 0xccc, 0x000007ff, 0x0);
	odm_set_bb_reg(p_dm, 0xcd4, 0x000007ff, 0x200);
	odm_set_bb_reg(p_dm, 0xecc, 0x000007ff, 0x0);
	odm_set_bb_reg(p_dm, 0xed4, 0x000007ff, 0x200);
	odm_write_4byte(p_dm, 0xce8, 0x0);
	odm_write_4byte(p_dm, 0xee8, 0x0);
	odm_set_bb_reg(p_dm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	odm_set_bb_reg(p_dm, 0xc10, 0x000003ff, 0x100);
	odm_set_bb_reg(p_dm, 0xe10, 0x000003ff, 0x100);
}

void
_phy_iq_calibrate_by_fw_8812a(
	struct PHY_DM_STRUCT	*p_dm
)
{
	u8			iqk_cmd[3] = { *p_dm->p_channel, 0x0, 0x0};
	u8			buf1 = 0x0;
	u8			buf2 = 0x0;

	/* Byte 2, Bit 4 ~ Bit 5 : band_type */
	if (*p_dm->p_band_type == ODM_BAND_5G)
		buf1 = 0x2 << 4;
	else
		buf1 = 0x1 << 4;

	/* Byte 2, Bit 0 ~ Bit 3 : bandwidth */
	if (*p_dm->p_band_width == CHANNEL_WIDTH_20)
		buf2 = 0x1;
	else if (*p_dm->p_band_width == CHANNEL_WIDTH_40)
		buf2 = 0x1 << 1;
	else if (*p_dm->p_band_width == CHANNEL_WIDTH_80)
		buf2 = 0x1 << 2;
	else
		buf2 = 0x1 << 3;

	iqk_cmd[1] = buf1 | buf2;
	iqk_cmd[2] =  p_dm->ext_pa_5g | p_dm->ext_pa << 1 | p_dm->support_interface << 2 | p_dm->rfe_type << 5;

	odm_fill_h2c_cmd(p_dm, ODM_H2C_IQ_CALIBRATION, 3, iqk_cmd);
}

void
phy_iq_calibrate_8812a(
	void		*p_dm_void,
	boolean	is_recovery
)
{
	struct PHY_DM_STRUCT	*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct odm_rf_calibration_structure	*p_rf_calibrate_info = &(p_dm->rf_calibrate_info);
	u32			counter = 0;

	if (p_dm->fw_offload_ability & PHYDM_RF_IQK_OFFLOAD) {
		_phy_iq_calibrate_by_fw_8812a(p_dm);
		for (counter = 0; counter < 10; counter++) {
			ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("== FW IQK PROGRESS == #%d\n", counter));
			delay_ms(50);
			if (!p_rf_calibrate_info->is_iqk_in_progress) {
				ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("== FW IQK RETURN FROM WAITING ==\n"));
				break;
			}
		}
		if (p_rf_calibrate_info->is_iqk_in_progress)
			ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("== FW IQK TIMEOUT (Still in progress after 500ms) ==\n"));
	} else
		_phy_iq_calibrate_8812a(p_dm, *p_dm->p_channel);
}


void
phy_lc_calibrate_8812a(
	void		*p_dm_void
)
{
	struct PHY_DM_STRUCT	*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;

	_phy_lc_calibrate_8812a(p_dm, true);
}

void _phy_set_rf_path_switch_8812a(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct PHY_DM_STRUCT		*p_dm,
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
	struct PHY_DM_STRUCT		*p_dm = &p_hal_data->odmpriv;
#elif (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct PHY_DM_STRUCT		*p_dm = &p_hal_data->DM_OutSrc;
#endif

#endif

	if (IS_HARDWARE_TYPE_8821(p_adapter)) {
		if (is_main)
			odm_set_bb_reg(p_dm, REG_A_RFE_PINMUX_JAGUAR + 4, BIT(29) | BIT28, 0x1);	/* Main */
		else
			odm_set_bb_reg(p_dm, REG_A_RFE_PINMUX_JAGUAR + 4, BIT(29) | BIT28, 0x2);	/* Aux */
	} else if (p_dm->support_ic_type & ODM_RTL8812) {
		if (p_dm->rfe_type == 5) {
			if (is_main) {
				/* WiFi */
				odm_set_bb_reg(p_dm, REG_ANTSEL_SW_JAGUAR, BIT(1) | BIT(0), 0x2);
				odm_set_bb_reg(p_dm, REG_ANTSEL_SW_JAGUAR, BIT(9) | BIT(8), 0x3);
			} else {
				/* BT */
				odm_set_bb_reg(p_dm, REG_ANTSEL_SW_JAGUAR, BIT(1) | BIT(0), 0x1);
				odm_set_bb_reg(p_dm, REG_ANTSEL_SW_JAGUAR, BIT(9) | BIT(8), 0x3);
			}
		} else if (p_dm->rfe_type == 1) {
			/* <131224, VincentL> When rfe_type == 1 also Set 0x900, suggested by RF Binson. */
			if (is_main) {
				/* WiFi */
				odm_set_bb_reg(p_dm, REG_ANTSEL_SW_JAGUAR, BIT(1) | BIT(0), 0x2);
				odm_set_bb_reg(p_dm, REG_ANTSEL_SW_JAGUAR, BIT(9) | BIT(8), 0x3);
			} else {
				/* BT */
				odm_set_bb_reg(p_dm, REG_ANTSEL_SW_JAGUAR, BIT(1) | BIT(0), 0x1);
				odm_set_bb_reg(p_dm, REG_ANTSEL_SW_JAGUAR, BIT(9) | BIT(8), 0x3);
			}
		}

	}

}
void phy_set_rf_path_switch_8812a(
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct PHY_DM_STRUCT		*p_dm,
#else
	struct _ADAPTER	*p_adapter,
#endif
	boolean		is_main
)
{

#if DISABLE_BB_RF
	return;
#endif

#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)

	_phy_set_rf_path_switch_8812a(p_adapter, is_main, true);

#endif
}

#if 0
s32
_sign(
	u32 number
)
{
	if ((number & BIT(10)) == BIT10) {/* Negative */
		number &= (~0xFFFFFC00);         /* [9:0] */
		number = ~number;
		number &= (~0xFFFFFC00);         /* [9:0] */
		number += 1;
		number &= (~0xFFFFF800);		  /* [10:0] */
		return -1 * number;
	} else   /* Positive */
		return (s32)number;
}


void
_dpk_mac_bb_backup_path_a(
	struct PHY_DM_STRUCT	*p_dm,
	u32		*backup_reg_addr,
	u32		*backup_reg_data,
	u32		number
)
{
	u32 i;

	for (i = 0; i < number; i++)
		backup_reg_data[i] = odm_read_4byte(p_dm, backup_reg_addr[i]);
}
void
_dpk_mac_bb_restore_path_a(
	struct PHY_DM_STRUCT	*p_dm,
	u32		*backup_reg_addr,
	u32		*backup_reg_data,
	u32		number
)
{
	u32 i;

	for (i = 0; i < number; i++)
		odm_write_4byte(p_dm, backup_reg_addr[i], backup_reg_data[i]);
}


void
_dpk_rf_backup_path_a(
	struct PHY_DM_STRUCT	*p_dm,
	u32				*backup_reg_addr,
	enum rf_path	rf_path,
	u32				*backup_reg_data,
	u32		        number
)
{
	u32 i;

	for (i = 0; i < number; i++)
		backup_reg_data[i] = odm_get_rf_reg(p_dm, (enum rf_path)rf_path, backup_reg_addr[i], RFREGOFFSETMASK);
}

void
_dpk_rf_restore_path_a(
	struct PHY_DM_STRUCT	*p_dm,
	u32				*backup_reg_addr,
	enum rf_path	rf_path,
	u32		*backup_reg_data,
	u32		        number
)
{
	u32 i;

	for (i = 0; i < number; i++)
		odm_set_rf_reg(p_dm, (enum rf_path)rf_path, backup_reg_addr[i], RFREGOFFSETMASK, backup_reg_data[i]);
}



s32
_compute_loop_back_gain_path_a(
	struct PHY_DM_STRUCT			*p_dm
)
{
	/* compute loopback gain */
	/* �p��tmp_lb_gain �Ψ쪺function hex2dec() �O�N twos complement���Q���i���ন�Q�i��, �Q���i�쪺�榡��s11.10
	* �|��1, d00[10:0] = h'3ff, �g�L hex2dec(h'3ff) = 1023,
	* �|��2, d00[10:0] = h'400, �g�L hex2dec(h'400) = -1024,
	* �|��3, d00[10:0] = h'0A9, �g�L hex2dec(h'0A9) = 169,
	* �|��4, d00[10:0] = h'54c, �g�L hex2dec(h'54c) = -692,
	* �|��5, d00[10:0] = h'7ff, �g�L hex2dec(h'7ff) = -1, */
	u32 reg0xd00_26_16 = odm_get_bb_reg(p_dm, 0xD00, 0x7FF0000);
	u32 reg0xd00_10_0  = odm_get_bb_reg(p_dm, 0xD00, 0x7FF);

	s32 tmp_lb_gain = _sign(reg0xd00_26_16) * _sign(reg0xd00_26_16) + _sign(reg0xd00_10_0) * _sign(reg0xd00_10_0);

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("<=== _compute_loop_back_gain_path_a, tmp_lb_gain = 0x%X\n", tmp_lb_gain));

	return tmp_lb_gain;
}


boolean
_fine_tune_loop_back_gain_path_a(
	struct PHY_DM_STRUCT			*p_dm,
	u32				dpk_tx_agc
)
{
	struct odm_rf_calibration_structure  *p_rf_calibrate_info = &(p_dm->rf_calibrate_info);
	s32 tmp_lb_gain = 0;

	u32 rf0x64_orig = odm_get_rf_reg(p_dm, RF_PATH_A, 0x64, 0x7);
	u32 rf0x64_new = rf0x64_orig;

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("===> _FineTuneLoopBackGain\n"));

	do {
		/* RF setting */
		odm_set_rf_reg(p_dm, RF_PATH_A, 0x00, RFREGOFFSETMASK, dpk_tx_agc);
		odm_write_4byte(p_dm, 0x82c, 0x802083dd);
		/* �`�Npage c1��c90�bDPK�L�{���d�U����Q��Lthread or process��g��page c��c90�� */
		odm_write_4byte(p_dm, 0xc90, 0x0101f018);

		/* one shot */
		odm_write_4byte(p_dm, 0xcc8, 0x800c5599);
		odm_write_4byte(p_dm, 0xcc8, 0x000c5599);
		/* delay 50 ms,�� delay �ɶ����@�I, �T�w PA Scan function ������ */
		delay_ms(50);

		/* reg82c[31] = 0 --> ���� page C */
		odm_write_4byte(p_dm, 0x82c, 0x002083dd);
		odm_set_rf_reg(p_dm, RF_PATH_A, 0x00, RFREGOFFSETMASK, 0x33d8d);

		tmp_lb_gain = _compute_loop_back_gain_path_a(p_dm);

		if ((rf0x64_new == (BIT(2) | BIT(1) | BIT(0))) || (rf0x64_new == 0)) {
			/* printf('DPK Phase 1 failed'); */
			p_rf_calibrate_info->is_dpk_fail = true;
			break;
			/* Go to DPK Phase 5 */
		} else {
			if (tmp_lb_gain < 263390) {/* fine tune loopback path gain: newReg64[2:0] = reg64[2:0] - 3b'001 */
				rf0x64_new -= 1;
				odm_set_rf_reg(p_dm, RF_PATH_A, 0x64, 0x7, rf0x64_new);

			} else if (tmp_lb_gain > 661607) {/* fine tune loopback path gain: newReg64 [2:0] = reg64[2:0] + 3b'001 */
				rf0x64_new += 1;
				odm_set_rf_reg(p_dm, RF_PATH_A, 0x64, 0x7, rf0x64_new);

			} else
				p_rf_calibrate_info->is_dpk_fail = false;
		}

	} while (tmp_lb_gain < 263390 || 661607 < tmp_lb_gain);

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("<=== _FineTuneLoopBackGain\n"));

	return p_rf_calibrate_info->is_dpk_fail ? false : true;

}



void
_dpk_init_path_a(
	struct PHY_DM_STRUCT	*p_dm
)
{
	struct odm_rf_calibration_structure  *p_rf_calibrate_info = &(p_dm->rf_calibrate_info);

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("===> _DPK_Init\n"));

	/* TX pause */
	odm_write_1byte(p_dm, 0x522, 0x7f);

	/* reg82c[31] = b'0, ������ page C */
	odm_write_4byte(p_dm, 0x82c, 0x002083dd);

	/* AFE setting */
	odm_write_4byte(p_dm, 0xc68, 0x19791979);
	odm_write_4byte(p_dm, 0xc60, 0x77777777);
	odm_write_4byte(p_dm, 0xc64, 0x77777777);

	/* external TRSW ���� T */
	odm_write_4byte(p_dm, 0xcb0, 0x77777777);
	odm_write_4byte(p_dm, 0xcb4, 0x01000077);

	/* hardware 3-wire off */
	odm_write_4byte(p_dm, 0xc00, 0x00000004);

	/* CCA off */
	odm_write_4byte(p_dm, 0x838, 0x16C89B4c);

	/* 90c[15]: dac fifo reset by CSWU */
	odm_write_4byte(p_dm, 0x90c, 0x00008000);

	/* r_gothrough_iqkdpk */
	odm_write_4byte(p_dm, 0xc94, 0x0100005D);

	/* reg82c[31] = 1 --> ���� page C1 */
	odm_write_4byte(p_dm, 0x82c, 0x802083dd);

	/* IQK Amp off */
	odm_write_4byte(p_dm, 0xc8c, 0x3c000000);

	/* tx_amp */
	odm_write_4byte(p_dm, 0xc98, 0x41382e21);
	odm_write_4byte(p_dm, 0xc9c, 0x5b554f48);
	odm_write_4byte(p_dm, 0xca0, 0x6f6b6661);
	odm_write_4byte(p_dm, 0xca4, 0x817d7874);
	odm_write_4byte(p_dm, 0xca8, 0x908c8884);
	odm_write_4byte(p_dm, 0xcac, 0x9d9a9793);
	odm_write_4byte(p_dm, 0xcb0, 0xaaa7a4a1);
	odm_write_4byte(p_dm, 0xcb4, 0xb6b3b0ad);

	/* tx_inverse */
	odm_write_4byte(p_dm, 0xc40, 0x02ce03e9);
	odm_write_4byte(p_dm, 0xc44, 0x01fd0249);
	odm_write_4byte(p_dm, 0xc48, 0x01a101c9);
	odm_write_4byte(p_dm, 0xc4c, 0x016a0181);
	odm_write_4byte(p_dm, 0xc50, 0x01430155);
	odm_write_4byte(p_dm, 0xc54, 0x01270135);
	odm_write_4byte(p_dm, 0xc58, 0x0112011c);
	odm_write_4byte(p_dm, 0xc5c, 0x01000108);
	odm_write_4byte(p_dm, 0xc60, 0x00f100f8);
	odm_write_4byte(p_dm, 0xc64, 0x00e500eb);
	odm_write_4byte(p_dm, 0xc68, 0x00db00e0);
	odm_write_4byte(p_dm, 0xc6c, 0x00d100d5);
	odm_write_4byte(p_dm, 0xc70, 0x00c900cd);
	odm_write_4byte(p_dm, 0xc74, 0x00c200c5);
	odm_write_4byte(p_dm, 0xc78, 0x00bb00be);
	odm_write_4byte(p_dm, 0xc7c, 0x00b500b8);

	/* reg82c[31] = b'0, ������ page C */
	odm_write_4byte(p_dm, 0x82c, 0x002083dd);

	p_rf_calibrate_info->is_dpk_fail = false;

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("<=== _DPK_Init\n"));

}


boolean
_dpk_adjust_rf_gain_path_a(
	struct PHY_DM_STRUCT	*p_dm
)
{
	struct odm_rf_calibration_structure  *p_rf_calibrate_info = &(p_dm->rf_calibrate_info);
	s32 tmp_lb_gain = 0;

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("===> _DPK_AdjustRFGain\n"));

	/* RF setting */
	odm_set_rf_reg(p_dm, RF_PATH_A, 0x00, RFREGOFFSETMASK, 0x50bfc);
	/* Attn: A mode @ reg64[2:0], G mode @ reg56 */
	odm_set_rf_reg(p_dm, RF_PATH_A, 0x64, RFREGOFFSETMASK, 0x19aac);
	/* PGA gain: RF reg8f[14:13] */
	odm_set_rf_reg(p_dm, RF_PATH_A, 0x8f, RFREGOFFSETMASK, 0x8a001);

	/* reg82c[31] = 1 --> ���� page C1 */
	odm_write_4byte(p_dm, 0x82c, 0x802083dd);

	/* DPK setting */
	odm_write_4byte(p_dm, 0xc94, 0xf76c9f84);
	odm_write_4byte(p_dm, 0xcc8, 0x000c5599);
	odm_write_4byte(p_dm, 0xcc4, 0x11838000);
	odm_set_bb_reg(p_dm, 0xcd4, 0xFFF000, 0x100);  /* �Ncd4[23:12] �令 h'100, �䥦�줸�ЫO�d��Ȥ��n�g��
     * �`�Npage c1��c90�bDPK�L�{���d�U����Q��Lthread or process��g��page c��c90�� */
	odm_write_4byte(p_dm, 0xc90, 0x0101f018);

	/* one shot */
	odm_write_4byte(p_dm, 0xcc8, 0x800c5599);
	odm_write_4byte(p_dm, 0xcc8, 0x000c5599);
	/* delay 50 ms,�� delay �ɶ����@�I, �T�w PA Scan function ������ */
	delay_ms(50);

	/* read back Loopback Gain */
	odm_write_4byte(p_dm, 0xcb8, 0x09000000);

	/* reg82c[31] = 0 --> ���� page C */
	odm_write_4byte(p_dm, 0x82c, 0x002083dd);
	odm_set_rf_reg(p_dm, RF_PATH_A, 0x00, RFREGOFFSETMASK, 0x33d8d);

	tmp_lb_gain = _compute_loop_back_gain_path_a(p_dm);

	/* coarse tune loopback gain by RF reg8f[14:13] = 2b'11 */
	if (tmp_lb_gain < 263390) {
		odm_set_rf_reg(p_dm, RF_PATH_A, 0x8f, RFREGOFFSETMASK, 0x8e001);
		_fine_tune_loop_back_gain_path_a(p_dm, 0x50bfc);

	} else if (tmp_lb_gain > 661607) {
		/* coarse tune loopback gain by RF reg8f[14:13] = 2b'00 */
		odm_set_rf_reg(p_dm, RF_PATH_A, 0x8f, RFREGOFFSETMASK, 0x88001);
		_fine_tune_loop_back_gain_path_a(p_dm, 0x50bfc);
	} else
		p_rf_calibrate_info->is_dpk_fail = false;

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("<=== _DPK_AdjustRFGain\n"));

	return p_rf_calibrate_info->is_dpk_fail ? false : true;

}



void
_dpk_gain_loss_to_find_tx_agc_path_a(
	struct PHY_DM_STRUCT	*p_dm
)
{
	struct odm_rf_calibration_structure  *p_rf_calibrate_info = &(p_dm->rf_calibrate_info);
	u32 reg0xd00 = 0;

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("===> _DPK_GainLossToFindTxAGC\n"));

	/* RF setting */
	odm_set_rf_reg(p_dm, RF_PATH_A, 0x00, RFREGOFFSETMASK, 0x50bfc);

	/* reg82c[31] = 1 --> ���� page C1 */
	odm_write_4byte(p_dm, 0x82c, 0x802083dd);

	/* regc20[15:13] = dB sel, �i�D Gain Loss function �h�M�� dB_sel �ҳ]�w��PA gain loss�ؼЩҹ����� Tx AGC ����. */
	/* dB_sel = b'000 ' 1.0 dB PA gain loss */
	/* dB_sel = b'001 ' 1.5 dB PA gain loss */
	/* dB_sel = b'010 ' 2.0 dB PA gain loss */
	/* dB_sel = b'011 ' 2.5 dB PA gain loss */
	/* dB_sel = b'100 ' 3.0 dB PA gain loss */
	/* dB_sel = b'101 ' 3.5 dB PA gain loss */
	/* dB_sel = b'110 ' 4.0 dB PA gain loss */
	odm_write_4byte(p_dm, 0xc20, 0x00002000);

	/* DPK setting */
	odm_write_4byte(p_dm, 0xc94, 0xf76c9f84);
	odm_write_4byte(p_dm, 0xcc8, 0x000c5599);
	odm_write_4byte(p_dm, 0xcc4, 0x148b8000);

	/* �`�Npage c1��c90�bDPK�L�{���d�U����Q��Lthread or process��g��page c��c90�� */
	odm_write_4byte(p_dm, 0xc90, 0x0401f018);

	/* one shot */
	odm_write_4byte(p_dm, 0xcc8, 0x800c5599);
	odm_write_4byte(p_dm, 0xcc8, 0x000c5599);
	/* delay 50 ms,�� delay �ɶ����@�I, �T�w PA Scan function ������ */
	delay_ms(50);

	/* read back Loopback Gain */
	/* �i�H�b d00[3:0] ��Ū�^, dB_sel ���ҳ]�w�� gain loss �|���b���@�� Tx AGC �]�w */
	/* Ū�^d00[3:0] = h'1 ' Tx AGC = h'13 */
	/* Ū�^d00[3:0] = h'2 ' Tx AGC = h'14 */
	/* Ū�^d00[3:0] = h'3 ' Tx AGC = h'15 */
	/* Ū�^d00[3:0] = h'4 ' Tx AGC = h'16 */
	/* Ū�^d00[3:0] = h'5 ' Tx AGC = h'17 */
	/* Ū�^d00[3:0] = h'6 ' Tx AGC = h'18 */
	/* Ū�^d00[3:0] = h'7 ' Tx AGC = h'19 */
	/* Ū�^d00[3:0] = h'8 ' Tx AGC = h'1a */
	/* Ū�^d00[3:0] = h'9 ' Tx AGC = h'1b */
	/* Ū�^d00[3:0] = h'a ' Tx AGC = h'1c */
	/*  */
	reg0xd00 = odm_read_4byte(p_dm, 0xd00);
	switch (odm_read_4byte(p_dm, 0xd00) & (BIT(3) | BIT(2) | BIT(1) | BIT(0))) {
	case 0x0:
		p_rf_calibrate_info->dpk_tx_agc = 0x50bf0 | 0x2;
		break;
	case 0x1:
		p_rf_calibrate_info->dpk_tx_agc = 0x50bf0 | 0x3;
		break;
	case 0x2:
		p_rf_calibrate_info->dpk_tx_agc = 0x50bf0 | 0x4;
		break;
	case 0x3:
		p_rf_calibrate_info->dpk_tx_agc = 0x50bf0 | 0x5;
		break;
	case 0x4:
		p_rf_calibrate_info->dpk_tx_agc = 0x50bf0 | 0x6;
		break;
	case 0x5:
		p_rf_calibrate_info->dpk_tx_agc = 0x50bf0 | 0x7;
		break;
	case 0x6:
		p_rf_calibrate_info->dpk_tx_agc = 0x50bf0 | 0x8;
		break;
	case 0x7:
		p_rf_calibrate_info->dpk_tx_agc = 0x50bf0 | 0x9;
		break;
	case 0x8:
		p_rf_calibrate_info->dpk_tx_agc = 0x50bf0 | 0xa;
		break;
	case 0x9:
		p_rf_calibrate_info->dpk_tx_agc = 0x50bf0 | 0xb;
		break;
	case 0xa:
		p_rf_calibrate_info->dpk_tx_agc = 0x50bf0 | 0xc;
		break;
	}
	/* reg82c[31] = 0 --> ���� page C */
	odm_write_4byte(p_dm, 0x82c, 0x002083dd);
	odm_set_rf_reg(p_dm, RF_PATH_A, 0x00, RFREGOFFSETMASK, 0x33d8d);

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("<=== _DPK_GainLossToFindTxAGC\n"));
}


boolean
_dpk_adjust_rf_gain_by_found_tx_agc_path_a
(
	struct PHY_DM_STRUCT	*p_dm
)
{
	struct odm_rf_calibration_structure  *p_rf_calibrate_info = &(p_dm->rf_calibrate_info);
	s32 tmp_lb_gain = 0;

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("===> _DPK_AdjustRFGainByFoundTxAGC\n"));

	/* RF setting, ���s�]�wRF reg00, �|�ҥ�DPK Phase 2�o�쪺 d00[3:0] = 0x6 ' TX AGC= 0x18 ' RF reg00[4:0] = 0x18 */
	odm_set_rf_reg(p_dm, RF_PATH_A, 0x00, RFREGOFFSETMASK, p_rf_calibrate_info->dpk_tx_agc);
	/* Attn: A mode @ reg64[2:0], G mode @ reg56 */
	odm_set_rf_reg(p_dm, RF_PATH_A, 0x64, RFREGOFFSETMASK, 0x19aac);
	/* PGA gain: RF reg8f[14:13] */
	odm_set_rf_reg(p_dm, RF_PATH_A, 0x8f, RFREGOFFSETMASK, 0x8a001);

	/* reg82c[31] = 1 --> ���� page C1 */
	odm_write_4byte(p_dm, 0x82c, 0x802083dd);

	/* DPK setting */
	odm_write_4byte(p_dm, 0xc94, 0xf76c9f84);
	odm_write_4byte(p_dm, 0xcc8, 0x000c5599);
	odm_write_4byte(p_dm, 0xcc4, 0x11838000);

	/* �`�Npage c1��c90�bDPK�L�{���d�U����Q��Lthread or process��g��page c��c90�� */
	odm_write_4byte(p_dm, 0xc90, 0x0101f018);

	/* one shot */
	odm_write_4byte(p_dm, 0xcc8, 0x800c5599);
	odm_write_4byte(p_dm, 0xcc8, 0x000c5599);
	/* delay 50 ms,�� delay �ɶ����@�I, �T�w PA Scan function ������ */
	delay_ms(50);

	/* reg82c[31] = 0 --> ���� page C */
	odm_write_4byte(p_dm, 0x82c, 0x002083dd);
	odm_set_rf_reg(p_dm, RF_PATH_A, 0x00, RFREGOFFSETMASK, 0x33d8d);


	tmp_lb_gain = _compute_loop_back_gain_path_a(p_dm);

	if (tmp_lb_gain < 263390) {
		/* coarse tune loopback gain by RF reg8f[14:13] = 2b'11 */
		odm_set_rf_reg(p_dm, RF_PATH_A, 0x8f, RFREGOFFSETMASK, 0x8e001);
		_fine_tune_loop_back_gain_path_a(p_dm, p_rf_calibrate_info->dpk_tx_agc);
	} else if (tmp_lb_gain > 661607) {
		/* coarse tune loopback gain by RF reg8f[14:13] = 2b'00 */
		odm_set_rf_reg(p_dm, RF_PATH_A, 0x8f, RFREGOFFSETMASK, 0x88001);
		_fine_tune_loop_back_gain_path_a(p_dm, p_rf_calibrate_info->dpk_tx_agc);
	} else
		p_rf_calibrate_info->is_dpk_fail = false;/* Go to DPK Phase 4 */

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("<=== _DPK_AdjustRFGainByFoundTxAGC\n"));

	return p_rf_calibrate_info->is_dpk_fail ? false : true;

}


void
_dpk_do_auto_dpk_path_a(
	struct PHY_DM_STRUCT	*p_dm
)
{
	struct odm_rf_calibration_structure  *p_rf_calibrate_info = &(p_dm->rf_calibrate_info);
	u32 tmp_lb_gain = 0, reg0xd00 = 0;

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("===> _DPK_DoAutoDPK\n"));


	/* reg82c[31] = 0 --> ���� page C */
	odm_write_4byte(p_dm, 0x82c, 0x002083dd);
	/* RF setting, ���BRF reg00, �P DPK Phase 3 �@�P */
	odm_set_rf_reg(p_dm, RF_PATH_A, 0x00, RFREGOFFSETMASK, p_rf_calibrate_info->dpk_tx_agc);
	/* Baseband data rate setting */
	odm_write_4byte(p_dm, 0xc20, 0x3c3c3c3c);
	odm_write_4byte(p_dm, 0xc24, 0x3c3c3c3c);
	odm_write_4byte(p_dm, 0xc28, 0x3c3c3c3c);
	odm_write_4byte(p_dm, 0xc2c, 0x3c3c3c3c);
	odm_write_4byte(p_dm, 0xc30, 0x3c3c3c3c);
	odm_write_4byte(p_dm, 0xc34, 0x3c3c3c3c);
	odm_write_4byte(p_dm, 0xc38, 0x3c3c3c3c);
	odm_write_4byte(p_dm, 0xc3c, 0x3c3c3c3c);
	odm_write_4byte(p_dm, 0xc40, 0x3c3c3c3c);
	odm_write_4byte(p_dm, 0xc44, 0x3c3c3c3c);
	odm_write_4byte(p_dm, 0xc48, 0x3c3c3c3c);
	odm_write_4byte(p_dm, 0xc4c, 0x3c3c3c3c);

	/* reg82c[31] = 1 --> ���� page C1 */
	odm_write_4byte(p_dm, 0x82c, 0x802083dd);

	/* DPK setting */
	odm_write_4byte(p_dm, 0xc94, 0xf76C9f84);
	odm_write_4byte(p_dm, 0xcc8, 0x400C5599);
	odm_write_4byte(p_dm, 0xcc4, 0x11938080);    /* 0xcc4[9:4]= DPk fail threshold */

	if (36 <= *(p_dm->p_channel) && *(p_dm->p_channel) <= 53) /* Channelchannel at low band) */
		/* r_agc */
		odm_write_4byte(p_dm, 0xcbc, 0x00022a1f);
	else
		/* r_agc */
		odm_write_4byte(p_dm, 0xcbc, 0x00009dbf);

	/* �`�Npage c1��c90�bDPK�L�{���d�U����Q��Lthread or process��g��page c��c90�� */
	odm_write_4byte(p_dm, 0xc90, 0x0101f018); /* TODO: 0xC90(rA_LSSIWrite_Jaguar) can not be overwritten. */

	/* one shot */
	odm_write_4byte(p_dm, 0xcc8, 0xc00c5599);
	odm_write_4byte(p_dm, 0xcc8, 0x400c5599);
	/* delay 50 ms,�� delay �ɶ����@�I, �T�w PA Scan function ������ */
	delay_ms(50);

	/* reg82c[31] = 0 --> ���� page C */
	odm_write_4byte(p_dm, 0x82c, 0x002083dd);
	/* T-meter RFReg42[17] = 1 to enable read T-meter, [15:10] ' T-meter value */
	odm_set_rf_reg(p_dm, RF_PATH_A, 0x42, BIT(17), 1);
	p_rf_calibrate_info->dpk_thermal[RF_PATH_A] = odm_get_rf_reg(p_dm, RF_PATH_A, 0x42, 0xFC00);					/* Ū�X42[15:10] �Ȩæs���ܼ�TMeter */
	dbg_print("p_rf_calibrate_info->dpk_thermal[RF_PATH_A] = 0x%X\n", p_rf_calibrate_info->dpk_thermal[RF_PATH_A]);
	odm_set_rf_reg(p_dm, RF_PATH_A, 0x00, RFREGOFFSETMASK, 0x33D8D);

	/* reg82c[31] = 1 --> ���� page C1 */
	odm_write_4byte(p_dm, 0x82c, 0x802083dd);
	/* read back dp_fail report */
	odm_write_4byte(p_dm, 0xcb8, 0x00000000);
	/* dp_fail�o��bit�bd00[6], �� d00[6] = 1, ���calibration����. */
	reg0xd00 = odm_read_4byte(p_dm, 0xd00);

	if ((reg0xd00 & BIT(6)) == BIT(6)) {
		/* printf('DPK fail') */
		p_rf_calibrate_info->is_dpk_fail = true;
		/* Go to DPK Phase 5 */
	} else {
		/* printf('DPK success') */
	}


	/* read back */
	odm_write_4byte(p_dm, 0xc90, 0x0201f01f);
	odm_write_4byte(p_dm, 0xcb8, 0x0c000000);

	/* reg82c[31] = 1 --> ���� page C1 */
	odm_write_4byte(p_dm, 0x82c, 0x002083dd);

	/* �p��tmpGainLoss �Ψ쪺function hex2dec() �O�N twos complement���Q���i���ন�Q�i��, �Q���i�쪺�榡��s11.9 */
	/* �|��1, d00[10:0] = h'3ff, �g�L hex2dec(h'3ff) = 1023, */
	/* �|��2, d00[10:0] = h'400, �g�L hex2dec(h'400) = -1024, */
	/* �|��3, d00[10:0] = h'0A9, �g�L hex2dec(h'0A9) = 169, */
	/* �|��4, d00[10:0] = h'54c, �g�L hex2dec(h'54c) = -692, */
	/* �|��4, d00[10:0] = h'7ff, �g�L hex2dec(h'7ff) = -1, */
	tmp_lb_gain = _compute_loop_back_gain_path_a(p_dm);


	/* Gain Scaling */
	if (227007 < tmp_lb_gain)
		p_rf_calibrate_info->dpk_gain = 0x43ca43ca;
	else if (214309 < tmp_lb_gain && tmp_lb_gain <= 227007)
		p_rf_calibrate_info->dpk_gain = 0x45c545c5;
	else if (202321 < tmp_lb_gain && tmp_lb_gain <= 214309)
		p_rf_calibrate_info->dpk_gain = 0x47cf47cf;
	else if (191003 < tmp_lb_gain && tmp_lb_gain <= 202321)
		p_rf_calibrate_info->dpk_gain = 0x49e749e7;
	else if (180318 < tmp_lb_gain && tmp_lb_gain <= 191003)
		p_rf_calibrate_info->dpk_gain = 0x4c104c10;
	else if (170231 < tmp_lb_gain && tmp_lb_gain <= 180318)
		p_rf_calibrate_info->dpk_gain = 0x4e494e49;
	else if (160709 < tmp_lb_gain && tmp_lb_gain <= 170231)
		p_rf_calibrate_info->dpk_gain = 0x50925092;
	else if (151719 < tmp_lb_gain && tmp_lb_gain <= 160709)
		p_rf_calibrate_info->dpk_gain = 0x52ec52ec;
	else if (151719 <= tmp_lb_gain)
		p_rf_calibrate_info->dpk_gain = 0x55585558;

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("<=== _DPK_DoAutoDPK\n"));
}


void
_dpk_enable_dp_path_a(
	struct PHY_DM_STRUCT	*p_dm
)
{
	struct odm_rf_calibration_structure  *p_rf_calibrate_info = &(p_dm->rf_calibrate_info);

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("===> _dpk_enable_dp\n"));

	/* [31] = 1 --> switch to page C1 */
	odm_write_4byte(p_dm, 0x82c, 0x802083dd);

	/* enable IQC matrix --> �]�� BB �H���|���g�L predistortion module, �~�g�L IQC matrix �� DAC ���X�h */
	/* �ҥH�n�� enable predistortion module {c90[7] = 1 (enable_predis) cc4[18] = 1 (�T�w�� IQK/DPK module ��clock), cc8[29] = 1 (�@��IQK/DPK module �̪�mux, �T�{ data path ��IQK/DPK)} */
	odm_write_4byte(p_dm, 0xc90, 0x0000f098);
	odm_write_4byte(p_dm, 0xc94, 0x776d9f84);
	odm_write_4byte(p_dm, 0xcc8, 0x20000000);
	odm_write_4byte(p_dm, 0xc8c, 0x3c000000);
	/* r_bnd */
	odm_write_4byte(p_dm, 0xcb8, 0x000fffff);

	if (p_rf_calibrate_info->is_dpk_fail) {
		/* cc4[29] = 1 (bypass DP LUT) */
		odm_write_4byte(p_dm, 0xc98, 0x40004000);
		odm_write_4byte(p_dm, 0xcc4, 0x28840000);
	} else {
		odm_write_4byte(p_dm, 0xc98, p_rf_calibrate_info->dpk_gain);
		odm_write_4byte(p_dm, 0xcc4, 0x08840000);

		/* PWSF */
		/* �gPWSF table in 1st SRAM for PA = 11 use */
		odm_write_4byte(p_dm, 0xc20, 0x00000800);

		/* ******************************************************* */
		/* 0xce4[0]�Owrite enable�A0xce4[7:1]�Oaddress�A0xce4[15:8]�M0xce4[23:16]������TX index�A */
		/* �Y��m��0(0xce4[7:1] = 0x0)����0xce4[15:8]�����쪺TX RF index �O0x1f,�A */
		/* 0xce4[23:16]�����쪺�O0x1e�A�Y��m��1(0xce4[7:1] = 0x1)�A����0xce4 [15:8]������0x1d�A */
		/* 0xce4[23:16]����0x1c�A��L�̦������A�C��data��쳣�O�ۮt1dB�C�Ygainloss��쪺RF TX index=0x18�A */
		/* �h�b0xce4 address������0x18���a��n��0x40(�]�N�O0dB)�A��L�h�ӤU��table�����ǨC��1dB�̧ǱƦC */
		/* �N�Ҧ���0xce4c��data��쳣��J�۹������ȡC */
		/* ************************************************************* */

		{
			const s32 LEN = 25;
			u32 base_idx = 6; /* 0 dB: 0x40 */
			u32 table_pwsf[] = {
				0xff, 0xca, 0xa1, 0x80, 0x65, 0x51, 0x40/* 0 dB */,
				0x33, 0x28, 0x20, 0x19, 0x14, 0x10,
				0x0d, 0x0a, 0x08, 0x06, 0x05, 0x04,
				0x03, 0x03, 0x02, 0x02, 0x01, 0x01
			};

			u32 center_tx_idx = p_rf_calibrate_info->dpk_tx_agc & 0x1F;
			u32 center_addr = (0x1F - center_tx_idx) / 2;
			s32 i = 0, j = 0, value = 0, start_idx = 0;

			/* Upper */
			start_idx = (((0x1F - center_tx_idx) % 2 == 0) ? base_idx + 1 : base_idx);

			for (i = start_idx, j = 0; (center_addr - j + 1) >= 1; i -= 2, j++) {
				if (i - 1 < 0)
					value = (table_pwsf[0] << 16) | (table_pwsf[0] << 8) | ((center_addr - j) << 1) | 0x1;
				else
					value = (table_pwsf[i] << 16) | (table_pwsf[i - 1] << 8) | ((center_addr - j) << 1) | 0x1;

				odm_write_4byte(p_dm, 0xce4, value);
				odm_write_1byte(p_dm, 0xce4, 0x0);		/* write disable */
			}

			/* Lower */
			start_idx = (((0x1F - center_tx_idx) % 2 == 0) ? base_idx + 2 : base_idx + 1);
			center_addr++; /* Skip center_tx_idx */
			for (i = start_idx, j = 0; (center_addr + j) < 16; i += 2, j++) { /* Total: 16*2 = 32 values (Upper+Lower) */
				if (i + 1 >= LEN)
					value = (table_pwsf[LEN - 1] << 16) | (table_pwsf[LEN - 1] << 8) | ((center_addr + j) << 1) | 0x1;
				else
					value = (table_pwsf[i + 1] << 16) | (table_pwsf[i] << 8) | ((center_addr + j) << 1) | 0x1;
				odm_write_4byte(p_dm, 0xce4, value);
				odm_write_1byte(p_dm, 0xce4, 0x0);		/* write disable */
			}
		}
	}
	/* [31] = 0 --> switch to page C */
	odm_write_4byte(p_dm, 0x82c, 0x002083dd);

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("<=== _dpk_enable_dp\n"));

}





void
_phy_dp_calibrate_path_a_8812a(
	struct PHY_DM_STRUCT	*p_dm
)
{
	u32 backup_mac_bb_reg_addrs[] = {
		0xC60, 0xC64, 0xC68, 0x82C, 0x838, 0x90C, 0x522, 0xC00, 0xC20, 0xC24, /* 10 */
		0xC28, 0xC2C, 0xC30, 0xC34, 0xC38, 0xC3C, 0xC40, 0xC44, 0xC48, 0xC4C, /* 20 */
		0xC94, 0xCB0, 0xCB4
	};
	u32 backup_rf_reg_addrs[] = {0x00, 0x64, 0x8F};

	u32 backup_mac_bb_reg_data[sizeof(backup_mac_bb_reg_addrs) / sizeof(u32)];
	u32 backup_rf_reg_data_a[sizeof(backup_rf_reg_addrs) / sizeof(u32)];
	/* u32 backupRFRegData_B[sizeof(backup_rf_reg_addrs)/sizeof(u32)]; */


	_dpk_mac_bb_backup_path_a(p_dm, backup_mac_bb_reg_addrs, backup_mac_bb_reg_data, sizeof(backup_mac_bb_reg_addrs) / sizeof(u32));
	_dpk_rf_backup_path_a(p_dm, backup_rf_reg_addrs, RF_PATH_A, backup_rf_reg_data_a, sizeof(backup_rf_reg_addrs) / sizeof(u32));

	_dpk_init_path_a(p_dm);

	if (_dpk_adjust_rf_gain_path_a(p_dm)) {               /* Phase 1 */
		_dpk_gain_loss_to_find_tx_agc_path_a(p_dm);          /* Phase 2 */
		if (_dpk_adjust_rf_gain_by_found_tx_agc_path_a(p_dm)) /* Phase 3 */
			_dpk_do_auto_dpk_path_a(p_dm);                /* Phase 4 */
	}
	_dpk_enable_dp_path_a(p_dm);                         /* Phase 5 */

	_dpk_mac_bb_restore_path_a(p_dm, backup_mac_bb_reg_addrs, backup_mac_bb_reg_data, sizeof(backup_mac_bb_reg_addrs) / sizeof(u32));
	_dpk_rf_restore_path_a(p_dm, backup_rf_reg_addrs, RF_PATH_A, backup_rf_reg_data_a, sizeof(backup_rf_reg_addrs) / sizeof(u32));
}


void
_dpk_mac_bb_backup_path_b(
	struct PHY_DM_STRUCT	*p_dm,
	u32		*backup_reg_addr,
	u32		*backup_reg_data,
	u32		number
)
{
	u32 i;

	for (i = 0; i < number; i++)
		backup_reg_data[i] = odm_read_4byte(p_dm, backup_reg_addr[i]);
}

void
_dpk_mac_bb_restore_path_b(
	struct PHY_DM_STRUCT	*p_dm,
	u32		*backup_reg_addr,
	u32		*backup_reg_data,
	u32		number
)
{
	u32 i;

	for (i = 0; i < number; i++)
		odm_write_4byte(p_dm, backup_reg_addr[i], backup_reg_data[i]);
}


void
_dpk_rf_backup_path_b(
	struct PHY_DM_STRUCT	*p_dm,
	u32				*backup_reg_addr,
	enum rf_path	rf_path,
	u32				*backup_reg_data,
	u32		        number
)
{
	u32 i;

	for (i = 0; i < number; i++)
		backup_reg_data[i] = odm_get_rf_reg(p_dm, (enum rf_path)rf_path, backup_reg_addr[i], RFREGOFFSETMASK);
}

void
_dpk_rf_restore_path_b(
	struct PHY_DM_STRUCT	*p_dm,
	u32				*backup_reg_addr,
	enum rf_path	rf_path,
	u32		*backup_reg_data,
	u32		        number
)
{
	u32 i;

	for (i = 0; i < number; i++)
		odm_set_rf_reg(p_dm, (enum rf_path)rf_path, backup_reg_addr[i], RFREGOFFSETMASK, backup_reg_data[i]);
}



s32
_compute_loop_back_gain_path_b(
	struct PHY_DM_STRUCT			*p_dm
)
{
	/* compute loopback gain */
	/* �p��tmp_lb_gain �Ψ쪺function hex2dec() �O�N twos complement���Q���i���ন�Q�i��, �Q���i�쪺�榡��s11.10
	* �|��1, d00[10:0] = h'3ff, �g�L hex2dec(h'3ff) = 1023,
	* �|��2, d00[10:0] = h'400, �g�L hex2dec(h'400) = -1024,
	* �|��3, d00[10:0] = h'0A9, �g�L hex2dec(h'0A9) = 169,
	* �|��4, d00[10:0] = h'54c, �g�L hex2dec(h'54c) = -692,
	* �|��5, d00[10:0] = h'7ff, �g�L hex2dec(h'7ff) = -1, */
	u32 reg0xd40_26_16 = odm_get_bb_reg(p_dm, 0xD40, 0x7FF0000);
	u32 reg0xd40_10_0  = odm_get_bb_reg(p_dm, 0xD40, 0x7FF);

	s32 tmp_lb_gain = _sign(reg0xd40_26_16) * _sign(reg0xd40_26_16) + _sign(reg0xd40_10_0) * _sign(reg0xd40_10_0);

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("<=== _compute_loop_back_gain_path_b, tmp_lb_gain = 0x%X\n", tmp_lb_gain));

	return tmp_lb_gain;
}


boolean
_fine_tune_loop_back_gain_path_b(
	struct PHY_DM_STRUCT			*p_dm,
	u32				dpk_tx_agc
)
{
	struct odm_rf_calibration_structure  *p_rf_calibrate_info = &(p_dm->rf_calibrate_info);
	s32 tmp_lb_gain = 0;

	u32 rf0x64_orig = odm_get_rf_reg(p_dm, RF_PATH_B, 0x64, 0x7);
	u32 rf0x64_new = rf0x64_orig;

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("===> _fine_tune_loop_back_gain_path_b\n"));

	do {
		/* RF setting */
		odm_set_rf_reg(p_dm, RF_PATH_B, 0x00, RFREGOFFSETMASK, dpk_tx_agc);
		odm_write_4byte(p_dm, 0x82c, 0x802083dd);
		/* �`�Npage c1��c90�bDPK�L�{���d�U����Q��Lthread or process��g��page c��c90�� */
		odm_write_4byte(p_dm, 0xe90, 0x0101f018);

		/* one shot */
		odm_write_4byte(p_dm, 0xec8, 0x800c5599);
		odm_write_4byte(p_dm, 0xec8, 0x000c5599);
		/* delay 50 ms,�� delay �ɶ����@�I, �T�w PA Scan function ������ */
		delay_ms(50);

		/* reg82c[31] = 0 --> ���� page C */
		odm_write_4byte(p_dm, 0x82c, 0x002083dd);
		odm_set_rf_reg(p_dm, RF_PATH_B, 0x00, RFREGOFFSETMASK, 0x33d8d);

		tmp_lb_gain = _compute_loop_back_gain_path_b(p_dm);

		if ((rf0x64_new == (BIT(2) | BIT(1) | BIT(0))) || (rf0x64_new == 0)) {
			/* printf('DPK Phase 1 failed'); */
			p_rf_calibrate_info->is_dpk_fail = true;
			break;
			/* Go to DPK Phase 5 */
		} else {
			if (tmp_lb_gain < 263390) {/* fine tune loopback path gain: newReg64[2:0] = reg64[2:0] - 3b'001 */
				rf0x64_new -= 1;
				odm_set_rf_reg(p_dm, RF_PATH_B, 0x64, 0x7, rf0x64_new);

			} else if (tmp_lb_gain > 661607) {/* fine tune loopback path gain: newReg64 [2:0] = reg64[2:0] + 3b'001 */
				rf0x64_new += 1;
				odm_set_rf_reg(p_dm, RF_PATH_B, 0x64, 0x7, rf0x64_new);

			} else
				p_rf_calibrate_info->is_dpk_fail = false;
		}

	} while (tmp_lb_gain < 263390 || 661607 < tmp_lb_gain);

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("<=== _fine_tune_loop_back_gain_path_b\n"));

	return p_rf_calibrate_info->is_dpk_fail ? false : true;

}



void
_dpk_init_path_b(
	struct PHY_DM_STRUCT	*p_dm
)
{
	struct odm_rf_calibration_structure  *p_rf_calibrate_info = &(p_dm->rf_calibrate_info);

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("===> _DPK_Init\n"));

	/* TX pause */
	odm_write_1byte(p_dm, 0x522, 0x7f);

	/* reg82c[31] = b'0, ������ page C */
	odm_write_4byte(p_dm, 0x82c, 0x002083dd);

	/* AFE setting */
	odm_write_4byte(p_dm, 0xc68, 0x59791979);
	odm_write_4byte(p_dm, 0xe60, 0x77777777);
	odm_write_4byte(p_dm, 0xe64, 0x77777777);

	/* external TRSW ���� T */
	odm_write_4byte(p_dm, 0xeb0, 0x77777777);
	odm_write_4byte(p_dm, 0xeb4, 0x01000077);

	/* hardware 3-wire off */
	odm_write_4byte(p_dm, 0xe00, 0x00000004);

	/* CCA off */
	odm_write_4byte(p_dm, 0x838, 0x16C89B4c);

	/* 90c[15]: dac fifo reset by CSWU */
	odm_write_4byte(p_dm, 0x90c, 0x00008000);

	/* r_gothrough_iqkdpk */
	odm_write_4byte(p_dm, 0xe94, 0x0100005D);

	/* reg82c[31] = 1 --> ���� page C1 */
	odm_write_4byte(p_dm, 0x82c, 0x802083dd);

	/* IQK Amp off */
	odm_write_4byte(p_dm, 0xe8c, 0x3c000000);

	/* tx_amp */
	odm_write_4byte(p_dm, 0xe98, 0x41382e21);
	odm_write_4byte(p_dm, 0xe9c, 0x5b554f48);
	odm_write_4byte(p_dm, 0xea0, 0x6f6b6661);
	odm_write_4byte(p_dm, 0xea4, 0x817d7874);
	odm_write_4byte(p_dm, 0xea8, 0x908c8884);
	odm_write_4byte(p_dm, 0xeac, 0x9d9a9793);
	odm_write_4byte(p_dm, 0xeb0, 0xaaa7a4a1);
	odm_write_4byte(p_dm, 0xeb4, 0xb6b3b0ad);

	/* tx_inverse */
	odm_write_4byte(p_dm, 0xe40, 0x02ce03e9);
	odm_write_4byte(p_dm, 0xe44, 0x01fd0249);
	odm_write_4byte(p_dm, 0xe48, 0x01a101c9);
	odm_write_4byte(p_dm, 0xe4c, 0x016a0181);
	odm_write_4byte(p_dm, 0xe50, 0x01430155);
	odm_write_4byte(p_dm, 0xe54, 0x01270135);
	odm_write_4byte(p_dm, 0xe58, 0x0112011c);
	odm_write_4byte(p_dm, 0xe5c, 0x01000108);
	odm_write_4byte(p_dm, 0xe60, 0x00f100f8);
	odm_write_4byte(p_dm, 0xe64, 0x00e500eb);
	odm_write_4byte(p_dm, 0xe68, 0x00db00e0);
	odm_write_4byte(p_dm, 0xe6c, 0x00d100d5);
	odm_write_4byte(p_dm, 0xe70, 0x00c900cd);
	odm_write_4byte(p_dm, 0xe74, 0x00c200c5);
	odm_write_4byte(p_dm, 0xe78, 0x00bb00be);
	odm_write_4byte(p_dm, 0xe7c, 0x00b500b8);

	/* reg82c[31] = b'0, ������ page C */
	odm_write_4byte(p_dm, 0x82c, 0x002083dd);

	p_rf_calibrate_info->is_dpk_fail = false;

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("<=== _DPK_Init\n"));

}


boolean
_dpk_adjust_rf_gain_path_b(
	struct PHY_DM_STRUCT	*p_dm
)
{
	struct odm_rf_calibration_structure  *p_rf_calibrate_info = &(p_dm->rf_calibrate_info);
	s32 tmp_lb_gain = 0;

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("===> _DPK_AdjustRFGain\n"));

	/* RF setting */
	odm_set_rf_reg(p_dm, RF_PATH_B, 0x00, RFREGOFFSETMASK, 0x50bfc);
	/* Attn: A mode @ reg64[2:0], G mode @ reg56 */
	odm_set_rf_reg(p_dm, RF_PATH_B, 0x64, RFREGOFFSETMASK, 0x19aac);
	/* PGA gain: RF reg8f[14:13] */
	odm_set_rf_reg(p_dm, RF_PATH_B, 0x8f, RFREGOFFSETMASK, 0x8a001);

	/* reg82c[31] = 1 --> ���� page C1 */
	odm_write_4byte(p_dm, 0x82c, 0x802083dd);

	/* DPK setting */
	odm_write_4byte(p_dm, 0xe94, 0xf76c9f84);
	odm_write_4byte(p_dm, 0xec8, 0x000c5599);
	odm_write_4byte(p_dm, 0xec4, 0x11838000);
	odm_set_bb_reg(p_dm, 0xed4, 0xFFF000, 0x100);  /* �Ncd4[23:12] �令 h'100, �䥦�줸�ЫO�d��Ȥ��n�g��
     * �`�Npage c1��c90�bDPK�L�{���d�U����Q��Lthread or process��g��page c��c90�� */
	odm_write_4byte(p_dm, 0xe90, 0x0101f018);

	/* one shot */
	odm_write_4byte(p_dm, 0xec8, 0x800c5599);
	odm_write_4byte(p_dm, 0xec8, 0x000c5599);
	/* delay 50 ms,�� delay �ɶ����@�I, �T�w PA Scan function ������ */
	delay_ms(50);

	/* read back Loopback Gain */
	odm_write_4byte(p_dm, 0xeb8, 0x09000000);

	/* reg82c[31] = 0 --> ���� page C */
	odm_write_4byte(p_dm, 0x82c, 0x002083dd);
	odm_set_rf_reg(p_dm, RF_PATH_B, 0x00, RFREGOFFSETMASK, 0x33d8d);

	tmp_lb_gain = _compute_loop_back_gain_path_b(p_dm);

	/* coarse tune loopback gain by RF reg8f[14:13] = 2b'11 */
	if (tmp_lb_gain < 263390) {
		odm_set_rf_reg(p_dm, RF_PATH_B, 0x8f, RFREGOFFSETMASK, 0x8e001);
		_fine_tune_loop_back_gain_path_b(p_dm, 0x50bfc);

	} else if (tmp_lb_gain > 661607) {
		/* coarse tune loopback gain by RF reg8f[14:13] = 2b'00 */
		odm_set_rf_reg(p_dm, RF_PATH_B, 0x8f, RFREGOFFSETMASK, 0x88001);
		_fine_tune_loop_back_gain_path_b(p_dm, 0x50bfc);
	} else
		p_rf_calibrate_info->is_dpk_fail = false;

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("<=== _DPK_AdjustRFGain\n"));

	return p_rf_calibrate_info->is_dpk_fail ? false : true;

}



void
_dpk_gain_loss_to_find_tx_agc_path_b(
	struct PHY_DM_STRUCT	*p_dm
)
{
	struct odm_rf_calibration_structure  *p_rf_calibrate_info = &(p_dm->rf_calibrate_info);
	u32 reg0xd40 = 0;

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("===> _DPK_GainLossToFindTxAGC\n"));

	/* RF setting */
	odm_set_rf_reg(p_dm, RF_PATH_B, 0x00, RFREGOFFSETMASK, 0x50bfc);

	/* reg82c[31] = 1 --> ���� page C1 */
	odm_write_4byte(p_dm, 0x82c, 0x802083dd);

	/* regc20[15:13] = dB sel, �i�D Gain Loss function �h�M�� dB_sel �ҳ]�w��PA gain loss�ؼЩҹ����� Tx AGC ����. */
	/* dB_sel = b'000 ' 1.0 dB PA gain loss */
	/* dB_sel = b'001 ' 1.5 dB PA gain loss */
	/* dB_sel = b'010 ' 2.0 dB PA gain loss */
	/* dB_sel = b'011 ' 2.5 dB PA gain loss */
	/* dB_sel = b'100 ' 3.0 dB PA gain loss */
	/* dB_sel = b'101 ' 3.5 dB PA gain loss */
	/* dB_sel = b'110 ' 4.0 dB PA gain loss */
	odm_write_4byte(p_dm, 0xe20, 0x00002000);

	/* DPK setting */
	odm_write_4byte(p_dm, 0xe94, 0xf76c9f84);
	odm_write_4byte(p_dm, 0xec8, 0x000c5599);
	odm_write_4byte(p_dm, 0xec4, 0x148b8000);

	/* �`�Npage c1��c90�bDPK�L�{���d�U����Q��Lthread or process��g��page c��c90�� */
	odm_write_4byte(p_dm, 0xe90, 0x0401f018);

	/* one shot */
	odm_write_4byte(p_dm, 0xec8, 0x800c5599);
	odm_write_4byte(p_dm, 0xec8, 0x000c5599);
	/* delay 50 ms,�� delay �ɶ����@�I, �T�w PA Scan function ������ */
	delay_ms(50);

	/* read back Loopback Gain */
	/* �i�H�b d40[3:0] ��Ū�^, dB_sel ���ҳ]�w�� gain loss �|���b���@�� Tx AGC �]�w */
	/* Ū�^d00[3:0] = h'1 ' Tx AGC = h'13 */
	/* Ū�^d00[3:0] = h'2 ' Tx AGC = h'14 */
	/* Ū�^d00[3:0] = h'3 ' Tx AGC = h'15 */
	/* Ū�^d00[3:0] = h'4 ' Tx AGC = h'16 */
	/* Ū�^d00[3:0] = h'5 ' Tx AGC = h'17 */
	/* Ū�^d00[3:0] = h'6 ' Tx AGC = h'18 */
	/* Ū�^d00[3:0] = h'7 ' Tx AGC = h'19 */
	/* Ū�^d00[3:0] = h'8 ' Tx AGC = h'1a */
	/* Ū�^d00[3:0] = h'9 ' Tx AGC = h'1b */
	/* Ū�^d00[3:0] = h'a ' Tx AGC = h'1c */
	/*  */
	reg0xd40 = odm_read_4byte(p_dm, 0xd40);
	switch (odm_read_4byte(p_dm, 0xd40) & (BIT(3) | BIT(2) | BIT(1) | BIT(0))) {
	case 0x0:
		p_rf_calibrate_info->dpk_tx_agc = 0x50bf0 | 0x2;
		break;
	case 0x1:
		p_rf_calibrate_info->dpk_tx_agc = 0x50bf0 | 0x3;
		break;
	case 0x2:
		p_rf_calibrate_info->dpk_tx_agc = 0x50bf0 | 0x4;
		break;
	case 0x3:
		p_rf_calibrate_info->dpk_tx_agc = 0x50bf0 | 0x5;
		break;
	case 0x4:
		p_rf_calibrate_info->dpk_tx_agc = 0x50bf0 | 0x6;
		break;
	case 0x5:
		p_rf_calibrate_info->dpk_tx_agc = 0x50bf0 | 0x7;
		break;
	case 0x6:
		p_rf_calibrate_info->dpk_tx_agc = 0x50bf0 | 0x8;
		break;
	case 0x7:
		p_rf_calibrate_info->dpk_tx_agc = 0x50bf0 | 0x9;
		break;
	case 0x8:
		p_rf_calibrate_info->dpk_tx_agc = 0x50bf0 | 0xa;
		break;
	case 0x9:
		p_rf_calibrate_info->dpk_tx_agc = 0x50bf0 | 0xb;
		break;
	case 0xa:
		p_rf_calibrate_info->dpk_tx_agc = 0x50bf0 | 0xc;
		break;
	}
	/* reg82c[31] = 0 --> ���� page C */
	odm_write_4byte(p_dm, 0x82c, 0x002083dd);
	odm_set_rf_reg(p_dm, RF_PATH_B, 0x00, RFREGOFFSETMASK, 0x33d8d);

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("<=== _DPK_GainLossToFindTxAGC\n"));
}


boolean
_dpk_adjust_rf_gain_by_found_tx_agc_path_b
(
	struct PHY_DM_STRUCT	*p_dm
)
{
	struct odm_rf_calibration_structure  *p_rf_calibrate_info = &(p_dm->rf_calibrate_info);
	s32 tmp_lb_gain = 0;

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("===> _DPK_AdjustRFGainByFoundTxAGC\n"));

	/* RF setting, ���s�]�wRF reg00, �|�ҥ�DPK Phase 2�o�쪺 d40[3:0] = 0x6 ' TX AGC= 0x18 ' RF reg00[4:0] = 0x18 */
	odm_set_rf_reg(p_dm, RF_PATH_B, 0x00, RFREGOFFSETMASK, p_rf_calibrate_info->dpk_tx_agc);
	/* Attn: A mode @ reg64[2:0], G mode @ reg56 */
	odm_set_rf_reg(p_dm, RF_PATH_B, 0x64, RFREGOFFSETMASK, 0x19aac);
	/* PGA gain: RF reg8f[14:13] */
	odm_set_rf_reg(p_dm, RF_PATH_B, 0x8f, RFREGOFFSETMASK, 0x8a001);

	/* reg82c[31] = 1 --> ���� page C1 */
	odm_write_4byte(p_dm, 0x82c, 0x802083dd);

	/* DPK setting */
	odm_write_4byte(p_dm, 0xe94, 0xf76c9f84);
	odm_write_4byte(p_dm, 0xec8, 0x000c5599);
	odm_write_4byte(p_dm, 0xec4, 0x11838000);

	/* �`�Npage c1��c90�bDPK�L�{���d�U����Q��Lthread or process��g��page c��c90�� */
	odm_write_4byte(p_dm, 0xe90, 0x0101f018);

	/* one shot */
	odm_write_4byte(p_dm, 0xec8, 0x800c5599);
	odm_write_4byte(p_dm, 0xec8, 0x000c5599);
	/* delay 50 ms,�� delay �ɶ����@�I, �T�w PA Scan function ������ */
	delay_ms(50);

	/* reg82c[31] = 0 --> ���� page C */
	odm_write_4byte(p_dm, 0x82c, 0x002083dd);
	odm_set_rf_reg(p_dm, RF_PATH_B, 0x00, RFREGOFFSETMASK, 0x33d8d);


	tmp_lb_gain = _compute_loop_back_gain_path_b(p_dm);

	if (tmp_lb_gain < 263390) {
		/* coarse tune loopback gain by RF reg8f[14:13] = 2b'11 */
		odm_set_rf_reg(p_dm, RF_PATH_B, 0x8f, RFREGOFFSETMASK, 0x8e001);
		_fine_tune_loop_back_gain_path_b(p_dm, p_rf_calibrate_info->dpk_tx_agc);
	} else if (tmp_lb_gain > 661607) {
		/* coarse tune loopback gain by RF reg8f[14:13] = 2b'00 */
		odm_set_rf_reg(p_dm, RF_PATH_B, 0x8f, RFREGOFFSETMASK, 0x88001);
		_fine_tune_loop_back_gain_path_b(p_dm, p_rf_calibrate_info->dpk_tx_agc);
	} else
		p_rf_calibrate_info->is_dpk_fail = false;/* Go to DPK Phase 4 */

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("<=== _DPK_AdjustRFGainByFoundTxAGC\n"));

	return p_rf_calibrate_info->is_dpk_fail ? false : true;

}


void
_dpk_do_auto_dpk_path_b(
	struct PHY_DM_STRUCT	*p_dm
)
{
	struct odm_rf_calibration_structure  *p_rf_calibrate_info = &(p_dm->rf_calibrate_info);
	u32 tmp_lb_gain = 0, reg0xd40 = 0;

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("===> _DPK_DoAutoDPK\n"));


	/* reg82c[31] = 0 --> ���� page C */
	odm_write_4byte(p_dm, 0x82c, 0x002083dd);
	/* RF setting, ���BRF reg00, �P DPK Phase 3 �@�P */
	odm_set_rf_reg(p_dm, RF_PATH_B, 0x00, RFREGOFFSETMASK, p_rf_calibrate_info->dpk_tx_agc);
	/* Baseband data rate setting */
	odm_write_4byte(p_dm, 0xe20, 0x3c3c3c3c);
	odm_write_4byte(p_dm, 0xe24, 0x3c3c3c3c);
	odm_write_4byte(p_dm, 0xe28, 0x3c3c3c3c);
	odm_write_4byte(p_dm, 0xe2c, 0x3c3c3c3c);
	odm_write_4byte(p_dm, 0xe30, 0x3c3c3c3c);
	odm_write_4byte(p_dm, 0xe34, 0x3c3c3c3c);
	odm_write_4byte(p_dm, 0xe38, 0x3c3c3c3c);
	odm_write_4byte(p_dm, 0xe3c, 0x3c3c3c3c);
	odm_write_4byte(p_dm, 0xe40, 0x3c3c3c3c);
	odm_write_4byte(p_dm, 0xe44, 0x3c3c3c3c);
	odm_write_4byte(p_dm, 0xe48, 0x3c3c3c3c);
	odm_write_4byte(p_dm, 0xe4c, 0x3c3c3c3c);

	/* reg82c[31] = 1 --> ���� page C1 */
	odm_write_4byte(p_dm, 0x82c, 0x802083dd);

	/* DPK setting */
	odm_write_4byte(p_dm, 0xe94, 0xf76C9f84);
	odm_write_4byte(p_dm, 0xec8, 0x400C5599);
	odm_write_4byte(p_dm, 0xec4, 0x11938080);    /* 0xec4[9:4]= DPk fail threshold */

	if (36 <= *(p_dm->p_channel) && *(p_dm->p_channel) <= 53) /* Channelchannel at low band) */
		/* r_agc */
		odm_write_4byte(p_dm, 0xebc, 0x00022a1f);
	else
		/* r_agc */
		odm_write_4byte(p_dm, 0xebc, 0x00009dbf);

	/* �`�Npage c1��c90�bDPK�L�{���d�U����Q��Lthread or process��g��page c��c90�� */
	odm_write_4byte(p_dm, 0xe90, 0x0101f018); /* TODO: 0xe90(rA_LSSIWrite_Jaguar) can not be overwritten. */

	/* one shot */
	odm_write_4byte(p_dm, 0xec8, 0xc00c5599);
	odm_write_4byte(p_dm, 0xec8, 0x400c5599);
	/* delay 50 ms,�� delay �ɶ����@�I, �T�w PA Scan function ������ */
	delay_ms(50);

	/* reg82c[31] = 0 --> ���� page C */
	odm_write_4byte(p_dm, 0x82c, 0x002083dd);
	/* T-meter RFReg42[17] = 1 to enable read T-meter, [15:10] ' T-meter value */
	odm_set_rf_reg(p_dm, RF_PATH_A, 0x42, BIT(17), 1);
	p_rf_calibrate_info->dpk_thermal[RF_PATH_B] = odm_get_rf_reg(p_dm, RF_PATH_A, 0x42, 0xFC00);					/* Ū�X42[15:10] �Ȩæs���ܼ�TMeter */
	dbg_print("p_rf_calibrate_info->dpk_thermal[RF_PATH_B] = 0x%X\n", p_rf_calibrate_info->dpk_thermal[RF_PATH_B]);
	odm_set_rf_reg(p_dm, RF_PATH_B, 0x00, RFREGOFFSETMASK, 0x33D8D);

	/* reg82c[31] = 1 --> ���� page C1 */
	odm_write_4byte(p_dm, 0x82c, 0x802083dd);
	/* read back dp_fail report */
	odm_write_4byte(p_dm, 0xeb8, 0x00000000);
	/* dp_fail�o��bit�bd40[6], �� d40[6] = 1, ���calibration����. */
	reg0xd40 = odm_read_4byte(p_dm, 0xd40);

	if ((reg0xd40 & BIT(6)) == BIT(6)) {
		/* printf('DPK fail') */
		p_rf_calibrate_info->is_dpk_fail = true;
		/* Go to DPK Phase 5 */
	} else {
		/* printf('DPK success') */
	}


	/* read back */
	odm_write_4byte(p_dm, 0xe90, 0x0201f01f);
	odm_write_4byte(p_dm, 0xeb8, 0x0c000000);

	/* reg82c[31] = 1 --> ���� page C1 */
	odm_write_4byte(p_dm, 0x82c, 0x002083dd);

	/* �p��tmpGainLoss �Ψ쪺function hex2dec() �O�N twos complement���Q���i���ন�Q�i��, �Q���i�쪺�榡��s11.9 */
	/* �|��1, d00[10:0] = h'3ff, �g�L hex2dec(h'3ff) = 1023, */
	/* �|��2, d00[10:0] = h'400, �g�L hex2dec(h'400) = -1024, */
	/* �|��3, d00[10:0] = h'0A9, �g�L hex2dec(h'0A9) = 169, */
	/* �|��4, d00[10:0] = h'54c, �g�L hex2dec(h'54c) = -692, */
	/* �|��4, d00[10:0] = h'7ff, �g�L hex2dec(h'7ff) = -1, */
	tmp_lb_gain = _compute_loop_back_gain_path_b(p_dm);


	/* Gain Scaling */
	if (227007 < tmp_lb_gain)
		p_rf_calibrate_info->dpk_gain = 0x43ca43ca;
	else if (214309 < tmp_lb_gain && tmp_lb_gain <= 227007)
		p_rf_calibrate_info->dpk_gain = 0x45c545c5;
	else if (202321 < tmp_lb_gain && tmp_lb_gain <= 214309)
		p_rf_calibrate_info->dpk_gain = 0x47cf47cf;
	else if (191003 < tmp_lb_gain && tmp_lb_gain <= 202321)
		p_rf_calibrate_info->dpk_gain = 0x49e749e7;
	else if (180318 < tmp_lb_gain && tmp_lb_gain <= 191003)
		p_rf_calibrate_info->dpk_gain = 0x4c104c10;
	else if (170231 < tmp_lb_gain && tmp_lb_gain <= 180318)
		p_rf_calibrate_info->dpk_gain = 0x4e494e49;
	else if (160709 < tmp_lb_gain && tmp_lb_gain <= 170231)
		p_rf_calibrate_info->dpk_gain = 0x50925092;
	else if (151719 < tmp_lb_gain && tmp_lb_gain <= 160709)
		p_rf_calibrate_info->dpk_gain = 0x52ec52ec;
	else if (151719 <= tmp_lb_gain)
		p_rf_calibrate_info->dpk_gain = 0x55585558;

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("<=== _DPK_DoAutoDPK\n"));
}


void
_dpk_enable_dp_path_b(
	struct PHY_DM_STRUCT	*p_dm
)
{
	struct odm_rf_calibration_structure  *p_rf_calibrate_info = &(p_dm->rf_calibrate_info);

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("===> _dpk_enable_dp\n"));

	/* [31] = 1 --> switch to page C1 */
	odm_write_4byte(p_dm, 0x82c, 0x802083dd);

	/* enable IQC matrix --> �]�� BB �H���|���g�L predistortion module, �~�g�L IQC matrix �� DAC ���X�h */
	/* �ҥH�n�� enable predistortion module {c90[7] = 1 (enable_predis) cc4[18] = 1 (�T�w�� IQK/DPK module ��clock), cc8[29] = 1 (�@��IQK/DPK module �̪�mux, �T�{ data path ��IQK/DPK)} */
	odm_write_4byte(p_dm, 0xe90, 0x0000f098);
	odm_write_4byte(p_dm, 0xe94, 0x776d9f84);
	odm_write_4byte(p_dm, 0xec8, 0x20000000);
	odm_write_4byte(p_dm, 0xe8c, 0x3c000000);
	/* r_bnd */
	odm_write_4byte(p_dm, 0xeb8, 0x000fffff);

	if (p_rf_calibrate_info->is_dpk_fail) {
		/* cc4[29] = 1 (bypass DP LUT) */
		odm_write_4byte(p_dm, 0xe98, 0x40004000);
		odm_write_4byte(p_dm, 0xec4, 0x28840000);
	} else {
		odm_write_4byte(p_dm, 0xe98, p_rf_calibrate_info->dpk_gain);
		odm_write_4byte(p_dm, 0xec4, 0x08840000);

		/* PWSF */
		/* �gPWSF table in 1st SRAM for PA = 11 use */
		odm_write_4byte(p_dm, 0xe20, 0x00000800);

		/* ******************************************************* */
		/* 0xee4[0]�Owrite enable�A0xee4[7:1]�Oaddress�A0xee4[15:8]�M0xee4[23:16]������TX index�A */
		/* �Y��m��0(0xee4[7:1] = 0x0)����0xee4[15:8]�����쪺TX RF index �O0x1f,�A */
		/* 0xee4[23:16]�����쪺�O0x1e�A�Y��m��1(0xee4[7:1] = 0x1)�A����0xee4 [15:8]������0x1d�A */
		/* 0xee4[23:16]����0x1c�A��L�̦������A�C��data��쳣�O�ۮt1dB�C�Ygainloss��쪺RF TX index=0x18�A */
		/* �h�b0xee4 address������0x18���a��n��0x40(�]�N�O0dB)�A��L�h�ӤU��table�����ǨC��1dB�̧ǱƦC */
		/* �N�Ҧ���0xee4c��data��쳣��J�۹������ȡC */
		/* ************************************************************* */

		{
			const s32 LEN = 25;
			u32 base_idx = 6; /* 0 dB: 0x40 */
			u32 table_pwsf[] = {
				0xff, 0xca, 0xa1, 0x80, 0x65, 0x51, 0x40/* 0 dB */,
				0x33, 0x28, 0x20, 0x19, 0x14, 0x10,
				0x0d, 0x0a, 0x08, 0x06, 0x05, 0x04,
				0x03, 0x03, 0x02, 0x02, 0x01, 0x01
			};

			u32 center_tx_idx = p_rf_calibrate_info->dpk_tx_agc & 0x1F;
			u32 center_addr = (0x1F - center_tx_idx) / 2;
			s32 i = 0, j = 0, value = 0, start_idx = 0;

			/* Upper */
			start_idx = (((0x1F - center_tx_idx) % 2 == 0) ? base_idx + 1 : base_idx);

			for (i = start_idx, j = 0; (center_addr - j + 1) >= 1; i -= 2, j++) {
				if (i - 1 < 0)
					value = (table_pwsf[0] << 16) | (table_pwsf[0] << 8) | ((center_addr - j) << 1) | 0x1;
				else
					value = (table_pwsf[i] << 16) | (table_pwsf[i - 1] << 8) | ((center_addr - j) << 1) | 0x1;

				odm_write_4byte(p_dm, 0xee4, value);
				odm_write_1byte(p_dm, 0xee4, 0x0);		/* write disable */
			}

			/* Lower */
			start_idx = (((0x1F - center_tx_idx) % 2 == 0) ? base_idx + 2 : base_idx + 1);
			center_addr++; /* Skip center_tx_idx */
			for (i = start_idx, j = 0; (center_addr + j) < 16; i += 2, j++) { /* Total: 16*2 = 32 values (Upper+Lower) */
				if (i + 1 >= LEN)
					value = (table_pwsf[LEN - 1] << 16) | (table_pwsf[LEN - 1] << 8) | ((center_addr + j) << 1) | 0x1;
				else
					value = (table_pwsf[i + 1] << 16) | (table_pwsf[i] << 8) | ((center_addr + j) << 1) | 0x1;
				odm_write_4byte(p_dm, 0xee4, value);
				odm_write_1byte(p_dm, 0xee4, 0x0);		/* write disable */
			}
		}
	}
	/* [31] = 0 --> switch to page C */
	odm_write_4byte(p_dm, 0x82c, 0x002083dd);

	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("<=== _dpk_enable_dp\n"));

}





void
_phy_dp_calibrate_path_b_8812a(
	struct PHY_DM_STRUCT	*p_dm
)
{
	u32 backup_mac_bb_reg_addrs[] = {
		0xE60, 0xE64, 0xC68, 0x82C, 0x838, 0x90C, 0x522, 0xE00, 0xE20, 0xE24, /* 10 */
		0xE28, 0xE2C, 0xE30, 0xE34, 0xE38, 0xE3C, 0xE40, 0xE44, 0xE48, 0xE4C, /* 20 */
		0xE94, 0xEB0, 0xEB4
	};
	u32 backup_rf_reg_addrs[] = {0x00, 0x64, 0x8F};

	u32 backup_mac_bb_reg_data[sizeof(backup_mac_bb_reg_addrs) / sizeof(u32)];
	u32 backup_rf_reg_data_a[sizeof(backup_rf_reg_addrs) / sizeof(u32)];
	/* u32 backupRFRegData_B[sizeof(backup_rf_reg_addrs)/sizeof(u32)]; */


	_dpk_mac_bb_backup_path_b(p_dm, backup_mac_bb_reg_addrs, backup_mac_bb_reg_data, sizeof(backup_mac_bb_reg_addrs) / sizeof(u32));
	_dpk_rf_backup_path_b(p_dm, backup_rf_reg_addrs, RF_PATH_B, backup_rf_reg_data_a, sizeof(backup_rf_reg_addrs) / sizeof(u32));

	_dpk_init_path_b(p_dm);

	if (_dpk_adjust_rf_gain_path_b(p_dm)) {               /* Phase 1 */
		_dpk_gain_loss_to_find_tx_agc_path_b(p_dm);          /* Phase 2 */
		if (_dpk_adjust_rf_gain_by_found_tx_agc_path_b(p_dm)) /* Phase 3 */
			_dpk_do_auto_dpk_path_b(p_dm);                /* Phase 4 */
	}
	_dpk_enable_dp_path_b(p_dm);                         /* Phase 5 */

	_dpk_mac_bb_restore_path_b(p_dm, backup_mac_bb_reg_addrs, backup_mac_bb_reg_data, sizeof(backup_mac_bb_reg_addrs) / sizeof(u32));
	_dpk_rf_restore_path_b(p_dm, backup_rf_reg_addrs, RF_PATH_B, backup_rf_reg_data_a, sizeof(backup_rf_reg_addrs) / sizeof(u32));
}
#endif
void
phy_dp_calibrate_8812a(
	struct PHY_DM_STRUCT	*p_dm
)
{
#if 0	

	struct _hal_rf_		*p_rf = &(p_dm->rf_table);
	
	p_rf->dpk_done = true;
	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("===> phy_dp_calibrate_8812a\n"));
	_phy_dp_calibrate_path_a_8812a(p_dm);
	_phy_dp_calibrate_path_b_8812a(p_dm);
	ODM_RT_TRACE(p_dm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("<=== phy_dp_calibrate_8812a\n"));
#endif
}

