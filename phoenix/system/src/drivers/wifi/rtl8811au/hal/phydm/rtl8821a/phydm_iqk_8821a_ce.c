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

#include "mp_precomp.h"
#include "../phydm_precomp.h"



/*---------------------------Define Local Constant---------------------------*/
#define cal_num_8821A 3
#define MACBB_REG_NUM_8821A 8
#define AFE_REG_NUM_8821A 4
#define RF_REG_NUM_8821A 3
/*---------------------------Define Local Constant---------------------------*/
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
void do_iqk_8821a(
	void		*p_dm_void,
	u8		delta_thermal_index,
	u8		thermal_value,
	u8		threshold
)
{
	struct PHY_DM_STRUCT	*p_dm_odm = (struct PHY_DM_STRUCT *)p_dm_void;
	p_dm_odm->rf_calibrate_info.thermal_value_iqk = thermal_value;
	phy_iq_calibrate_8821a(p_dm_odm, false);
}
#endif
void _iqk_rx_fill_iqc_8821a(
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
		odm_set_bb_reg(p_dm_odm, 0xc10, 0x03ff0000, (RX_Y >> 1) & 0x000003ff);
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RX_X = %x;;RX_Y = %x ====>fill to IQC\n", RX_X >> 1, RX_Y >> 1));
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xc10 = %x ====>fill to IQC\n", odm_read_4byte(p_dm_odm, 0xc10)));
	}
	break;
	default:
		break;
	};
}

void _iqk_tx_fill_iqc_8821a(
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
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("TX_X = %x;;TX_Y = %x =====> fill to IQC\n", TX_X, TX_Y));
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("0xcd4 = %x;;0xccc = %x ====>fill to IQC\n", odm_get_bb_reg(p_dm_odm, 0xcd4, 0x000007ff), odm_get_bb_reg(p_dm_odm, 0xccc, 0x000007ff)));
	}
	break;
	default:
		break;
	};
}

void _iqk_backup_mac_bb_8821a(
	struct PHY_DM_STRUCT	*p_dm_odm,
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

void _iqk_backup_rf_8821a(
	struct PHY_DM_STRUCT	*p_dm_odm,
	u32		*RFA_backup,
	u32		*RFB_backup,
	u32		*backup_rf_reg,
	u32		RF_NUM
)
{

	u32 i;
	odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	/* Save RF Parameters */
	for (i = 0; i < RF_NUM; i++)
		RFA_backup[i] = odm_get_rf_reg(p_dm_odm, ODM_RF_PATH_A, backup_rf_reg[i], MASKDWORD);
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("BackupRF Success!!!!\n"));
}

void _iqk_backup_afe_8821a(
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

void _iqk_restore_mac_bb_8821a(
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

void _iqk_restore_rf_8821a(
	struct PHY_DM_STRUCT			*p_dm_odm,
	enum odm_rf_radio_path_e	path,
	u32				*backup_rf_reg,
	u32				*RF_backup,
	u32				RF_REG_NUM
)
{
	u32 i;

	odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	for (i = 0; i < RF_REG_NUM; i++)
		odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, backup_rf_reg[i], RFREGOFFSETMASK, RF_backup[i]);

	switch (path) {
	case ODM_RF_PATH_A:
	{
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RestoreRF path A Success!!!!\n"));
	}
	break;
	default:
		break;
	}
}

void _iqk_restore_afe_8821a(
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
	odm_write_4byte(p_dm_odm, 0xc90, 0x00000080);
	odm_write_4byte(p_dm_odm, 0xc94, 0x00000000);
	odm_write_4byte(p_dm_odm, 0xcc4, 0x20040000);
	odm_write_4byte(p_dm_odm, 0xcc8, 0x20000000);
	odm_write_4byte(p_dm_odm, 0xcb8, 0x0);
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RestoreAFE Success!!!!\n"));
}

void _iqk_configure_mac_8821a(
	struct PHY_DM_STRUCT		*p_dm_odm
)
{
	/* ========MAC register setting======== */
	odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	odm_write_1byte(p_dm_odm, 0x522, 0x3f);
	odm_set_bb_reg(p_dm_odm, 0x550, BIT(11) | BIT(3), 0x0);
	odm_write_1byte(p_dm_odm, 0x808, 0x00);		/*		RX ante off */
	odm_set_bb_reg(p_dm_odm, 0x838, 0xf, 0xc);		/*		CCA off */
	odm_write_1byte(p_dm_odm, 0xa07, 0xf);		/*		CCK RX path off */
}

void _iqk_tx_8821a(
	struct PHY_DM_STRUCT		*p_dm_odm,
	enum odm_rf_radio_path_e path
)
{
	u32		TX_fail, RX_fail, delay_count, IQK_ready, cal_retry, cal = 0;
	int		TX_X = 0, TX_Y = 0, RX_X = 0, RX_Y = 0, tx_average = 0, rx_average = 0, rx_iqk_loop = 0, RX_X_temp = 0, RX_Y_temp = 0;
	int		TX_X0[cal_num_8821A], TX_Y0[cal_num_8821A], RX_X0[2][cal_num_8821A], RX_Y0[2][cal_num_8821A];
	boolean	TX0IQKOK = false, RX0IQKOK = false;
	boolean	VDF_enable = false;
	int			i, k, VDF_Y[3], VDF_X[3], tx_dt[3], ii, dx = 0, dy = 0, TX_finish = 0, RX_finish1 = 0, RX_finish2 = 0;

	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("band_width = %d, support_interface = %d, ext_pa = %d, ext_pa_5g = %d\n", *p_dm_odm->p_band_width, p_dm_odm->support_interface, p_dm_odm->ext_pa, p_dm_odm->ext_pa_5g));
	if (*p_dm_odm->p_band_width == 2)
		VDF_enable = true;

	while (cal < cal_num_8821A) {
		switch (path) {
		case ODM_RF_PATH_A:
		{
			/* path-A LOK */
			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
			/* ========path-A AFE all on======== */
			/* Port 0 DAC/ADC on */
			odm_write_4byte(p_dm_odm, 0xc60, 0x77777777);
			odm_write_4byte(p_dm_odm, 0xc64, 0x77777777);

			odm_write_4byte(p_dm_odm, 0xc68, 0x19791979);

			odm_set_bb_reg(p_dm_odm, 0xc00, 0xf, 0x4);/*	hardware 3-wire off */

			/* LOK setting */
			/* ====== LOK ====== */
			/* 1. DAC/ADC sampling rate (160 MHz) */
			odm_set_bb_reg(p_dm_odm, 0xc5c, BIT(26) | BIT(25) | BIT(24), 0x7);

			/* 2. LoK RF setting (at BW = 20M) */
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0xef, RFREGOFFSETMASK, 0x80002);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x18, 0x00c00, 0x3);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x30, RFREGOFFSETMASK, 0x20000);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x31, RFREGOFFSETMASK, 0x0003f);

			if (p_dm_odm->rf_calibrate_info.is_iqk_pa_off == 1)
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x32, RFREGOFFSETMASK, 0xf3ec3);
			else
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x32, RFREGOFFSETMASK, 0xf3fc3);

			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x65, RFREGOFFSETMASK, 0x931d5);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x8f, RFREGOFFSETMASK, 0x8a001);
			odm_write_4byte(p_dm_odm, 0x90c, 0x00008000);
			odm_set_bb_reg(p_dm_odm, 0xc94, BIT(0), 0x1);
			odm_write_4byte(p_dm_odm, 0x978, 0x29002000);/* TX (X,Y) */
			odm_write_4byte(p_dm_odm, 0x97c, 0xa9002000);/* RX (X,Y) */
			odm_write_4byte(p_dm_odm, 0x984, 0x00462910);/* [0]:AGC_en, [15]:idac_K_Mask */

			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1); /* [31] = 1 --> Page C1 */

			if (p_dm_odm->ext_pa_5g)
				odm_write_4byte(p_dm_odm, 0xc88, 0x821403f7);
			else
				odm_write_4byte(p_dm_odm, 0xc88, 0x821403f4);

			if (*p_dm_odm->p_band_type == ODM_BAND_5G)
				odm_write_4byte(p_dm_odm, 0xc8c, 0x68163e96);
			else
				odm_write_4byte(p_dm_odm, 0xc8c, 0x28163e96);

			odm_write_4byte(p_dm_odm, 0xc80, 0x18008c10);/* TX_Tone_idx[9:0], TxK_Mask[29] TX_Tone = 16 */
			odm_write_4byte(p_dm_odm, 0xc84, 0x38008c10);/* RX_Tone_idx[9:0], RxK_Mask[29] */
			odm_write_4byte(p_dm_odm, 0xcb8, 0x00100000);/* cb8[20] 將 SI/PI 使用權切給 iqk_dpk module */
			odm_write_4byte(p_dm_odm, 0x980, 0xfa000000);
			odm_write_4byte(p_dm_odm, 0x980, 0xf8000000);

			ODM_delay_ms(10); /* delay 10ms */
			odm_write_4byte(p_dm_odm, 0xcb8, 0x00000000);

			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x58, 0x7fe00, odm_get_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x8, 0xffc00));
			switch (*p_dm_odm->p_band_width) {
			case 1:
			{
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x18, 0x00c00, 0x1);
			}
			break;
			case 2:
			{
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x18, 0x00c00, 0x0);
			}
			break;
			default:
				break;
			}
			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1); /* [31] = 1 --> Page C1 */

			/* 3. TX RF setting */
			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0xef, RFREGOFFSETMASK, 0x80000);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x30, RFREGOFFSETMASK, 0x20000);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x31, RFREGOFFSETMASK, 0x0003f);

			if (p_dm_odm->rf_calibrate_info.is_iqk_pa_off == 1)
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x32, RFREGOFFSETMASK, 0xf3ec3);
			else
				odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x32, RFREGOFFSETMASK, 0xf3fc3);

			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x65, RFREGOFFSETMASK, 0x931d5);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x8f, RFREGOFFSETMASK, 0x8a001);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0xef, RFREGOFFSETMASK, 0x00000);
			odm_write_4byte(p_dm_odm, 0x90c, 0x00008000);
			odm_set_bb_reg(p_dm_odm, 0xc94, BIT(0), 0x1);
			odm_write_4byte(p_dm_odm, 0x978, 0x29002000);/* TX (X,Y) */
			odm_write_4byte(p_dm_odm, 0x97c, 0xa9002000);/* RX (X,Y) */
			odm_write_4byte(p_dm_odm, 0x984, 0x0046a910);/* [0]:AGC_en, [15]:idac_K_Mask */

			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1); /* [31] = 1 --> Page C1 */

			if (p_dm_odm->ext_pa_5g)
				odm_write_4byte(p_dm_odm, 0xc88, 0x821403f7);
			else
				odm_write_4byte(p_dm_odm, 0xc88, 0x821403e3);

			if (*p_dm_odm->p_band_type == ODM_BAND_5G)
				odm_write_4byte(p_dm_odm, 0xc8c, 0x40163e96);
			else
				odm_write_4byte(p_dm_odm, 0xc8c, 0x00163e96);

			if (VDF_enable == 1) {
				for (k = 0; k <= 2; k++) {
					switch (k) {
					case 0:
					{
						odm_write_4byte(p_dm_odm, 0xc80, 0x18008c38);/* TX_Tone_idx[9:0], TxK_Mask[29] TX_Tone = 16 */
						odm_write_4byte(p_dm_odm, 0xc84, 0x38008c38);/* RX_Tone_idx[9:0], RxK_Mask[29] */
						odm_set_bb_reg(p_dm_odm, 0xce8, BIT(31), 0x0);
					}
					break;
					case 1:
					{
						odm_set_bb_reg(p_dm_odm, 0xc80, BIT(28), 0x0);
						odm_set_bb_reg(p_dm_odm, 0xc84, BIT(28), 0x0);
						odm_set_bb_reg(p_dm_odm, 0xce8, BIT(31), 0x0);
					}
					break;
					case 2:
					{
						ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("VDF_Y[1] = %x;;;VDF_Y[0] = %x\n", VDF_Y[1] >> 21 & 0x00007ff, VDF_Y[0] >> 21 & 0x00007ff));
						ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("VDF_X[1] = %x;;;VDF_X[0] = %x\n", VDF_X[1] >> 21 & 0x00007ff, VDF_X[0] >> 21 & 0x00007ff));
						tx_dt[cal] = (VDF_Y[1] >> 20) - (VDF_Y[0] >> 20);
						tx_dt[cal] = ((16 * tx_dt[cal]) * 10000 / 15708);
						tx_dt[cal] = (tx_dt[cal] >> 1) + (tx_dt[cal] & BIT(0));
						odm_write_4byte(p_dm_odm, 0xc80, 0x18008c20);/* TX_Tone_idx[9:0], TxK_Mask[29] TX_Tone = 16 */
						odm_write_4byte(p_dm_odm, 0xc84, 0x38008c20);/* RX_Tone_idx[9:0], RxK_Mask[29] */
						odm_set_bb_reg(p_dm_odm, 0xce8, BIT(31), 0x1);
						odm_set_bb_reg(p_dm_odm, 0xce8, 0x3fff0000, tx_dt[cal] & 0x00003fff);
					}
					break;
					}
					odm_write_4byte(p_dm_odm, 0xcb8, 0x00100000);/* cb8[20] 將 SI/PI 使用權切給 iqk_dpk module */
					cal_retry = 0;
					while (1) {
						/* one shot */
						odm_write_4byte(p_dm_odm, 0x980, 0xfa000000);
						odm_write_4byte(p_dm_odm, 0x980, 0xf8000000);

						ODM_delay_ms(10); /* delay 10ms */
						odm_write_4byte(p_dm_odm, 0xcb8, 0x00000000);
						delay_count = 0;
						while (1) {
							IQK_ready = odm_get_bb_reg(p_dm_odm, 0xd00, BIT(10));
							if ((~IQK_ready) || (delay_count > 20))
								break;
							else {
								ODM_delay_ms(1);
								delay_count++;
							}
						}

						if (delay_count < 20) {							/* If 20ms No Result, then cal_retry++ */
							/* ============TXIQK Check============== */
							TX_fail = odm_get_bb_reg(p_dm_odm, 0xd00, BIT(12));

							if (~TX_fail) {
								odm_write_4byte(p_dm_odm, 0xcb8, 0x02000000);
								VDF_X[k] = odm_get_bb_reg(p_dm_odm, 0xd00, 0x07ff0000) << 21;
								odm_write_4byte(p_dm_odm, 0xcb8, 0x04000000);
								VDF_Y[k] = odm_get_bb_reg(p_dm_odm, 0xd00, 0x07ff0000) << 21;
								TX0IQKOK = true;
								break;
							} else {
								odm_set_bb_reg(p_dm_odm, 0xccc, 0x000007ff, 0x0);
								odm_set_bb_reg(p_dm_odm, 0xcd4, 0x000007ff, 0x200);
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
				if (k == 3) {
					TX_X0[cal] = VDF_X[k - 1] ;
					TX_Y0[cal] = VDF_Y[k - 1];
				}
			} else {
				odm_write_4byte(p_dm_odm, 0xc80, 0x18008c10);/* TX_Tone_idx[9:0], TxK_Mask[29] TX_Tone = 16 */
				odm_write_4byte(p_dm_odm, 0xc84, 0x38008c10);/* RX_Tone_idx[9:0], RxK_Mask[29] */
				odm_write_4byte(p_dm_odm, 0xcb8, 0x00100000);/* cb8[20] 將 SI/PI 使用權切給 iqk_dpk module */
				cal_retry = 0;
				while (1) {
					/* one shot */
					odm_write_4byte(p_dm_odm, 0x980, 0xfa000000);
					odm_write_4byte(p_dm_odm, 0x980, 0xf8000000);

					ODM_delay_ms(10); /* delay 10ms */
					odm_write_4byte(p_dm_odm, 0xcb8, 0x00000000);
					delay_count = 0;
					while (1) {
						IQK_ready = odm_get_bb_reg(p_dm_odm, 0xd00, BIT(10));
						if ((~IQK_ready) || (delay_count > 20))
							break;
						else {
							ODM_delay_ms(1);
							delay_count++;
						}
					}

					if (delay_count < 20) {							/* If 20ms No Result, then cal_retry++ */
						/* ============TXIQK Check============== */
						TX_fail = odm_get_bb_reg(p_dm_odm, 0xd00, BIT(12));

						if (~TX_fail) {
							odm_write_4byte(p_dm_odm, 0xcb8, 0x02000000);
							TX_X0[cal] = odm_get_bb_reg(p_dm_odm, 0xd00, 0x07ff0000) << 21;
							odm_write_4byte(p_dm_odm, 0xcb8, 0x04000000);
							TX_Y0[cal] = odm_get_bb_reg(p_dm_odm, 0xd00, 0x07ff0000) << 21;
							TX0IQKOK = true;
							break;
						} else {
							odm_set_bb_reg(p_dm_odm, 0xccc, 0x000007ff, 0x0);
							odm_set_bb_reg(p_dm_odm, 0xcd4, 0x000007ff, 0x200);
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

			if (TX0IQKOK == false)
				break;				/* TXK fail, Don't do RXK */

			/* ====== RX IQK ====== */
			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
			/* 1. RX RF setting */
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0xef, RFREGOFFSETMASK, 0x80000);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x30, RFREGOFFSETMASK, 0x30000);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x31, RFREGOFFSETMASK, 0x0002f);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x32, RFREGOFFSETMASK, 0xfffbb);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x8f, RFREGOFFSETMASK, 0x88001);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0x65, RFREGOFFSETMASK, 0x931d8);
			odm_set_rf_reg(p_dm_odm, (enum odm_rf_radio_path_e)path, 0xef, RFREGOFFSETMASK, 0x00000);

			odm_set_bb_reg(p_dm_odm, 0x978, 0x03FF8000, (TX_X0[cal]) >> 21 & 0x000007ff);
			odm_set_bb_reg(p_dm_odm, 0x978, 0x000007FF, (TX_Y0[cal]) >> 21 & 0x000007ff);
			odm_set_bb_reg(p_dm_odm, 0x978, BIT(31), 0x1);
			odm_set_bb_reg(p_dm_odm, 0x97c, BIT(31), 0x0);
			odm_write_4byte(p_dm_odm, 0x90c, 0x00008000);
			odm_write_4byte(p_dm_odm, 0x984, 0x0046a911);

			odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1); /* [31] = 1 --> Page C1 */
			odm_write_4byte(p_dm_odm, 0xc80, 0x38008c10);/* TX_Tone_idx[9:0], TxK_Mask[29] TX_Tone = 16 */
			odm_write_4byte(p_dm_odm, 0xc84, 0x18008c10);/* RX_Tone_idx[9:0], RxK_Mask[29] */
			odm_write_4byte(p_dm_odm, 0xc88, 0x02140119);

			if (p_dm_odm->support_interface == 1) {
				rx_iqk_loop = 2;				/* for 2% fail; */
			} else
				rx_iqk_loop = 1;
			for (i = 0; i < rx_iqk_loop; i++) {
				if (p_dm_odm->support_interface == 1)
					if (i == 0)
						odm_write_4byte(p_dm_odm, 0xc8c, 0x28161100);  /* Good */
					else
						odm_write_4byte(p_dm_odm, 0xc8c, 0x28160d00);
				else
					odm_write_4byte(p_dm_odm, 0xc8c, 0x28160d00);

				odm_write_4byte(p_dm_odm, 0xcb8, 0x00100000);/* cb8[20] 將 SI/PI 使用權切給 iqk_dpk module */

				cal_retry = 0;
				while (1) {
					/* one shot */
					odm_write_4byte(p_dm_odm, 0x980, 0xfa000000);
					odm_write_4byte(p_dm_odm, 0x980, 0xf8000000);

					ODM_delay_ms(10); /* delay 10ms */
					odm_write_4byte(p_dm_odm, 0xcb8, 0x00000000);
					delay_count = 0;
					while (1) {
						IQK_ready = odm_get_bb_reg(p_dm_odm, 0xd00, BIT(10));
						if ((~IQK_ready) || (delay_count > 20))
							break;
						else {
							ODM_delay_ms(1);
							delay_count++;
						}
					}

					if (delay_count < 20) {	/* If 20ms No Result, then cal_retry++ */
						/* ============RXIQK Check============== */
						RX_fail = odm_get_bb_reg(p_dm_odm, 0xd00, BIT(11));
						if (RX_fail == 0) {
							/*
							dbg_print("====== RXIQK (%d) ======", i);
							odm_write_4byte(p_dm_odm, 0xcb8, 0x05000000);
							reg1 = odm_get_bb_reg(p_dm_odm, 0xd00, 0xffffffff);
							odm_write_4byte(p_dm_odm, 0xcb8, 0x06000000);
							reg2 = odm_get_bb_reg(p_dm_odm, 0xd00, 0x0000001f);
							dbg_print("reg1 = %d, reg2 = %d", reg1, reg2);
							image_power = (reg2<<32)+reg1;
							dbg_print("Before PW = %d\n", image_power);
							odm_write_4byte(p_dm_odm, 0xcb8, 0x07000000);
							reg1 = odm_get_bb_reg(p_dm_odm, 0xd00, 0xffffffff);
							odm_write_4byte(p_dm_odm, 0xcb8, 0x08000000);
							reg2 = odm_get_bb_reg(p_dm_odm, 0xd00, 0x0000001f);
							image_power = (reg2<<32)+reg1;
							dbg_print("After PW = %d\n", image_power);
							*/

							odm_write_4byte(p_dm_odm, 0xcb8, 0x06000000);
							RX_X0[i][cal] = odm_get_bb_reg(p_dm_odm, 0xd00, 0x07ff0000) << 21;
							odm_write_4byte(p_dm_odm, 0xcb8, 0x08000000);
							RX_Y0[i][cal] = odm_get_bb_reg(p_dm_odm, 0xd00, 0x07ff0000) << 21;
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

			if (TX0IQKOK)
				tx_average++;
			if (RX0IQKOK)
				rx_average++;
		}
		break;
		default:
			break;
		}
		cal++;
	}
	/* FillIQK Result */
	switch (path) {
	case ODM_RF_PATH_A:
	{
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("========Path_A =======\n"));
		if (tx_average == 0)
			break;

		for (i = 0; i < tx_average; i++)
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("TX_X0[%d] = %x ;; TX_Y0[%d] = %x\n", i, (TX_X0[i]) >> 21 & 0x000007ff, i, (TX_Y0[i]) >> 21 & 0x000007ff));
		for (i = 0; i < tx_average; i++) {
			for (ii = i + 1; ii < tx_average; ii++) {
				dx = (TX_X0[i] >> 21) - (TX_X0[ii] >> 21);
				if (dx < 3 && dx > -3) {
					dy = (TX_Y0[i] >> 21) - (TX_Y0[ii] >> 21);
					if (dy < 3 && dy > -3) {
						TX_X = ((TX_X0[i] >> 21) + (TX_X0[ii] >> 21)) / 2;
						TX_Y = ((TX_Y0[i] >> 21) + (TX_Y0[ii] >> 21)) / 2;
						TX_finish = 1;
						break;
					}
				}
			}
			if (TX_finish == 1)
				break;
		}

		if (TX_finish == 1)
			_iqk_tx_fill_iqc_8821a(p_dm_odm, path, TX_X, TX_Y);
		else
			_iqk_tx_fill_iqc_8821a(p_dm_odm, path, 0x200, 0x0);

		if (rx_average == 0)
			break;

		for (i = 0; i < rx_average; i++) {
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RX_X0[0][%d] = %x ;; RX_Y0[0][%d] = %x\n", i, (RX_X0[0][i]) >> 21 & 0x000007ff, i, (RX_Y0[0][i]) >> 21 & 0x000007ff));
			if (rx_iqk_loop == 2)
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("RX_X0[1][%d] = %x ;; RX_Y0[1][%d] = %x\n", i, (RX_X0[1][i]) >> 21 & 0x000007ff, i, (RX_Y0[1][i]) >> 21 & 0x000007ff));
		}
		for (i = 0; i < rx_average; i++) {
			for (ii = i + 1; ii < rx_average; ii++) {
				dx = (RX_X0[0][i] >> 21) - (RX_X0[0][ii] >> 21);
				if (dx < 4 && dx > -4) {
					dy = (RX_Y0[0][i] >> 21) - (RX_Y0[0][ii] >> 21);
					if (dy < 4 && dy > -4) {
						RX_X_temp = ((RX_X0[0][i] >> 21) + (RX_X0[0][ii] >> 21)) / 2;
						RX_Y_temp = ((RX_Y0[0][i] >> 21) + (RX_Y0[0][ii] >> 21)) / 2;
						RX_finish1 = 1;
						break;
					}
				}
			}
			if (RX_finish1 == 1) {
				RX_X = RX_X_temp;
				RX_Y = RX_Y_temp;
				break;
			}
		}
		if (rx_iqk_loop == 2) {
			for (i = 0; i < rx_average; i++) {
				for (ii = i + 1; ii < rx_average; ii++) {
					dx = (RX_X0[1][i] >> 21) - (RX_X0[1][ii] >> 21);
					if (dx < 4 && dx > -4) {
						dy = (RX_Y0[1][i] >> 21) - (RX_Y0[1][ii] >> 21);
						if (dy < 4 && dy > -4) {
							RX_X = ((RX_X0[1][i] >> 21) + (RX_X0[1][ii] >> 21)) / 2;
							RX_Y = ((RX_Y0[1][i] >> 21) + (RX_Y0[1][ii] >> 21)) / 2;
							RX_finish2 = 1;
							break;
						}
					}
				}
				if (RX_finish2 == 1)
					break;
			}
			if (RX_finish1 && RX_finish2) {
				RX_X = (RX_X + RX_X_temp) / 2;
				RX_Y = (RX_Y + RX_Y_temp) / 2;
			}
		}
		if (RX_finish1 || RX_finish2)
			_iqk_rx_fill_iqc_8821a(p_dm_odm, path, RX_X, RX_Y);
		else
			_iqk_rx_fill_iqc_8821a(p_dm_odm, path, 0x200, 0x0);
	}
	break;
	default:
		break;
	}
}

#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
void
_phy_iq_calibrate_by_fw_8821a(
	struct PHY_DM_STRUCT	*p_dm_odm
)
{

	u8			iqk_cmd[3] = { *p_dm_odm->p_channel, 0x0, 0x0};
	u8			buf1 = 0x0;
	u8			buf2 = 0x0;
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("p_channel: %d\n", *p_dm_odm->p_channel));


	/* Byte 2, Bit 4 ~ Bit 5 : band_type */
	if (*p_dm_odm->p_band_type == ODM_BAND_5G)
		buf1 = 0x2 << 4;
	else
		buf1 = 0x1 << 4;

	/* Byte 2, Bit 0 ~ Bit 3 : bandwidth */
	if (*p_dm_odm->p_band_width == ODM_BW20M)
		buf2 = 0x1;
	else if (*p_dm_odm->p_band_width == ODM_BW40M)
		buf2 = 0x1 << 1;
	else if (*p_dm_odm->p_band_width == ODM_BW80M)
		buf2 = 0x1 << 2;
	else
		buf2 = 0x1 << 3;

	iqk_cmd[1] = buf1 | buf2;
	iqk_cmd[2] = p_dm_odm->ext_pa_5g | p_dm_odm->ext_lna_5g << 1;

	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("== FW IQK Start ==\n"));
	p_dm_odm->rf_calibrate_info.iqk_start_time = 0;
	p_dm_odm->rf_calibrate_info.iqk_start_time = odm_get_current_time(p_dm_odm);
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("== start_time: %lld\n", p_dm_odm->rf_calibrate_info.iqk_start_time));
	odm_fill_h2c_cmd(p_dm_odm, ODM_H2C_IQ_CALIBRATION, 3, iqk_cmd);


}
#endif

void
_phy_iq_calibrate_8821a(
	struct PHY_DM_STRUCT		*p_dm_odm
)
{
	u32	MACBB_backup[MACBB_REG_NUM_8821A], AFE_backup[AFE_REG_NUM_8821A], RFA_backup[RF_REG_NUM_8821A], RFB_backup[RF_REG_NUM_8821A];
	u32	backup_macbb_reg[MACBB_REG_NUM_8821A] = {0x520, 0x550, 0x808, 0xa04, 0x90c, 0xc00, 0x838, 0x82c};
	u32	backup_afe_reg[AFE_REG_NUM_8821A] = {0xc5c, 0xc60, 0xc64, 0xc68};
	u32	backup_rf_reg[RF_REG_NUM_8821A] = {0x65, 0x8f, 0x0};

	_iqk_backup_mac_bb_8821a(p_dm_odm, MACBB_backup, backup_macbb_reg, MACBB_REG_NUM_8821A);
	_iqk_backup_afe_8821a(p_dm_odm, AFE_backup, backup_afe_reg, AFE_REG_NUM_8821A);
	_iqk_backup_rf_8821a(p_dm_odm, RFA_backup, RFB_backup, backup_rf_reg, RF_REG_NUM_8821A);

	_iqk_configure_mac_8821a(p_dm_odm);
	_iqk_tx_8821a(p_dm_odm, ODM_RF_PATH_A);

	_iqk_restore_rf_8821a(p_dm_odm, ODM_RF_PATH_A, backup_rf_reg, RFA_backup, RF_REG_NUM_8821A);
	_iqk_restore_afe_8821a(p_dm_odm, AFE_backup, backup_afe_reg, AFE_REG_NUM_8821A);
	_iqk_restore_mac_bb_8821a(p_dm_odm, MACBB_backup, backup_macbb_reg, MACBB_REG_NUM_8821A);
}

void
phy_reset_iqk_result_8821a(
	struct PHY_DM_STRUCT	*p_dm_odm
)
{
	odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x1); /* [31] = 1 --> Page C1 */
	odm_set_bb_reg(p_dm_odm, 0xccc, 0x000007ff, 0x0);
	odm_set_bb_reg(p_dm_odm, 0xcd4, 0x000007ff, 0x200);
	odm_write_4byte(p_dm_odm, 0xce8, 0x0);
	odm_set_bb_reg(p_dm_odm, 0x82c, BIT(31), 0x0); /* [31] = 0 --> Page C */
	odm_set_bb_reg(p_dm_odm, 0xc10, 0x000003ff, 0x100);
}


#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
void
phy_iq_calibrate_8821a(
	struct PHY_DM_STRUCT	*p_dm_odm,
	boolean	is_recovery
)
{
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct _ADAPTER		*p_adapter = p_dm_odm->adapter;
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	u32			counter = 0;
#endif
#endif

#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
	if (odm_check_power_status(p_adapter) == false)
		return;
#endif

	if (p_dm_odm->mp_mode) {
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
#if (MP_DRIVER == 1)
		PMPT_CONTEXT	p_mpt_ctx = &(p_adapter->mpt_ctx);
		if (p_mpt_ctx->is_single_tone || p_mpt_ctx->is_carrier_suppression)
			return;
#endif
#else	/*(DM_ODM_SUPPORT_TYPE == ODM_CE)*/
		PMPT_CONTEXT	p_mpt_ctx = &(p_adapter->mppriv.mpt_ctx);
		if (p_mpt_ctx->is_single_tone || p_mpt_ctx->is_carrier_suppression)
			return;
#endif
	}
	/* 3 == FW IQK == */
	if (p_dm_odm->iqk_fw_offload && !(p_dm_odm->mp_mode)) {
		if (!p_dm_odm->rf_calibrate_info.is_iqk_in_progress) {
			odm_acquire_spin_lock(p_dm_odm, RT_IQK_SPINLOCK);
			p_dm_odm->rf_calibrate_info.is_iqk_in_progress = true;
			odm_release_spin_lock(p_dm_odm, RT_IQK_SPINLOCK);
			_phy_iq_calibrate_by_fw_8821a(p_dm_odm);
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
			for (counter = 0; counter < 10; counter++) {
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("== FW IQK PROGRESS == #%d\n", counter));
				ODM_delay_ms(50);
				if (!p_dm_odm->rf_calibrate_info.is_iqk_in_progress) {
					ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("== FW IQK RETURN FROM WAITING ==\n"));
					break;
				}
			}
#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
			rtl8812_iqk_wait(p_adapter, 500);
#endif
			if (p_dm_odm->rf_calibrate_info.is_iqk_in_progress) {
				ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("== FW IQK TIMEOUT (Still in progress after 500ms) ==\n"));
				odm_acquire_spin_lock(p_dm_odm, RT_IQK_SPINLOCK);
				p_dm_odm->rf_calibrate_info.is_iqk_in_progress = false;
				odm_release_spin_lock(p_dm_odm, RT_IQK_SPINLOCK);
			}
		} else
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("== Return the IQK CMD, because the IQK in Progress ==\n"));
	}
	/* 3 == Driver IQK == */
	else {
		if (!p_dm_odm->rf_calibrate_info.is_iqk_in_progress) {
			odm_acquire_spin_lock(p_dm_odm, RT_IQK_SPINLOCK);
			p_dm_odm->rf_calibrate_info.is_iqk_in_progress = true;
			odm_release_spin_lock(p_dm_odm, RT_IQK_SPINLOCK);

			p_dm_odm->rf_calibrate_info.iqk_start_time = odm_get_current_time(p_dm_odm);
			_phy_iq_calibrate_8821a(p_dm_odm);
			p_dm_odm->rf_calibrate_info.iqk_progressing_time = odm_get_progressing_time(p_dm_odm, p_dm_odm->rf_calibrate_info.iqk_start_time);
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("IQK progressing_time = %lld ms\n", p_dm_odm->rf_calibrate_info.iqk_progressing_time));

			odm_acquire_spin_lock(p_dm_odm, RT_IQK_SPINLOCK);
			p_dm_odm->rf_calibrate_info.is_iqk_in_progress = false;
			odm_release_spin_lock(p_dm_odm, RT_IQK_SPINLOCK);
		} else
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("== Return the IQK CMD, because the IQK in Progress ==\n"));
	}

}
#endif
