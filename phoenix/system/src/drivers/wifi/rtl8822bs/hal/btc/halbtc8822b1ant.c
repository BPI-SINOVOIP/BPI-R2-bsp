/******************************************************************************
 *
 * Copyright(c) 2016 - 2017 Realtek Corporation.
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

#if (BT_SUPPORT == 1 && COEX_SUPPORT == 1)

#if (RTL8822B_SUPPORT == 1)
static u8 *trace_buf = &gl_btc_trace_buf[0];

static const char *const glbt_info_src_8822b_1ant[] = {
	"BT Info[wifi fw]",
	"BT Info[bt rsp]",
	"BT Info[bt auto report]",
};

u32 glcoex_ver_date_8822b_1ant = 20180430;
u32 glcoex_ver_8822b_1ant = 0x5e;
u32 glcoex_ver_btdesired_8822b_1ant = 0x5a;

#if 0
static
u8 halbtc8822b1ant_bt_rssi_state(struct btc_coexist *btc,
				 u8 *ppre_bt_rssi_state, u8 level_num,
				 u8 rssi_thresh, u8 rssi_thresh1)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	s32	bt_rssi = 0;
	u8			bt_rssi_state = *ppre_bt_rssi_state;

	bt_rssi = coex_sta->bt_rssi;

	if (level_num == 2) {
		if ((*ppre_bt_rssi_state == BTC_RSSI_STATE_LOW) ||
		    (*ppre_bt_rssi_state == BTC_RSSI_STATE_STAY_LOW)) {
			if (bt_rssi >= (rssi_thresh +
					BTC_RSSI_COEX_THRESH_TOL_8822B_1ANT))
				bt_rssi_state = BTC_RSSI_STATE_HIGH;
			else
				bt_rssi_state = BTC_RSSI_STATE_STAY_LOW;
		} else {
			if (bt_rssi < rssi_thresh)
				bt_rssi_state = BTC_RSSI_STATE_LOW;
			else
				bt_rssi_state = BTC_RSSI_STATE_STAY_HIGH;
		}
	} else if (level_num == 3) {
		if (rssi_thresh > rssi_thresh1) {
			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], BT Rssi thresh error!!\n");
			BTC_TRACE(trace_buf);
			return *ppre_bt_rssi_state;
		}

		if ((*ppre_bt_rssi_state == BTC_RSSI_STATE_LOW) ||
		    (*ppre_bt_rssi_state == BTC_RSSI_STATE_STAY_LOW)) {
			if (bt_rssi >= (rssi_thresh +
					BTC_RSSI_COEX_THRESH_TOL_8822B_1ANT))
				bt_rssi_state = BTC_RSSI_STATE_MEDIUM;
			else
				bt_rssi_state = BTC_RSSI_STATE_STAY_LOW;
		} else if ((*ppre_bt_rssi_state == BTC_RSSI_STATE_MEDIUM) ||
			(*ppre_bt_rssi_state == BTC_RSSI_STATE_STAY_MEDIUM)) {
			if (bt_rssi >= (rssi_thresh1 +
					BTC_RSSI_COEX_THRESH_TOL_8822B_1ANT))
				bt_rssi_state = BTC_RSSI_STATE_HIGH;
			else if (bt_rssi < rssi_thresh)
				bt_rssi_state = BTC_RSSI_STATE_LOW;
			else
				bt_rssi_state = BTC_RSSI_STATE_STAY_MEDIUM;
		} else {
			if (bt_rssi < rssi_thresh1)
				bt_rssi_state = BTC_RSSI_STATE_MEDIUM;
			else
				bt_rssi_state = BTC_RSSI_STATE_STAY_HIGH;
		}
	}

	*ppre_bt_rssi_state = bt_rssi_state;

	return bt_rssi_state;
}
#endif

static u8 halbtc8822b1ant_wifi_rssi_state(struct btc_coexist *btc,
					  u8 *pprewifi_rssi_state, u8 level_num,
					  u8 rssi_thresh, u8 rssi_thresh1)
{
	s32 wifi_rssi = 0;
	u8 wifi_rssi_state = *pprewifi_rssi_state;

	btc->btc_get(btc, BTC_GET_S4_WIFI_RSSI, &wifi_rssi);

	if (level_num == 2) {
		if ((*pprewifi_rssi_state == BTC_RSSI_STATE_LOW) ||
		    (*pprewifi_rssi_state == BTC_RSSI_STATE_STAY_LOW)) {
			if (wifi_rssi >= (rssi_thresh +
					  BTC_RSSI_COEX_THRESH_TOL_8822B_1ANT))
				wifi_rssi_state = BTC_RSSI_STATE_HIGH;
			else
				wifi_rssi_state = BTC_RSSI_STATE_STAY_LOW;
		} else {
			if (wifi_rssi < rssi_thresh)
				wifi_rssi_state = BTC_RSSI_STATE_LOW;
			else
				wifi_rssi_state = BTC_RSSI_STATE_STAY_HIGH;
		}
	} else if (level_num == 3) {
		if (rssi_thresh > rssi_thresh1) {
			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], wifi RSSI thresh error!!\n");
			BTC_TRACE(trace_buf);
			return *pprewifi_rssi_state;
		}

		if ((*pprewifi_rssi_state == BTC_RSSI_STATE_LOW) ||
		    (*pprewifi_rssi_state == BTC_RSSI_STATE_STAY_LOW)) {
			if (wifi_rssi >= (rssi_thresh +
					  BTC_RSSI_COEX_THRESH_TOL_8822B_1ANT))
				wifi_rssi_state = BTC_RSSI_STATE_MEDIUM;
			else
				wifi_rssi_state = BTC_RSSI_STATE_STAY_LOW;
		} else if ((*pprewifi_rssi_state == BTC_RSSI_STATE_MEDIUM) ||
			   (*pprewifi_rssi_state ==
			    BTC_RSSI_STATE_STAY_MEDIUM)) {
			if (wifi_rssi >= (rssi_thresh1 +
					  BTC_RSSI_COEX_THRESH_TOL_8822B_1ANT))
				wifi_rssi_state = BTC_RSSI_STATE_HIGH;
			else if (wifi_rssi < rssi_thresh)
				wifi_rssi_state = BTC_RSSI_STATE_LOW;
			else
				wifi_rssi_state = BTC_RSSI_STATE_STAY_MEDIUM;
		} else {
			if (wifi_rssi < rssi_thresh1)
				wifi_rssi_state = BTC_RSSI_STATE_MEDIUM;
			else
				wifi_rssi_state = BTC_RSSI_STATE_STAY_HIGH;
		}
	}

	*pprewifi_rssi_state = wifi_rssi_state;

	return wifi_rssi_state;
}

/* rx agg size setting :
 * 1:      TRUE / don't care / don't care
 * max: FALSE / FALSE / don't care
 * 7:     FALSE / TRUE / 7
 */
static void halbtc8822b1ant_limited_rx(struct btc_coexist *btc,
				       boolean force_exec,
				       boolean rej_ap_agg_pkt,
				       boolean bt_ctrl_agg_buf_size,
				       u8 agg_buf_size)
{
	boolean reject_rx_agg = rej_ap_agg_pkt;
	boolean bt_ctrl_rx_agg_size = bt_ctrl_agg_buf_size;
	u8 rx_agg_size = agg_buf_size;

	/* ============================================ */
	/*	Rx Aggregation related setting */
	/* ============================================ */
	btc->btc_set(btc, BTC_SET_BL_TO_REJ_AP_AGG_PKT, &reject_rx_agg);
	/* decide BT control aggregation buf size or not */
	btc->btc_set(btc, BTC_SET_BL_BT_CTRL_AGG_SIZE, &bt_ctrl_rx_agg_size);
	/* aggregation buf size, only work when BT control Rx aggregation size*/
	btc->btc_set(btc, BTC_SET_U1_AGG_BUF_SIZE, &rx_agg_size);
	/* real update aggregation setting */
	btc->btc_set(btc, BTC_SET_ACT_AGGREGATE_CTRL, NULL);
}

static void halbtc8822b1ant_low_penalty_ra(struct btc_coexist *btc,
					   boolean force_exec,
					   boolean low_penalty_ra,
					   u8 thres)
{
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;
	static u8 cur_thres;

	if (!force_exec) {
		if (low_penalty_ra == coex_dm->cur_low_penalty_ra &&
		    thres == cur_thres)
			return;
	}

	if (low_penalty_ra)
		btc->btc_phydm_modify_RA_PCR_threshold(btc, 0, thres);
	else
		btc->btc_phydm_modify_RA_PCR_threshold(btc, 0, 0);

	coex_dm->cur_low_penalty_ra = low_penalty_ra;
	cur_thres = thres;
}

static void halbtc8822b1ant_write_scbd(struct btc_coexist *btc, u16 bitpos,
				       boolean state)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	static u16 originalval = 0x8002, preval;

	if (state)
		originalval = originalval | bitpos;
	else
		originalval = originalval & (~bitpos);

	coex_sta->score_board_WB = originalval;

	if (originalval != preval) {
		preval = originalval;
		btc->btc_write_2byte(btc, 0xaa, originalval);
	} else {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], %s: return for nochange\n", __func__);
		BTC_TRACE(trace_buf);
	}
}

static void halbtc8822b1ant_read_scbd(struct btc_coexist *btc,
				      u16 *score_board_val)
{
	*score_board_val = (btc->btc_read_2byte(btc, 0xaa)) & 0x7fff;
}

static void halbtc8822b1ant_query_bt_info(struct btc_coexist *btc)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	u8 h2c_parameter[1] = {0};

	if (coex_sta->bt_disabled) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], No query BT info because BT is disabled!\n");
		BTC_TRACE(trace_buf);
		return;
	}

	h2c_parameter[0] |= BIT(0); /* trigger */

	btc->btc_fill_h2c(btc, 0x61, 1, h2c_parameter);

	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
		    "[BTCoex], WL query BT info!!\n");
	BTC_TRACE(trace_buf);
}

static void halbtc8822b1ant_enable_gnt_to_gpio(struct btc_coexist *btc,
					       boolean isenable)
{
	static u8 bit_val[5] = {0, 0, 0, 0, 0};
	static boolean state;

	if (!btc->dbg_mode)
		return;

	if (state == isenable)
		return;

	state = isenable;

	if (isenable) {
		/* enable GNT_WL, GNT_BT to GPIO for debug */
		btc->btc_write_1byte_bitmask(btc, 0x73, 0x8, 0x1);

		/* store original value */
		bit_val[0] = (btc->btc_read_1byte(btc, 0x66) &
			      BIT(4)) >> 4; /*0x66[4] */
		bit_val[1] = (btc->btc_read_1byte(btc, 0x67) &
			      BIT(0)); /*0x66[8] */
		bit_val[2] = (btc->btc_read_1byte(btc, 0x42) &
			      BIT(3)) >> 3; /*0x40[19] */
		bit_val[3] = (btc->btc_read_1byte(btc, 0x65) &
			      BIT(7)) >> 7; /*0x64[15] */
		bit_val[4] = (btc->btc_read_1byte(btc, 0x72) &
			      BIT(2)) >> 2; /*0x70[18] */

		/*  switch GPIO Mux */
		btc->btc_write_1byte_bitmask(btc, 0x66, BIT(4),
						   0x0); /*0x66[4] = 0 */
		btc->btc_write_1byte_bitmask(btc, 0x67, BIT(0),
						   0x0); /*0x66[8] = 0 */
		btc->btc_write_1byte_bitmask(btc, 0x42, BIT(3),
						   0x0); /*0x40[19] = 0 */
		btc->btc_write_1byte_bitmask(btc, 0x65, BIT(7),
						   0x0); /*0x64[15] = 0 */
		btc->btc_write_1byte_bitmask(btc, 0x72, BIT(2),
						   0x0); /*0x70[18] = 0 */

	} else {
		btc->btc_write_1byte_bitmask(btc, 0x73, 0x8, 0x0);

		/*  Restore original value  */
		/*  switch GPIO Mux */
		btc->btc_write_1byte_bitmask(btc, 0x66, BIT(4),
						   bit_val[0]); /*0x66[4] = 0 */
		btc->btc_write_1byte_bitmask(btc, 0x67, BIT(0),
						   bit_val[1]); /*0x66[8] = 0 */
		btc->btc_write_1byte_bitmask(btc, 0x42, BIT(3),
						   bit_val[2]); /*0x40[19] = 0*/
		btc->btc_write_1byte_bitmask(btc, 0x65, BIT(7),
						   bit_val[3]); /*0x64[15] = 0*/
		btc->btc_write_1byte_bitmask(btc, 0x72, BIT(2),
						   bit_val[4]); /*0x70[18] = 0*/
	}
}

static void halbtc8822b1ant_monitor_bt_ctr(struct btc_coexist *btc)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	u32 reg_hp_txrx, reg_lp_txrx, u32tmp;
	u32 reg_hp_tx = 0, reg_hp_rx = 0, reg_lp_tx = 0, reg_lp_rx = 0;
	static u8 num_of_bt_counter_chk, cnt_slave, cnt_autoslot_hang;
	struct btc_bt_link_info *bt_link_info = &btc->bt_link_info;

	reg_hp_txrx = 0x770;
	reg_lp_txrx = 0x774;

	u32tmp = btc->btc_read_4byte(btc, reg_hp_txrx);
	reg_hp_tx = u32tmp & MASKLWORD;
	reg_hp_rx = (u32tmp & MASKHWORD) >> 16;

	u32tmp = btc->btc_read_4byte(btc, reg_lp_txrx);
	reg_lp_tx = u32tmp & MASKLWORD;
	reg_lp_rx = (u32tmp & MASKHWORD) >> 16;

	coex_sta->high_priority_tx = reg_hp_tx;
	coex_sta->high_priority_rx = reg_hp_rx;
	coex_sta->low_priority_tx = reg_lp_tx;
	coex_sta->low_priority_rx = reg_lp_rx;

	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
		    "[BTCoex], Hi-Pri Rx/Tx: %d/%d, Lo-Pri Rx/Tx: %d/%d\n",
		    reg_hp_rx, reg_hp_tx, reg_lp_rx, reg_lp_tx);
	BTC_TRACE(trace_buf);

	/* reset counter */
	btc->btc_write_1byte(btc, 0x76e, 0xc);

	if (coex_sta->low_priority_tx > 1150 &&
	    !coex_sta->c2h_bt_inquiry_page)
		coex_sta->pop_event_cnt++;

	if (coex_sta->low_priority_rx >= 1150 &&
	    coex_sta->low_priority_rx >= coex_sta->low_priority_tx &&
	    !coex_sta->under_ips && !coex_sta->c2h_bt_inquiry_page &&
	    coex_sta->bt_link_exist) {
		if (cnt_slave >= 3) {
			bt_link_info->slave_role = TRUE;
			cnt_slave = 3;
		} else {
			cnt_slave++;
		}
	} else {
		if (cnt_slave == 0) {
			bt_link_info->slave_role = FALSE;
			cnt_slave = 0;
		} else {
			cnt_slave--;
		}
	}

	if (coex_sta->is_tdma_btautoslot) {
		if (coex_sta->low_priority_tx >= 1300 &&
		    coex_sta->low_priority_rx <= 150) {
			if (cnt_autoslot_hang >= 2) {
				coex_sta->is_tdma_btautoslot_hang = TRUE;
				cnt_autoslot_hang = 2;
			} else {
				cnt_autoslot_hang++;
			}
		} else {
			if (cnt_autoslot_hang == 0) {
				coex_sta->is_tdma_btautoslot_hang = FALSE;
				cnt_autoslot_hang = 0;
			} else {
				cnt_autoslot_hang--;
			}
		}
	}

	if (bt_link_info->hid_only) {
		if (coex_sta->low_priority_tx > 100)
			coex_sta->is_hid_low_pri_tx_overhead = TRUE;
		else
			coex_sta->is_hid_low_pri_tx_overhead = FALSE;
	}

	if (coex_sta->high_priority_tx == 0 &&
	    coex_sta->high_priority_rx == 0 &&
	    coex_sta->low_priority_tx == 0 &&
	    coex_sta->low_priority_rx == 0) {
		num_of_bt_counter_chk++;

		if (num_of_bt_counter_chk >= 3) {
			halbtc8822b1ant_query_bt_info(btc);
			num_of_bt_counter_chk = 0;
		}
	}
}

static void halbtc8822b1ant_monitor_wifi_ctr(struct btc_coexist *btc)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;
	boolean wifi_busy = FALSE, wifi_scan = FALSE;
	static u8 wl_noisy_count0, wl_noisy_count1 = 3, wl_noisy_count2;
	u32 cnt_cck;
	static u8 cnt_ccklocking;
	u8 h2c_parameter[1] = {0};
	struct btc_bt_link_info *bt_link_info = &btc->bt_link_info;

	/* Only enable for windows becaus 8821cu H2C 0x69 unknown fail @ linux*/
	if (btc->chip_interface != BTC_INTF_USB) {
		/*send h2c to query WL FW dbg info  */
		if ((coex_dm->cur_ps_tdma_on && coex_sta->force_lps_ctrl) ||
		    (coex_sta->acl_busy && bt_link_info->a2dp_exist)) {
			h2c_parameter[0] = 0x8;
			btc->btc_fill_h2c(btc, 0x69, 1, h2c_parameter);
		}
	}

	btc->btc_get(btc, BTC_GET_BL_WIFI_BUSY, &wifi_busy);
	btc->btc_get(btc, BTC_GET_BL_WIFI_SCAN, &wifi_scan);

	coex_sta->crc_ok_cck =
		btc->btc_phydm_query_PHY_counter(btc, PHYDM_INFO_CRC32_OK_CCK);
	coex_sta->crc_ok_11g =
	    btc->btc_phydm_query_PHY_counter(btc, PHYDM_INFO_CRC32_OK_LEGACY);
	coex_sta->crc_ok_11n =
		btc->btc_phydm_query_PHY_counter(btc, PHYDM_INFO_CRC32_OK_HT);
	coex_sta->crc_ok_11n_vht =
		btc->btc_phydm_query_PHY_counter(btc, PHYDM_INFO_CRC32_OK_VHT);

	coex_sta->crc_err_cck =
	    btc->btc_phydm_query_PHY_counter(btc, PHYDM_INFO_CRC32_ERROR_CCK);
	coex_sta->crc_err_11g =
	  btc->btc_phydm_query_PHY_counter(btc, PHYDM_INFO_CRC32_ERROR_LEGACY);
	coex_sta->crc_err_11n =
	    btc->btc_phydm_query_PHY_counter(btc, PHYDM_INFO_CRC32_ERROR_HT);
	coex_sta->crc_err_11n_vht =
	    btc->btc_phydm_query_PHY_counter(btc, PHYDM_INFO_CRC32_ERROR_VHT);

	/* CCK lock identification */
	if (coex_sta->cck_lock)
		cnt_ccklocking++;
	else if (cnt_ccklocking != 0)
		cnt_ccklocking--;

	if (cnt_ccklocking >= 3) {
		cnt_ccklocking = 3;
		coex_sta->cck_lock_ever = TRUE;
	}

	/* WiFi environment noisy identification */
	cnt_cck = coex_sta->crc_ok_cck + coex_sta->crc_err_cck;

	if (!wifi_busy && !coex_sta->cck_lock) {
		if (cnt_cck > 250) {
			if (wl_noisy_count2 < 3)
				wl_noisy_count2++;

			if (wl_noisy_count2 == 3) {
				wl_noisy_count0 = 0;
				wl_noisy_count1 = 0;
			}

		} else if (cnt_cck < 100) {
			if (wl_noisy_count0 < 3)
				wl_noisy_count0++;

			if (wl_noisy_count0 == 3) {
				wl_noisy_count1 = 0;
				wl_noisy_count2 = 0;
			}

		} else {
			if (wl_noisy_count1 < 3)
				wl_noisy_count1++;

			if (wl_noisy_count1 == 3) {
				wl_noisy_count0 = 0;
				wl_noisy_count2 = 0;
			}
		}

		if (wl_noisy_count2 == 3)
			coex_sta->wl_noisy_level = 2;
		else if (wl_noisy_count1 == 3)
			coex_sta->wl_noisy_level = 1;
		else
			coex_sta->wl_noisy_level = 0;
	}
}

static void
halbtc8822b1ant_monitor_bt_enable(struct btc_coexist *btc)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	static u32 bt_disable_cnt;
	boolean bt_active = TRUE, bt_disabled = FALSE;
	u16 u16tmp;

	/* Read BT on/off status from scoreboard[1],
	 * enable this only if BT patch support this feature
	 */
	halbtc8822b1ant_read_scbd(btc, &u16tmp);

	bt_active = u16tmp & BIT(1);

	if (bt_active) {
		bt_disable_cnt = 0;
		bt_disabled = FALSE;
		btc->btc_set(btc, BTC_SET_BL_BT_DISABLE, &bt_disabled);
	} else {
		bt_disable_cnt++;
		if (bt_disable_cnt >= 2) {
			bt_disabled = TRUE;
			bt_disable_cnt = 2;
		}

		btc->btc_set(btc, BTC_SET_BL_BT_DISABLE, &bt_disabled);
	}

	if (coex_sta->bt_disabled != bt_disabled) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], BT is from %s to %s!!\n",
			    (coex_sta->bt_disabled ? "disabled" : "enabled"),
			    (bt_disabled ? "disabled" : "enabled"));
		BTC_TRACE(trace_buf);
		coex_sta->bt_disabled = bt_disabled;

		/*for win10 BT disable->enable trigger wifi scan issue   */
		if (!coex_sta->bt_disabled) {
			coex_sta->is_bt_reenable = TRUE;
			coex_sta->cnt_bt_reenable = 15;
		} else {
			coex_sta->is_bt_reenable = FALSE;
			coex_sta->cnt_bt_reenable = 0;
		}
	}
}

static boolean
halbtc8822b1ant_moniter_wifibt_status(struct btc_coexist *btc)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct wifi_link_info_8822b_1ant *wifi_link_info_ext =
					 &btc->wifi_link_info_8822b_1ant;
	static boolean pre_wifi_busy, pre_under_4way,
		       pre_rf4ce_enabled,
		       pre_bt_off, pre_bt_slave,
		       pre_hid_low_pri_tx_overhead,
		       pre_wifi_under_lps, pre_bt_setup_link,
		       pre_bt_acl_busy;
	static u8 pre_hid_busy_num, pre_wl_noisy_level;
	boolean wifi_busy = FALSE, under_4way = FALSE, rf4ce_enabled = FALSE;
	boolean wifi_connected = FALSE;
	struct btc_bt_link_info *bt_link_info = &btc->bt_link_info;
	static u8 cnt_wifi_busytoidle;
	u32 num_of_wifi_link = 0, wifi_link_mode = 0;
	static u32 pre_num_of_wifi_link, pre_wifi_link_mode;
	boolean miracast_plus_bt = FALSE;
	u8 lna_lvl = 1;

	btc->btc_get(btc, BTC_GET_BL_WIFI_CONNECTED, &wifi_connected);
	btc->btc_get(btc, BTC_GET_BL_WIFI_BUSY, &wifi_busy);
	btc->btc_get(btc, BTC_GET_BL_WIFI_4_WAY_PROGRESS, &under_4way);

	if (wifi_busy) {
		coex_sta->gl_wifi_busy = TRUE;
		cnt_wifi_busytoidle = 6;
	} else {
		if (coex_sta->gl_wifi_busy && cnt_wifi_busytoidle > 0)
			cnt_wifi_busytoidle--;
		else if (cnt_wifi_busytoidle == 0)
			coex_sta->gl_wifi_busy = FALSE;
	}

	if (coex_sta->bt_disabled != pre_bt_off) {
		pre_bt_off = coex_sta->bt_disabled;

		if (coex_sta->bt_disabled)
			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], BT is disabled !!\n");
		else
			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], BT is enabled !!\n");

		BTC_TRACE(trace_buf);

		coex_sta->bt_coex_supported_feature = 0;
		coex_sta->bt_coex_supported_version = 0;
		coex_sta->bt_ble_scan_type = 0;
		coex_sta->bt_ble_scan_para[0] = 0;
		coex_sta->bt_ble_scan_para[1] = 0;
		coex_sta->bt_ble_scan_para[2] = 0;
		coex_sta->bt_reg_vendor_ac = 0xffff;
		coex_sta->bt_reg_vendor_ae = 0xffff;
		coex_sta->legacy_forbidden_slot = 0;
		coex_sta->le_forbidden_slot = 0;
		coex_sta->bt_a2dp_vendor_id = 0;
		coex_sta->bt_a2dp_device_name = 0;
		return TRUE;
	}

	num_of_wifi_link = wifi_link_info_ext->num_of_active_port;

	if (num_of_wifi_link != pre_num_of_wifi_link) {
		pre_num_of_wifi_link = num_of_wifi_link;

		if (wifi_link_info_ext->is_p2p_connected) {
			if (bt_link_info->bt_link_exist)
				miracast_plus_bt = TRUE;
			else
				miracast_plus_bt = FALSE;

			btc->btc_set(btc, BTC_SET_BL_MIRACAST_PLUS_BT,
				     &miracast_plus_bt);
		}

		return TRUE;
	}

	wifi_link_mode = btc->wifi_link_info.link_mode;
	if (wifi_link_mode != pre_wifi_link_mode) {
		pre_wifi_link_mode = wifi_link_mode;
		return TRUE;
	}

	btc->btc_get(btc, BTC_GET_BL_RF4CE_CONNECTED, &rf4ce_enabled);

	if (rf4ce_enabled != pre_rf4ce_enabled) {
		pre_rf4ce_enabled = rf4ce_enabled;

		if (rf4ce_enabled)
			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], rf4ce is enabled !!\n");
		else
			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], rf4ce is disabled !!\n");

		BTC_TRACE(trace_buf);

		return TRUE;
	}

	if (wifi_connected) {
		if (wifi_busy != pre_wifi_busy) {
			pre_wifi_busy = wifi_busy;
			return TRUE;
		}
		if (under_4way != pre_under_4way) {
			pre_under_4way = under_4way;
			return TRUE;
		}
		if (coex_sta->wl_noisy_level != pre_wl_noisy_level) {
			pre_wl_noisy_level = coex_sta->wl_noisy_level;
			return TRUE;
		}
		if (coex_sta->under_lps != pre_wifi_under_lps) {
			pre_wifi_under_lps = coex_sta->under_lps;
			if (coex_sta->under_lps)
				return TRUE;
		}
	}

	if (!coex_sta->bt_disabled) {
		if (coex_sta->acl_busy != pre_bt_acl_busy) {
			pre_bt_acl_busy = coex_sta->acl_busy;
			btc->btc_set(btc, BTC_SET_BL_BT_LNA_CONSTRAIN_LEVEL,
				     &lna_lvl);
			return TRUE;
		}

		if (coex_sta->hid_busy_num != pre_hid_busy_num) {
			pre_hid_busy_num = coex_sta->hid_busy_num;
			return TRUE;
		}

		if (bt_link_info->slave_role != pre_bt_slave) {
			pre_bt_slave = bt_link_info->slave_role;
			return TRUE;
		}

		if (pre_hid_low_pri_tx_overhead !=
		    coex_sta->is_hid_low_pri_tx_overhead) {
			pre_hid_low_pri_tx_overhead =
				coex_sta->is_hid_low_pri_tx_overhead;
			return TRUE;
		}

		if (pre_bt_setup_link != coex_sta->is_setup_link) {
			pre_bt_setup_link = coex_sta->is_setup_link;
			return TRUE;
		}
	}

	return FALSE;
}

static void halbtc8822b1ant_update_wifi_link_info(struct btc_coexist *btc,
						  u8 reason)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct wifi_link_info_8822b_1ant *wifi_link_info_ext =
					 &btc->wifi_link_info_8822b_1ant;
	struct btc_wifi_link_info wifi_link_info;
	u8 wifi_central_chnl = 0, num_of_wifi_link = 0;
	boolean isunder5G = FALSE, ismcc25g = FALSE, isp2pconnected = FALSE;
	u32 wifi_link_status = 0;

	btc->btc_get(btc, BTC_GET_U4_WIFI_LINK_STATUS, &wifi_link_status);
	wifi_link_info_ext->port_connect_status = wifi_link_status & 0xffff;

	btc->btc_get(btc, BTC_GET_BL_WIFI_LINK_INFO, &wifi_link_info);
	btc->wifi_link_info = wifi_link_info;

	btc->btc_get(btc, BTC_GET_U1_WIFI_CENTRAL_CHNL, &wifi_central_chnl);
	coex_sta->wl_center_channel = wifi_central_chnl;

	/* Check scan/connect/special-pkt action first  */
	switch (reason) {
	case BT_8822B_1ANT_RSN_5GSCANSTART:
	case BT_8822B_1ANT_RSN_5GSWITCHBAND:
	case BT_8822B_1ANT_RSN_5GCONSTART:

		isunder5G = TRUE;
		break;
	case BT_8822B_1ANT_RSN_2GSCANSTART:
	case BT_8822B_1ANT_RSN_2GSWITCHBAND:
	case BT_8822B_1ANT_RSN_2GCONSTART:

		isunder5G = FALSE;
		break;
	case BT_8822B_1ANT_RSN_2GCONFINISH:
	case BT_8822B_1ANT_RSN_5GCONFINISH:
	case BT_8822B_1ANT_RSN_2GMEDIA:
	case BT_8822B_1ANT_RSN_5GMEDIA:
	case BT_8822B_1ANT_RSN_BTINFO:
	case BT_8822B_1ANT_RSN_PERIODICAL:
	case BT_8822B_1ANT_RSN_2GSPECIALPKT:
	case BT_8822B_1ANT_RSN_5GSPECIALPKT:
	default:
		switch (wifi_link_info.link_mode) {
		case BTC_LINK_5G_MCC_GO_STA:
		case BTC_LINK_5G_MCC_GC_STA:
		case BTC_LINK_5G_SCC_GO_STA:
		case BTC_LINK_5G_SCC_GC_STA:

			isunder5G = TRUE;
			break;
		case BTC_LINK_2G_MCC_GO_STA:
		case BTC_LINK_2G_MCC_GC_STA:
		case BTC_LINK_2G_SCC_GO_STA:
		case BTC_LINK_2G_SCC_GC_STA:

			isunder5G = FALSE;
			break;
		case BTC_LINK_25G_MCC_GO_STA:
		case BTC_LINK_25G_MCC_GC_STA:

			isunder5G = FALSE;
			ismcc25g = TRUE;
			break;
		case BTC_LINK_ONLY_STA:
			if (wifi_link_info.sta_center_channel > 14)
				isunder5G = TRUE;
			else
				isunder5G = FALSE;
			break;
		case BTC_LINK_ONLY_GO:
		case BTC_LINK_ONLY_GC:
		case BTC_LINK_ONLY_AP:
		default:
			if (wifi_link_info.p2p_center_channel > 14)
				isunder5G = TRUE;
			else
				isunder5G = FALSE;

			break;
		}
		break;
	}

	wifi_link_info_ext->is_all_under_5g = isunder5G;
	wifi_link_info_ext->is_mcc_25g = ismcc25g;

	if (wifi_link_status & WIFI_STA_CONNECTED)
		num_of_wifi_link++;

	if (wifi_link_status & WIFI_AP_CONNECTED)
		num_of_wifi_link++;

	if (wifi_link_status & WIFI_P2P_GO_CONNECTED) {
		num_of_wifi_link++;
		isp2pconnected = TRUE;
	}

	if (wifi_link_status & WIFI_P2P_GC_CONNECTED) {
		num_of_wifi_link++;
		isp2pconnected = TRUE;
	}

	wifi_link_info_ext->num_of_active_port = num_of_wifi_link;
	wifi_link_info_ext->is_p2p_connected = isp2pconnected;

	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
		    "[BTCoex], wifi_link_info: link_mode=%d, STA_Ch=%d, P2P_Ch=%d, AnyClient_Join_Go=%d !\n",
		    btc->wifi_link_info.link_mode,
		    btc->wifi_link_info.sta_center_channel,
		    btc->wifi_link_info.p2p_center_channel,
		    btc->wifi_link_info.bany_client_join_go);
	BTC_TRACE(trace_buf);

	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
		    "[BTCoex], wifi_link_info: center_ch=%d, is_all_under_5g=%d, is_mcc_25g=%d!\n",
		    coex_sta->wl_center_channel,
		    wifi_link_info_ext->is_all_under_5g,
		    wifi_link_info_ext->is_mcc_25g);
	BTC_TRACE(trace_buf);

	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
		    "[BTCoex], wifi_link_info: port_connect_status=0x%x, active_port_cnt=%d, P2P_Connect=%d!\n",
		    wifi_link_info_ext->port_connect_status,
		    wifi_link_info_ext->num_of_active_port,
		    wifi_link_info_ext->is_p2p_connected);
	BTC_TRACE(trace_buf);

	switch (reason) {
	case BT_8822B_1ANT_RSN_2GSCANSTART:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Update reason = %s\n", "2GSCANSTART");
		break;
	case BT_8822B_1ANT_RSN_5GSCANSTART:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Update reason = %s\n", "5GSCANSTART");
		break;
	case BT_8822B_1ANT_RSN_SCANFINISH:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Update reason = %s\n", "SCANFINISH");
		break;
	case BT_8822B_1ANT_RSN_2GSWITCHBAND:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Update reason = %s\n", "2GSWITCHBAND");
		break;
	case BT_8822B_1ANT_RSN_5GSWITCHBAND:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Update reason = %s\n", "5GSWITCHBAND");
		break;
	case BT_8822B_1ANT_RSN_2GCONSTART:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Update reason = %s\n", "2GCONNECTSTART");
		break;
	case BT_8822B_1ANT_RSN_5GCONSTART:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Update reason = %s\n", "5GCONNECTSTART");
		break;
	case BT_8822B_1ANT_RSN_2GCONFINISH:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Update reason = %s\n",
			    "2GCONNECTFINISH");
		break;
	case BT_8822B_1ANT_RSN_5GCONFINISH:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Update reason = %s\n",
			    "5GCONNECTFINISH");
		break;
	case BT_8822B_1ANT_RSN_2GMEDIA:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Update reason = %s\n", "2GMEDIASTATUS");
		break;
	case BT_8822B_1ANT_RSN_5GMEDIA:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Update reason = %s\n", "5GMEDIASTATUS");
		break;
	case BT_8822B_1ANT_RSN_MEDIADISCON:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Update reason = %s\n",
			    "MEDIADISCONNECT");
		break;
	case BT_8822B_1ANT_RSN_2GSPECIALPKT:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Update reason = %s\n", "2GSPECIALPKT");
		break;
	case BT_8822B_1ANT_RSN_5GSPECIALPKT:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Update reason = %s\n", "5GSPECIALPKT");
		break;
	case BT_8822B_1ANT_RSN_BTINFO:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Update reason = %s\n", "BTINFO");
		break;
	case BT_8822B_1ANT_RSN_PERIODICAL:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Update reason = %s\n", "PERIODICAL");
		break;
	case BT_8822B_1ANT_RSN_PNP:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Update reason = %s\n", "PNPNotify");
		break;
	default:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Update reason = %s\n", "UNKNOWN");
		break;
	}

	BTC_TRACE(trace_buf);

	if (wifi_link_info_ext->is_all_under_5g ||
	    coex_sta->num_of_profile == 0)
		halbtc8822b1ant_low_penalty_ra(btc, NM_EXCU, FALSE, 0);
	else if (wifi_link_info_ext->is_p2p_connected)
		halbtc8822b1ant_low_penalty_ra(btc, NM_EXCU, TRUE, 30);
	else
		halbtc8822b1ant_low_penalty_ra(btc, NM_EXCU, TRUE, 15);
}

static void halbtc8822b1ant_update_bt_link_info(struct btc_coexist *btc)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;
	struct btc_bt_link_info *bt_link_info = &btc->bt_link_info;
	boolean bt_busy = FALSE, increase_scan_dev_num = FALSE;
	u32 val = 0;
	static u8 pre_num_of_profile, cur_num_of_profile, cnt, ble_cnt;

	if (++ble_cnt >= 3)
		ble_cnt = 0;

	if (coex_sta->is_ble_scan_en && ble_cnt == 0) {
		u32 *p = NULL;
		u8 scantype;

		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], BT ext info bit4 check, query BLE Scan type!!\n");
		BTC_TRACE(trace_buf);
		coex_sta->bt_ble_scan_type =
			btc->btc_get_ble_scan_type_from_bt(btc);

		if ((coex_sta->bt_ble_scan_type & 0x1) == 0x1) {
			p = &coex_sta->bt_ble_scan_para[0];
			scantype = 0x1;
		}

		if ((coex_sta->bt_ble_scan_type & 0x2) == 0x2) {
			p = &coex_sta->bt_ble_scan_para[1];
			scantype = 0x2;
		}

		if ((coex_sta->bt_ble_scan_type & 0x4) == 0x4) {
			p = &coex_sta->bt_ble_scan_para[2];
			scantype = 0x4;
		}

		if (p)
			*p = btc->btc_get_ble_scan_para_from_bt(btc, scantype);
	}

	coex_sta->num_of_profile = 0;

	/* set link exist status */
	if (!(coex_sta->bt_info & BT_INFO_8822B_1ANT_B_CONNECTION)) {
		coex_sta->bt_link_exist = FALSE;
		coex_sta->pan_exist = FALSE;
		coex_sta->a2dp_exist = FALSE;
		coex_sta->hid_exist = FALSE;
		coex_sta->sco_exist = FALSE;
		coex_sta->msft_mr_exist = FALSE;
	} else { /* connection exists */
		coex_sta->bt_link_exist = TRUE;
		if (coex_sta->bt_info & BT_INFO_8822B_1ANT_B_FTP) {
			coex_sta->pan_exist = TRUE;
			coex_sta->num_of_profile++;
		} else {
			coex_sta->pan_exist = FALSE;
		}

		if (coex_sta->bt_info & BT_INFO_8822B_1ANT_B_A2DP) {
			coex_sta->a2dp_exist = TRUE;
			coex_sta->num_of_profile++;
		} else {
			coex_sta->a2dp_exist = FALSE;
		}

		if (coex_sta->bt_info & BT_INFO_8822B_1ANT_B_HID) {
			coex_sta->hid_exist = TRUE;
			coex_sta->num_of_profile++;
		} else {
			coex_sta->hid_exist = FALSE;
		}

		if (coex_sta->bt_info & BT_INFO_8822B_1ANT_B_SCO_ESCO) {
			coex_sta->sco_exist = TRUE;
			coex_sta->num_of_profile++;
		} else {
			coex_sta->sco_exist = FALSE;
		}

		if (coex_sta->hid_busy_num == 0 &&
		    coex_sta->hid_pair_cnt > 0 &&
		    coex_sta->low_priority_tx > 1000 &&
		    coex_sta->low_priority_rx > 1000 &&
		    !coex_sta->c2h_bt_inquiry_page)
			coex_sta->msft_mr_exist = TRUE;
		else
			coex_sta->msft_mr_exist = FALSE;
	}

	bt_link_info->bt_link_exist = coex_sta->bt_link_exist;
	bt_link_info->sco_exist = coex_sta->sco_exist;
	bt_link_info->a2dp_exist = coex_sta->a2dp_exist;
	bt_link_info->pan_exist = coex_sta->pan_exist;
	bt_link_info->hid_exist = coex_sta->hid_exist;
	bt_link_info->acl_busy = coex_sta->acl_busy;

	/* check if Sco only */
	if (bt_link_info->sco_exist && !bt_link_info->a2dp_exist &&
	    !bt_link_info->pan_exist && !bt_link_info->hid_exist)
		bt_link_info->sco_only = TRUE;
	else
		bt_link_info->sco_only = FALSE;

	/* check if A2dp only */
	if (!bt_link_info->sco_exist && bt_link_info->a2dp_exist &&
	    !bt_link_info->pan_exist && !bt_link_info->hid_exist)
		bt_link_info->a2dp_only = TRUE;
	else
		bt_link_info->a2dp_only = FALSE;

	/* check if Pan only */
	if (!bt_link_info->sco_exist && !bt_link_info->a2dp_exist &&
	    bt_link_info->pan_exist && !bt_link_info->hid_exist)
		bt_link_info->pan_only = TRUE;
	else
		bt_link_info->pan_only = FALSE;

	/* check if Hid only */
	if (!bt_link_info->sco_exist && !bt_link_info->a2dp_exist &&
	    !bt_link_info->pan_exist && bt_link_info->hid_exist)
		bt_link_info->hid_only = TRUE;
	else
		bt_link_info->hid_only = FALSE;

	if (coex_sta->bt_info & BT_INFO_8822B_1ANT_B_INQ_PAGE) {
		coex_dm->bt_status = BT_8822B_1ANT_BSTATUS_INQ_PAGE;
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], BtInfoNotify(), BT Inq/page!!!\n");
	} else if (!(coex_sta->bt_info & BT_INFO_8822B_1ANT_B_CONNECTION)) {
		coex_dm->bt_status = BT_8822B_1ANT_BSTATUS_NCON_IDLE;
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], BtInfoNotify(), BT Non-Connected idle!!!\n");
	} else if (coex_sta->bt_info == BT_INFO_8822B_1ANT_B_CONNECTION) {
		/* connection exists but no busy */

		if (coex_sta->msft_mr_exist) {
			coex_dm->bt_status = BT_8822B_1ANT_BSTATUS_ACL_BUSY;
			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], BtInfoNotify(),  BT ACL busy!!\n");
		} else {
			coex_dm->bt_status =
				BT_8822B_1ANT_BSTATUS_CON_IDLE;
			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], BtInfoNotify(), BT Connected-idle!!!\n");
		}
	} else if (((coex_sta->bt_info & BT_INFO_8822B_1ANT_B_SCO_ESCO) ||
		    (coex_sta->bt_info & BT_INFO_8822B_1ANT_B_SCO_BUSY)) &&
		   (coex_sta->bt_info & BT_INFO_8822B_1ANT_B_ACL_BUSY)) {
		coex_dm->bt_status = BT_8822B_1ANT_BSTATUS_ACL_SCO_BUSY;
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], BtInfoNotify(), BT ACL SCO busy!!!\n");
	} else if ((coex_sta->bt_info & BT_INFO_8822B_1ANT_B_SCO_ESCO) ||
		   (coex_sta->bt_info & BT_INFO_8822B_1ANT_B_SCO_BUSY)) {
		coex_dm->bt_status = BT_8822B_1ANT_BSTATUS_SCO_BUSY;
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], BtInfoNotify(), BT SCO busy!!!\n");
	} else if (coex_sta->bt_info & BT_INFO_8822B_1ANT_B_ACL_BUSY) {
		coex_dm->bt_status = BT_8822B_1ANT_BSTATUS_ACL_BUSY;
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], BtInfoNotify(), BT ACL busy!!!\n");
	} else {
		coex_dm->bt_status = BT_8822B_1ANT_BSTATUS_MAX;
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], BtInfoNotify(), BT Non-Defined state!!!\n");
	}

	BTC_TRACE(trace_buf);

	if (coex_dm->bt_status == BT_8822B_1ANT_BSTATUS_ACL_BUSY ||
	    coex_dm->bt_status == BT_8822B_1ANT_BSTATUS_SCO_BUSY ||
	    coex_dm->bt_status == BT_8822B_1ANT_BSTATUS_ACL_SCO_BUSY) {
		bt_busy = TRUE;
		increase_scan_dev_num = TRUE;
	} else {
		bt_busy = FALSE;
		increase_scan_dev_num = FALSE;
	}

	btc->btc_set(btc, BTC_SET_BL_BT_TRAFFIC_BUSY, &bt_busy);
	btc->btc_set(btc, BTC_SET_BL_INC_SCAN_DEV_NUM, &increase_scan_dev_num);

	cur_num_of_profile = coex_sta->num_of_profile;

	if (cur_num_of_profile != pre_num_of_profile)
		cnt = 2;

	if (bt_link_info->a2dp_exist) {
		if ((coex_sta->bt_a2dp_vendor_id == 0 &&
		     coex_sta->bt_a2dp_device_name == 0) ||
		    cur_num_of_profile != pre_num_of_profile) {
			btc->btc_get(btc, BTC_GET_U4_BT_DEVICE_INFO, &val);

			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], BtInfoNotify(), get BT DEVICE_INFO = %x\n",
				    val);
			BTC_TRACE(trace_buf);

			coex_sta->bt_a2dp_vendor_id = (u8)(val & 0xff);
			coex_sta->bt_a2dp_device_name = (val & 0xffffff00) >> 8;
		}

		if ((coex_sta->legacy_forbidden_slot == 0 &&
		     coex_sta->le_forbidden_slot == 0) ||
		    cur_num_of_profile != pre_num_of_profile || cnt > 0) {
			if (cnt > 0)
				cnt--;

			btc->btc_get(btc, BTC_GET_U4_BT_FORBIDDEN_SLOT_VAL,
				     &val);

			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], BtInfoNotify(), get BT FORBIDDEN_SLOT_VAL = %x\n",
				    val);
			BTC_TRACE(trace_buf);

			coex_sta->legacy_forbidden_slot = (u16)(val & 0xffff);
			coex_sta->le_forbidden_slot =
				(u16)((val & 0xffff0000) >> 16);
		}
	}

	pre_num_of_profile = coex_sta->num_of_profile;
}

static void
halbtc8822b1ant_update_wifi_ch_info(struct btc_coexist *btc, u8 type)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;
	struct wifi_link_info_8822b_1ant *wifi_link_info_ext =
					 &btc->wifi_link_info_8822b_1ant;
	u8 h2c_parameter[3] = {0}, i;
	u32 wifi_bw;
	u8 wifi_central_chnl = 0;
	u8 wifi_5g_chnl[19] = {120, 124, 128, 132, 136, 140, 144, 149, 153, 157,
			       118, 126, 134, 142, 151, 159, 122, 138, 155};
	u8 bt_skip_cneter_chanl[19] = {2, 8,  17, 26, 34, 42, 51, 62, 71, 77,
				       2, 12, 29, 46, 66, 76, 10, 37, 68};
	u8 bt_skip_span[19] = {4, 8,  8,  10, 8,  10, 8,  8,  10, 4,
			       4, 16, 16, 16, 16, 4,  20, 34, 20};
	boolean is_any_connected = FALSE;

	if (btc->manual_control)
		return;

	btc->btc_get(btc, BTC_GET_U4_WIFI_BW, &wifi_bw);

	if (btc->stop_coex_dm || coex_sta->is_rf_state_off) {
		is_any_connected = FALSE;
		wifi_central_chnl = 0;
	} else if (type != BTC_MEDIA_DISCONNECT ||
		   (type == BTC_MEDIA_DISCONNECT &&
		    wifi_link_info_ext->num_of_active_port > 0)) {
		if (wifi_link_info_ext->num_of_active_port == 1) {
			if (wifi_link_info_ext->is_p2p_connected)
				wifi_central_chnl =
					btc->wifi_link_info
						.p2p_center_channel;
			else
				wifi_central_chnl =
					btc->wifi_link_info
						.sta_center_channel;
		} else { /* port > 2 */
			if ((btc->wifi_link_info
				     .p2p_center_channel > 14) &&
			    (btc->wifi_link_info
				     .sta_center_channel > 14))
				wifi_central_chnl =
					btc->wifi_link_info
						.p2p_center_channel;
			else if (btc->wifi_link_info
					 .p2p_center_channel <= 14)
				wifi_central_chnl =
					btc->wifi_link_info
						.p2p_center_channel;
			else if (btc->wifi_link_info
					 .sta_center_channel <= 14)
				wifi_central_chnl =
					btc->wifi_link_info
						.sta_center_channel;
		}
	}

	if (wifi_central_chnl > 0)
		is_any_connected = TRUE;

	if (is_any_connected) {
		if (wifi_central_chnl <= 14) {
			h2c_parameter[0] = 0x1;
			h2c_parameter[1] = wifi_central_chnl;

			if (wifi_bw == BTC_WIFI_BW_HT40)
				h2c_parameter[2] = 0x36;
			else
				h2c_parameter[2] = 0x24;
		} else { /* for 5G  */

			for (i = 0; i <= 18; i++) {
				if (wifi_central_chnl == wifi_5g_chnl[i])
					break;
			}

			if (i <= 18) {
				h2c_parameter[0] = 0x3;
				h2c_parameter[1] = bt_skip_cneter_chanl[i];
				h2c_parameter[2] = bt_skip_span[i];
			}
		}
	}

	coex_dm->wifi_chnl_info[0] = h2c_parameter[0];
	coex_dm->wifi_chnl_info[1] = h2c_parameter[1];
	coex_dm->wifi_chnl_info[2] = h2c_parameter[2];

	btc->btc_fill_h2c(btc, 0x66, 3, h2c_parameter);

	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
		    "[BTCoex], para[0:2] = 0x%x 0x%x 0x%x\n", h2c_parameter[0],
		    h2c_parameter[1], h2c_parameter[2]);
	BTC_TRACE(trace_buf);
}

static void halbtc8822b1ant_set_wl_tx_power(struct btc_coexist *btc,
					    boolean force_exec, u8 wl_pwr_lvl)
{
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;

	if (!force_exec) {
		if (wl_pwr_lvl == coex_dm->cur_wl_pwr_lvl)
			return;
	}

	btc->btc_write_1byte_bitmask(btc, 0xc5b, 0xff, wl_pwr_lvl);
	btc->btc_write_1byte_bitmask(btc, 0xe5b, 0xff, wl_pwr_lvl);

	coex_dm->cur_wl_pwr_lvl = wl_pwr_lvl;
}

static void halbtc8822b1ant_set_bt_tx_power(struct btc_coexist *btc,
					    boolean force_exec, u8 bt_pwr_lvl)
{
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;
	u8 h2c_parameter[1] = {0};

	if (!force_exec) {
		if (bt_pwr_lvl == coex_dm->cur_bt_pwr_lvl)
			return;
	}

	h2c_parameter[0] = 0 - bt_pwr_lvl;

	btc->btc_fill_h2c(btc, 0x62, 1, h2c_parameter);

	coex_dm->cur_bt_pwr_lvl = bt_pwr_lvl;
}

static void halbtc8822b1ant_set_wl_rx_gain(struct btc_coexist *btc,
					   boolean force_exec,
					   boolean agc_table_en)
{
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;
	u8 i;

	/*20180228--58~-110:use from line 5 to line 8 , others :line 5*/
	u32 rx_gain_value_en[] = {
		0xff000003, 0xf4120003, 0xf5100003, 0xf60e0003, 0xf70c0003,
		0xf80a0003, 0xf3140003, 0xf2160003, 0xf1180003, 0xf01a0003,
		0xef1c0003, 0xee1e0003, 0xed200003, 0xec220003, 0xeb240003,
		0xea260003, 0xe9280003, 0xe82a0003, 0xe72c0003, 0xe62e0003,
		0xe5300003, 0xc8320003, 0xc7340003, 0xab360003, 0x8d380003,
		0x8c3a0003, 0x8b3c0003, 0x8a3e0003, 0x6e400003, 0x6d420003,
		0x6c440003, 0x6b460003, 0x6a480003, 0x694a0003, 0x684c0003,
		0x674e0003, 0x66500003, 0x65520003, 0x64540003, 0x64560003,
		0x007e0403};

	u32 rx_gain_value_dis[] = {
		0xff000003, 0xf4120003, 0xf5100003, 0xf60e0003, 0xf70c0003,
		0xf80a0003, 0xf3140003, 0xf2160003, 0xf1180003, 0xf01a0003,
		0xef1c0003, 0xee1e0003, 0xed200003, 0xec220003, 0xeb240003,
		0xea260003, 0xe9280003, 0xe82a0003, 0xe72c0003, 0xe62e0003,
		0xe5300003, 0xc8320003, 0xc7340003, 0xc6360003, 0xc5380003,
		0xc43a0003, 0xc33c0003, 0xc23e0003, 0xc1400003, 0xc0420003,
		0xa5440003, 0xa4460003, 0xa3480003, 0xa24a0003, 0xa14c0003,
		0x834e0003, 0x82500003, 0x81520003, 0x80540003, 0x65560003,
		0x007e0403};
#if 0
	/*20171116*/
	u32 rx_gain_value_en[] = {
		0xff000003, 0xbd120003, 0xbe100003, 0xbf080003, 0xbf060003,
		0xbf050003, 0xbc140003, 0xbb160003, 0xba180003, 0xb91a0003,
		0xb81c0003, 0xb71e0003, 0xb4200003, 0xb5220003, 0xb4240003,
		0xb3260003, 0xb2280003, 0xb12a0003, 0xb02c0003, 0xaf2e0003,
		0xae300003, 0xad320003, 0xac340003, 0xab360003, 0x8d380003,
		0x8c3a0003, 0x8b3c0003, 0x8a3e0003, 0x6e400003, 0x6d420003,
		0x6c440003, 0x6b460003, 0x6a480003, 0x694a0003, 0x684c0003,
		0x674e0003, 0x66500003, 0x65520003, 0x64540003, 0x64560003,
		0x007e0403};

	u32 rx_gain_value_dis[] = {
		0xff000003, 0xf4120003, 0xf5100003, 0xf60e0003, 0xf70c0003,
		0xf80a0003, 0xf3140003, 0xf2160003, 0xf1180003, 0xf01a0003,
		0xef1c0003, 0xee1e0003, 0xed200003, 0xec220003, 0xeb240003,
		0xea260003, 0xe9280003, 0xe82a0003, 0xe72c0003, 0xe62e0003,
		0xe5300003, 0xc8320003, 0xc7340003, 0xc6360003, 0xc5380003,
		0xc43a0003, 0xc33c0003, 0xc23e0003, 0xc1400003, 0xc0420003,
		0xa5440003, 0xa4460003, 0xa3480003, 0xa24a0003, 0xa14c0003,
		0x834e0003, 0x82500003, 0x81520003, 0x80540003, 0x65560003,
		0x007e0403};
#endif

#if 0
		/*20170110*/

		u32 rx_gain_value_en[] = {
		0xff000003, 0xb6200003, 0xb5220003, 0xb4240003, 0xb3260003,
		0xb2280003, 0xb12a0003, 0xb02c0003, 0xaf2e0003, 0xae300003,
		0xad320003, 0xac340003, 0xab360003, 0x8d380003, 0x8c3a0003,
		0x8b3c0003, 0x8a3e0003, 0x6e400003, 0x6d420003, 0x6c440003,
		0x6b460003, 0x6a480003, 0x694a0003, 0x684c0003, 0x674e0003,
		0x66500003, 0x65520003, 0x64540003, 0x64560003, 0x007e0403};

		/*20170110*/

		u32 rx_gain_value_dis[] = {
		0xff000003, 0xeb240003, 0xea260003,
		0xe9280003, 0xe82a0003, 0xe72c0003, 0xe62e0003, 0xe5300003,
		0xc8320003, 0xc7340003, 0xc6360003, 0xc5380003, 0xc43a0003,
		0xc33c0003, 0xc23e0003, 0xc1400003, 0xc0420003, 0xa5440003,
		0xa4460003, 0xa3480003, 0xa24a0003, 0xa14c0003, 0x834e0003,
		0x82500003, 0x81520003, 0x80540003, 0x65560003, 0x007e0403};
#endif

	if (!force_exec) {
		if (agc_table_en == coex_dm->cur_agc_table_en)
			return;
	}

	if (agc_table_en) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], BB Agc Table On!\n");
		BTC_TRACE(trace_buf);

		for (i = 0; i < ARRAY_SIZE(rx_gain_value_en); i++) {
			btc->btc_write_4byte(btc, 0x81c, rx_gain_value_en[i]);

			if (rx_gain_value_en[i] == 0x007e0403)
				break;
		}

	} else {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], BB Agc Table Off!\n");
		BTC_TRACE(trace_buf);

		for (i = 0; i < ARRAY_SIZE(rx_gain_value_dis); i++) {
			btc->btc_write_4byte(btc, 0x81c, rx_gain_value_dis[i]);

			if (rx_gain_value_dis[i] == 0x007e0403)
				break;
		}
	}

	coex_dm->cur_agc_table_en = agc_table_en;
}

static void halbtc8822b1ant_set_bt_rx_gain(struct btc_coexist *btc,
					   boolean force_exec,
					   boolean rx_gain_en)
{
	u8 lna_lvl = 1;

	btc->btc_set(btc, BTC_SET_BL_BT_LNA_CONSTRAIN_LEVEL, &lna_lvl);

	/* use scoreboard[4] to notify BT Rx gain table change	 */
	halbtc8822b1ant_write_scbd(btc, BT_8822B_1ANT_SCBD_RXGAIN, rx_gain_en);
}

static
u32 halbtc8822b1ant_read_indirect_reg(struct btc_coexist *btc, u16 reg_addr)
{
	u32 delay_count = 0;

	/* wait for ready bit before access 0x1700 */
	while (1) {
		if ((btc->btc_read_1byte(btc, 0x1703) &
		     BIT(5)) == 0) {
			delay_ms(10);
			delay_count++;
			if (delay_count >= 10) {
				delay_count = 0;
				break;
			}
		} else {
			break;
		}
	}

	btc->btc_write_4byte(btc, 0x1700, 0x800F0000 | reg_addr);

	return btc->btc_read_4byte(btc, 0x1708); /* get read data */
}

static
void halbtc8822b1ant_write_indirect_reg(struct btc_coexist *btc, u16 reg_addr,
					u32 bit_mask, u32 reg_value)
{
	u32 val, i = 0, bitpos = 0, delay_count = 0;

	if (bit_mask == 0x0)
		return;

	if (bit_mask == 0xffffffff) {
		/* wait for ready bit before access 0x1700/0x1704 */
		while (1) {
			if ((btc->btc_read_1byte(btc, 0x1703) &
			     BIT(5)) == 0) {
				delay_ms(10);
				delay_count++;
				if (delay_count >= 10) {
					delay_count = 0;
					break;
				}
			} else {
				break;
			}
		}

		/* put write data */
		btc->btc_write_4byte(btc, 0x1704, reg_value);

		btc->btc_write_4byte(btc, 0x1700, 0xc00F0000 | reg_addr);
	} else {
		for (i = 0; i <= 31; i++) {
			if (((bit_mask >> i) & 0x1) == 0x1) {
				bitpos = i;
				break;
			}
		}

		/* read back register value before write */
		val = halbtc8822b1ant_read_indirect_reg(btc, reg_addr);
		val = (val & (~bit_mask)) | (reg_value << bitpos);

		/* wait for ready bit before access 0x1700/0x1704 */
		while (1) {
			if ((btc->btc_read_1byte(btc, 0x1703) &
			     BIT(5)) == 0) {
				delay_ms(10);
				delay_count++;
				if (delay_count >= 10) {
					delay_count = 0;
					break;
				}
			} else {
				break;
			}
		}

		btc->btc_write_4byte(btc, 0x1704, val); /* put write data */

		btc->btc_write_4byte(btc, 0x1700, 0xc00F0000 | reg_addr);
	}
}

static
void halbtc8822b1ant_ltecoex_enable(struct btc_coexist *btc, boolean enable)
{
	u8 val;

	val = (enable) ? 1 : 0;
	/* 0x38[7] */
	halbtc8822b1ant_write_indirect_reg(btc, 0x38, 0x80, val);
}

static
void hallbtc8822b1ant_ltecoex_table(struct btc_coexist *btc,
				    u8 table_type, u16 table_val)
{
	u16 reg_addr = 0x0000;

	switch (table_type) {
	case BT_8822B_1ANT_CTT_WL_VS_LTE:
		reg_addr = 0xa0;
		break;
	case BT_8822B_1ANT_CTT_BT_VS_LTE:
		reg_addr = 0xa4;
		break;
	}

	if (reg_addr == 0x0000)
		return;

	/* 0xa0[15:0] or 0xa4[15:0] */
	halbtc8822b1ant_write_indirect_reg(btc, reg_addr, 0xffff, table_val);
}

static
void halbtc8822b1ant_coex_ctrl_owner(struct btc_coexist *btc,
				     boolean wifi_control)
{
	u8 val;

	val = (wifi_control) ? 1 : 0;
	/* 0x70[26] */
	btc->btc_write_1byte_bitmask(btc, 0x73, 0x4, val);
}

static void halbtc8822b1ant_set_gnt_bt(struct btc_coexist *btc,
				       u8 control_block, u8 sw_control,
				       u8 state)
{
	u32 val = 0, bit_mask;

	state = state & 0x1;
	/*LTE indirect 0x38=0xccxx (sw : gnt_wl=1,sw gnt_bt=1)
	 *0x38=0xddxx (sw : gnt_bt=1 , sw gnt_wl=0)
	 *0x38=0x55xx(hw pta :gnt_wl /gnt_bt )
	 */
	val = (sw_control) ? ((state << 1) | 0x1) : 0;

	switch (control_block) {
	case BT_8822B_1ANT_GNT_BLOCK_RFC_BB:
	default:
		/* 0x38[15:14] */
		bit_mask = 0xc000;
		halbtc8822b1ant_write_indirect_reg(btc, 0x38, bit_mask, val);
		/* 0x38[11:10] */
		bit_mask = 0x0c00;
		halbtc8822b1ant_write_indirect_reg(btc, 0x38, bit_mask, val);
		break;
	case BT_8822B_1ANT_GNT_BLOCK_RFC:
		/* 0x38[15:14] */
		bit_mask = 0xc000;
		halbtc8822b1ant_write_indirect_reg(btc, 0x38, bit_mask, val);
		break;
	case BT_8822B_1ANT_GNT_BLOCK_BB:
		/* 0x38[11:10] */
		bit_mask = 0x0c00;
		halbtc8822b1ant_write_indirect_reg(btc, 0x38, bit_mask, val);
		break;
	}
}

static void halbtc8822b1ant_set_gnt_wl(struct btc_coexist *btc,
				       u8 control_block, u8 sw_control,
				       u8 state)
{
	u32 val = 0, bit_mask;
	/*LTE indirect 0x38=0xccxx (sw : gnt_wl=1,sw gnt_bt=1)
	 *0x38=0xddxx (sw : gnt_bt=1 , sw gnt_wl=0)
	 *0x38=0x55xx(hw pta :gnt_wl /gnt_bt )
	 */

	state = state & 0x1;
	val = (sw_control) ? ((state << 1) | 0x1) : 0;

	switch (control_block) {
	case BT_8822B_1ANT_GNT_BLOCK_RFC_BB:
	default:
		/* 0x38[13:12] */
		bit_mask = 0x3000;
		halbtc8822b1ant_write_indirect_reg(btc, 0x38, bit_mask, val);
		/* 0x38[9:8] */
		bit_mask = 0x0300;
		halbtc8822b1ant_write_indirect_reg(btc, 0x38, bit_mask, val);
		break;
	case BT_8822B_1ANT_GNT_BLOCK_RFC:
		/* 0x38[13:12] */
		bit_mask = 0x3000;
		halbtc8822b1ant_write_indirect_reg(btc, 0x38, bit_mask, val);
		break;
	case BT_8822B_1ANT_GNT_BLOCK_BB:
		/* 0x38[9:8] */
		bit_mask = 0x0300;
		halbtc8822b1ant_write_indirect_reg(btc, 0x38, bit_mask, val);
		break;
	}
}

#if 0
static
void halbtc8822b1ant_ltcoex_set_break_table(struct btc_coexist *btc,
					    u8 table_type, u8 table_val)
{
	u16 reg_addr = 0x0000;

	switch (table_type) {
	case BT_8822B_1ANT_LBTT_WL_BREAK_LTE:
		reg_addr = 0xa8;
		break;
	case BT_8822B_1ANT_LBTT_BT_BREAK_LTE:
		reg_addr = 0xac;
		break;
	case BT_8822B_1ANT_LBTT_LTE_BREAK_WL:
		reg_addr = 0xb0;
		break;
	case BT_8822B_1ANT_LBTT_LTE_BREAK_BT:
		reg_addr = 0xb4;
		break;
	}

	if (reg_addr == 0x0000)
		return;

	/* 0xa8[15:0] or 0xb4[15:0] */
	halbtc8822b1ant_write_indirect_reg(btc, reg_addr, 0xff, table_val);
}
#endif

#if 0
static
void halbtc8822b1ant_set_wltoggle_coex_table(struct btc_coexist *btc,
					     boolean force_exec, u8 interval,
					     u8 val0x6c4_b0, u8 val0x6c4_b1,
					     u8 val0x6c4_b2, u8 val0x6c4_b3)
{
	static u8 pre_h2c_parameter[6] = {0};
	u8 cur_h2c_parameter[6] = {0};
	u8 i, match_cnt = 0;

	cur_h2c_parameter[0] = 0x7; /* op_code, 0x7= wlan toggle slot*/

	cur_h2c_parameter[1] = interval;
	cur_h2c_parameter[2] = val0x6c4_b0;
	cur_h2c_parameter[3] = val0x6c4_b1;
	cur_h2c_parameter[4] = val0x6c4_b2;
	cur_h2c_parameter[5] = val0x6c4_b3;

	if (!force_exec) {
		for (i = 1; i <= 5; i++) {
			if (cur_h2c_parameter[i] != pre_h2c_parameter[i])
				break;

			match_cnt++;
		}

		if (match_cnt == 5)
			return;
	}

	for (i = 1; i <= 5; i++)
		pre_h2c_parameter[i] = cur_h2c_parameter[i];

	btc->btc_fill_h2c(btc, 0x69, 6, cur_h2c_parameter);
}
#endif

static void halbtc8822b1ant_mimo_ps(struct btc_coexist *btc,
				    boolean force_exec, u8 state)
{
	static u8 pre_state;

	if (!force_exec) {
		if (state == pre_state)
			return;
	}

	pre_state = state;

	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
		    "[BTCoex], %s(), state = %d\n", __func__, state);
	BTC_TRACE(trace_buf);

	btc->btc_set(btc, BTC_SET_MIMO_PS_MODE, &state);
}

static void halbtc8822b1ant_set_table(struct btc_coexist *btc,
				      boolean force_exec, u32 val0x6c0,
				      u32 val0x6c4, u32 val0x6c8, u8 val0x6cc)
{
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;

	if (!force_exec) {
		if (val0x6c0 == coex_dm->cur_val0x6c0 &&
		    val0x6c4 == coex_dm->cur_val0x6c4 &&
		    val0x6c8 == coex_dm->cur_val0x6c8 &&
		    val0x6cc == coex_dm->cur_val0x6cc)
			return;
	}

	btc->btc_write_4byte(btc, 0x6c0, val0x6c0);
	btc->btc_write_4byte(btc, 0x6c4, val0x6c4);
	btc->btc_write_4byte(btc, 0x6c8, val0x6c8);
	btc->btc_write_1byte(btc, 0x6cc, val0x6cc);

	coex_dm->cur_val0x6c0 = val0x6c0;
	coex_dm->cur_val0x6c4 = val0x6c4;
	coex_dm->cur_val0x6c8 = val0x6c8;
	coex_dm->cur_val0x6cc = val0x6cc;
}

static
void halbtc8822b1ant_table(struct btc_coexist *btc, boolean force_exec, u8 type)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	u32 break_table;
	u8 select_table;

	coex_sta->coex_table_type = type;

	if (coex_sta->concurrent_rx_mode_on) {
		/* set WL hi-pri can break BT */
		break_table = 0xf0ffffff;
		/* set Tx response = Hi-Pri (ex: Transmitting ACK,BA,CTS) */
		select_table = 0x1b;
	} else {
		break_table = 0xffffff;
		select_table = 0x13;
	}

	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
		    "[BTCoex], ********** Table-%d **********\n",
		    coex_sta->coex_table_type);
	BTC_TRACE(trace_buf);

	switch (type) {
	case 0:
		halbtc8822b1ant_set_table(btc, force_exec, 0x55555555,
					  0x55555555, break_table,
					  select_table);
		break;
	case 1:
		halbtc8822b1ant_set_table(btc, force_exec, 0x65555555,
					  0x6a5a5a5a, break_table,
					  select_table);
		break;
	case 2:
		halbtc8822b1ant_set_table(btc, force_exec, 0x65555555,
					  0xaaaaaaaa, break_table,
					  select_table);
		break;
	case 3:
		halbtc8822b1ant_set_table(btc, force_exec, 0x65555555,
					  0x6a5a5a5a, break_table,
					  select_table);
		break;
	case 4:
		halbtc8822b1ant_set_table(btc, force_exec, 0x65555555,
					  0x6a5a5a5a, break_table,
					  select_table);
		break;
	case 5:
		halbtc8822b1ant_set_table(btc, force_exec, 0x6a5a5a5a,
					  0x6a5a5a5a, break_table,
					  select_table);
		break;
	case 6:
		halbtc8822b1ant_set_table(btc, force_exec, 0x65555555,
					  0x6a5a5a5a, break_table,
					  select_table);
		break;
	case 7:
		halbtc8822b1ant_set_table(btc, force_exec, 0x65555555,
					  0xaaaa5aaa, break_table,
					  select_table);
		break;
	case 8:
		halbtc8822b1ant_set_table(btc, force_exec, 0xffffffff,
					  0xffffffff, break_table,
					  select_table);
		break;
	case 9:
		halbtc8822b1ant_set_table(btc, force_exec, 0x65555555,
					  0xaaaaaaaa, break_table,
					  select_table);
		break;
	case 10:
		halbtc8822b1ant_set_table(btc, force_exec, 0xaa5555aa,
					  0xaaaaaaaa, break_table,
					  select_table);
		break;
	case 11:
		halbtc8822b1ant_set_table(btc, force_exec, 0x65a55555,
					  0x6aaa5a5a, break_table,
					  select_table);
		break;
	case 12: /* not use */
		halbtc8822b1ant_set_table(btc, force_exec, 0xaaaaa5aa,
					  0xaaaaa5aa, break_table,
					  select_table);
		break;
	case 13:
		halbtc8822b1ant_set_table(btc, force_exec, 0xaa5555aa,
					  0x6a5a5a5a, break_table,
					  select_table);
		break;
	case 14:
		halbtc8822b1ant_set_table(btc, force_exec, 0xaa5555aa,
					  0x6a5a5a5a, break_table,
					  select_table);
		break;
	case 15: /* not use */
		halbtc8822b1ant_set_table(btc, force_exec, 0x55555555,
					  0xaaaa55aa, break_table,
					  select_table);
		break;
	case 16: /* not use */
		halbtc8822b1ant_set_table(btc, force_exec, 0x55555555,
					  0xaaaaaaaa, break_table,
					  select_table);
		break;
	case 17: /* not use */
		halbtc8822b1ant_set_table(btc, force_exec, 0xaaaa55aa,
					  0xaaaa55aa, break_table,
					  select_table);
		break;
	case 18: /* not use */
		halbtc8822b1ant_set_table(btc, force_exec, 0x55555555,
					  0x5aaa5a5a, break_table,
					  select_table);
		break;
	case 19:
		halbtc8822b1ant_set_table(btc, force_exec, 0x65555555,
					  0x6aaa5aaa, break_table,
					  select_table);
		break;
	case 20: /* not use */
		halbtc8822b1ant_set_table(btc, force_exec, 0x55555555,
					  0xaaaa5aaa, break_table,
					   select_table);
		break;
	case 21:
		halbtc8822b1ant_set_table(btc, force_exec, 0x65555555,
					  0x6aaa5afa, break_table,
					  select_table);
		break;
	case 22:
		halbtc8822b1ant_set_table(btc, force_exec, 0xaaffffaa,
					  0xfafafafa, break_table,
					  select_table);
		break;
	case 23:
		halbtc8822b1ant_set_table(btc, force_exec, 0xffff55ff,
					  0xfafafafa, break_table,
					  select_table);
		break;
	case 24:
		halbtc8822b1ant_set_table(btc, force_exec, 0x65555555,
					  0xfafafafa, break_table,
					  select_table);
		break;
	case 25:
		halbtc8822b1ant_set_table(btc, force_exec, 0xffffffff,
					  0x6a5a5a5a, break_table,
					  select_table);
		break;
	case 26:
		halbtc8822b1ant_set_table(btc, force_exec, 0xffffffff,
					  0x65555555, break_table,
					  select_table);
		break;
	default:
		break;
	}
}

static void halbtc8822b1ant_ignore_wlan_act(struct btc_coexist *btc,
					    boolean force_exec,
					    boolean enable)
{
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;

	u8 h2c_parameter[1] = {0};

	if (btc->manual_control || btc->stop_coex_dm)
		return;

	if (!force_exec) {
		if (enable == coex_dm->cur_ignore_wlan_act)
			return;
	}

	if (enable)
		h2c_parameter[0] |= BIT(0); /* function enable */

	btc->btc_fill_h2c(btc, 0x63, 1, h2c_parameter);

	coex_dm->cur_ignore_wlan_act = enable;
}

static void halbtc8822b1ant_lps_rpwm(struct btc_coexist *btc,
				     boolean force_exec, u8 lps_val,
				     u8 rpwm_val)
{
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;

	if (!force_exec) {
		if (lps_val == coex_dm->cur_lps &&
		    rpwm_val == coex_dm->cur_rpwm)
			return;
	}

	btc->btc_set(btc, BTC_SET_U1_LPS_VAL, &lps_val);
	btc->btc_set(btc, BTC_SET_U1_RPWM_VAL, &rpwm_val);

	coex_dm->cur_lps = lps_val;
	coex_dm->cur_rpwm = rpwm_val;
}

static void halbtc8822b1ant_multiport_tdma(struct btc_coexist *btc,
					   u8 multi_port_mode)
{
#if 0
	struct struct btc_multi_port_tdma_info multiport_tdma_para;
	static u8 pre_state = BTC_MULTI_PORT_TDMA_MODE_NONE;

	if (multi_port_mode == pre_state)
		return;

	multiport_tdma_para.btc_multi_port_tdma_mode = multi_port_mode;

	switch (multi_port_mode) {
	case BTC_MULTI_PORT_TDMA_MODE_NONE:
		multiport_tdma_para.start_time_from_bcn = 0;
		multiport_tdma_para.bt_time = 0;
		break;
	case BTC_MULTI_PORT_TDMA_MODE_2G_SCC_GO:
		multiport_tdma_para.start_time_from_bcn = 65;
		multiport_tdma_para.bt_time = 35;
		break;
	case BTC_MULTI_PORT_TDMA_MODE_2G_P2P_GO:
		multiport_tdma_para.start_time_from_bcn = 55;
		multiport_tdma_para.bt_time = 45;
		break;
	}

	btc->btc_set(btc, BTC_SET_WIFI_BT_COEX_MODE, &multiport_tdma_para);

	pre_state = multi_port_mode;
#endif
}

static void
halbtc8822b1ant_tdma_check(struct btc_coexist *btc, boolean new_ps_state)
{
	u8 lps_mode = 0x0;
	u8 h2c_parameter[5] = {0x8, 0, 0, 0, 0};

	btc->btc_get(btc, BTC_GET_U1_LPS_MODE, &lps_mode);

	if (lps_mode) { /* already under LPS state */
		if (new_ps_state) {
			/* keep state under LPS, do nothing. */
		} else {
			/* will leave LPS state, turn off psTdma first */
			btc->btc_fill_h2c(btc, 0x60, 5, h2c_parameter);
		}
	} else { /* NO PS state */
		if (new_ps_state) {
			/* will enter LPS state, turn off psTdma first */
			btc->btc_fill_h2c(btc, 0x60, 5, h2c_parameter);
		} else {
			/* keep state under NO PS state, do nothing. */
		}
	}
}

static boolean
halbtc8822b1ant_power_save_state(struct btc_coexist *btc,
				 u8 ps_type, u8 lps_val, u8 rpwm_val)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	boolean low_pwr_dis = FALSE, result = TRUE;

	switch (ps_type) {
	case BTC_PS_WIFI_NATIVE:
		/* recover to original 32k low power setting */
		coex_sta->force_lps_ctrl = FALSE;
		low_pwr_dis = FALSE;
		/* btc->btc_set(btc,
		 * over to original 32k low power setting
		 */

		btc->btc_set(btc, BTC_SET_ACT_PRE_NORMAL_LPS, NULL);
		break;
	case BTC_PS_LPS_ON:

		coex_sta->force_lps_ctrl = TRUE;
		halbtc8822b1ant_tdma_check(btc, TRUE);
		halbtc8822b1ant_lps_rpwm(btc, NM_EXCU, lps_val, rpwm_val);
		/* when coex force to enter LPS, do not enter 32k low power. */
		low_pwr_dis = TRUE;
		btc->btc_set(btc, BTC_SET_ACT_DISABLE_LOW_POWER, &low_pwr_dis);
		/* power save must executed before psTdma. */
		btc->btc_set(btc, BTC_SET_ACT_ENTER_LPS, NULL);

		break;
	case BTC_PS_LPS_OFF:

		coex_sta->force_lps_ctrl = TRUE;
		halbtc8822b1ant_tdma_check(btc, FALSE);
		result = btc->btc_set(btc, BTC_SET_ACT_LEAVE_LPS, NULL);

		break;
	default:
		break;
	}

	return result;
}

static
void halbtc8822b1ant_set_tdma(struct btc_coexist *btc, u8 byte1,
			      u8 byte2, u8 byte3, u8 byte4, u8 byte5)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;
	u8 h2c_parameter[5] = {0};
	u8 real_byte1 = byte1, real_byte5 = byte5;
	boolean ap_enable = FALSE, result = FALSE;
	struct btc_bt_link_info *bt_link_info = &btc->bt_link_info;
	u8 ps_type = BTC_PS_WIFI_NATIVE;

	if (byte5 & BIT(2))
		coex_sta->is_tdma_btautoslot = TRUE;
	else
		coex_sta->is_tdma_btautoslot = FALSE;

	/* release bt-auto slot for auto-slot hang is detected!! */
	if (coex_sta->is_tdma_btautoslot)
		if (coex_sta->is_tdma_btautoslot_hang ||
		    bt_link_info->slave_role)
			byte5 = byte5 & 0xfb;
#if 1
	btc->btc_get(btc, BTC_GET_BL_WIFI_AP_MODE_ENABLE, &ap_enable);
#else
	if (btc->wifi_link_info.link_mode == BTC_LINK_ONLY_GO &&
	    btc->wifi_link_info.bhotspot &&
	    btc->wifi_link_info.bany_client_join_go)
		ap_enable = TRUE;
#endif

	if ((ap_enable) && (byte1 & BIT(4) && !(byte1 & BIT(5)))) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], %s == FW for 1Ant AP mode\n", __func__);
		BTC_TRACE(trace_buf);

		real_byte1 &= ~BIT(4);
		real_byte1 |= BIT(5);

		real_byte5 |= BIT(5);
		real_byte5 &= ~BIT(6);

		ps_type = BTC_PS_WIFI_NATIVE;
		halbtc8822b1ant_power_save_state(btc, ps_type, 0x0, 0x0);

	} else if (byte1 & BIT(4) && !(byte1 & BIT(5))) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], %s == Force LPS (byte1 = 0x%x)\n",
			    __func__, byte1);
		BTC_TRACE(trace_buf);

		ps_type = BTC_PS_LPS_OFF;
		if (!halbtc8822b1ant_power_save_state(btc, ps_type, 0x50, 0x4))
			result = TRUE;
	} else {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], %s == native power save (byte1 = 0x%x)\n",
			    __func__, byte1);
		BTC_TRACE(trace_buf);

		ps_type = BTC_PS_WIFI_NATIVE;
		halbtc8822b1ant_power_save_state(btc, ps_type, 0x0, 0x0);
	}

	coex_sta->is_set_ps_state_fail = result;

	if (!coex_sta->is_set_ps_state_fail) {
		h2c_parameter[0] = real_byte1;
		h2c_parameter[1] = byte2;
		h2c_parameter[2] = byte3;
		h2c_parameter[3] = byte4;
		h2c_parameter[4] = real_byte5;

		coex_dm->ps_tdma_para[0] = real_byte1;
		coex_dm->ps_tdma_para[1] = byte2;
		coex_dm->ps_tdma_para[2] = byte3;
		coex_dm->ps_tdma_para[3] = byte4;
		coex_dm->ps_tdma_para[4] = real_byte5;

		btc->btc_fill_h2c(btc, 0x60, 5, h2c_parameter);

	} else {
		coex_sta->cnt_set_ps_state_fail++;
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], %s == Force Leave LPS Fail (cnt = %d)\n",
			    __func__, coex_sta->cnt_set_ps_state_fail);
		BTC_TRACE(trace_buf);
	}

	if (ps_type == BTC_PS_WIFI_NATIVE)
		btc->btc_set(btc, BTC_SET_ACT_POST_NORMAL_LPS, NULL);
}

static
void halbtc8822b1ant_tdma(struct btc_coexist *btc, boolean force_exec,
			  boolean turn_on, u8 type)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;
	struct btc_bt_link_info *bt_link_info = &btc->bt_link_info;
	boolean wifi_busy = FALSE;
	static u8 tdma_byte4_modify, pre_tdma_byte4_modify;
	static boolean pre_wifi_busy;
	u8 multiport_tdma = BTC_MULTI_PORT_TDMA_MODE_NONE;

	btc->btc_set_atomic(btc, &coex_dm->setting_tdma, TRUE);

	btc->btc_get(btc, BTC_GET_BL_WIFI_BUSY, &wifi_busy);

	if (wifi_busy != pre_wifi_busy) {
		force_exec = TRUE;
		pre_wifi_busy = wifi_busy;
	}

	/* 0x778 = 0x1 at wifi slot (no blocking BT Low-Pri pkts) */
	if (bt_link_info->slave_role)
		tdma_byte4_modify = 0x1;
	else
		tdma_byte4_modify = 0x0;

	if (pre_tdma_byte4_modify != tdma_byte4_modify) {
		force_exec = TRUE;
		pre_tdma_byte4_modify = tdma_byte4_modify;
	}

	if (!force_exec) {
		if (turn_on == coex_dm->cur_ps_tdma_on &&
		    type == coex_dm->cur_ps_tdma) {
			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], Skip TDMA because no change TDMA(%s, %d)\n",
				    (coex_dm->cur_ps_tdma_on ? "on" : "off"),
				    coex_dm->cur_ps_tdma);
			BTC_TRACE(trace_buf);

			btc->btc_set_atomic(btc, &coex_dm->setting_tdma, FALSE);
			return;
		}
	}

	if (turn_on) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], ********** TDMA(on, %d) **********\n",
			    type);
		BTC_TRACE(trace_buf);

		/* enable TBTT nterrupt */
		btc->btc_write_1byte_bitmask(btc, 0x550, 0x8, 0x1);
	} else {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], ********** TDMA(off, %d) **********\n",
			    type);
		BTC_TRACE(trace_buf);
	}

	if (turn_on) {
		/* enable TBTT nterrupt */
		btc->btc_write_1byte_bitmask(btc, 0x550, 0x8, 0x1);

		switch (type) {
		default:
			halbtc8822b1ant_set_tdma(btc, 0x61, 0x35, 0x03, 0x11,
						 0x11);
			break;
		case 1: /* not use */
			halbtc8822b1ant_set_tdma(btc, 0x61, 0x3a, 0x03, 0x11,
						 0x10);
			break;
		case 3:
			halbtc8822b1ant_set_tdma(btc, 0x51, 0x30, 0x03, 0x10,
						 0x50);
			break;
		case 4:
			halbtc8822b1ant_set_tdma(btc, 0x51, 0x21, 0x03, 0x10,
						 0x50);
			break;
		case 5:
			halbtc8822b1ant_set_tdma(btc, 0x61, 0x15, 0x3, 0x11,
						 0x11);
			break;
		case 6:
			halbtc8822b1ant_set_tdma(btc, 0x61, 0x20, 0x3, 0x11,
						 0x11);
			break;
		case 7:
			halbtc8822b1ant_set_tdma(btc, 0x51, 0x10, 0x03, 0x10,
						 0x54 | tdma_byte4_modify);
			break;
		case 8: /* not use */
			halbtc8822b1ant_set_tdma(btc, 0x51, 0x10, 0x03, 0x10,
						 0x14 | tdma_byte4_modify);
			break;
		case 10:
			halbtc8822b1ant_set_tdma(btc, 0x51, 0x10, 0x07, 0x10,
						 0x55);
			break;
		case 11:
			halbtc8822b1ant_set_tdma(btc, 0x61, 0x25, 0x03, 0x11,
						 0x11);
			break;
		case 12:
			halbtc8822b1ant_set_tdma(btc, 0x51, 0x30, 0x03, 0x10,
						 0x50 | tdma_byte4_modify);
			break;
		case 13:
			halbtc8822b1ant_set_tdma(btc, 0x51, 0x10, 0x07, 0x10,
						 0x54);
			break;
		case 14:
			halbtc8822b1ant_set_tdma(btc, 0x51, 0x15, 0x03, 0x10,
						 0x50 | tdma_byte4_modify);
			break;
		case 15:
			halbtc8822b1ant_set_tdma(btc, 0x51, 0x20, 0x03, 0x10,
						 0x10 | tdma_byte4_modify);
			break;
		case 17:
			halbtc8822b1ant_set_tdma(btc, 0x61, 0x10, 0x03, 0x11,
						 0x14 | tdma_byte4_modify);
			break;
		case 18:
			halbtc8822b1ant_set_tdma(btc, 0x51, 0x30, 0x03, 0x10,
						 0x50);
			break;
		case 20: /* not use */
			halbtc8822b1ant_set_tdma(btc, 0x61, 0x30, 0x03, 0x11,
						 0x10);
			break;
		case 21:
			halbtc8822b1ant_set_tdma(btc, 0x61, 0x30, 0x03, 0x11,
						 0x10);
			break;
		case 22:
			halbtc8822b1ant_set_tdma(btc, 0x61, 0x25, 0x03, 0x11,
						 0x10);
			break;
		case 23:
			halbtc8822b1ant_set_tdma(btc, 0x61, 0x10, 0x03, 0x11,
						 0x10);
			break;
		case 25:
			halbtc8822b1ant_set_tdma(btc, 0x51, 0x3a, 0x3, 0x11,
						 0x50);
			break;
		case 26:
			halbtc8822b1ant_set_tdma(btc, 0x51, 0x10, 0x03, 0x10,
						 0x55);
			break;
		case 27:
			halbtc8822b1ant_set_tdma(btc, 0x61, 0x10, 0x03, 0x11,
						 0x11);
			break;
		case 32:
			halbtc8822b1ant_set_tdma(btc, 0x61, 0x35, 0x3, 0x11,
						 0x11);
			break;
		case 33:
			halbtc8822b1ant_set_tdma(btc, 0x61, 0x35, 0x03, 0x11,
						 0x10);
			break;
		case 36:
			halbtc8822b1ant_set_tdma(btc, 0x61, 0x50, 0x03, 0x11,
						 0x10);
			break;
		case 37:
			halbtc8822b1ant_set_tdma(btc, 0x61, 0x3c, 0x03, 0x11,
						 0x10);
			break;
		case 50:
			halbtc8822b1ant_set_tdma(btc, 0x51, 0x4a, 0x3, 0x10,
						 0x10);
			break;
		case 105:
			halbtc8822b1ant_set_tdma(btc, 0x51, 0x3f, 0x3, 0x10,
						 0x50);
			break;
		case 201:
			multiport_tdma = BTC_MULTI_PORT_TDMA_MODE_2G_SCC_GO;
			halbtc8822b1ant_set_tdma(btc, 0x51, 0x3f, 0x3, 0x10,
						 0x50);
			break;
		case 202:
			multiport_tdma = BTC_MULTI_PORT_TDMA_MODE_2G_P2P_GO;
			halbtc8822b1ant_set_tdma(btc, 0x51, 0x35, 0x3, 0x10,
						 0x50);
			break;
		}
	} else {
		switch (type) {
		case 0:
		default: /* Software control, Antenna at BT side */
			halbtc8822b1ant_set_tdma(btc, 0x0, 0x0, 0x0, 0x0, 0x0);
			break;
		case 8: /* PTA Control */
			halbtc8822b1ant_set_tdma(btc, 0x8, 0x0, 0x0, 0x0, 0x0);
			break;
		case 9: /* Software control, Antenna at WiFi side */
			halbtc8822b1ant_set_tdma(btc, 0x0, 0x0, 0x0, 0x0, 0x0);
			break;
		case 10: /* under 5G , 0x778=1*/
			halbtc8822b1ant_set_tdma(btc, 0x0, 0x0, 0x0, 0x0, 0x0);
			break;
		}
	}

	halbtc8822b1ant_multiport_tdma(btc, multiport_tdma);

	if (!coex_sta->is_set_ps_state_fail) {
		/* update pre state */
		coex_dm->cur_ps_tdma_on = turn_on;
		coex_dm->cur_ps_tdma = type;
	}

	btc->btc_set_atomic(btc, &coex_dm->setting_tdma, FALSE);
}

/* rf4 type by efuse, and for ant at main aux inverse use,
 * because is 2x2, and control types are the same, does not need
 */
static void halbtc8822b1ant_set_rfe_type(struct btc_coexist *btc)
{
	struct rfe_type_8822b_1ant *rfe_type = &btc->rfe_type_8822b_1ant;
	struct btc_board_info *board_info = &btc->board_info;

	/* Ext switch buffer mux */
	btc->btc_write_1byte(btc, 0x974, 0xff);
	btc->btc_write_1byte_bitmask(btc, 0x1991, 0x3, 0x0);
	btc->btc_write_1byte_bitmask(btc, 0xcbe, 0x8, 0x0);

	/* the following setup should be got from Efuse in the future */
	rfe_type->rfe_module_type = board_info->rfe_type;

	rfe_type->ext_switch_polarity = 0;
	rfe_type->ext_switch_exist = TRUE;
	rfe_type->ext_switch_type = BT_8822B_1ANT_SWITCH_USE_SPDT;
}

/*anttenna control by bb mac bt antdiv pta to write 0x4c 0xcb4,0xcbd*/

static
void hallbtc8822b1ant_set_ant_switch(struct btc_coexist *btc,
				     boolean force_exec, u8 ctrl_type,
				     u8 pos_type)
{
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;
	struct rfe_type_8822b_1ant *rfe_type = &btc->rfe_type_8822b_1ant;
	boolean switch_polarity_inverse;
	u8 regval_0xcbd = 0, regval_0x64;

	if (!rfe_type->ext_switch_exist)
		return;

	if (!force_exec) {
		if (((ctrl_type << 8) + pos_type) == coex_dm->cur_switch_status)
			return;
	}

	coex_dm->cur_switch_status = (ctrl_type << 8) + pos_type;

	/* swap control polarity if use different switch control polarity*/
	/* Normal switch polarity for SPDT,
	 * 0xcbd[1:0] = 2b'01 => Ant to BTG, WLA
	 * 0xcbd[1:0] = 2b'10 => Ant to WLG
	 */
	switch_polarity_inverse = (rfe_type->ext_switch_polarity == 1);

	if (rfe_type->ext_switch_type ==
	    BT_8822B_1ANT_SWITCH_USE_SPDT) {
		switch (ctrl_type) {
		default:
		case BT_8822B_1ANT_CTRL_BY_BBSW:
			/*  0x4c[23] = 0 */
			btc->btc_write_1byte_bitmask(btc, 0x4e, 0x80, 0x0);
			/* 0x4c[24] = 1 */
			btc->btc_write_1byte_bitmask(btc, 0x4f, 0x01, 0x1);
			/* BB SW, DPDT use RFE_ctrl8 and RFE_ctrl9 as ctrl pin*/
			btc->btc_write_1byte_bitmask(btc, 0xcb4, 0xff, 0x77);

			/* 0xcbd[1:0] = 2b'01 for no switch_polarity_inverse,
			 * ANTSWB =1, ANTSW =0
			 */
			if (pos_type == BT_8822B_1ANT_TO_S0WLG_S1BT)
				regval_0xcbd = 0x3;
			else if (pos_type == BT_8822B_1ANT_TO_WLG)
				regval_0xcbd =
					(!switch_polarity_inverse ? 0x2 : 0x1);
			else
				regval_0xcbd =
					(!switch_polarity_inverse ? 0x1 : 0x2);
			btc->btc_write_1byte_bitmask(btc, 0xcbd, 0x3,
						     regval_0xcbd);
			break;
		case BT_8822B_1ANT_CTRL_BY_PTA:
			/* 0x4c[23] = 0 */
			btc->btc_write_1byte_bitmask(btc, 0x4e, 0x80, 0x0);
			/* 0x4c[24] = 1 */
			btc->btc_write_1byte_bitmask(btc, 0x4f, 0x01, 0x1);
			/* PTA,  DPDT use RFE_ctrl8 and RFE_ctrl9 as ctrl pin */
			btc->btc_write_1byte_bitmask(btc, 0xcb4, 0xff, 0x66);

			/* 0xcbd[1:0] = 2b'10 for no switch_polarity_inverse,
			 * ANTSWB =1, ANTSW =0  @ GNT_BT=1
			 */
			regval_0xcbd = (!switch_polarity_inverse ? 0x2 : 0x1);
			btc->btc_write_1byte_bitmask(btc, 0xcbd, 0x3,
						     regval_0xcbd);
			break;
		case BT_8822B_1ANT_CTRL_BY_ANTDIV:
			/* 0x4c[23] = 0 */
			btc->btc_write_1byte_bitmask(btc, 0x4e, 0x80, 0x0);
			/* 0x4c[24] = 1 */
			btc->btc_write_1byte_bitmask(btc, 0x4f, 0x01, 0x1);
			btc->btc_write_1byte_bitmask(btc, 0xcb4, 0xff, 0x88);

			/* no regval_0xcbd setup required, because
			 * antenna switch control value by antenna diversity
			 */
			break;
		case BT_8822B_1ANT_CTRL_BY_MAC:
			/*  0x4c[23] = 1 */
			btc->btc_write_1byte_bitmask(btc, 0x4e, 0x80, 0x1);

			/* 0x64[0] = 1b'0 for no switch_polarity_inverse,
			 * DPDT_SEL_N =1, DPDT_SEL_P =0
			 */
			regval_0x64 = (!switch_polarity_inverse ? 0x0 : 0x1);
			btc->btc_write_1byte_bitmask(btc, 0x64, 0x1,
						     regval_0x64);
			break;
		case BT_8822B_1ANT_CTRL_BY_BT:
			/* 0x4c[23] = 0 */
			btc->btc_write_1byte_bitmask(btc, 0x4e, 0x80, 0x0);
			/* 0x4c[24] = 0 */
			btc->btc_write_1byte_bitmask(btc, 0x4f, 0x01, 0x0);

			/* no setup required, because antenna switch control
			 * value by BT vendor 0xac[1:0]
			 */
			break;
		}
	}
}

/* set gnt_wl gnt_bt control by sw high low, or
 * hwpta while in power on, ini, wlan off, wlan only, wl2g non-currrent,
 * wl2g current, wl5g
 */

static
void halbtc8822b1ant_set_ant_path(struct btc_coexist *btc,
				  u8 ant_pos_type, boolean force_exec,
				  u8 phase)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;
	u32 u32tmp1 = 0;
	u8 u8tmp = 0;
	u32 u32tmp2 = 0, u32tmp3 = 0;
	u8 ctrl_type, pos_type;

	if (!force_exec) {
		if (coex_dm->cur_ant_pos_type == ((ant_pos_type << 8) + phase))
			return;
	}

	coex_dm->cur_ant_pos_type = (ant_pos_type << 8) + phase;

	if (btc->dbg_mode) {
		u32tmp1 = halbtc8822b1ant_read_indirect_reg(btc, 0x38);

		/* To avoid indirect access fail  */
		if (((u32tmp1 & 0xf000) >> 12) != ((u32tmp1 & 0x0f00) >> 8)) {
			force_exec = TRUE;
			coex_sta->gnt_error_cnt++;

			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex],(Before Ant Setup) 0x38= 0x%x\n",
				    u32tmp1);
			BTC_TRACE(trace_buf);
		}

		u32tmp1 = halbtc8822b1ant_read_indirect_reg(btc, 0x38);
		u32tmp2 = btc->btc_read_4byte(btc, 0xcbc);
		u32tmp3 = btc->btc_read_4byte(btc, 0xcb4);
		u8tmp = btc->btc_read_1byte(btc, 0x73);

		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], (Before Ant Setup) 0xcb4 = 0x%x, 0xcbc = 0x%x, 0x73 = 0x%x, 0x38= 0x%x\n",
			    u32tmp3, u32tmp2, u8tmp, u32tmp1);
		BTC_TRACE(trace_buf);
	}

	switch (phase) {
	case BT_8822B_1ANT_PHASE_POWERON:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], ********** (set_ant_path - 1ANT_PHASE_COEX_POWERON) **********\n");
		BTC_TRACE(trace_buf);

		/* set Path control owner to BT at power-on step */
		halbtc8822b1ant_coex_ctrl_owner(btc, BT_8822B_1ANT_PCO_BTSIDE);

		if (ant_pos_type == BTC_ANT_PATH_AUTO)
			ant_pos_type = BTC_ANT_PATH_BT;

		coex_sta->run_time_state = FALSE;
		break;
	case BT_8822B_1ANT_PHASE_INIT:
		/* Ext switch buffer mux */
		btc->btc_write_1byte(btc, 0x974, 0xff);
		btc->btc_write_1byte_bitmask(btc, 0x1991, 0x3, 0x0);
		btc->btc_write_1byte_bitmask(btc, 0xcbe, 0x8, 0x0);

		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], ********** (set_ant_path - 1ANT_PHASE_COEX_INIT) **********\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_ltecoex_enable(btc, 0x0);

		hallbtc8822b1ant_ltecoex_table(btc, BT_8822B_1ANT_CTT_WL_VS_LTE,
					       0xffff);

		hallbtc8822b1ant_ltecoex_table(btc, BT_8822B_1ANT_CTT_BT_VS_LTE,
					       0xffff);

		/* set GNT_BT to SW high */
		halbtc8822b1ant_set_gnt_bt(btc, BT_8822B_1ANT_GNT_BLOCK_RFC_BB,
					   BT_8822B_1ANT_GNT_CTRL_BY_SW,
					   BT_8822B_1ANT_GNT_SET_TO_HIGH);

		/* set GNT_WL to SW low */
		halbtc8822b1ant_set_gnt_wl(btc, BT_8822B_1ANT_GNT_BLOCK_RFC_BB,
					   BT_8822B_1ANT_GNT_CTRL_BY_SW,
					   BT_8822B_1ANT_GNT_SET_TO_LOW);

		/* set Path control owner to WL at initial step */
		halbtc8822b1ant_coex_ctrl_owner(btc, BT_8822B_1ANT_PCO_WLSIDE);

		coex_sta->run_time_state = FALSE;

		if (ant_pos_type == BTC_ANT_PATH_AUTO)
			ant_pos_type = BTC_ANT_PATH_BT;

		break;
	case BT_8822B_1ANT_PHASE_WONLY:
		/* Ext switch buffer mux */
		btc->btc_write_1byte(btc, 0x974, 0xff);
		btc->btc_write_1byte_bitmask(btc, 0x1991, 0x3, 0x0);
		btc->btc_write_1byte_bitmask(btc, 0xcbe, 0x8, 0x0);

		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], ********** (set_ant_path - 1ANT_PHASE_WLANONLY_INIT) **********\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_ltecoex_enable(btc, 0x0);

		hallbtc8822b1ant_ltecoex_table(btc, BT_8822B_1ANT_CTT_WL_VS_LTE,
					       0xffff);

		hallbtc8822b1ant_ltecoex_table(btc, BT_8822B_1ANT_CTT_BT_VS_LTE,
					       0xffff);

		/* set GNT_BT to SW Low */
		halbtc8822b1ant_set_gnt_bt(btc, BT_8822B_1ANT_GNT_BLOCK_RFC_BB,
					   BT_8822B_1ANT_GNT_CTRL_BY_SW,
					   BT_8822B_1ANT_GNT_SET_TO_LOW);

		/* Set GNT_WL to SW high */
		halbtc8822b1ant_set_gnt_wl(btc, BT_8822B_1ANT_GNT_BLOCK_RFC_BB,
					   BT_8822B_1ANT_GNT_CTRL_BY_SW,
					   BT_8822B_1ANT_GNT_SET_TO_HIGH);

		/* set Path control owner to WL at initial step */
		halbtc8822b1ant_coex_ctrl_owner(btc, BT_8822B_1ANT_PCO_WLSIDE);

		coex_sta->run_time_state = FALSE;

		if (ant_pos_type == BTC_ANT_PATH_AUTO)
			ant_pos_type = BTC_ANT_PATH_WIFI;
		break;
	case BT_8822B_1ANT_PHASE_WOFF:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], ********** (set_ant_path - 1ANT_PHASE_WLAN_OFF) **********\n");
		BTC_TRACE(trace_buf);

		/* Disable LTE Coex Function in WiFi side */
		halbtc8822b1ant_ltecoex_enable(btc, 0x0);

		/* set Path control owner to BT */
		halbtc8822b1ant_coex_ctrl_owner(btc, BT_8822B_1ANT_PCO_BTSIDE);

		/* Set Ext Ant Switch to BT control at wifi off step */
		hallbtc8822b1ant_set_ant_switch(btc, FC_EXCU,
						BT_8822B_1ANT_CTRL_BY_BT,
						BT_8822B_1ANT_TO_NOCARE);

		coex_sta->run_time_state = FALSE;
		break;
	case BT_8822B_1ANT_PHASE_2G:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], ********** (set_ant_path - 1ANT_PHASE_2G_RUNTIME) **********\n");
		BTC_TRACE(trace_buf);

		/* set GNT_BT to PTA */
		halbtc8822b1ant_set_gnt_bt(btc, BT_8822B_1ANT_GNT_BLOCK_RFC_BB,
					   BT_8822B_1ANT_GNT_CTRL_BY_PTA,
					   BT_8822B_1ANT_GNT_SET_BY_HW);

		/* Set GNT_WL to PTA */
		halbtc8822b1ant_set_gnt_wl(btc, BT_8822B_1ANT_GNT_BLOCK_RFC_BB,
					   BT_8822B_1ANT_GNT_CTRL_BY_PTA,
					   BT_8822B_1ANT_GNT_SET_BY_HW);

		/* set Path control owner to WL at runtime step */
		halbtc8822b1ant_coex_ctrl_owner(btc, BT_8822B_1ANT_PCO_WLSIDE);

		coex_sta->run_time_state = TRUE;

		if (ant_pos_type == BTC_ANT_PATH_AUTO)
			ant_pos_type = BTC_ANT_PATH_PTA;
		break;
	case BT_8822B_1ANT_PHASE_5G:
	case BT_8822B_1ANT_PHASE_2G_WL:
	case BT_8822B_1ANT_PHASE_2G_BT:

		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], ********** (set_ant_path - %d)\n",
			    phase);
		BTC_TRACE(trace_buf);

		/* set GNT_BT to SW Hi */
		halbtc8822b1ant_set_gnt_bt(btc, BT_8822B_1ANT_GNT_BLOCK_RFC_BB,
					   BT_8822B_1ANT_GNT_CTRL_BY_SW,
					   BT_8822B_1ANT_GNT_SET_TO_HIGH);

		/* Set GNT_WL to SW Hi */
		halbtc8822b1ant_set_gnt_wl(btc, BT_8822B_1ANT_GNT_BLOCK_RFC_BB,
					   BT_8822B_1ANT_GNT_CTRL_BY_SW,
					   BT_8822B_1ANT_GNT_SET_TO_HIGH);

		/* set Path control owner to WL at runtime step */
		halbtc8822b1ant_coex_ctrl_owner(btc, BT_8822B_1ANT_PCO_WLSIDE);

		coex_sta->run_time_state = TRUE;

		if (phase == BT_8822B_1ANT_PHASE_2G_WL)
			ant_pos_type = BTC_ANT_PATH_WIFI;
		else if (phase == BT_8822B_1ANT_PHASE_2G_BT)
			ant_pos_type = BTC_ANT_PATH_BT;
		if (ant_pos_type == BTC_ANT_PATH_AUTO)
			ant_pos_type = BTC_ANT_PATH_WIFI5G;
		break;
	case BT_8822B_1ANT_PHASE_BTMP:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], ********** (set_ant_path - 1ANT_PHASE_BTMPMODE) **********\n");
		BTC_TRACE(trace_buf);

		/* Disable LTE Coex Function in WiFi side */
		halbtc8822b1ant_ltecoex_enable(btc, 0x0);

		/* set GNT_BT to SW Hi */
		halbtc8822b1ant_set_gnt_bt(btc, BT_8822B_1ANT_GNT_BLOCK_RFC_BB,
					   BT_8822B_1ANT_GNT_CTRL_BY_SW,
					   BT_8822B_1ANT_GNT_SET_TO_HIGH);

		/* Set GNT_WL to SW Lo */
		halbtc8822b1ant_set_gnt_wl(btc, BT_8822B_1ANT_GNT_BLOCK_RFC_BB,
					   BT_8822B_1ANT_GNT_CTRL_BY_SW,
					   BT_8822B_1ANT_GNT_SET_TO_LOW);

		/* set Path control owner to WL */
		halbtc8822b1ant_coex_ctrl_owner(btc, BT_8822B_1ANT_PCO_WLSIDE);

		coex_sta->run_time_state = FALSE;

		/* Set Ext Ant Switch to BT side at BT MP mode */
		if (ant_pos_type == BTC_ANT_PATH_AUTO)
			ant_pos_type = BTC_ANT_PATH_BT;
		break;
	case BT_8822B_1ANT_PHASE_MCC:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], (set_ant_path - 1ANT_PHASE_MCC_DUALBAND_RUNTIME)\n");
		BTC_TRACE(trace_buf);

		/* set GNT_BT to PTA */
		halbtc8822b1ant_set_gnt_bt(btc, BT_8822B_1ANT_GNT_BLOCK_RFC_BB,
					   BT_8822B_1ANT_GNT_CTRL_BY_PTA,
					   BT_8822B_1ANT_GNT_SET_BY_HW);

		/* Set GNT_WL to PTA */
		halbtc8822b1ant_set_gnt_wl(btc, BT_8822B_1ANT_GNT_BLOCK_RFC_BB,
					   BT_8822B_1ANT_GNT_CTRL_BY_PTA,
					   BT_8822B_1ANT_GNT_SET_BY_HW);

		/* set Path control owner to WL at runtime step */
		halbtc8822b1ant_coex_ctrl_owner(btc, BT_8822B_1ANT_PCO_WLSIDE);

		coex_sta->run_time_state = TRUE;

		if (ant_pos_type == BTC_ANT_PATH_AUTO)
			ant_pos_type = BTC_ANT_PATH_PTA;
		break;
	}

	if (phase != BT_8822B_1ANT_PHASE_WOFF &&
	    phase != BT_8822B_1ANT_PHASE_MCC) {
		switch (ant_pos_type) {
		case BTC_ANT_PATH_WIFI:
			if (phase == BT_8822B_1ANT_PHASE_2G_WL) {
				ctrl_type = BT_8822B_1ANT_CTRL_BY_BBSW;
				pos_type = BT_8822B_1ANT_TO_S0WLG_S1BT;
			} else {
				ctrl_type = BT_8822B_1ANT_CTRL_BY_BBSW;
				pos_type = BT_8822B_1ANT_TO_WLG;
			}
			break;
		case BTC_ANT_PATH_WIFI5G:
			ctrl_type = BT_8822B_1ANT_CTRL_BY_BBSW;
			pos_type = BT_8822B_1ANT_TO_WLA;
			break;
		case BTC_ANT_PATH_BT:
			ctrl_type = BT_8822B_1ANT_CTRL_BY_BBSW;
			pos_type = BT_8822B_1ANT_TO_BT;
			break;
		default:
		case BTC_ANT_PATH_PTA:
			ctrl_type = BT_8822B_1ANT_CTRL_BY_PTA;
			pos_type = BT_8822B_1ANT_TO_NOCARE;
			break;
		}

		hallbtc8822b1ant_set_ant_switch(btc, force_exec, ctrl_type,
						pos_type);
	}

	if (btc->dbg_mode) {
		u32tmp1 = halbtc8822b1ant_read_indirect_reg(btc, 0x38);
		u32tmp2 = btc->btc_read_4byte(btc, 0xcbc);
		u32tmp3 = btc->btc_read_4byte(btc, 0xcb4);
		u8tmp = btc->btc_read_1byte(btc, 0x73);

		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], (After Ant Setup) 0xcb4 = 0x%x, 0xcbc = 0x%x, 0x73 = 0x%x, 0x38= 0x%x\n",
			    u32tmp3, u32tmp2, u8tmp, u32tmp1);
		BTC_TRACE(trace_buf);
	}
}

static u8 halbtc8822b1ant_action_algorithm(struct btc_coexist *btc)
{
	struct btc_bt_link_info *bt_link_info = &btc->bt_link_info;
	boolean bt_hs_on = FALSE;
	u8 algorithm = BT_8822B_1ANT_COEX_UNDEFINED;
	u8 num_of_diff_profile = 0;

	if (!bt_link_info->bt_link_exist) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], No BT link exists!!!\n");
		BTC_TRACE(trace_buf);
		return algorithm;
	}

	if (bt_link_info->sco_exist)
		num_of_diff_profile++;
	if (bt_link_info->hid_exist)
		num_of_diff_profile++;
	if (bt_link_info->pan_exist)
		num_of_diff_profile++;
	if (bt_link_info->a2dp_exist)
		num_of_diff_profile++;

	if (num_of_diff_profile == 1) {
		if (bt_link_info->sco_exist) {
			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], BT Profile = SCO only\n");
			BTC_TRACE(trace_buf);
			algorithm = BT_8822B_1ANT_COEX_SCO;
		} else {
			if (bt_link_info->hid_exist) {
				BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
					    "[BTCoex], BT Profile = HID only\n");
				BTC_TRACE(trace_buf);
				algorithm = BT_8822B_1ANT_COEX_HID;
			} else if (bt_link_info->a2dp_exist) {
				BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
					    "[BTCoex], BT Profile = A2DP only\n");
				BTC_TRACE(trace_buf);
				algorithm = BT_8822B_1ANT_COEX_A2DP;
			} else if (bt_link_info->pan_exist) {
				if (bt_hs_on) {
					BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
						    "[BTCoex], BT Profile = PAN(HS) only\n");
					BTC_TRACE(trace_buf);
					algorithm =
						BT_8822B_1ANT_COEX_PANHS;
				} else {
					BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
						    "[BTCoex], BT Profile = PAN(EDR) only\n");
					BTC_TRACE(trace_buf);
					algorithm = BT_8822B_1ANT_COEX_PAN;
				}
			}
		}
	} else if (num_of_diff_profile == 2) {
		if (bt_link_info->sco_exist) {
			if (bt_link_info->hid_exist) {
				BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
					    "[BTCoex], BT Profile = SCO + HID\n");
				BTC_TRACE(trace_buf);
				algorithm = BT_8822B_1ANT_COEX_HID;
			} else if (bt_link_info->a2dp_exist) {
				BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
					    "[BTCoex], BT Profile = SCO + A2DP ==> SCO\n");
				BTC_TRACE(trace_buf);
				algorithm = BT_8822B_1ANT_COEX_SCO;
			} else if (bt_link_info->pan_exist) {
				if (bt_hs_on) {
					BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
						    "[BTCoex], BT Profile = SCO + PAN(HS)\n");
					BTC_TRACE(trace_buf);
					algorithm = BT_8822B_1ANT_COEX_SCO;
				} else {
					BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
						    "[BTCoex], BT Profile = SCO + PAN(EDR)\n");
					BTC_TRACE(trace_buf);
					algorithm = BT_8822B_1ANT_COEX_PAN_HID;
				}
			}
		} else {
			if (bt_link_info->hid_exist &&
			    bt_link_info->a2dp_exist) {
				BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
					    "[BTCoex], BT Profile = HID + A2DP\n");
				BTC_TRACE(trace_buf);
				algorithm = BT_8822B_1ANT_COEX_HID_A2DP;
			} else if (bt_link_info->hid_exist &&
				   bt_link_info->pan_exist) {
				if (bt_hs_on) {
					BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
						    "[BTCoex], BT Profile = HID + PAN(HS)\n");
					BTC_TRACE(trace_buf);
					algorithm =
						BT_8822B_1ANT_COEX_HID_A2DP;
				} else {
					BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
						    "[BTCoex], BT Profile = HID + PAN(EDR)\n");
					BTC_TRACE(trace_buf);
					algorithm = BT_8822B_1ANT_COEX_PAN_HID;
				}
			} else if (bt_link_info->pan_exist &&
				   bt_link_info->a2dp_exist) {
				if (bt_hs_on) {
					BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
						    "[BTCoex], BT Profile = A2DP + PAN(HS)\n");
					BTC_TRACE(trace_buf);
					algorithm =
						BT_8822B_1ANT_COEX_A2DP_PANHS;
				} else {
					BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
						    "[BTCoex], BT Profile = A2DP + PAN(EDR)\n");
					BTC_TRACE(trace_buf);
					algorithm = BT_8822B_1ANT_COEX_PAN_A2DP;
				}
			}
		}
	} else if (num_of_diff_profile == 3) {
		if (bt_link_info->sco_exist) {
			if (bt_link_info->hid_exist &&
			    bt_link_info->a2dp_exist) {
				BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
					    "[BTCoex], BT Profile = SCO + HID + A2DP ==> HID\n");
				BTC_TRACE(trace_buf);
				algorithm = BT_8822B_1ANT_COEX_HID;
			} else if (bt_link_info->hid_exist &&
				   bt_link_info->pan_exist) {
				if (bt_hs_on) {
					BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
						    "[BTCoex], BT Profile = SCO + HID + PAN(HS)\n");
					BTC_TRACE(trace_buf);
					algorithm =
						BT_8822B_1ANT_COEX_HID_A2DP;
				} else {
					BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
						    "[BTCoex], BT Profile = SCO + HID + PAN(EDR)\n");
					BTC_TRACE(trace_buf);
					algorithm = BT_8822B_1ANT_COEX_PAN_HID;
				}
			} else if (bt_link_info->pan_exist &&
				   bt_link_info->a2dp_exist) {
				if (bt_hs_on) {
					BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
						    "[BTCoex], BT Profile = SCO + A2DP + PAN(HS)\n");
					BTC_TRACE(trace_buf);
					algorithm = BT_8822B_1ANT_COEX_SCO;
				} else {
					BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
						    "[BTCoex], BT Profile = SCO + A2DP + PAN(EDR) ==> HID\n");
					BTC_TRACE(trace_buf);
					algorithm = BT_8822B_1ANT_COEX_PAN_HID;
				}
			}
		} else {
			if (bt_link_info->hid_exist &&
			    bt_link_info->pan_exist &&
			    bt_link_info->a2dp_exist) {
				if (bt_hs_on) {
					BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
						    "[BTCoex], BT Profile = HID + A2DP + PAN(HS)\n");
					BTC_TRACE(trace_buf);
					algorithm =
						BT_8822B_1ANT_COEX_HID_A2DP;
				} else {
					BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
						    "[BTCoex], BT Profile = HID + A2DP + PAN(EDR)\n");
					BTC_TRACE(trace_buf);
					algorithm =
						BT_8822B_1ANT_COEX_HID_A2DP_PAN;
				}
			}
		}
	} else if (num_of_diff_profile >= 3) {
		if (bt_link_info->sco_exist) {
			if (bt_link_info->hid_exist &&
			    bt_link_info->pan_exist &&
			    bt_link_info->a2dp_exist) {
				if (bt_hs_on) {
					BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
						    "[BTCoex], Error!!! BT Profile = SCO + HID + A2DP + PAN(HS)\n");
					BTC_TRACE(trace_buf);

				} else {
					BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
						    "[BTCoex], BT Profile = SCO + HID + A2DP + PAN(EDR)==>PAN(EDR)+HID\n");
					BTC_TRACE(trace_buf);
					algorithm = BT_8822B_1ANT_COEX_PAN_HID;
				}
			}
		}
	}

	return algorithm;
}

static void halbtc8822b1ant_action_coex_all_off(struct btc_coexist *btc)
{
	halbtc8822b1ant_table(btc, NM_EXCU, 0);
	halbtc8822b1ant_tdma(btc, NM_EXCU, FALSE, 0);
}

static void halbtc8822b1ant_action_bt_whql_test(struct btc_coexist *btc)
{
	halbtc8822b1ant_set_wl_tx_power(btc, NM_EXCU, 0xd8);
	halbtc8822b1ant_set_bt_tx_power(btc, NM_EXCU, 0);
	halbtc8822b1ant_set_wl_rx_gain(btc, NM_EXCU, FALSE);
	halbtc8822b1ant_set_bt_rx_gain(btc, NM_EXCU, FALSE);

	halbtc8822b1ant_table(btc, NM_EXCU, 0);
	halbtc8822b1ant_tdma(btc, NM_EXCU, FALSE, 8);
}

static void halbtc8822b1ant_action_bt_relink(struct btc_coexist *btc)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct btc_bt_link_info *bt_link_info = &btc->bt_link_info;

	if ((!coex_sta->is_bt_multi_link && !bt_link_info->pan_exist) ||
	    (bt_link_info->a2dp_exist && bt_link_info->hid_exist)) {
		halbtc8822b1ant_set_wl_tx_power(btc, NM_EXCU, 0xd8);
		halbtc8822b1ant_set_bt_tx_power(btc, NM_EXCU, 0);
		halbtc8822b1ant_set_wl_rx_gain(btc, NM_EXCU, FALSE);
		halbtc8822b1ant_set_bt_rx_gain(btc, NM_EXCU, FALSE);

		halbtc8822b1ant_table(btc, NM_EXCU, 8);
		halbtc8822b1ant_tdma(btc, NM_EXCU, FALSE, 8);
	}
}

static void halbtc8822b1ant_action_bt_idle(struct btc_coexist *btc)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;
	boolean wifi_busy = FALSE;

	btc->btc_get(btc, BTC_GET_BL_WIFI_BUSY, &wifi_busy);

	halbtc8822b1ant_set_wl_tx_power(btc, NM_EXCU, 0xd8);
	halbtc8822b1ant_set_bt_tx_power(btc, NM_EXCU, 0);
	halbtc8822b1ant_set_wl_rx_gain(btc, NM_EXCU, FALSE);
	halbtc8822b1ant_set_bt_rx_gain(btc, NM_EXCU, FALSE);

	if (!wifi_busy) {
		halbtc8822b1ant_table(btc, NM_EXCU, 7);
		halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 32);
	} else { /* if wl busy */
		/*for initiator scan on*/
		if ((coex_sta->bt_ble_scan_type & 0x2) &&
		   (BT_8822B_1ANT_BSTATUS_NCON_IDLE ==
		    coex_dm->bt_status)) {
			halbtc8822b1ant_table(btc, NM_EXCU, 2);
			halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 36);
		} else {
			halbtc8822b1ant_table(btc, NM_EXCU, 2);
			halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 33);
		}
	}
}

static void halbtc8822b1ant_action_bt_inquiry(struct btc_coexist *btc)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct	btc_bt_link_info *bt_link_info = &btc->bt_link_info;
	boolean wifi_connected = FALSE, wifi_busy = FALSE;

	halbtc8822b1ant_set_wl_tx_power(btc, NM_EXCU, 0xd8);
	halbtc8822b1ant_set_bt_tx_power(btc, NM_EXCU, 0);
	halbtc8822b1ant_set_wl_rx_gain(btc, NM_EXCU, FALSE);
	halbtc8822b1ant_set_bt_rx_gain(btc, NM_EXCU, FALSE);

	btc->btc_get(btc, BTC_GET_BL_WIFI_CONNECTED, &wifi_connected);
	btc->btc_get(btc, BTC_GET_BL_WIFI_BUSY, &wifi_busy);

	if (coex_sta->is_wifi_linkscan_process ||
	    coex_sta->wifi_high_pri_task1 || coex_sta->wifi_high_pri_task2) {
		if (coex_sta->bt_create_connection) {
			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], bt page +  wifi hi-pri task\n");
			BTC_TRACE(trace_buf);

			halbtc8822b1ant_table(btc, NM_EXCU, 23);

			if (bt_link_info->a2dp_exist &&
			    !bt_link_info->pan_exist) {
				halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 17);
			} else if (coex_sta->wifi_high_pri_task1) {
				halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 36);
			} else {
				halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 33);
			}
		} else {
			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], bt inquiry +  wifi hi-pri task\n");
			BTC_TRACE(trace_buf);

			halbtc8822b1ant_table(btc, NM_EXCU, 23);
			halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 21);
		}
	} else if (wifi_busy) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], bt inq/page +  wifi busy\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_table(btc, NM_EXCU, 23);
		/* for android 6.0  remote name request */
		halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 23);
	} else if (wifi_connected) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], bt inq/page +  wifi connected\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_table(btc, NM_EXCU, 23);
		halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 23);
	} else {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], bt inq/page +  wifi not-connected\n");
		BTC_TRACE(trace_buf);
		halbtc8822b1ant_table(btc, NM_EXCU, 0);
		halbtc8822b1ant_tdma(btc, NM_EXCU, FALSE, 8);
	}
}

static
void halbtc8822b1ant_action_bt_sco_hid_busy(struct btc_coexist *btc)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct btc_bt_link_info *bt_link_info = &btc->bt_link_info;
	boolean wifi_connected = FALSE, wifi_busy = FALSE;
	u32 wifi_bw = 1;

	halbtc8822b1ant_set_wl_tx_power(btc, NM_EXCU, 0xd8);
	halbtc8822b1ant_set_bt_tx_power(btc, NM_EXCU, 0);
	halbtc8822b1ant_set_wl_rx_gain(btc, NM_EXCU, FALSE);
	halbtc8822b1ant_set_bt_rx_gain(btc, NM_EXCU, FALSE);

	btc->btc_get(btc, BTC_GET_BL_WIFI_CONNECTED, &wifi_connected);
	btc->btc_get(btc, BTC_GET_U4_WIFI_BW, &wifi_bw);
	btc->btc_get(btc, BTC_GET_BL_WIFI_BUSY, &wifi_busy);

	if (!wifi_busy)
		wifi_busy = coex_sta->gl_wifi_busy;

	if (bt_link_info->sco_exist) {
		if (coex_sta->is_bt_multi_link) {
			halbtc8822b1ant_table(btc, NM_EXCU, 1);
			halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 25);
		} else {
			halbtc8822b1ant_table(btc, NM_EXCU, 5);
			halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 5);
		}
	} else if (coex_sta->is_hid_rcu) {
		if (coex_sta->voice_over_HOGP) { /* voice by RCU */
			/* change coex table if slave latency support or not */
			if (!wifi_busy)
				halbtc8822b1ant_table(btc, NM_EXCU, 7);
			else if (coex_sta->bt_coex_supported_feature & BIT(11))
				halbtc8822b1ant_table(btc, NM_EXCU, 3);
			else
				halbtc8822b1ant_table(btc, NM_EXCU, 2);

			if (wifi_busy)
				halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 37);
			else
				halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 6);
		} else { /* RCU */
			if (!wifi_busy)
				halbtc8822b1ant_table(btc, NM_EXCU, 7);
			else if (coex_sta->bt_coex_supported_feature & BIT(11))
				halbtc8822b1ant_table(btc, NM_EXCU, 3);
			else
				halbtc8822b1ant_table(btc, NM_EXCU, 2);

			if (wifi_busy && coex_sta->wl_noisy_level == 0)
				halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 36);
			else if (wifi_busy)
				halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 37);
			else
				halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 6);
		}
	} else {
		if (coex_sta->is_hid_low_pri_tx_overhead) {
			if (coex_sta->hid_busy_num < 2)
				halbtc8822b1ant_table(btc, NM_EXCU, 1);
			else
				halbtc8822b1ant_table(btc, NM_EXCU, 6);
			halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 18);
		} else if (coex_sta->hid_busy_num < 2) {
			halbtc8822b1ant_table(btc, NM_EXCU, 1);
			halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 11);
		} else if (wifi_bw == 0) { /* if 11bg mode */
			halbtc8822b1ant_table(btc, NM_EXCU, 11);
			halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 11);
		} else {
			halbtc8822b1ant_table(btc, NM_EXCU, 6);
			halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 11);
		}
	}
}

static void halbtc8822b1ant_action_bt_acl_busy(struct btc_coexist *btc)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct btc_bt_link_info *bt_link_info = &btc->bt_link_info;
	boolean wifi_busy = FALSE, wifi_turbo = FALSE,
		wifi_cckdeadlock_ap = FALSE, bt_slave_latency = FALSE,
		ap_enable = FALSE;
	u32 wifi_bw = 1;
	static u8 prewifi_rssi_state = BTC_RSSI_STATE_LOW;
	u8 wifi_rssi_state;
	static u8 prewifi_rssi_state2 = BTC_RSSI_STATE_LOW;
	u8 wifi_rssi_state2;
	u8 iot_peer = BTC_IOT_PEER_UNKNOWN;

	btc->btc_get(btc, BTC_GET_U4_WIFI_BW, &wifi_bw);
	btc->btc_get(btc, BTC_GET_BL_WIFI_BUSY, &wifi_busy);
	btc->btc_get(btc, BTC_GET_U1_AP_NUM,
			   &coex_sta->scan_ap_num);
	btc->btc_get(btc, BTC_GET_U1_IOT_PEER, &iot_peer);
	btc->btc_get(btc, BTC_GET_BL_WIFI_AP_MODE_ENABLE, &ap_enable);

	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
		    "############# [BTCoex],  scan_ap_num = %d, wl_noisy_level = %d\n",
		    coex_sta->scan_ap_num, coex_sta->wl_noisy_level);
	BTC_TRACE(trace_buf);

	if (!wifi_busy)
		wifi_busy = coex_sta->gl_wifi_busy;

	if (wifi_busy && coex_sta->wl_noisy_level == 0)
		wifi_turbo = TRUE;

	if (iot_peer == BTC_IOT_PEER_ATHEROS && coex_sta->cck_lock_ever)
		wifi_cckdeadlock_ap = TRUE;

	if (coex_sta->bt_coex_supported_feature & BIT(11))
		bt_slave_latency = TRUE;
	else
		bt_slave_latency = FALSE;

	wifi_rssi_state =
		halbtc8822b1ant_wifi_rssi_state(btc, &prewifi_rssi_state, 2,
						40, 0);

	wifi_rssi_state2 =
		halbtc8822b1ant_wifi_rssi_state(btc, &prewifi_rssi_state2, 2,
						42, 0);

	if (btc->board_info.customer_id == RT_CID_LENOVO_CHINA &&
	    coex_sta->scan_ap_num <= 10 &&
	    iot_peer == BTC_IOT_PEER_ATHEROS &&
	    ((bt_link_info->a2dp_only && coex_sta->is_bt_multi_link &&
	     coex_sta->hid_pair_cnt == 0) ||
	     (bt_link_info->a2dp_exist && bt_link_info->pan_exist))) {
			/* OPP may disappear during CPT_for_WiFi test */
			/* for CPT_for_WiFi   */

		if (BTC_RSSI_LOW(wifi_rssi_state2)) {
			halbtc8822b1ant_set_bt_tx_power(btc, NM_EXCU, 20);

			if (wifi_busy) {
				halbtc8822b1ant_table(btc, NM_EXCU, 22);
				halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 105);
			} else {
				halbtc8822b1ant_table(btc, NM_EXCU, 23);
				halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 13);
			}
		} else { /* for CPT_for_BT   */
			halbtc8822b1ant_set_bt_tx_power(btc, NM_EXCU, 0);
			halbtc8822b1ant_table(btc, NM_EXCU, 24);
			halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 13);
		}

			return;
	}

	halbtc8822b1ant_set_bt_tx_power(btc, NM_EXCU, 0);

	if (bt_link_info->a2dp_exist && coex_sta->is_bt_a2dp_sink) {
		if (ap_enable)
			halbtc8822b1ant_table(btc, NM_EXCU, 0);
		else if (coex_sta->wl_noisy_level == 0 && wifi_cckdeadlock_ap)
			halbtc8822b1ant_table(btc, NM_EXCU, 13);
		else
			halbtc8822b1ant_table(btc, NM_EXCU, 6);
		if (ap_enable)
			halbtc8822b1ant_tdma(btc, NM_EXCU, FALSE, 8);
		else
			halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 12);
	} else if (bt_link_info->a2dp_only) { /* A2DP */
		if (wifi_busy && (coex_sta->bt_ble_scan_type & 0x2)) {
			if (coex_sta->wl_noisy_level == 0 &&
			    wifi_cckdeadlock_ap)
				halbtc8822b1ant_table(btc, NM_EXCU, 10);
			else if (coex_sta->wl_noisy_level == 0 &&
				 !wifi_cckdeadlock_ap)
				halbtc8822b1ant_table(btc, NM_EXCU, 9);
			else
				halbtc8822b1ant_table(btc, NM_EXCU, 3);
		} else if (wifi_busy && !(coex_sta->bt_ble_scan_type & 0x2)) {
			if (coex_sta->wl_noisy_level == 0 &&
			    wifi_cckdeadlock_ap)
				halbtc8822b1ant_table(btc, NM_EXCU, 13);
			else
				halbtc8822b1ant_table(btc, NM_EXCU, 3);
		} else { /* wifi idle  */
			halbtc8822b1ant_table(btc, NM_EXCU, 7);
		}

		if (coex_sta->connect_ap_period_cnt > 0 || !wifi_busy)
			halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 26);
		else
			halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 7);
	} else if ((bt_link_info->a2dp_exist && bt_link_info->pan_exist) ||
		   (bt_link_info->hid_exist && bt_link_info->a2dp_exist &&
		    bt_link_info->pan_exist)) {
		/* A2DP+PAN(OPP,FTP), HID+A2DP+PAN(OPP,FTP) */
		if (coex_sta->wl_noisy_level == 0 && wifi_cckdeadlock_ap)
			halbtc8822b1ant_table(btc, NM_EXCU, 14);
		if (bt_link_info->hid_exist)
			halbtc8822b1ant_table(btc, NM_EXCU, 1);
		else if (wifi_turbo)
			halbtc8822b1ant_table(btc, NM_EXCU, 19);
		else
			halbtc8822b1ant_table(btc, NM_EXCU, 4);

		if (wifi_busy)
			halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 13);
		else
			halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 14);
	} else if (bt_link_info->hid_exist && coex_sta->is_hid_rcu &&
		   coex_sta->voice_over_HOGP && bt_link_info->a2dp_exist) {
		/* RCU voice + A2DP */
		/* change coex table if slave latency support or not */
		if (wifi_busy && !bt_slave_latency) {
			if (coex_sta->wl_noisy_level == 0 &&
			    wifi_cckdeadlock_ap)
				halbtc8822b1ant_table(btc, NM_EXCU, 10);
			else if (coex_sta->wl_noisy_level == 0 &&
				 !wifi_cckdeadlock_ap)
				halbtc8822b1ant_table(btc, NM_EXCU, 9);
			else
				halbtc8822b1ant_table(btc, NM_EXCU, 3);
		} else if (wifi_busy && bt_slave_latency) {
			if (coex_sta->wl_noisy_level == 0 &&
			    wifi_cckdeadlock_ap)
				halbtc8822b1ant_table(btc, NM_EXCU, 13);
			else
				halbtc8822b1ant_table(btc, NM_EXCU, 3);
		} else { /* wifi idle  */
			halbtc8822b1ant_table(btc, NM_EXCU, 7);
		}

		if (coex_sta->connect_ap_period_cnt > 0 || !wifi_busy)
			halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 10);
		else
			halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 13);
	} else if (bt_link_info->hid_exist && coex_sta->is_hid_rcu &&
		   bt_link_info->a2dp_exist) {
		/* RCU + A2DP */
		/* change coex table if slave latency support or not */
		if (wifi_busy && !bt_slave_latency) {
			if (coex_sta->wl_noisy_level == 0 &&
			    wifi_cckdeadlock_ap)
				halbtc8822b1ant_table(btc, NM_EXCU, 10);
			else if (coex_sta->wl_noisy_level == 0 &&
				 !wifi_cckdeadlock_ap)
				halbtc8822b1ant_table(btc, NM_EXCU, 9);
			else
				halbtc8822b1ant_table(btc, NM_EXCU, 3);
		} else if (wifi_busy && bt_slave_latency) {
			if (coex_sta->wl_noisy_level == 0 &&
			    wifi_cckdeadlock_ap)
				halbtc8822b1ant_table(btc, NM_EXCU, 13);
			else
				halbtc8822b1ant_table(btc, NM_EXCU, 3);
		} else { /* wifi idle  */
			halbtc8822b1ant_table(btc, NM_EXCU, 7);
		}

		if (coex_sta->connect_ap_period_cnt > 0 || !wifi_busy)
			halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 26);
		else
			halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 7);
	} else if (bt_link_info->hid_exist &&
		   bt_link_info->a2dp_exist) { /* HID + A2DP */
		if (coex_sta->wl_noisy_level == 0 && wifi_cckdeadlock_ap) {
			halbtc8822b1ant_table(btc, NM_EXCU, 14);
		} else if (wifi_bw == 0) { /* if 11bg mode */
			if (coex_sta->hid_busy_num < 2)
				/* for 2/18-4/18 HID lag @ Asus */
				halbtc8822b1ant_table(btc, NM_EXCU, 1);
			else
				halbtc8822b1ant_table(btc, NM_EXCU, 21);
		} else {
			halbtc8822b1ant_table(btc, NM_EXCU, 1);
		}

		if (coex_sta->connect_ap_period_cnt > 0 || !wifi_busy)
			halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 26);
		else
			halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 7);
		/* PAN(OPP,FTP), HID+PAN(OPP,FTP) */
	} else if ((bt_link_info->pan_only) ||
		   (bt_link_info->hid_exist && bt_link_info->pan_exist)) {
		if (coex_sta->wl_noisy_level == 0 && wifi_cckdeadlock_ap) {
			if (bt_link_info->hid_exist &&
			    coex_sta->hid_busy_num < 2)
				halbtc8822b1ant_table(btc, NM_EXCU, 14);
			else
				halbtc8822b1ant_table(btc, NM_EXCU, 13);
		} else if (bt_link_info->hid_exist) {
			if (coex_sta->hid_busy_num < 2)
				halbtc8822b1ant_table(btc, NM_EXCU, 1);
			else
				halbtc8822b1ant_table(btc, NM_EXCU, 3);
		} else if (wifi_turbo) {
			halbtc8822b1ant_table(btc, NM_EXCU, 19);
		} else {
			halbtc8822b1ant_table(btc, NM_EXCU, 4);
		}

		if (!wifi_busy)
			halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 4);
		else
			halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 3);
	} else {
		/* BT no-profile busy (0x9) */
		halbtc8822b1ant_table(btc, NM_EXCU, 4);
		halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 33);
	}
}

static void halbtc8822b1ant_action_bt_mr(struct btc_coexist *btc)
{
	struct wifi_link_info_8822b_1ant *wifi_link_info_ext =
					 &btc->wifi_link_info_8822b_1ant;

	if (!wifi_link_info_ext->is_all_under_5g) {
		halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, NM_EXCU,
					     BT_8822B_1ANT_PHASE_2G_WL);

		halbtc8822b1ant_table(btc, NM_EXCU, 8);
		halbtc8822b1ant_tdma(btc, NM_EXCU, FALSE, 8);

		/* Enter MIMO Power Save, 0:enable */
		halbtc8822b1ant_mimo_ps(btc, NM_EXCU, 0);
	} else {
		halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, NM_EXCU,
					     BT_8822B_1ANT_PHASE_5G);

		halbtc8822b1ant_table(btc, NM_EXCU, 0);
		halbtc8822b1ant_tdma(btc, NM_EXCU, FALSE, 8);

		/* No MIMO Power Save, 3:disable */
		halbtc8822b1ant_mimo_ps(btc, NM_EXCU, 3);
	}
}

static void halbtc8822b1ant_action_rf4ce(struct btc_coexist *btc)
{
	halbtc8822b1ant_set_wl_tx_power(btc, NM_EXCU, 0xd8);
	halbtc8822b1ant_set_bt_tx_power(btc, NM_EXCU, 0);
	halbtc8822b1ant_set_wl_rx_gain(btc, NM_EXCU, FALSE);
	halbtc8822b1ant_set_bt_rx_gain(btc, NM_EXCU, FALSE);

	halbtc8822b1ant_table(btc, NM_EXCU, 1);
	halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 50);
}

static void halbtc8822b1ant_action_wifi_under5g(struct btc_coexist *btc)
{
	halbtc8822b1ant_set_wl_tx_power(btc, NM_EXCU, 0xd8);
	halbtc8822b1ant_set_bt_tx_power(btc, NM_EXCU, 0);
	halbtc8822b1ant_set_wl_rx_gain(btc, NM_EXCU, FALSE);
	halbtc8822b1ant_set_bt_rx_gain(btc, NM_EXCU, FALSE);

	halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, FC_EXCU,
				     BT_8822B_1ANT_PHASE_5G);

	halbtc8822b1ant_table(btc, NM_EXCU, 0);
	halbtc8822b1ant_tdma(btc, NM_EXCU, FALSE, 8);
}

#if 0
static void halbtc8822b1ant_action_wifi_freerun(struct btc_coexist *btc)
{
#if 0
	struct  btc_bt_link_info *bt_link_info = &btc->bt_link_info;

	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE, "[BTCoex], under 5g start\n");
	BTC_TRACE(trace_buf);

	 /* for SUMA fine tune  */
	if (bt_link_info->a2dp_only) {
		halbtc8822b1ant_set_wl_tx_power(btc, NM_EXCU, 0xc8);
		halbtc8822b1ant_set_bt_tx_power(btc, NM_EXCU, 2);
		halbtc8822b1ant_set_wl_rx_gain(btc, NM_EXCU, TRUE);
		halbtc8822b1ant_set_bt_rx_gain(btc, NM_EXCU, TRUE);

		halbtc8822b1ant_table(btc, NM_EXCU, 8);
		halbtc8822b1ant_tdma(btc, NM_EXCU, FALSE, 8);

		halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, FC_EXCU,
					     BT_8822B_1ANT_PHASE_2G_BT);
	} else if ((bt_link_info->a2dp_exist) && (bt_link_info->pan_exist)) {
		halbtc8822b1ant_set_wl_tx_power(btc, NM_EXCU, 0xd8);
		halbtc8822b1ant_set_bt_tx_power(btc, NM_EXCU, 10);
		halbtc8822b1ant_set_wl_rx_gain(btc, NM_EXCU, TRUE);
		halbtc8822b1ant_set_bt_rx_gain(btc, NM_EXCU, TRUE);

		halbtc8822b1ant_table(btc, NM_EXCU, 8);
		halbtc8822b1ant_tdma(btc, NM_EXCU, FALSE, 8);

		halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, FC_EXCU,
					     BT_8822B_1ANT_PHASE_2G_WL);
	}
#endif
}
#endif

static void halbtc8822b1ant_action_wifi_only(struct btc_coexist *btc)
{
	boolean rf4ce_enabled = FALSE, wifi_connected = FALSE;

	halbtc8822b1ant_set_wl_tx_power(btc, NM_EXCU, 0xd8);
	halbtc8822b1ant_set_bt_tx_power(btc, NM_EXCU, 0);
	halbtc8822b1ant_set_wl_rx_gain(btc, NM_EXCU, FALSE);
	halbtc8822b1ant_set_bt_rx_gain(btc, NM_EXCU, FALSE);

	btc->btc_get(btc, BTC_GET_BL_RF4CE_CONNECTED, &rf4ce_enabled);
	btc->btc_get(btc, BTC_GET_BL_WIFI_CONNECTED, &wifi_connected);

	if ((rf4ce_enabled) && (wifi_connected)) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], ********** (wlan only -- rf4ce enable ) **********\n");
		BTC_TRACE(trace_buf);
		halbtc8822b1ant_action_rf4ce(btc);
	} else {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], ********** (wlan only -- under 2g ) **********\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_table(btc, FC_EXCU, 0);
		halbtc8822b1ant_tdma(btc, FC_EXCU, FALSE, 8);
	}
}

static void
halbtc8822b1ant_action_wifi_native_lps(struct btc_coexist *btc)
{
	halbtc8822b1ant_set_wl_tx_power(btc, NM_EXCU, 0xd8);
	halbtc8822b1ant_set_bt_tx_power(btc, NM_EXCU, 0);
	halbtc8822b1ant_set_wl_rx_gain(btc, NM_EXCU, FALSE);
	halbtc8822b1ant_set_bt_rx_gain(btc, NM_EXCU, FALSE);

	halbtc8822b1ant_table(btc, NM_EXCU, 5);
	halbtc8822b1ant_tdma(btc, NM_EXCU, FALSE, 8);
}

static void halbtc8822b1ant_action_wifi_linkscan(struct btc_coexist *btc)
{
	struct btc_bt_link_info *bt_link_info = &btc->bt_link_info;

	halbtc8822b1ant_set_wl_tx_power(btc, NM_EXCU, 0xd8);
	halbtc8822b1ant_set_bt_tx_power(btc, NM_EXCU, 0);
	halbtc8822b1ant_set_wl_rx_gain(btc, NM_EXCU, FALSE);
	halbtc8822b1ant_set_bt_rx_gain(btc, NM_EXCU, FALSE);

	if (bt_link_info->pan_exist) {
		halbtc8822b1ant_table(btc, NM_EXCU, 1);
		halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 22);
	} else if (bt_link_info->a2dp_exist) {
		halbtc8822b1ant_table(btc, NM_EXCU, 1);
		halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 27);
	} else {
		halbtc8822b1ant_table(btc, NM_EXCU, 1);
		halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 21);
	}
}

/* wifi connected input point:
 * to set different ps and tdma case (+bt different case)
 */

/*wifi not connected + bt action*/
static void
halbtc8822b1ant_action_wifi_not_connected(struct btc_coexist *btc)
{
	halbtc8822b1ant_set_wl_tx_power(btc, NM_EXCU, 0xd8);
	halbtc8822b1ant_set_bt_tx_power(btc, NM_EXCU, 0);
	halbtc8822b1ant_set_wl_rx_gain(btc, NM_EXCU, FALSE);
	halbtc8822b1ant_set_bt_rx_gain(btc, NM_EXCU, FALSE);

	/* tdma and coex table */
	halbtc8822b1ant_table(btc, NM_EXCU, 0);
	halbtc8822b1ant_tdma(btc, FC_EXCU, FALSE, 8);
}

static void halbtc8822b1ant_action_wifi_connected(struct btc_coexist *btc)
{
	struct btc_bt_link_info *bt_link_info = &btc->bt_link_info;
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;
	boolean rf4ce_enabled = FALSE;
	u32 wifi_bw;
	u8 iot_peer = BTC_IOT_PEER_UNKNOWN, algorithm;

	btc->btc_get(btc, BTC_GET_U4_WIFI_BW, &wifi_bw);
	btc->btc_get(btc, BTC_GET_BL_RF4CE_CONNECTED, &rf4ce_enabled);
	btc->btc_get(btc, BTC_GET_U1_IOT_PEER, &iot_peer);

	if (bt_link_info->bt_link_exist && iot_peer == BTC_IOT_PEER_CISCO) {
		if (wifi_bw == BTC_WIFI_BW_HT40)
			halbtc8822b1ant_limited_rx(btc, NM_EXCU, FALSE, TRUE,
						   0x10);
		else
			halbtc8822b1ant_limited_rx(btc, NM_EXCU, FALSE, TRUE,
						   0x8);
	}

	algorithm = halbtc8822b1ant_action_algorithm(btc);

	switch (algorithm) {
	case BT_8822B_1ANT_COEX_SCO:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Action algorithm = SCO.\n");
		BTC_TRACE(trace_buf);
		break;
	case BT_8822B_1ANT_COEX_HID:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Action algorithm = HID.\n");
		BTC_TRACE(trace_buf);
		break;
	case BT_8822B_1ANT_COEX_A2DP:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Action algorithm = A2DP.\n");
		BTC_TRACE(trace_buf);
		break;
	case BT_8822B_1ANT_COEX_A2DP_PANHS:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Action algorithm = A2DP+PAN(HS).\n");
		BTC_TRACE(trace_buf);
		break;
	case BT_8822B_1ANT_COEX_PAN:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Action algorithm = PAN(EDR).\n");
		BTC_TRACE(trace_buf);
		break;
	case BT_8822B_1ANT_COEX_PANHS:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Action algorithm = HS mode.\n");
		BTC_TRACE(trace_buf);
		break;
	case BT_8822B_1ANT_COEX_PAN_A2DP:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Action algorithm = PAN+A2DP.\n");
		BTC_TRACE(trace_buf);
		break;
	case BT_8822B_1ANT_COEX_PAN_HID:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Action algorithm = PAN(EDR)+HID.\n");
		BTC_TRACE(trace_buf);
		break;
	case BT_8822B_1ANT_COEX_HID_A2DP_PAN:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Action algorithm = HID+A2DP+PAN.\n");
		BTC_TRACE(trace_buf);
		break;
	case BT_8822B_1ANT_COEX_HID_A2DP:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Action algorithm = HID+A2DP.\n");
		BTC_TRACE(trace_buf);
		break;
	default:
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Action algorithm = coexist All Off!!\n");
		BTC_TRACE(trace_buf);
		break;
	}

	coex_dm->cur_algorithm = algorithm;

	if (coex_dm->bt_status == BT_8822B_1ANT_BSTATUS_ACL_BUSY ||
	    coex_dm->bt_status == BT_8822B_1ANT_BSTATUS_ACL_SCO_BUSY) {
		if (bt_link_info->hid_only) /* HID only */
			halbtc8822b1ant_action_bt_sco_hid_busy(btc);
		else
			halbtc8822b1ant_action_bt_acl_busy(btc);

	} else if (coex_dm->bt_status == BT_8822B_1ANT_BSTATUS_SCO_BUSY) {
		halbtc8822b1ant_action_bt_sco_hid_busy(btc);
	} else if (rf4ce_enabled) {
		halbtc8822b1ant_action_rf4ce(btc);
	} else {
		halbtc8822b1ant_action_bt_idle(btc);
	}
}

static void
halbtc8822b1ant_action_wifi_multiport25g(struct btc_coexist *btc)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;

	halbtc8822b1ant_set_wl_tx_power(btc, NM_EXCU, 0xd8);
	halbtc8822b1ant_set_bt_tx_power(btc, NM_EXCU, 0);
	halbtc8822b1ant_set_wl_rx_gain(btc, NM_EXCU, FALSE);
	halbtc8822b1ant_set_bt_rx_gain(btc, NM_EXCU, FALSE);

	halbtc8822b1ant_write_scbd(btc, BT_8822B_1ANT_SCBD_CQDDR, TRUE);

	halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, NM_EXCU,
				     BT_8822B_1ANT_PHASE_MCC);

	if (coex_sta->is_setup_link || coex_sta->bt_relink_downcount != 0) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], wifi_multiport25g(), BT Relink!!\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_table(btc, NM_EXCU, 26);
		halbtc8822b1ant_tdma(btc, NM_EXCU, FALSE, 8);
	} else if (coex_sta->c2h_bt_inquiry_page) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], wifi_multiport25g(), BT Inq-Page!!\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_table(btc, NM_EXCU, 25);
		halbtc8822b1ant_tdma(btc, NM_EXCU, FALSE, 8);
	} else {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], wifi_multiport25g(), BT idle or busy!!\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_table(btc, NM_EXCU, 25);
		halbtc8822b1ant_tdma(btc, NM_EXCU, FALSE, 8);
	}
}

static void
halbtc8822b1ant_action_wifi_multiport2g(struct btc_coexist *btc)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct btc_bt_link_info *bt_link_info = &btc->bt_link_info;

	halbtc8822b1ant_set_wl_tx_power(btc, NM_EXCU, 0xd8);
	halbtc8822b1ant_set_bt_tx_power(btc, NM_EXCU, 0);
	halbtc8822b1ant_set_wl_rx_gain(btc, NM_EXCU, FALSE);
	halbtc8822b1ant_set_bt_rx_gain(btc, NM_EXCU, FALSE);

	halbtc8822b1ant_write_scbd(btc, BT_8822B_1ANT_SCBD_CQDDR, TRUE);

	if (coex_sta->is_setup_link || coex_sta->bt_relink_downcount != 0) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], wifi_multiport2g, BT Relink!!\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, NM_EXCU,
					     BT_8822B_1ANT_PHASE_2G_WL);

		halbtc8822b1ant_table(btc, NM_EXCU, 8);
		halbtc8822b1ant_tdma(btc, NM_EXCU, FALSE, 8);
	} else if (coex_sta->c2h_bt_inquiry_page) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], wifi_multiport2g, BT Inq-Page!!\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, NM_EXCU,
					     BT_8822B_1ANT_PHASE_2G_WL);

		halbtc8822b1ant_table(btc, NM_EXCU, 8);
		halbtc8822b1ant_tdma(btc, NM_EXCU, FALSE, 8);
	} else if (coex_sta->num_of_profile == 0) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], wifi_multiport2g, BT idle!!\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, NM_EXCU,
					     BT_8822B_1ANT_PHASE_2G);

		halbtc8822b1ant_table(btc, NM_EXCU, 0);
		halbtc8822b1ant_tdma(btc, NM_EXCU, FALSE, 8);
	} else if (coex_sta->is_wifi_linkscan_process) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], wifi_multiport2g, WL scan!!\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, NM_EXCU,
					     BT_8822B_1ANT_PHASE_2G);
		halbtc8822b1ant_action_wifi_linkscan(btc);
	} else {
		halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, NM_EXCU,
					     BT_8822B_1ANT_PHASE_2G);

		if (!coex_sta->is_bt_multi_link &&
		    (bt_link_info->sco_exist || bt_link_info->hid_exist ||
		     coex_sta->is_hid_rcu)) {
			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], wifi_multiport2g, 2G multi-port + BT HID/HFP/RCU!!\n");
			BTC_TRACE(trace_buf);

			halbtc8822b1ant_table(btc, NM_EXCU, 8);
			halbtc8822b1ant_tdma(btc, NM_EXCU, FALSE, 8);
		} else {
			switch (btc->wifi_link_info.link_mode) {
#if 0
			case BTC_LINK_2G_SCC_GO_STA:
				BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
					    "[BTCoex], wifi_multiport2g, 2G_SCC_GO_STA + BT busy!!\n");
				BTC_TRACE(trace_buf);

				halbtc8822b1ant_table(btc, NM_EXCU, 4);
				halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 201);
				break;
			case BTC_LINK_ONLY_GO:
				BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
					    "[BTCoex], wifi_singleport2g, Only_P2PGO with client-join + BT busy!!\n");
				BTC_TRACE(trace_buf);
				halbtc8822b1ant_table(btc, NM_EXCU, 4);
				halbtc8822b1ant_tdma(btc, NM_EXCU, TRUE, 202);
				break;
#endif
			default:
				BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
					    "[BTCoex], wifi_multiport2g, Other multi-port + BT busy!!\n");
				BTC_TRACE(trace_buf);

				if (!bt_link_info->pan_exist)
					halbtc8822b1ant_table(btc, NM_EXCU, 8);
				else
					halbtc8822b1ant_table(btc, NM_EXCU, 5);

				halbtc8822b1ant_tdma(btc, NM_EXCU, FALSE, 8);
				break;
			}
		}
	}
}

static void halbtc8822b1ant_run_coex(struct btc_coexist *btc, u8 reason)
{
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct wifi_link_info_8822b_1ant *wifi_link_info_ext =
					 &btc->wifi_link_info_8822b_1ant;
	boolean wifi_connected = FALSE, wifi_32k = false;
	boolean scan = FALSE, link = FALSE, roam = FALSE, under_4way = FALSE;

	btc->btc_get(btc, BTC_GET_BL_WIFI_SCAN, &scan);
	btc->btc_get(btc, BTC_GET_BL_WIFI_LINK, &link);
	btc->btc_get(btc, BTC_GET_BL_WIFI_ROAM, &roam);
	btc->btc_get(btc, BTC_GET_BL_WIFI_4_WAY_PROGRESS, &under_4way);
	btc->btc_get(btc, BTC_GET_BL_WIFI_CONNECTED, &wifi_connected);
	btc->btc_get(btc, BTC_GET_BL_WIFI_LW_PWR_STATE, &wifi_32k);

	if (scan || link || roam || under_4way ||
	    reason == BT_8822B_1ANT_RSN_2GSCANSTART ||
	    reason == BT_8822B_1ANT_RSN_2GSWITCHBAND ||
	    reason == BT_8822B_1ANT_RSN_2GCONSTART ||
	    reason == BT_8822B_1ANT_RSN_2GSPECIALPKT) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], scan = %d, link = %d, roam = %d 4way = %d!!!\n",
			    scan, link, roam, under_4way);
		BTC_TRACE(trace_buf);
		coex_sta->is_wifi_linkscan_process = TRUE;
	} else {
		coex_sta->is_wifi_linkscan_process = FALSE;
	}

	/* update wifi_link_info_ext variable */
	halbtc8822b1ant_update_wifi_link_info(btc, reason);

	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
		    "[BTCoex], RunCoexistMechanism()===> reason = %d\n",
		    reason);
	BTC_TRACE(trace_buf);

	if (btc->manual_control) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], RunCoexistMechanism(), return for Manual CTRL <===\n");
		BTC_TRACE(trace_buf);
		return;
	}

	if (btc->stop_coex_dm) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], RunCoexistMechanism(), return for Stop Coex DM <===\n");
		BTC_TRACE(trace_buf);
		return;
	}

	if (coex_sta->under_ips) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], RunCoexistMechanism(), return for wifi is under IPS !!!\n");
		BTC_TRACE(trace_buf);
		return;
	}

	if (coex_sta->under_lps && wifi_32k) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], RunCoexistMechanism(), return for wifi is under LPS-32K !!!\n");
		BTC_TRACE(trace_buf);
		return;
	}

	if (!coex_sta->run_time_state) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], RunCoexistMechanism(), return for run_time_state = FALSE !!!\n");
		BTC_TRACE(trace_buf);
		return;
	}

	if (coex_sta->freeze_coexrun_by_btinfo && !coex_sta->is_setup_link) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], RunCoexistMechanism(), return for freeze_coexrun_by_btinfo\n");
		BTC_TRACE(trace_buf);
		return;
	}

	if (coex_sta->msft_mr_exist && wifi_connected) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], RunCoexistMechanism(), microsoft MR!!\n");
		BTC_TRACE(trace_buf);

		coex_sta->wl_coex_mode = BT_8822B_1ANT_WLINK_BTMR;
		halbtc8822b1ant_action_bt_mr(btc);
		return;
	}

	/* No MIMO Power Save, 3:disable */
	halbtc8822b1ant_mimo_ps(btc, NM_EXCU, 3);

	/* Pure-5G Coex Process */
	if (wifi_link_info_ext->is_all_under_5g) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], WiFi is under 5G!!!\n");
		BTC_TRACE(trace_buf);

		coex_sta->wl_coex_mode = BT_8822B_1ANT_WLINK_5G;
		halbtc8822b1ant_action_wifi_under5g(btc);
		return;
	}

	if (wifi_link_info_ext->is_mcc_25g) { /* not iclude scan action */

		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], WiFi is under mcc dual-band!!!\n");
		BTC_TRACE(trace_buf);

		coex_sta->wl_coex_mode = BT_8822B_1ANT_WLINK_25GMPORT;
		halbtc8822b1ant_action_wifi_multiport25g(btc);
		return;
	}

	/* if multi-port or P2PGO+Client_Join  */
	if (wifi_link_info_ext->num_of_active_port > 1 ||
	    (btc->wifi_link_info.link_mode == BTC_LINK_ONLY_GO &&
	     !btc->wifi_link_info.bhotspot &&
	     btc->wifi_link_info.bany_client_join_go)) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], WiFi is under scc-2g/mcc-2g/p2pGO-only!!!\n");
		BTC_TRACE(trace_buf);

		if (btc->wifi_link_info.link_mode ==
		    BTC_LINK_ONLY_GO)
			coex_sta->wl_coex_mode =
				BT_8822B_1ANT_WLINK_2GGO;
		else
			coex_sta->wl_coex_mode =
				BT_8822B_1ANT_WLINK_2GMPORT;
		halbtc8822b1ant_action_wifi_multiport2g(btc);
		return;
	}

	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
		    "[BTCoex], WiFi is single-port 2G!!!\n");
	BTC_TRACE(trace_buf);

	coex_sta->wl_coex_mode = BT_8822B_1ANT_WLINK_2G1PORT;

	halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, NM_EXCU,
				     BT_8822B_1ANT_PHASE_2G);

	halbtc8822b1ant_write_scbd(btc, BT_8822B_1ANT_SCBD_CQDDR, TRUE);

	if (coex_sta->bt_disabled) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], BT is disabled !!!\n");
		BTC_TRACE(trace_buf);
		halbtc8822b1ant_action_wifi_only(btc);
		return;
	}

	if (coex_sta->under_lps && !coex_sta->force_lps_ctrl &&
	    !coex_sta->acl_busy) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], RunCoexistMechanism(), wifi is under LPS !!!\n");
		BTC_TRACE(trace_buf);
		halbtc8822b1ant_action_wifi_native_lps(btc);
		return;
	}

	if (coex_sta->bt_whck_test) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], BT is under WHCK TEST!!!\n");
		BTC_TRACE(trace_buf);
		halbtc8822b1ant_action_bt_whql_test(btc);
		return;
	}

	if (coex_sta->is_setup_link || coex_sta->bt_relink_downcount != 0) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], BT is re-link !!!\n");
		BTC_TRACE(trace_buf);
		halbtc8822b1ant_action_bt_relink(btc);
		return;
	}

	if (coex_sta->c2h_bt_inquiry_page) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], BT is under inquiry/page !!\n");
		BTC_TRACE(trace_buf);
		halbtc8822b1ant_action_bt_inquiry(btc);
		return;
	}

	if (coex_dm->bt_status == BT_8822B_1ANT_BSTATUS_NCON_IDLE ||
	    coex_dm->bt_status == BT_8822B_1ANT_BSTATUS_CON_IDLE) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "############# [BTCoex],  BT Is idle\n");
		BTC_TRACE(trace_buf);
		halbtc8822b1ant_action_bt_idle(btc);
		return;
	}

	if (coex_sta->is_wifi_linkscan_process) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], wifi is under linkscan process!!\n");
		BTC_TRACE(trace_buf);
		halbtc8822b1ant_action_wifi_linkscan(btc);
		return;
	}

	if (wifi_connected) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], wifi is under connected!!\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_action_wifi_connected(btc);
	} else {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], wifi is under not-connected!!\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_action_wifi_not_connected(btc);
	}
}

static void halbtc8822b1ant_init_coex_dm(struct btc_coexist *btc)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;
	/* force to reset coex mechanism */

	halbtc8822b1ant_low_penalty_ra(btc, NM_EXCU, FALSE, 0);

	/* sw all off */
	coex_sta->pop_event_cnt = 0;
	coex_sta->cnt_remote_name_req = 0;
	coex_sta->cnt_reinit = 0;
	coex_sta->cnt_setup_link = 0;
	coex_sta->cnt_ign_wlan_act = 0;
	coex_sta->cnt_page = 0;
	coex_sta->cnt_role_switch = 0;
	coex_sta->switch_band_notify_to = BTC_NOT_SWITCH;

	coex_sta->pop_event_cnt = 0;
	coex_dm->setting_tdma = FALSE;

	halbtc8822b1ant_query_bt_info(btc);
}

static void halbtc8822b1ant_init_hw_config(struct btc_coexist *btc,
					   boolean back_up, boolean wifi_only)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct rfe_type_8822b_1ant *rfe_type = &btc->rfe_type_8822b_1ant;
	u8 u8tmp = 0, i = 0;
	u32 u32tmp1 = 0, u32tmp2 = 0, u32tmp3 = 0;

	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
		    "[BTCoex], 1Ant Init HW Config!!\n");
	BTC_TRACE(trace_buf);

	u32tmp3 = btc->btc_read_4byte(btc, 0xcbc);
	u32tmp1 = halbtc8822b1ant_read_indirect_reg(btc, 0x38);
	u32tmp2 = halbtc8822b1ant_read_indirect_reg(btc, 0x54);

	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
		    "[BTCoex], ********** (Before Init HW config) 0xcbc = 0x%x, 0x38= 0x%x, 0x54= 0x%x**********\n",
		    u32tmp3, u32tmp1, u32tmp2);
	BTC_TRACE(trace_buf);
#if 0
	coex_sta->bt_coex_supported_feature = 0;
	coex_sta->bt_coex_supported_version = 0;
	coex_sta->bt_ble_scan_type = 0;
	coex_sta->bt_ble_scan_para[0] = 0;
	coex_sta->bt_ble_scan_para[1] = 0;
	coex_sta->bt_ble_scan_para[2] = 0;
#endif
	coex_sta->bt_reg_vendor_ac = 0xffff;
	coex_sta->bt_reg_vendor_ae = 0xffff;

	coex_sta->isolation_btween_wb = BT_8822B_1ANT_DEFAULT_ISOLATION;
	coex_sta->gnt_error_cnt = 0;
	coex_sta->bt_relink_downcount = 0;
	coex_sta->is_set_ps_state_fail = FALSE;
	coex_sta->cnt_set_ps_state_fail = 0;

	for (i = 0; i <= 9; i++)
		coex_sta->bt_afh_map[i] = 0;

	/* Setup RF front end type */
	halbtc8822b1ant_set_rfe_type(btc);

	if (rfe_type->rfe_module_type == 2 || rfe_type->rfe_module_type == 4)
		halbtc8822b1ant_write_scbd(btc, BT_8822B_1ANT_SCBD_EXTFEM,
					   TRUE);
	else
		halbtc8822b1ant_write_scbd(btc, BT_8822B_1ANT_SCBD_EXTFEM,
					   FALSE);

	/* 0xf0[15:12] --> Chip Cut information */
	coex_sta->cut_version =
		(btc->btc_read_1byte(btc, 0xf1) & 0xf0) >> 4;

	/* enable TBTT nterrupt */
	btc->btc_write_1byte_bitmask(btc, 0x550, 0x8, 0x1);

	/* BT report packet sample rate	 */
	/* 0x790[5:0]=0x5 */
	u8tmp = btc->btc_read_1byte(btc, 0x790);
	u8tmp &= 0xc0;
	u8tmp |= 0x5;
	btc->btc_write_1byte(btc, 0x790, u8tmp);

	/* Enable BT counter statistics */
	btc->btc_write_1byte(btc, 0x778, 0x1);

	/* Enable PTA (3-wire function form BT side) */
	btc->btc_write_1byte_bitmask(btc, 0x40, 0x20, 0x1);
	btc->btc_write_1byte_bitmask(btc, 0x41, 0x02, 0x1);

	/* Enable PTA (tx/rx signal form WiFi side) */
	btc->btc_write_1byte_bitmask(btc, 0x4c6, 0x10, 0x1);
	/*GNT_BT=1 while select both */
	btc->btc_write_1byte_bitmask(btc, 0x763, 0x10, 0x1);

	halbtc8822b1ant_enable_gnt_to_gpio(btc, TRUE);

#if 0
	if (btc->btc_read_1byte(btc, 0x80) == 0xc6)
		halbtc8822b1ant_write_scbd(btc, BT_8822B_1ANT_SCBD_ONOFF,
					   TRUE);
#endif

	/* PTA parameter */
	halbtc8822b1ant_table(btc, FC_EXCU, 0);
	halbtc8822b1ant_tdma(btc, FC_EXCU, FALSE, 8);

	/* Antenna config */
	if (coex_sta->is_rf_state_off) {
		halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, FC_EXCU,
					     BT_8822B_1ANT_PHASE_WOFF);

		btc->stop_coex_dm = TRUE;

		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], **********  %s (RF Off)**********\n",
			    __func__);
		BTC_TRACE(trace_buf);
	} else if (wifi_only) {
		coex_sta->concurrent_rx_mode_on = FALSE;
		halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_WIFI, FC_EXCU,
					     BT_8822B_1ANT_PHASE_WONLY);
		btc->stop_coex_dm = TRUE;
	} else {
		coex_sta->concurrent_rx_mode_on = TRUE;

		halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, FC_EXCU,
					     BT_8822B_1ANT_PHASE_INIT);
		btc->stop_coex_dm = FALSE;
	}

	u32tmp3 = btc->btc_read_4byte(btc, 0xcbc);
	u32tmp1 = halbtc8822b1ant_read_indirect_reg(btc, 0x38);
	u32tmp2 = halbtc8822b1ant_read_indirect_reg(btc, 0x54);

	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
		    "[BTCoex], ********** (After Init HW config) 0xcbc = 0x%x, 0x38= 0x%x, 0x54= 0x%x**********\n",
		    u32tmp3, u32tmp1, u32tmp2);
	BTC_TRACE(trace_buf);
}

#if 0
void halbtc8822b1ant_wifi_info_notify(struct btc_coexist *btc)
{
	u8			h2c_para[4] = {0};
	u8			opcode_ver = 0;
	u8			ap_num = 0;
	s32			wifi_rssi = 0;
	boolean			wifi_busy = FALSE;

	btc->btc_get(btc, BTC_GET_S4_WIFI_RSSI, &wifi_rssi);
	btc->btc_get(btc, BTC_GET_U1_AP_NUM, &ap_num);
	btc->btc_get(btc, BTC_GET_BL_WIFI_BUSY, &wifi_busy);

	h2c_para[0] = ap_num;					/* AP number */
	h2c_para[1] = (u8)wifi_busy;		/* Busy */
	h2c_para[2] = (u8)wifi_rssi;			/* RSSI */

	btc->btc_coex_h2c_process(btc, COL_OP_WIFI_INFO_NOTIFY,
					opcode_ver, &h2c_para[0], 3);
}

void halbtc8822b1ant_set_manual(struct btc_coexist *btc,
				boolean manual)
{
	u8			h2c_para[4] = {0};
	u8			opcode_ver = 0;
	u8			set_type = 0;

	if (manual)
		set_type = 1;
	else
		set_type = 0;

	h2c_para[0] = set_type;				/* set_type */

	btc->btc_coex_h2c_process(btc, COL_OP_SET_CONTROL,
					opcode_ver,
					&h2c_para[0], 1);
}

/* ************************************************************
 * work around function start with wa_halbtc8822b1ant_
 * ************************************************************
 * ************************************************************
 * extern function start with ex_halbtc8822b1ant_
 * *************************************************************/

void ex_halbtc8822b1ant_power_on_setting(struct btc_coexist *btc)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	coex_sta->is_rf_state_off = FALSE;
}

void ex_halbtc8822b1ant_pre_load_firmware(struct btc_coexist *btc)
{
}

void ex_halbtc8822b1ant_init_hw_config(struct btc_coexist *btc,
				       boolean wifi_only)
{
}

static
void ex_halbtc8822b1ant_init_coex_dm(struct btc_coexist *btc)
{
}

void ex_halbtc8822b1ant_ips_notify(struct btc_coexist *btc, u8 type)
{
	u8			h2c_para[4] = {0};
	u8			opcode_ver = 0;
	u8			ips_notify = 0;

	if (type == BTC_IPS_ENTER) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], IPS ENTER notify\n");
		BTC_TRACE(trace_buf);
		ips_notify = 1;
	} else if (type == BTC_IPS_LEAVE) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], IPS LEAVE notify\n");
		BTC_TRACE(trace_buf);
	}

	h2c_para[0] = ips_notify;		/* IPS notify */
	h2c_para[1] = 0xff;			/* LPS notify */
	h2c_para[2] = 0xff;			/* RF state notify */
	h2c_para[3] = 0xff;			/* pnp notify */

	btc->btc_coex_h2c_process(btc,
					COL_OP_WIFI_POWER_STATE_NOTIFY,
					opcode_ver, &h2c_para[0], 4);
}

void ex_halbtc8822b1ant_lps_notify(struct btc_coexist *btc, u8 type)
{
	u8			h2c_para[4] = {0};
	u8			opcode_ver = 0;
	u8			lps_notify = 0;

	if (type == BTC_LPS_ENABLE) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], LPS ENABLE notify\n");
		BTC_TRACE(trace_buf);
		lps_notify = 1;
	} else if (type == BTC_LPS_DISABLE) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], LPS DISABLE notify\n");
		BTC_TRACE(trace_buf);
	}

	h2c_para[0] = 0xff;			/* IPS notify */
	h2c_para[1] = lps_notify;		/* LPS notify */
	h2c_para[2] = 0xff;			/* RF state notify */
	h2c_para[3] = 0xff;			/* pnp notify */

	btc->btc_coex_h2c_process(btc,
					COL_OP_WIFI_POWER_STATE_NOTIFY,
					opcode_ver, &h2c_para[0], 4);
}

void ex_halbtc8822b1ant_scan_notify(struct btc_coexist *btc,
				    u8 type)
{
	u8			h2c_para[4] = {0};
	u8			opcode_ver = 0;
	u8			scan_start = 0;
	boolean			under_4way = FALSE;

	btc->btc_get(btc, BTC_GET_BL_WIFI_4_WAY_PROGRESS,
			   &under_4way);
	if (type == BTC_SCAN_START) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], SCAN START notify\n");
		BTC_TRACE(trace_buf);
		scan_start = 1;
	} else {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], SCAN FINISH notify\n");
		BTC_TRACE(trace_buf);
	}

	h2c_para[0] = scan_start;		/* scan notify */
	h2c_para[1] = 0xff;			/* connect notify */
	h2c_para[2] = 0xff;			/* specific packet notify */
	if (under_4way)
		h2c_para[3] = 1;		/* under 4way progress */
	else
		h2c_para[3] = 0;

	btc->btc_coex_h2c_process(btc, COL_OP_WIFI_PROGRESS_NOTIFY,
					opcode_ver, &h2c_para[0], 4);
}

void ex_halbtc8822b1ant_connect_notify(struct btc_coexist *btc,
				       u8 type)
{
	u8			h2c_para[4] = {0};
	u8			opcode_ver = 0;
	u8			connect_start = 0;
	boolean			under_4way = FALSE;

	btc->btc_get(btc, BTC_GET_BL_WIFI_4_WAY_PROGRESS,
			   &under_4way);
	if (type == BTC_ASSOCIATE_START) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], CONNECT START notify\n");
		BTC_TRACE(trace_buf);
		connect_start = 1;
	} else {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], CONNECT FINISH notify\n");
		BTC_TRACE(trace_buf);
	}

	h2c_para[0] = 0xff;			/* scan notify */
	h2c_para[1] = connect_start;		/* connect notify */
	h2c_para[2] = 0xff;			/* specific packet notify */
	if (under_4way)
		h2c_para[3] = 1;		/* under 4way progress */
	else
		h2c_para[3] = 0;

	btc->btc_coex_h2c_process(btc, COL_OP_WIFI_PROGRESS_NOTIFY,
					opcode_ver, &h2c_para[0], 4);
}

void ex_halbtc8822b1ant_media_status_notify(struct btc_coexist *btc,
					    u8 type)
{
	u32	wifi_bw;
	u8	wifi_central_chnl;
	u8	h2c_para[5] = {0};
	u8	opcode_ver = 0;
	u8	port = 0, connected = 0, freq = 0, bandwidth = 0, iot_peer = 0;
	boolean	wifi_under_5g = FALSE;

	if (type == BTC_MEDIA_CONNECT)
		connected = 1;

	btc->btc_get(btc, BTC_GET_U4_WIFI_BW, &wifi_bw);
	bandwidth = (u8)wifi_bw;
	btc->btc_get(btc, BTC_GET_BL_WIFI_UNDER_5G, &wifi_under_5g);
	if (wifi_under_5g)
		freq = 1;
	else
		freq = 0;
	btc->btc_get(btc, BTC_GET_U1_WIFI_CENTRAL_CHNL,
			   &wifi_central_chnl);
	btc->btc_get(btc, BTC_GET_U1_IOT_PEER, &iot_peer);

	/* port need to be implemented in the future (p2p port, ...) */
	h2c_para[0] = (connected << 4) | port;
	h2c_para[1] = (freq << 4) | bandwidth;
	h2c_para[2] = wifi_central_chnl;
	h2c_para[3] = iot_peer;
	btc->btc_coex_h2c_process(btc, COL_OP_WIFI_STATUS_NOTIFY,
					opcode_ver, &h2c_para[0], 4);
}

void ex_halbtc8822b1ant_specific_packet_notify(struct btc_coexist *btc,
					       u8 type)
{
	u8			h2c_para[4] = {0};
	u8			opcode_ver = 0;
	u8			connect_start = 0;
	boolean			under_4way = FALSE;

	btc->btc_get(btc, BTC_GET_BL_WIFI_4_WAY_PROGRESS,
			   &under_4way);

	h2c_para[0] = 0xff;			/* scan notify */
	h2c_para[1] = 0xff;			/* connect notify */
	h2c_para[2] = type;			/* specific packet notify */
	if (under_4way)
		h2c_para[3] = 1;		/* under 4way progress */
	else
		h2c_para[3] = 0;

	btc->btc_coex_h2c_process(btc, COL_OP_WIFI_PROGRESS_NOTIFY,
					opcode_ver, &h2c_para[0], 4);
}

void ex_halbtc8822b1ant_bt_info_notify(struct btc_coexist *btc,
				       u8 *tmp_buf, u8 length)
{
}

void ex_halbtc8822b1ant_rf_status_notify(struct btc_coexist *btc,
					 u8 type)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	u8			h2c_para[4] = {0};
	u8			opcode_ver = 0;
	u8			rfstate_notify = 0;

	if (type == BTC_RF_ON) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], RF is turned ON!!\n");
		BTC_TRACE(trace_buf);
		rfstate_notify = 1;
		coex_sta->is_rf_state_off = FALSE;
	} else if (type == BTC_RF_OFF) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], RF is turned OFF!!\n");
		BTC_TRACE(trace_buf);
		coex_sta->is_rf_state_off = TRUE;
	}

	h2c_para[0] = 0xff;			/* IPS notify */
	h2c_para[1] = 0xff;			/* LPS notify */
	h2c_para[2] = rfstate_notify;	/* RF state notify */
	h2c_para[3] = 0xff;			/* pnp notify */

	btc->btc_coex_h2c_process(btc,
					COL_OP_WIFI_POWER_STATE_NOTIFY,
					opcode_ver, &h2c_para[0], 4);
}

void ex_halbtc8822b1ant_halt_notify(struct btc_coexist *btc)
{
}

void ex_halbtc8822b1ant_pnp_notify(struct btc_coexist *btc,
				   u8 pnp_state)
{
	u8			h2c_para[4] = {0};
	u8			opcode_ver = 0;
	u8			pnp_notify = 0;

	if (pnp_state == BTC_WIFI_PNP_SLEEP) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Pnp notify to SLEEP\n");
		BTC_TRACE(trace_buf);
		pnp_notify = 1;
	} else if (pnp_state == BTC_WIFI_PNP_WAKE_UP) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Pnp notify to WAKE UP\n");
		BTC_TRACE(trace_buf);
	}

	h2c_para[0] = 0xff;			/* IPS notify */
	h2c_para[1] = 0xff;			/* LPS notify */
	h2c_para[2] = 0xff;			/* RF state notify */
	h2c_para[3] = pnp_notify;		/* pnp notify */

	btc->btc_coex_h2c_process(btc,
					COL_OP_WIFI_POWER_STATE_NOTIFY,
					opcode_ver, &h2c_para[0], 4);
}

void ex_halbtc8822b1ant_coex_dm_reset(struct btc_coexist *btc)
{
}

void ex_halbtc8822b1ant_periodical(struct btc_coexist *btc)
{
	halbtc8822b1ant_wifi_info_notify(btc);
}

void ex_halbtc8822b1ant_display_coex_info(struct btc_coexist *btc)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;
	struct rfe_type_8822b_1ant *rfe_type = &btc->rfe_type_8822b_2ant;
	struct  btc_board_info	 *board_info = &btc->board_info;
	struct  btc_bt_link_info *bt_link_info = &btc->bt_link_info;
	u8				*cli_buf = btc->cli_buf;
	u8				u8tmp[4], i, ps_tdma_case = 0;
	u16				u16tmp[4];
	u32				u32tmp[4];
	u32				fa_ofdm, fa_cck, cca_ofdm, cca_cck;
	u32		fw_ver = 0, bt_patch_ver = 0, bt_coex_ver = 0;
	static u8	pop_report_in_10s;
	u32		phyver = 0;
	boolean		lte_coex_on = FALSE;

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n ============[BT Coexist info]============");
	CL_PRINTF(cli_buf);

	if (btc->manual_control) {
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n ============[Under Manual Control]============");
		CL_PRINTF(cli_buf);
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n ==========================================");
		CL_PRINTF(cli_buf);
	}
	if (btc->stop_coex_dm) {
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n ============[Coex is STOPPED]============");
		CL_PRINTF(cli_buf);
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n ==========================================");
		CL_PRINTF(cli_buf);
	}

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = %d/ %d/ %s / %d",
		   "Ant PG Num/ Mech/ Pos/ RFE",
		   board_info->pg_ant_num, board_info->btdm_ant_num,
		   (board_info->btdm_ant_pos == BTC_ANTENNA_AT_MAIN_PORT
		    ? "Main" : "Aux"),
		   rfe_type->rfe_module_type);
	CL_PRINTF(cli_buf);

	btc->btc_get(btc, BTC_GET_U4_BT_PATCH_VER, &bt_patch_ver);
	btc->btc_get(btc, BTC_GET_U4_WIFI_FW_VER, &fw_ver);
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = %d_%x/ 0x%x/ 0x%x(%d)",
		   "CoexVer/ FwVer/ PatchVer",
		   glcoex_ver_date_8822b_1ant, glcoex_ver_8822b_1ant, fw_ver,
		   bt_patch_ver, bt_patch_ver);
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %02x %02x %02x ",
		   "Wifi channel informed to BT",
		   coex_dm->wifi_chnl_info[0], coex_dm->wifi_chnl_info[1],
		   coex_dm->wifi_chnl_info[2]);
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %s/ %s/ %s/ %s/ %d",
		   "HiPr/ Locking/ warn/ Locked/ Noisy",
		   (coex_sta->wifi_high_pri_task1 ? "Yes" : "No"),
		   (coex_sta->cck_lock ? "Yes" : "No"),
		   (coex_sta->cck_lock_warn ? "Yes" : "No"),
		   (coex_sta->cck_lock_ever ? "Yes" : "No"),
		   coex_sta->wl_noisy_level);
	CL_PRINTF(cli_buf);

	/* wifi status */
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s",
		   "============[Wifi Status]============");
	CL_PRINTF(cli_buf);
	btc->btc_disp_dbg_msg(btc, BTC_DBG_DISP_WIFI_STATUS);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s",
		   "============[BT Status]============");
	CL_PRINTF(cli_buf);

	pop_report_in_10s++;
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = [%s/ %d/ %d/ %d] ",
		   "BT [status/ rssi/ retryCnt/ popCnt]",
		   ((coex_sta->bt_disabled) ? ("disabled") :
		    ((coex_sta->c2h_bt_inquiry_page) ? ("inquiry/page scan")
			   : ((BT_8822B_1ANT_BSTATUS_NCON_IDLE ==
			       coex_dm->bt_status) ? "non-connected idle" :
		((coex_dm->bt_status == BT_8822B_1ANT_BSTATUS_CON_IDLE)
				       ? "connected-idle" : "busy")))),
		   coex_sta->bt_rssi - 100, coex_sta->bt_retry_cnt,
		   coex_sta->pop_event_cnt);
	CL_PRINTF(cli_buf);

	if (pop_report_in_10s >= 5) {
		coex_sta->pop_event_cnt = 0;
		pop_report_in_10s = 0;
	}

	if (coex_sta->num_of_profile != 0)
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n %-35s = %s%s%s%s%s",
			   "Profiles",
			   ((bt_link_info->a2dp_exist) ? "A2DP," : ""),
			   ((bt_link_info->sco_exist) ?  "SCO," : ""),
			   ((bt_link_info->hid_exist) ?
			    ((coex_sta->hid_busy_num >= 2) ? "HID(4/18)," :
			      "HID(2/18),") : ""),
			   ((bt_link_info->pan_exist) ?  "PAN," : ""),
			   ((coex_sta->voice_over_HOGP) ? "Voice" : ""));
	else
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n %-35s = None", "Profiles");

	CL_PRINTF(cli_buf);

	if (bt_link_info->a2dp_exist) {
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %s/ %d/ %s",
			   "A2DP Rate/Bitpool/Auto_Slot",
			   ((coex_sta->is_A2DP_3M) ? "3M" : "No_3M"),
			   coex_sta->a2dp_bit_pool,
			   ((coex_sta->is_autoslot) ? "On" : "Off"));
		CL_PRINTF(cli_buf);
	}

	if (bt_link_info->hid_exist) {
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d",
			   "HID PairNum/Forbid_Slot",
			   coex_sta->hid_pair_cnt,
			   coex_sta->forbidden_slot);
		CL_PRINTF(cli_buf);
	}

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %s / %s/ 0x%x/ 0x%x",
		   "Role/IgnWlanAct/Feature/BLEScan",
		   ((bt_link_info->slave_role) ? "Slave" : "Master"),
		   ((coex_dm->cur_ignore_wlan_act) ? "Yes" : "No"),
		   coex_sta->bt_coex_supported_feature,
		   coex_sta->bt_ble_scan_type);
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d/ %d/ %d/ %d",
		   "ReInit/ReLink/IgnWlact/Page/NameReq",
		   coex_sta->cnt_reinit,
		   coex_sta->cnt_setup_link,
		   coex_sta->cnt_ign_wlan_act,
		   coex_sta->cnt_page,
		   coex_sta->cnt_remote_name_req);
		CL_PRINTF(cli_buf);

	halbtc8822b1ant_read_scbd(btc,	&u16tmp[0]);

	if (coex_sta->bt_reg_vendor_ae == 0xffff ||
	    coex_sta->bt_reg_vendor_ac == 0xffff)
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = x/ x/ %04x",
			   "0xae[4]/0xac[1:0]/Scoreboard", u16tmp[0]);
	else
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n %-35s = 0x%x/ 0x%x/ %04x",
			   "0xae[4]/0xac[1:0]/Scoreboard",
			 ((coex_sta->bt_reg_vendor_ae & BIT(4)) >> 4),
			   coex_sta->bt_reg_vendor_ac & 0x3, u16tmp[0]);
	CL_PRINTF(cli_buf);

	for (i = 0; i < BT_8822B_1ANT_INFO_SRC_MAX; i++) {
		if (coex_sta->bt_info_c2h_cnt[i]) {
			CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
				   "\r\n %-35s = %02x %02x %02x %02x %02x %02x %02x(%d)",
				   glbt_info_src_8822b_1ant[i],
				   coex_sta->bt_info_c2h[i][0],
				   coex_sta->bt_info_c2h[i][1],
				   coex_sta->bt_info_c2h[i][2],
				   coex_sta->bt_info_c2h[i][3],
				   coex_sta->bt_info_c2h[i][4],
				   coex_sta->bt_info_c2h[i][5],
				   coex_sta->bt_info_c2h[i][6],
				   coex_sta->bt_info_c2h_cnt[i]);
			CL_PRINTF(cli_buf);
		}
	}

	if (btc->manual_control)
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s",
			   "============[mechanisms] (before Manual)============");
	else
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s",
			   "============[mechanisms]============");
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d",
		   "SM[LowPenaltyRA]",
		   coex_dm->cur_low_penalty_ra);
	CL_PRINTF(cli_buf);

	ps_tdma_case = coex_dm->cur_ps_tdma;
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = %02x %02x %02x %02x %02x case-%d (%s,%s)",
		   "PS TDMA",
		   coex_dm->ps_tdma_para[0], coex_dm->ps_tdma_para[1],
		   coex_dm->ps_tdma_para[2], coex_dm->ps_tdma_para[3],
		   coex_dm->ps_tdma_para[4], ps_tdma_case,
		   (coex_dm->cur_ps_tdma_on ? "On" : "Off"),
		   (coex_dm->auto_tdma_adjust ? "Adj" : "Fix"));

	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d",
		   "WL/BT Coex Table Type",
		   coex_sta->coex_table_type);
	CL_PRINTF(cli_buf);

	u32tmp[0] = btc->btc_read_4byte(btc, 0x6c0);
	u32tmp[1] = btc->btc_read_4byte(btc, 0x6c4);
	u32tmp[2] = btc->btc_read_4byte(btc, 0x6c8);
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = 0x%x/ 0x%x/ 0x%x",
		   "0x6c0/0x6c4/0x6c8(coexTable)",
		   u32tmp[0], u32tmp[1], u32tmp[2]);
	CL_PRINTF(cli_buf);

	u8tmp[0] = btc->btc_read_1byte(btc, 0x778);
	u32tmp[0] = btc->btc_read_4byte(btc, 0x6cc);
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = 0x%x/ 0x%x/ 0x%x",
		   "0x778/0x6cc/IgnWlanAct",
		   u8tmp[0], u32tmp[0],  coex_dm->cur_ignore_wlan_act);
	CL_PRINTF(cli_buf);

	u32tmp[0] = halbtc8822b1ant_read_indirect_reg(btc, 0xa0);
	u32tmp[1] = halbtc8822b1ant_read_indirect_reg(btc, 0xa4);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x/ 0x%x",
		   "LTE Coex Table W_L/B_L",
		   u32tmp[0] & 0xffff, u32tmp[1] & 0xffff);
	CL_PRINTF(cli_buf);

	u32tmp[0] = halbtc8822b1ant_read_indirect_reg(btc, 0xa8);
	u32tmp[1] = halbtc8822b1ant_read_indirect_reg(btc, 0xac);
	u32tmp[2] = halbtc8822b1ant_read_indirect_reg(btc, 0xb0);
	u32tmp[3] = halbtc8822b1ant_read_indirect_reg(btc, 0xb4);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = 0x%x/ 0x%x/ 0x%x/ 0x%x",
		   "LTE Break Table W_L/B_L/L_W/L_B",
		   u32tmp[0] & 0xffff, u32tmp[1] & 0xffff,
		   u32tmp[2] & 0xffff, u32tmp[3] & 0xffff);
	CL_PRINTF(cli_buf);

	/* Hw setting		 */
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s",
		   "============[Hw setting]============");
	CL_PRINTF(cli_buf);
#if 0
	u32tmp[0] = btc->btc_read_4byte(btc, 0x430);
	u32tmp[1] = btc->btc_read_4byte(btc, 0x434);
	u16tmp[0] = btc->btc_read_2byte(btc, 0x42a);
	u8tmp[0] = btc->btc_read_1byte(btc, 0x456);
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x/0x%x/0x%x/0x%x",
		   "0x430/0x434/0x42a/0x456",
		   u32tmp[0], u32tmp[1], u16tmp[0], u8tmp[0]);
	CL_PRINTF(cli_buf);
#endif

	u32tmp[0] = halbtc8822b1ant_read_indirect_reg(btc, 0x38);
	u32tmp[1] = halbtc8822b1ant_read_indirect_reg(btc, 0x54);
	u8tmp[0] = btc->btc_read_1byte(btc, 0x73);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %s",
		   "LTE CoexOn/Path Ctrl Owner",
		   (int)((u32tmp[0] & BIT(7)) >> 7),
		   ((u8tmp[0] & BIT(2)) ? "WL" : "BT"));
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d/ %d/ %d",
		   "LTE 3Wire/OPMode/UART/UARTMode",
		   (int)((u32tmp[0] & BIT(6)) >> 6),
		   (int)((u32tmp[0] & (BIT(5) | BIT(4))) >> 4),
		   (int)((u32tmp[0] & BIT(3)) >> 3),
		   (int)(u32tmp[0] & (BIT(2) | BIT(1) | BIT(0))));
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d/ %s",
		   "GNT_WL_SWCtrl/GNT_BT_SWCtrl/Dbg",
		   (int)((u32tmp[0] & BIT(12)) >> 12),
		   (int)((u32tmp[0] & BIT(14)) >> 14),
		   ((u8tmp[0] & BIT(3)) ? "On" : "Off"));
	CL_PRINTF(cli_buf);

	u32tmp[0] = halbtc8822b1ant_read_indirect_reg(btc, 0x54);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d/ %d/ %d",
		   "GNT_WL/GNT_BT/LTE_Busy/UART_Busy",
		   (int)((u32tmp[0] & BIT(2)) >> 2),
		   (int)((u32tmp[0] & BIT(3)) >> 3),
		   (int)((u32tmp[0] & BIT(1)) >> 1), (int)(u32tmp[0] & BIT(0)));
	CL_PRINTF(cli_buf);

	u8tmp[0] = btc->btc_read_1byte(btc, 0x4c6);
	u8tmp[1] = btc->btc_read_1byte(btc, 0x40);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x/ 0x%x",
		   "0x4c6[4]/0x40[5] (WL/BT PTA)",
		   (int)((u8tmp[0] & BIT(4)) >> 4),
		   (int)((u8tmp[1] & BIT(5)) >> 5));
	CL_PRINTF(cli_buf);

	u32tmp[0] = btc->btc_read_4byte(btc, 0x550);
	u8tmp[0] = btc->btc_read_1byte(btc, 0x522);
	u8tmp[1] = btc->btc_read_1byte(btc, 0x953);
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x/ 0x%x/ %s",
		   "0x550(bcn ctrl)/0x522/4-RxAGC",
		   u32tmp[0], u8tmp[0], (u8tmp[1] & 0x2) ? "On" : "Off");
	CL_PRINTF(cli_buf);

	u32tmp[0] = btc->btc_read_4byte(btc, 0xda0);
	u32tmp[1] = btc->btc_read_4byte(btc, 0xda4);
	u32tmp[2] = btc->btc_read_4byte(btc, 0xda8);
	u32tmp[3] = btc->btc_read_4byte(btc, 0xcf0);

	u8tmp[0] = btc->btc_read_1byte(btc, 0xa5b);
	u8tmp[1] = btc->btc_read_1byte(btc, 0xa5c);

	fa_ofdm = ((u32tmp[0] & 0xffff0000) >> 16) + ((u32tmp[1] & 0xffff0000)
			>> 16) + (u32tmp[1] & 0xffff) + (u32tmp[2] & 0xffff) +
		   ((u32tmp[3] & 0xffff0000) >> 16) + (u32tmp[3] &
				   0xffff);
	fa_cck = (u8tmp[0] << 8) + u8tmp[1];

	u32tmp[1] = btc->btc_read_4byte(btc, 0xc50);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = 0x%x/ 0x%x/ 0x%x/ 0x%x",
		   "0xc50/OFDM-CCA/OFDM-FA/CCK-FA",
		   u32tmp[1] & 0xff, u32tmp[0] & 0xffff, fa_ofdm, fa_cck);
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d/ %d/ %d",
		   "CRC_OK CCK/11g/11n/11n-Agg",
		   coex_sta->crc_ok_cck, coex_sta->crc_ok_11g,
		   coex_sta->crc_ok_11n, coex_sta->crc_ok_11n_agg);
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d/ %d/ %d",
		   "CRC_Err CCK/11g/11n/11n-Agg",
		   coex_sta->crc_err_cck, coex_sta->crc_err_11g,
		   coex_sta->crc_err_11n, coex_sta->crc_err_11n_agg);
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d",
		   "0x770(high-pri rx/tx)",
		   coex_sta->high_priority_rx, coex_sta->high_priority_tx);
	CL_PRINTF(cli_buf);
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d",
		   "0x774(low-pri rx/tx)",
		   coex_sta->low_priority_rx, coex_sta->low_priority_tx);
	CL_PRINTF(cli_buf);
	btc->btc_disp_dbg_msg(btc, BTC_DBG_DISP_COEX_STATISTICS);
}

void ex_halbtc8822b1ant_dbg_control(struct btc_coexist *btc,
				    u8 op_code, u8 op_len, u8 *pdata)
{
	switch (op_code) {
	case BTC_DBG_SET_COEX_MANUAL_CTRL: {
		boolean		manual = (boolean)*pdata;

		halbtc8822b1ant_set_manual(btc, manual);
	}
	break;
	default:
		break;
	}
}

#else
void ex_halbtc8822b1ant_power_on_setting(struct btc_coexist *btc)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct btc_board_info *board_info = &btc->board_info;
	u8 u8tmp = 0x0;
	u16 u16tmp = 0x0;

	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
		    "[BTCoex], Execute %s !!\n", __func__);
	BTC_TRACE(trace_buf);

	btc->stop_coex_dm = TRUE;
	coex_sta->is_rf_state_off = FALSE;

	/* enable BB, REG_SYS_FUNC_EN such that we can write 0x948 correctly. */
	u16tmp = btc->btc_read_2byte(btc, 0x2);
	btc->btc_write_2byte(btc, 0x2, u16tmp | BIT(0) | BIT(1));

	/* Setup RF front end type */
	halbtc8822b1ant_set_rfe_type(btc);

	/* Set Antenna Path to BT side */
	halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, FC_EXCU,
				     BT_8822B_1ANT_PHASE_POWERON);

	/* set WLAN_ACT = 0 */
	/* btc->btc_write_1byte(btc, 0x76e, 0x4); */

	/* SD1 Chunchu red x issue */
	btc->btc_write_1byte(btc, 0xff1a, 0x0);

	halbtc8822b1ant_enable_gnt_to_gpio(btc, TRUE);

	/* S0 or S1 setting and Local register setting
	 * (By the setting fw can get ant number, S0/S1, ... info)
	 * Local setting bit define
	 *	BIT0: "0" for no antenna inverse; "1" for antenna inverse
	 *	BIT1: "0" for internal switch; "1" for external switch
	 *	BIT2: "0" for one antenna; "1" for two antenna
	 * NOTE: here default all internal switch and 1-antenna ==>
	 *       BIT1=0 and BIT2=0
	 */

	u8tmp = 0;
	board_info->btdm_ant_pos = BTC_ANTENNA_AT_MAIN_PORT;

	if (btc->chip_interface == BTC_INTF_PCI)
		btc->btc_write_local_reg_1byte(btc, 0x3e0, u8tmp);
	else if (btc->chip_interface == BTC_INTF_USB)
		btc->btc_write_local_reg_1byte(btc, 0xfe08, u8tmp);
	else if (btc->chip_interface == BTC_INTF_SDIO)
		btc->btc_write_local_reg_1byte(btc, 0x60, u8tmp);

	if (btc->dbg_mode) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], LTE coex Reg 0x38 (Power-On) = 0x%x\n",
			    halbtc8822b1ant_read_indirect_reg(btc, 0x38));
		BTC_TRACE(trace_buf);

		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], MACReg 0x70/ BBReg 0xcbc (Power-On) = 0x%x/ 0x%x\n",
			    btc->btc_read_4byte(btc, 0x70),
			    btc->btc_read_4byte(btc, 0xcbc));
		BTC_TRACE(trace_buf);
	}
}

void ex_halbtc8822b1ant_pre_load_firmware(struct btc_coexist *btc) {}

void ex_halbtc8822b1ant_init_hw_config(struct btc_coexist *btc,
				       boolean wifi_only)
{
	halbtc8822b1ant_init_hw_config(btc, TRUE, wifi_only);
}

void ex_halbtc8822b1ant_init_coex_dm(struct btc_coexist *btc)
{
	btc->stop_coex_dm = FALSE;
	btc->auto_report = TRUE;
	btc->dbg_mode = FALSE;

	halbtc8822b1ant_init_coex_dm(btc);
}

void ex_halbtc8822b1ant_display_simple_coex_info(struct btc_coexist *btc)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;
	struct rfe_type_8822b_1ant *rfe_type = &btc->rfe_type_8822b_1ant;
	struct btc_board_info *board_info = &btc->board_info;
	struct btc_bt_link_info *bt_link_info = &btc->bt_link_info;

	u8 *cli_buf = btc->cli_buf;
	u32 bt_patch_ver = 0, bt_coex_ver = 0;
	static u8 cnt;
	u8 * const p = &coex_sta->bt_afh_map[0];

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n _____[BT Coexist info]____");
	CL_PRINTF(cli_buf);

	if (btc->manual_control) {
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n __[Under Manual Control]_");
		CL_PRINTF(cli_buf);
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n _________________________");
		CL_PRINTF(cli_buf);
	}

	if (btc->stop_coex_dm) {
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n ____[Coex is STOPPED]____");
		CL_PRINTF(cli_buf);
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n _________________________");
		CL_PRINTF(cli_buf);
	}

	if (!coex_sta->bt_disabled &&
	    (coex_sta->bt_coex_supported_version == 0 ||
	    coex_sta->bt_coex_supported_version == 0xffff) &&
	    cnt == 0) {
		btc->btc_get(btc, BTC_GET_U4_SUPPORTED_FEATURE,
			     &coex_sta->bt_coex_supported_feature);

		btc->btc_get(btc, BTC_GET_U4_SUPPORTED_VERSION,
			     &coex_sta->bt_coex_supported_version);

		coex_sta->bt_reg_vendor_ac = (u16)(btc->btc_get_bt_reg(btc, 3,
								       0xac) &
						   0xffff);

		coex_sta->bt_reg_vendor_ae = (u16)(btc->btc_get_bt_reg(btc, 3,
								       0xae) &
						   0xffff);

		btc->btc_get(btc, BTC_GET_U4_BT_PATCH_VER, &bt_patch_ver);
		btc->bt_info.bt_get_fw_ver = bt_patch_ver;

		if (coex_sta->num_of_profile > 0)
			btc->btc_get_bt_afh_map_from_bt(btc, 0, p);

		if (++cnt >= 3)
			cnt = 0;
	}

	/* BT coex. info. */
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = %d/ %d/ %s / %d",
		   "Ant PG Num/ Mech/ Pos/ RFE", board_info->pg_ant_num,
		   board_info->btdm_ant_num,
		   (board_info->btdm_ant_pos ==
		    BTC_ANTENNA_AT_MAIN_PORT ? "Main" : "Aux"),
		   rfe_type->rfe_module_type);
	CL_PRINTF(cli_buf);

	bt_coex_ver = ((coex_sta->bt_coex_supported_version & 0xff00) >> 8);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = %d_%02x/ 0x%02x/ 0x%02x (%s)",
		   "CoexVer WL/  BT_Desired/ BT_Report",
		   glcoex_ver_date_8822b_1ant, glcoex_ver_8822b_1ant,
		   glcoex_ver_btdesired_8822b_1ant, bt_coex_ver,
		   (bt_coex_ver == 0xff ?
			 "Unknown" :
			 (coex_sta->bt_disabled ?
			  "BT-disable" :
			  (bt_coex_ver >= glcoex_ver_btdesired_8822b_1ant ?
				   "Match" :
				   "Mis-Match"))));
	CL_PRINTF(cli_buf);

	/* BT Status */
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %s", "BT status",
		   ((coex_sta->bt_disabled) ?
			 ("disabled") :
			 ((coex_sta->c2h_bt_inquiry_page) ?
			  ("inquiry/page") :
			  ((BT_8822B_1ANT_BSTATUS_NCON_IDLE ==
			    coex_dm->bt_status) ?
				   "non-connected idle" :
				   ((coex_dm->bt_status ==
				     BT_8822B_1ANT_BSTATUS_CON_IDLE) ?
					    "connected-idle" :
					    "busy")))));
	CL_PRINTF(cli_buf);

	/* HW Settings */
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d",
		   "0x770(Hi-pri rx/tx)", coex_sta->high_priority_rx,
		   coex_sta->high_priority_tx);
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d %s",
		   "0x774(Lo-pri rx/tx)", coex_sta->low_priority_rx,
		   coex_sta->low_priority_tx,
		   (bt_link_info->slave_role ?
			    "(Slave!!)" :
			    (coex_sta->is_tdma_btautoslot_hang ?
				     "(auto-slot hang!!)" :
				     "")));
	CL_PRINTF(cli_buf);
}

void ex_halbtc8822b1ant_display_coex_info(struct btc_coexist *btc)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;
	struct rfe_type_8822b_1ant *rfe_type = &btc->rfe_type_8822b_1ant;
	struct btc_board_info *board_info = &btc->board_info;
	struct btc_bt_link_info *bt_link_info = &btc->bt_link_info;

	u8 *cli_buf = btc->cli_buf;
	u8 u8tmp[4], i, ps_tdma_case = 0;
	u16 u16tmp[4];
	u32 u32tmp[4];
	u32 fa_ofdm, fa_cck, cca_ofdm, cca_cck;
	u32 fw_ver = 0, bt_patch_ver = 0, bt_coex_ver = 0;
	static u8 pop_report_in_10s;
	u32 phyver = 0;
	boolean lte_coex_on = FALSE;
	static u8 cnt;
	u8 * const p = &coex_sta->bt_afh_map[0];

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n ============[BT Coexist info 8822B]============");
	CL_PRINTF(cli_buf);

	if (btc->manual_control) {
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n ============[Under Manual Control]============");
		CL_PRINTF(cli_buf);
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n ==========================================");
		CL_PRINTF(cli_buf);
	}

	if (btc->stop_coex_dm) {
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n ============[Coex is STOPPED]============");
		CL_PRINTF(cli_buf);
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n ==========================================");
		CL_PRINTF(cli_buf);
	}

	if (!coex_sta->bt_disabled &&
	    (coex_sta->bt_coex_supported_version == 0 ||
	    coex_sta->bt_coex_supported_version == 0xffff) &&
	    cnt == 0) {
		btc->btc_get(btc, BTC_GET_U4_SUPPORTED_FEATURE,
			     &coex_sta->bt_coex_supported_feature);

		btc->btc_get(btc, BTC_GET_U4_SUPPORTED_VERSION,
			     &coex_sta->bt_coex_supported_version);

		coex_sta->bt_reg_vendor_ac = (u16)(btc->btc_get_bt_reg(btc, 3,
								       0xac) &
						   0xffff);

		coex_sta->bt_reg_vendor_ae = (u16)(btc->btc_get_bt_reg(btc, 3,
								       0xae) &
						   0xffff);

		btc->btc_get(btc, BTC_GET_U4_BT_PATCH_VER, &bt_patch_ver);
		btc->bt_info.bt_get_fw_ver = bt_patch_ver;

		if (coex_sta->num_of_profile > 0)
			btc->btc_get_bt_afh_map_from_bt(btc, 0, p);
	}

	if (++cnt >= 3)
		cnt = 0;

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %s/ %s / %d",
		   "Ant PG Num/ Mech/ Pos/ RFE", board_info->pg_ant_num,
		   (board_info->btdm_ant_num == 1 ? "Shared" : "Non-Shared"),
		   (board_info->btdm_ant_pos == BTC_ANTENNA_AT_MAIN_PORT ?
			    "Main" :
			    "Aux"),
		   rfe_type->rfe_module_type);
	CL_PRINTF(cli_buf);

	bt_patch_ver = btc->bt_info.bt_get_fw_ver;
	btc->btc_get(btc, BTC_GET_U4_WIFI_FW_VER, &fw_ver);
	phyver = btc->btc_get_bt_phydm_version(btc);
	bt_coex_ver = ((coex_sta->bt_coex_supported_version & 0xff00) >> 8);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = %d_%02x/ 0x%02x/ 0x%02x (%s)",
		   "CoexVer WL/  BT_Desired/ BT_Report",
		   glcoex_ver_date_8822b_1ant, glcoex_ver_8822b_1ant,
		   glcoex_ver_btdesired_8822b_1ant, bt_coex_ver,
		   (bt_coex_ver == 0xff ?
			 "Unknown" :
			 (coex_sta->bt_disabled ?
			  "BT-disable" :
			  (bt_coex_ver >= glcoex_ver_btdesired_8822b_1ant ?
				   "Match" :
				   "Mis-Match"))));
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x/ 0x%x/ v%d/ %c",
		   "W_FW/ B_FW/ Phy/ Kt", fw_ver, bt_patch_ver, phyver,
		   coex_sta->cut_version + 65);
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = %02x %02x %02x (RF-Ch = %d)", "AFH Map to BT",
		   coex_dm->wifi_chnl_info[0], coex_dm->wifi_chnl_info[1],
		   coex_dm->wifi_chnl_info[2], coex_sta->wl_center_channel);
	CL_PRINTF(cli_buf);

	/* wifi status */
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s",
		   "============[Wifi Status]============");
	CL_PRINTF(cli_buf);
	btc->btc_disp_dbg_msg(btc, BTC_DBG_DISP_WIFI_STATUS);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s",
		   "============[BT Status]============");
	CL_PRINTF(cli_buf);

	pop_report_in_10s++;
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %s/ %ddBm/ %d/ %d",
		   "BT status/ rssi/ retryCnt/ popCnt",
		   ((coex_sta->bt_disabled) ? ("disabled") :
		    ((coex_sta->c2h_bt_inquiry_page) ?
			  ("inquiry-page") :
			  ((BT_8822B_1ANT_BSTATUS_NCON_IDLE ==
			    coex_dm->bt_status) ?
				   "non-connecte-idle" :
				   ((coex_dm->bt_status ==
				     BT_8822B_1ANT_BSTATUS_CON_IDLE) ?
					    "connected-idle" :
					    "busy")))),
		coex_sta->bt_rssi - 100, coex_sta->bt_retry_cnt,
		coex_sta->pop_event_cnt);
	CL_PRINTF(cli_buf);

	if (pop_report_in_10s >= 5) {
		coex_sta->pop_event_cnt = 0;
		pop_report_in_10s = 0;
	}

	if (coex_sta->num_of_profile != 0)
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n %-35s = %s%s%s%s%s%s (multilink = %d)",
			   "Profiles", ((bt_link_info->a2dp_exist) ?
						((coex_sta->is_bt_a2dp_sink) ?
							 "A2DP sink," :
							 "A2DP,") :
						""),
			   ((bt_link_info->sco_exist) ? "HFP," : ""),
			   ((bt_link_info->hid_exist) ?
				    ((coex_sta->is_hid_rcu) ?
					     "HID(RCU)" :
					     ((coex_sta->hid_busy_num >= 2) ?
						      "HID(4/18)," :
						      "HID(2/18),")) :
				    ""),
			   ((bt_link_info->pan_exist) ?
				    ((coex_sta->is_bt_opp_exist) ? "OPP," :
								   "PAN,") :
				    ""),
			   ((coex_sta->voice_over_HOGP) ? "Voice," : ""),
			   ((coex_sta->msft_mr_exist) ? "MR" : ""),
			   coex_sta->is_bt_multi_link);
	else
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %s",
			   "Profiles",
			   (coex_sta->msft_mr_exist) ? "MR" : "None");

	CL_PRINTF(cli_buf);

	if (bt_link_info->a2dp_exist) {
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %s/ %d/ %s",
			   "CQDDR/Bitpool/Auto_Slot",
			   ((coex_sta->is_A2DP_3M) ? "On" : "Off"),
			   coex_sta->a2dp_bit_pool,
			   ((coex_sta->is_autoslot) ? "On" : "Off"));
		CL_PRINTF(cli_buf);

		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n %-35s = 0x%x/ 0x%x/ %d/ %d",
			   "V_ID/D_name/FBSlot_Legacy/FBSlot_Le",
			   coex_sta->bt_a2dp_vendor_id,
			   coex_sta->bt_a2dp_device_name,
			   coex_sta->legacy_forbidden_slot,
			   coex_sta->le_forbidden_slot);
		CL_PRINTF(cli_buf);
	}

	if (bt_link_info->hid_exist) {
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d",
			   "HID PairNum", coex_sta->hid_pair_cnt);
		CL_PRINTF(cli_buf);
	}

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %s/ %d/ %s/ 0x%x",
		   "Role/RoleSwCnt/IgnWlact/Feature",
		   ((bt_link_info->slave_role) ? "Slave" : "Master"),
		   coex_sta->cnt_role_switch,
		   ((coex_dm->cur_ignore_wlan_act) ? "Yes" : "No"),
		   coex_sta->bt_coex_supported_feature);
	CL_PRINTF(cli_buf);

	if (coex_sta->is_ble_scan_en) {
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n %-35s = 0x%x/ 0x%x/ 0x%x/ 0x%x",
			   "BLEScan Type/TV/Init/Ble",
			   coex_sta->bt_ble_scan_type,
			   (coex_sta->bt_ble_scan_type & 0x1 ?
				    coex_sta->bt_ble_scan_para[0] :
				    0x0),
			   (coex_sta->bt_ble_scan_type & 0x2 ?
				    coex_sta->bt_ble_scan_para[1] :
				    0x0),
			   (coex_sta->bt_ble_scan_type & 0x4 ?
				    coex_sta->bt_ble_scan_para[2] :
				    0x0));
		CL_PRINTF(cli_buf);
	}

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = %d/ %d/ %d/ %d/ %d %s",
		   "ReInit/ReLink/IgnWlact/Page/NameReq", coex_sta->cnt_reinit,
		   coex_sta->cnt_setup_link, coex_sta->cnt_ign_wlan_act,
		   coex_sta->cnt_page, coex_sta->cnt_remote_name_req,
		   (coex_sta->is_setup_link ? "(Relink!!)" : ""));
	CL_PRINTF(cli_buf);

	halbtc8822b1ant_read_scbd(btc, &u16tmp[0]);

	if (coex_sta->bt_reg_vendor_ae == 0xffff ||
	    coex_sta->bt_reg_vendor_ac == 0xffff)
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n %-35s = x/ x/ 0x%04x",
			   "0xae[4]/0xac[1:0]/Scoreboard(B->W)", u16tmp[0]);
	else
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n %-35s = 0x%x/ 0x%x/ 0x%04x",
			   "0xae[4]/0xac[1:0]/Scoreboard(B->W)",
			   (int)((coex_sta->bt_reg_vendor_ae & BIT(4)) >> 4),
			   coex_sta->bt_reg_vendor_ac & 0x3, u16tmp[0]);
	CL_PRINTF(cli_buf);

	if (coex_sta->num_of_profile > 0) {
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n %-35s = %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x",
			   "AFH MAP", coex_sta->bt_afh_map[0],
			   coex_sta->bt_afh_map[1], coex_sta->bt_afh_map[2],
			   coex_sta->bt_afh_map[3], coex_sta->bt_afh_map[4],
			   coex_sta->bt_afh_map[5], coex_sta->bt_afh_map[6],
			   coex_sta->bt_afh_map[7], coex_sta->bt_afh_map[8],
			   coex_sta->bt_afh_map[9]);
		CL_PRINTF(cli_buf);
	}

	for (i = 0; i < BT_8822B_1ANT_INFO_SRC_MAX; i++) {
		if (coex_sta->bt_info_c2h_cnt[i]) {
			CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
				   "\r\n %-35s = %02x %02x %02x %02x %02x %02x %02x (%d)",
				   glbt_info_src_8822b_1ant[i],
				   coex_sta->bt_info_c2h[i][0],
				   coex_sta->bt_info_c2h[i][1],
				   coex_sta->bt_info_c2h[i][2],
				   coex_sta->bt_info_c2h[i][3],
				   coex_sta->bt_info_c2h[i][4],
				   coex_sta->bt_info_c2h[i][5],
				   coex_sta->bt_info_c2h[i][6],
				   coex_sta->bt_info_c2h_cnt[i]);
			CL_PRINTF(cli_buf);
		}
	}

	if (btc->manual_control)
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s",
			   "============[mechanisms] (before Manual)============");
	else
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s",
			   "============[Mechanisms]============");

	CL_PRINTF(cli_buf);

	ps_tdma_case = coex_dm->cur_ps_tdma;
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = %02x %02x %02x %02x %02x (case-%d, %s)",
		   "TDMA",
		   coex_dm->ps_tdma_para[0], coex_dm->ps_tdma_para[1],
		   coex_dm->ps_tdma_para[2], coex_dm->ps_tdma_para[3],
		   coex_dm->ps_tdma_para[4], ps_tdma_case,
		   (coex_dm->cur_ps_tdma_on ? "TDMA-On" : "TDMA-Off"));
	CL_PRINTF(cli_buf);

	switch (coex_sta->wl_coex_mode) {
	case BT_8822B_1ANT_WLINK_2G1PORT:
	default:
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n %-35s = %s", "Coex_Mode", "2G-SP");
		break;
	case BT_8822B_1ANT_WLINK_2GMPORT:
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n %-35s = %s", "Coex_Mode", "2G-MP");
		break;
	case BT_8822B_1ANT_WLINK_25GMPORT:
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n %-35s = %s", "Coex_Mode", "25G-MP");
		break;
	case BT_8822B_1ANT_WLINK_5G:
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n %-35s = %s", "Coex_Mode", "5G");
		break;
	case BT_8822B_1ANT_WLINK_2GGO:
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n %-35s = %s", "Coex_Mode", "2G-P2P");
		break;
	case BT_8822B_1ANT_WLINK_BTMR:
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n %-35s = %s", "Coex_Mode", "BT-MR");
		break;
	}

	CL_PRINTF(cli_buf);

	u32tmp[0] = btc->btc_read_4byte(btc, 0x6c0);
	u32tmp[1] = btc->btc_read_4byte(btc, 0x6c4);
	u32tmp[2] = btc->btc_read_4byte(btc, 0x6c8);
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = %d/ 0x%x/ 0x%x/ 0x%x",
		   "Table/0x6c0/0x6c4/0x6c8", coex_sta->coex_table_type,
		   u32tmp[0], u32tmp[1], u32tmp[2]);
	CL_PRINTF(cli_buf);

	u8tmp[0] = btc->btc_read_1byte(btc, 0x778);
	u32tmp[0] = btc->btc_read_4byte(btc, 0x6cc);
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x/ 0x%x/ 0x%04x",
		   "0x778/0x6cc/Scoreboard(W->B)", u8tmp[0], u32tmp[0],
		   coex_sta->score_board_WB);
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %s/ %s/ %s/ %d/ %d",
		   "AntDiv/BtCtrlLPS/LPRA/PsFail/g_busy",
		   ((board_info->ant_div_cfg) ? "On" : "Off"),
		   ((coex_sta->force_lps_ctrl) ? "On" : "Off"),
		   ((coex_dm->cur_low_penalty_ra) ? "On" : "Off"),
		   coex_sta->cnt_set_ps_state_fail, coex_sta->gl_wifi_busy);
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d/ %d/ %d/ %d",
		   "Null All/Retry/Ack/BT_Empty/BT_Late",
		   coex_sta->wl_fw_dbg_info[1], coex_sta->wl_fw_dbg_info[2],
		   coex_sta->wl_fw_dbg_info[3], coex_sta->wl_fw_dbg_info[4],
		   coex_sta->wl_fw_dbg_info[5]);
	CL_PRINTF(cli_buf);

	u32tmp[0] = halbtc8822b1ant_read_indirect_reg(btc, 0x38);
	lte_coex_on = ((u32tmp[0] & BIT(7)) >> 7) ? TRUE : FALSE;

	if (lte_coex_on) {
		u32tmp[0] = halbtc8822b1ant_read_indirect_reg(btc, 0xa0);
		u32tmp[1] = halbtc8822b1ant_read_indirect_reg(btc, 0xa4);

		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x/ 0x%x",
			   "LTE Coex Table W_L/B_L", u32tmp[0] & 0xffff,
			   u32tmp[1] & 0xffff);
		CL_PRINTF(cli_buf);

		u32tmp[0] = halbtc8822b1ant_read_indirect_reg(btc, 0xa8);
		u32tmp[1] = halbtc8822b1ant_read_indirect_reg(btc, 0xac);
		u32tmp[2] = halbtc8822b1ant_read_indirect_reg(btc, 0xb0);
		u32tmp[3] = halbtc8822b1ant_read_indirect_reg(btc, 0xb4);

		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n %-35s = 0x%x/ 0x%x/ 0x%x/ 0x%x",
			   "LTE Break Table W_L/B_L/L_W/L_B",
			   u32tmp[0] & 0xffff, u32tmp[1] & 0xffff,
			   u32tmp[2] & 0xffff, u32tmp[3] & 0xffff);
		CL_PRINTF(cli_buf);
	}

	/* Hw setting		 */
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s",
		   "============[Hw setting]============");
	CL_PRINTF(cli_buf);

	u32tmp[0] = halbtc8822b1ant_read_indirect_reg(btc, 0x38);
	u32tmp[1] = halbtc8822b1ant_read_indirect_reg(btc, 0x54);
	u8tmp[0] = btc->btc_read_1byte(btc, 0x73);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %s/ %s",
		   "LTE Coex/Path Owner", ((lte_coex_on) ? "On" : "Off"),
		   ((u8tmp[0] & BIT(2)) ? "WL" : "BT"));
	CL_PRINTF(cli_buf);

	if (lte_coex_on) {
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
			   "\r\n %-35s = %d/ %d/ %d/ %d",
			   "LTE 3Wire/OPMode/UART/UARTMode",
			   (int)((u32tmp[0] & BIT(6)) >> 6),
			   (int)((u32tmp[0] & (BIT(5) | BIT(4))) >> 4),
			   (int)((u32tmp[0] & BIT(3)) >> 3),
			   (int)(u32tmp[0] & (BIT(2) | BIT(1) | BIT(0))));
		CL_PRINTF(cli_buf);

		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d",
			   "LTE_Busy/UART_Busy",
			   (int)((u32tmp[1] & BIT(1)) >> 1),
			   (int)(u32tmp[1] & BIT(0)));
		CL_PRINTF(cli_buf);
	}
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = %s (BB:%s)/ %s (BB:%s)/ %s (gnt_err = %d)",
		   "GNT_WL_Ctrl/GNT_BT_Ctrl/Dbg",
		   ((u32tmp[0] & BIT(12)) ? "SW" : "HW"),
		   ((u32tmp[0] & BIT(8)) ? "SW" : "HW"),
		   ((u32tmp[0] & BIT(14)) ? "SW" : "HW"),
		   ((u32tmp[0] & BIT(10)) ? "SW" : "HW"),
		   ((u8tmp[0] & BIT(3)) ? "On" : "Off"),
		   coex_sta->gnt_error_cnt);
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d",
		   "GNT_WL/GNT_BT", (int)((u32tmp[1] & BIT(2)) >> 2),
		   (int)((u32tmp[1] & BIT(3)) >> 3));
	CL_PRINTF(cli_buf);

	u32tmp[0] = btc->btc_read_4byte(btc, 0xcb0);
	u32tmp[1] = btc->btc_read_4byte(btc, 0xcb4);
	u8tmp[0] = btc->btc_read_1byte(btc, 0xcba);
	u8tmp[1] = btc->btc_read_1byte(btc, 0xcbd);
	u8tmp[2] = btc->btc_read_1byte(btc, 0xc58);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = 0x%04x/ 0x%04x/ 0x%02x/ 0x%02x/ 0x%02x %s",
		   "0xcb0/0xcb4/0xcba/0xcbd/0xc58", u32tmp[0], u32tmp[1],
		   u8tmp[0], u8tmp[1], u8tmp[2],
		   ((u8tmp[1] & 0x1) == 0x1 ? "(BT_WL5G)" : "(WL2G)"));
	CL_PRINTF(cli_buf);

	u32tmp[0] = btc->btc_read_4byte(btc, 0x4c);
	u8tmp[2] = btc->btc_read_1byte(btc, 0x64);
	u8tmp[0] = btc->btc_read_1byte(btc, 0x4c6);
	u8tmp[1] = btc->btc_read_1byte(btc, 0x40);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = 0x%x/ 0x%x/ 0x%x/ 0x%x",
		   "4c[24:23]/64[0]/4c6[4]/40[5]",
		   (int)(u32tmp[0] & (BIT(24) | BIT(23))) >> 23, u8tmp[2] & 0x1,
		   (int)((u8tmp[0] & BIT(4)) >> 4),
		   (int)((u8tmp[1] & BIT(5)) >> 5));
	CL_PRINTF(cli_buf);

	u32tmp[0] = btc->btc_read_4byte(btc, 0x550);
	u8tmp[0] = btc->btc_read_1byte(btc, 0x522);
	u8tmp[1] = btc->btc_read_1byte(btc, 0x953);
	u8tmp[2] = btc->btc_read_1byte(btc, 0xc50);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = 0x%x/ 0x%x/ %s/ 0x%x",
		   "0x550/0x522/4-RxAGC/0xc50", u32tmp[0], u8tmp[0],
		   (u8tmp[1] & 0x2) ? "On" : "Off", u8tmp[2]);
	CL_PRINTF(cli_buf);

	fa_ofdm = btc->btc_phydm_query_PHY_counter(btc, PHYDM_INFO_FA_OFDM);
	fa_cck = btc->btc_phydm_query_PHY_counter(btc, PHYDM_INFO_FA_CCK);
	cca_ofdm = btc->btc_phydm_query_PHY_counter(btc, PHYDM_INFO_CCA_OFDM);
	cca_cck = btc->btc_phydm_query_PHY_counter(btc, PHYDM_INFO_CCA_CCK);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = 0x%x/ 0x%x/ 0x%x/ 0x%x",
		   "CCK-CCA/CCK-FA/OFDM-CCA/OFDM-FA", cca_cck, fa_cck, cca_ofdm,
		   fa_ofdm);
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d/ %d/ %d",
		   "CRC_OK CCK/11g/11n/11ac",
		   coex_sta->crc_ok_cck, coex_sta->crc_ok_11g,
		   coex_sta->crc_ok_11n, coex_sta->crc_ok_11n_vht);
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d/ %d/ %d",
		   "CRC_Err CCK/11g/11n/11ac",
		   coex_sta->crc_err_cck, coex_sta->crc_err_11g,
		   coex_sta->crc_err_11n, coex_sta->crc_err_11n_vht);
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE,
		   "\r\n %-35s = %d/ %d/ %s-%d/ %d (Tx macid: %d)",
		   "Rate RxD/RxRTS/TxD/TxRetry_ratio",
		   coex_sta->wl_rx_rate, coex_sta->wl_rts_rx_rate,
		   (coex_sta->wl_tx_rate & 0x80 ? "SGI" : "LGI"),
		   coex_sta->wl_tx_rate & 0x7f,
		   coex_sta->wl_tx_retry_ratio,
		   coex_sta->wl_tx_macid);
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %s/ %s/ %s/ %s/ %d",
		   "HiPr/ Locking/ warn/ Locked/ Noisy",
		   (coex_sta->wifi_high_pri_task1 ? "Yes" : "No"),
		   (coex_sta->cck_lock ? "Yes" : "No"),
		   (coex_sta->cck_lock_warn ? "Yes" : "No"),
		   (coex_sta->cck_lock_ever ? "Yes" : "No"),
		   coex_sta->wl_noisy_level);
	CL_PRINTF(cli_buf);

	u8tmp[0] = btc->btc_read_1byte(btc, 0xf8e);
	u8tmp[1] = btc->btc_read_1byte(btc, 0xf8f);
	u8tmp[2] = btc->btc_read_1byte(btc, 0xd14);
	u8tmp[3] = btc->btc_read_1byte(btc, 0xd54);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d/ %d/ %d",
		   "EVM_A/ EVM_B/ SNR_A/ SNR_B",
		   (u8tmp[0] > 127 ? u8tmp[0] - 256 : u8tmp[0]),
		   (u8tmp[1] > 127 ? u8tmp[1] - 256 : u8tmp[1]),
		   (u8tmp[2] > 127 ? u8tmp[2] - 256 : u8tmp[2]),
		   (u8tmp[3] > 127 ? u8tmp[3] - 256 : u8tmp[3]));
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d",
		   "0x770(Hi-pri rx/tx)", coex_sta->high_priority_rx,
		   coex_sta->high_priority_tx);
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d %s",
		   "0x774(Lo-pri rx/tx)", coex_sta->low_priority_rx,
		   coex_sta->low_priority_tx,
		   (bt_link_info->slave_role ?
			    "(Slave!!)" :
			    (coex_sta->is_tdma_btautoslot_hang ?
				     "(auto-slot hang!!)" :
				     "")));
	CL_PRINTF(cli_buf);

	btc->btc_disp_dbg_msg(btc, BTC_DBG_DISP_COEX_STATISTICS);
}

void ex_halbtc8822b1ant_ips_notify(struct btc_coexist *btc, u8 type)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;

	if (btc->manual_control || btc->stop_coex_dm)
		return;

	if (type == BTC_IPS_ENTER) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], IPS ENTER notify\n");
		BTC_TRACE(trace_buf);
		coex_sta->under_ips = TRUE;

		/* Write WL "Active" in Score-board for LPS off */
		halbtc8822b1ant_write_scbd(btc, BT_8822B_1ANT_SCBD_ACTIVE |
					   BT_8822B_1ANT_SCBD_ONOFF |
					   BT_8822B_1ANT_SCBD_SCAN |
					   BT_8822B_1ANT_SCBD_UNDERTEST |
					   BT_8822B_1ANT_SCBD_RXGAIN,
					   FALSE);

		halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, FC_EXCU,
					     BT_8822B_1ANT_PHASE_WOFF);
		halbtc8822b1ant_action_coex_all_off(btc);
	} else if (type == BTC_IPS_LEAVE) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], IPS LEAVE notify\n");
		BTC_TRACE(trace_buf);
		halbtc8822b1ant_write_scbd(btc, BT_8822B_1ANT_SCBD_ACTIVE |
					   BT_8822B_1ANT_SCBD_ONOFF,
					   TRUE);

		/*leave IPS : run ini hw config (exclude wifi only)*/
		halbtc8822b1ant_init_hw_config(btc, FALSE, FALSE);
		/*sw all off*/
		halbtc8822b1ant_init_coex_dm(btc);

		coex_sta->under_ips = FALSE;
	}
}

void ex_halbtc8822b1ant_lps_notify(struct btc_coexist *btc, u8 type)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	static boolean pre_force_lps_on;

	if (btc->manual_control || btc->stop_coex_dm)
		return;

	if (type == BTC_LPS_ENABLE) {
		const u16 type_is_active = BT_8822B_1ANT_SCBD_ACTIVE;

		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], LPS ENABLE notify\n");
		BTC_TRACE(trace_buf);
		coex_sta->under_lps = TRUE;

		if (coex_sta->force_lps_ctrl) { /* LPS No-32K */
			/* Write WL "Active" in Score-board for PS-TDMA */
			pre_force_lps_on = TRUE;
			halbtc8822b1ant_write_scbd(btc, type_is_active, TRUE);
		} else {
			/* LPS-32K, need check if this h2c 0x71 can work??
			 * (2015/08/28)
			 */
			/* Write WL "Non-Active" in Score-board for Native-PS */
			pre_force_lps_on = FALSE;
			halbtc8822b1ant_write_scbd(btc, type_is_active, FALSE);

			halbtc8822b1ant_action_wifi_native_lps(btc);
		}
	} else if (type == BTC_LPS_DISABLE) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], LPS DISABLE notify\n");
		BTC_TRACE(trace_buf);
		coex_sta->under_lps = FALSE;

		/* Write WL "Active" in Score-board for LPS off */
		halbtc8822b1ant_write_scbd(btc, BT_8822B_1ANT_SCBD_ACTIVE,
					   TRUE);

		if (!pre_force_lps_on && !coex_sta->force_lps_ctrl)
			halbtc8822b1ant_query_bt_info(btc);
	}
}

void ex_halbtc8822b1ant_scan_notify(struct btc_coexist *btc, u8 type)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	boolean wifi_connected = FALSE;

	if (btc->manual_control || btc->stop_coex_dm)
		return;

	coex_sta->freeze_coexrun_by_btinfo = FALSE;

	btc->btc_get(btc, BTC_GET_BL_WIFI_CONNECTED, &wifi_connected);

	if (wifi_connected)
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], ********** WL connected before SCAN\n");
	else
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], **********  WL is not connected before SCAN\n");

	BTC_TRACE(trace_buf);

	if (type != BTC_SCAN_FINISH) {
		halbtc8822b1ant_write_scbd(btc, BT_8822B_1ANT_SCBD_ACTIVE |
					   BT_8822B_1ANT_SCBD_SCAN |
					   BT_8822B_1ANT_SCBD_ONOFF,
					   TRUE);
	}

	if (type == BTC_SCAN_START_5G) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], SCAN START notify (5G)\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, FC_EXCU,
					     BT_8822B_1ANT_PHASE_5G);
		halbtc8822b1ant_run_coex(btc, BT_8822B_1ANT_RSN_5GSCANSTART);
	} else if ((type == BTC_SCAN_START_2G) || (type == BTC_SCAN_START)) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], SCAN START notify (2G)\n");
		BTC_TRACE(trace_buf);

		if (!wifi_connected)
			coex_sta->wifi_high_pri_task2 = TRUE;

		/* Force antenna setup for no scan result issue */
		halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, FC_EXCU,
					     BT_8822B_1ANT_PHASE_2G);

		halbtc8822b1ant_run_coex(btc, BT_8822B_1ANT_RSN_2GSCANSTART);
	} else {
		btc->btc_get(btc, BTC_GET_U1_AP_NUM, &coex_sta->scan_ap_num);

		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], SCAN FINISH notify  (Scan-AP = %d)\n",
			    coex_sta->scan_ap_num);
		BTC_TRACE(trace_buf);

		coex_sta->wifi_high_pri_task2 = FALSE;

		halbtc8822b1ant_run_coex(btc, BT_8822B_1ANT_RSN_SCANFINISH);
	}
}

void ex_halbtc8822b1ant_scan_notify_without_bt(struct btc_coexist *btc, u8 type)
{
	struct wifi_link_info_8822b_1ant *wifi_link_info_ext =
					 &btc->wifi_link_info_8822b_1ant;

	if (type == BTC_SCAN_START) {
		if (wifi_link_info_ext->is_all_under_5g)
			btc->btc_write_1byte_bitmask(btc, 0xcbd, 0x3, 1);
		else /* under 2.4G */
			btc->btc_write_1byte_bitmask(btc, 0xcbd, 0x3, 2);
	} else if (type == BTC_SCAN_START_2G) {
		btc->btc_write_1byte_bitmask(btc, 0xcbd, 0x3, 2);
	}
}

void ex_halbtc8822b1ant_switchband_notify(struct btc_coexist *btc,
					  u8 type)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;

	if (btc->manual_control || btc->stop_coex_dm)
		return;

	coex_sta->switch_band_notify_to = type;

	if (type == BTC_SWITCH_TO_5G) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], switchband_notify --- BTC_SWITCH_TO_5G\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_run_coex(btc, BT_8822B_1ANT_RSN_5GSWITCHBAND);
	} else if (type == BTC_SWITCH_TO_24G_NOFORSCAN) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], switchband_notify --- BTC_SWITCH_TO_24G_NOFORSCAN\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_run_coex(btc, BT_8822B_1ANT_RSN_2GSWITCHBAND);
	} else {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], switchband_notify --- BTC_SWITCH_TO_2G\n");
		BTC_TRACE(trace_buf);

		ex_halbtc8822b1ant_scan_notify(btc, BTC_SCAN_START_2G);
	}

	coex_sta->switch_band_notify_to = BTC_NOT_SWITCH;
}

void
ex_halbtc8822b1ant_switchband_notify_without_bt(struct btc_coexist *btc,
						u8 type)
{
	if (type == BTC_SWITCH_TO_5G)
		btc->btc_write_1byte_bitmask(btc, 0xcbd, 0x3, 1);
	else if (type == BTC_SWITCH_TO_24G_NOFORSCAN)
		btc->btc_write_1byte_bitmask(btc, 0xcbd, 0x3, 2);
	else
		ex_halbtc8822b1ant_scan_notify_without_bt(btc,
							  BTC_SCAN_START_2G);
}

void ex_halbtc8822b1ant_connect_notify(struct btc_coexist *btc, u8 type)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;

	if (btc->manual_control || btc->stop_coex_dm)
		return;

	halbtc8822b1ant_write_scbd(btc, BT_8822B_1ANT_SCBD_ACTIVE |
				   BT_8822B_1ANT_SCBD_SCAN |
				   BT_8822B_1ANT_SCBD_ONOFF,
				   TRUE);

	if (type == BTC_ASSOCIATE_5G_START) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], CONNECT START notify (5G)\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, FC_EXCU,
					     BT_8822B_1ANT_PHASE_5G);

		halbtc8822b1ant_run_coex(btc, BT_8822B_1ANT_RSN_5GCONSTART);
	} else if (type == BTC_ASSOCIATE_5G_FINISH) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], CONNECT FINISH notify (5G)\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, FC_EXCU,
					     BT_8822B_1ANT_PHASE_5G);

		halbtc8822b1ant_run_coex(btc, BT_8822B_1ANT_RSN_5GCONFINISH);
	} else if (type == BTC_ASSOCIATE_START) {
		coex_sta->wifi_high_pri_task1 = TRUE;
		coex_dm->arp_cnt = 0;
		coex_sta->connect_ap_period_cnt = 2;

		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], CONNECT START notify (2G)\n");
		BTC_TRACE(trace_buf);

		/* Force antenna setup for no scan result issue */
		halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, FC_EXCU,
					     BT_8822B_1ANT_PHASE_2G);

		halbtc8822b1ant_run_coex(btc, BT_8822B_1ANT_RSN_2GCONSTART);

		/* To keep TDMA case during connect process,
		 * to avoid changed by Btinfo and runcoexmechanism
		 */
		coex_sta->freeze_coexrun_by_btinfo = TRUE;
	} else {
		coex_sta->wifi_high_pri_task1 = FALSE;
		coex_sta->freeze_coexrun_by_btinfo = FALSE;

		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], CONNECT FINISH notify (2G)\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_run_coex(btc, BT_8822B_1ANT_RSN_2GCONFINISH);
	}
}

void ex_halbtc8822b1ant_media_status_notify(struct btc_coexist *btc, u8 type)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;
	boolean wifi_under_b_mode = FALSE;

	if (btc->manual_control || btc->stop_coex_dm)
		return;

	if (type == BTC_MEDIA_CONNECT_5G) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], media_status_notify --- 5G\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_write_scbd(btc, BT_8822B_1ANT_SCBD_ACTIVE,
					   TRUE);

		halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, FC_EXCU,
					     BT_8822B_1ANT_PHASE_5G);

		halbtc8822b1ant_run_coex(btc, BT_8822B_1ANT_RSN_5GMEDIA);
	} else if (type == BTC_MEDIA_CONNECT) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], media_status_notify --- 2G\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_write_scbd(btc, BT_8822B_1ANT_SCBD_ACTIVE,
					   TRUE);

		/* Force antenna setup for no scan result issue */
		halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO,
					     FC_EXCU,
					     BT_8822B_1ANT_PHASE_2G);

		btc->btc_get(btc, BTC_GET_BL_WIFI_UNDER_B_MODE,
			     &wifi_under_b_mode);

		/* Set CCK Tx/Rx high Pri except 11b mode */
		if (wifi_under_b_mode) {
			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], media status notity --- under b mode\n");
			BTC_TRACE(trace_buf);
			btc->btc_write_1byte(btc, 0x6cd, 0x00); /* CCK Tx */
			btc->btc_write_1byte(btc, 0x6cf, 0x00); /* CCK Rx */
		} else {
			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], media status notity --- under b mode\n");
			BTC_TRACE(trace_buf);
			btc->btc_write_1byte(btc, 0x6cd, 0x00); /* CCK Tx */
			btc->btc_write_1byte(btc, 0x6cf, 0x10); /* CCK Rx */
		}

		halbtc8822b1ant_run_coex(btc, BT_8822B_1ANT_RSN_2GMEDIA);
	} else {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], media disconnect notify\n");
		BTC_TRACE(trace_buf);
		coex_dm->arp_cnt = 0;

		halbtc8822b1ant_write_scbd(btc, BT_8822B_1ANT_SCBD_ACTIVE,
					   FALSE);

		btc->btc_write_1byte(btc, 0x6cd, 0x0); /* CCK Tx */
		btc->btc_write_1byte(btc, 0x6cf, 0x0); /* CCK Rx */

		coex_sta->cck_lock_ever = FALSE;
		coex_sta->cck_lock_warn = FALSE;
		coex_sta->cck_lock = FALSE;

		halbtc8822b1ant_run_coex(btc, BT_8822B_1ANT_RSN_MEDIADISCON);
	}

	halbtc8822b1ant_update_wifi_ch_info(btc, type);
}

void ex_halbtc8822b1ant_specific_packet_notify(struct btc_coexist *btc, u8 type)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;
	boolean under_4way = FALSE;

	if (btc->manual_control || btc->stop_coex_dm)
		return;

	if (type & BTC_5G_BAND) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], 5g special packet notify\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_run_coex(btc, BT_8822B_1ANT_RSN_5GSPECIALPKT);
		return;
	}

	btc->btc_get(btc, BTC_GET_BL_WIFI_4_WAY_PROGRESS, &under_4way);

	if (under_4way) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], specific Packet ---- under_4way!!\n");
		BTC_TRACE(trace_buf);

		coex_sta->wifi_high_pri_task1 = TRUE;
		coex_sta->specific_pkt_period_cnt = 2;
	} else if (type == BTC_PACKET_ARP) {
		coex_dm->arp_cnt++;

		if (coex_sta->wifi_high_pri_task1) {
			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], specific Packet ARP notify -cnt = %d\n",
				    coex_dm->arp_cnt);
			BTC_TRACE(trace_buf);
		}

	} else {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], specific Packet DHCP or EAPOL notify [Type = %d]\n",
			    type);
		BTC_TRACE(trace_buf);

		coex_sta->wifi_high_pri_task1 = TRUE;
		coex_sta->specific_pkt_period_cnt = 2;
	}

	if (coex_sta->wifi_high_pri_task1) {
		halbtc8822b1ant_write_scbd(btc, BT_8822B_1ANT_SCBD_SCAN, TRUE);
		halbtc8822b1ant_run_coex(btc, BT_8822B_1ANT_RSN_2GSPECIALPKT);
	}
}

void ex_halbtc8822b1ant_bt_info_notify(struct btc_coexist *btc, u8 *tmp_buf,
				       u8 length)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	u8 i, rsp_source = 0;
	boolean wifi_connected = FALSE;
	boolean wifi_busy = FALSE;
	static boolean is_scoreboard_scan;
	const u16 type_is_scan = BT_8822B_1ANT_SCBD_SCAN;
	u8 type;

	rsp_source = tmp_buf[0] & 0xf;
	if (rsp_source >= BT_8822B_1ANT_INFO_SRC_MAX)
		rsp_source = BT_8822B_1ANT_INFO_SRC_WIFI_FW;
	coex_sta->bt_info_c2h_cnt[rsp_source]++;

	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
		    "[BTCoex], Bt_info[%d], len=%d, data=[", rsp_source,
		    length);
	BTC_TRACE(trace_buf);

	if (rsp_source == BT_8822B_1ANT_INFO_SRC_BT_RSP ||
	    rsp_source == BT_8822B_1ANT_INFO_SRC_BT_ACT) {
		if (coex_sta->bt_disabled) {
			coex_sta->bt_disabled = FALSE;
			coex_sta->is_bt_reenable = TRUE;
			coex_sta->cnt_bt_reenable = 15;

			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], BT enable detected by bt_info\n");
			BTC_TRACE(trace_buf);
		}
	}

	for (i = 0; i < length; i++) {
		coex_sta->bt_info_c2h[rsp_source][i] = tmp_buf[i];

		if (i == length - 1) {
			/* last one */
			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE, "0x%02x]\n",
				    tmp_buf[i]);
			BTC_TRACE(trace_buf);
		} else {
			/* normal */
			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE, "0x%02x, ",
				    tmp_buf[i]);
			BTC_TRACE(trace_buf);
		}
	}

	coex_sta->bt_info = coex_sta->bt_info_c2h[rsp_source][1];
	coex_sta->bt_info_ext = coex_sta->bt_info_c2h[rsp_source][4];
	coex_sta->bt_info_ext2 = coex_sta->bt_info_c2h[rsp_source][5];

	if (rsp_source != BT_8822B_1ANT_INFO_SRC_WIFI_FW) {
		/* if 0xff, it means BT is under WHCK test */
		coex_sta->bt_whck_test =
			((coex_sta->bt_info == 0xff) ? TRUE : FALSE);

		coex_sta->bt_create_connection =
			((coex_sta->bt_info_c2h[rsp_source][2] & 0x80) ? TRUE :
									 FALSE);

		/* unit: %, value-100 to translate to unit: dBm */
		coex_sta->bt_rssi =
			coex_sta->bt_info_c2h[rsp_source][3] * 2 + 10;

		coex_sta->c2h_bt_remote_name_req =
			((coex_sta->bt_info_c2h[rsp_source][2] & 0x20) ? TRUE :
									 FALSE);

		coex_sta->is_A2DP_3M =
			((coex_sta->bt_info_c2h[rsp_source][2] & 0x10) ? TRUE :
									 FALSE);

		coex_sta->acl_busy =
			((coex_sta->bt_info_c2h[rsp_source][1] & 0x8) ? TRUE :
									FALSE);

		coex_sta->voice_over_HOGP =
			((coex_sta->bt_info_ext & 0x10) ? TRUE : FALSE);

		coex_sta->c2h_bt_inquiry_page =
			((coex_sta->bt_info & BT_INFO_8822B_1ANT_B_INQ_PAGE) ?
				 TRUE :
				 FALSE);

		coex_sta->a2dp_bit_pool =
			(((coex_sta->bt_info_c2h[rsp_source][1] & 0x49) ==
			  0x49) ?
				 (coex_sta->bt_info_c2h[rsp_source][6] & 0x7f) :
				 0);

		coex_sta->is_bt_a2dp_sink =
			(coex_sta->bt_info_c2h[rsp_source][6] & 0x80) ? TRUE :
									FALSE;

		coex_sta->bt_retry_cnt =
			coex_sta->bt_info_c2h[rsp_source][2] & 0xf;

		coex_sta->is_autoslot = coex_sta->bt_info_ext2 & 0x8;

		coex_sta->forbidden_slot = coex_sta->bt_info_ext2 & 0x7;

		coex_sta->hid_busy_num = (coex_sta->bt_info_ext2 & 0x30) >> 4;

		coex_sta->hid_pair_cnt = (coex_sta->bt_info_ext2 & 0xc0) >> 6;

		coex_sta->is_bt_opp_exist =
			(coex_sta->bt_info_ext2 & 0x1) ? TRUE : FALSE;

		if (coex_sta->bt_retry_cnt >= 1)
			coex_sta->pop_event_cnt++;

		if (coex_sta->c2h_bt_remote_name_req)
			coex_sta->cnt_remote_name_req++;

		if (coex_sta->bt_info_ext & BIT(1))
			coex_sta->cnt_reinit++;

		if ((coex_sta->bt_info_ext & BIT(2)) ||
		    (coex_sta->bt_create_connection &&
		     coex_sta->wl_pnp_wakeup_downcnt > 0)) {
			coex_sta->cnt_setup_link++;
			coex_sta->is_setup_link = TRUE;

			if (coex_sta->is_bt_reenable)
				coex_sta->bt_relink_downcount = 6;
			else
				coex_sta->bt_relink_downcount = 2;
			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], Re-Link start in BT info!!\n");
			BTC_TRACE(trace_buf);
		} else {
			coex_sta->is_setup_link = FALSE;

			if (!coex_sta->is_bt_reenable)
				coex_sta->bt_relink_downcount = 0;
			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], Re-Link stop in BT info!!\n");
			BTC_TRACE(trace_buf);
		}

		if (coex_sta->bt_info_ext & BIT(3))
			coex_sta->cnt_ign_wlan_act++;

		if (coex_sta->bt_info_ext & BIT(6))
			coex_sta->cnt_role_switch++;

		if (coex_sta->bt_info_ext & BIT(7))
			coex_sta->is_bt_multi_link = TRUE;
		else
			coex_sta->is_bt_multi_link = FALSE;

		if (coex_sta->bt_info_ext & BIT(0))
			coex_sta->is_hid_rcu = TRUE;
		else
			coex_sta->is_hid_rcu = FALSE;

		if (coex_sta->bt_info_ext & BIT(5))
			coex_sta->is_ble_scan_en = TRUE;
		else
			coex_sta->is_ble_scan_en = FALSE;

		if (coex_sta->bt_create_connection) {
			coex_sta->cnt_page++;

			btc->btc_get(btc, BTC_GET_BL_WIFI_BUSY, &wifi_busy);

			if (coex_sta->is_wifi_linkscan_process ||
			    coex_sta->wifi_high_pri_task1 ||
			    coex_sta->wifi_high_pri_task2 || wifi_busy) {
				is_scoreboard_scan = TRUE;
				halbtc8822b1ant_write_scbd(btc, type_is_scan,
							   TRUE);
			} else {
				halbtc8822b1ant_write_scbd(btc, type_is_scan,
							   FALSE);
			}
		} else {
			if (is_scoreboard_scan) {
				halbtc8822b1ant_write_scbd(btc, type_is_scan,
							   FALSE);
				is_scoreboard_scan = FALSE;
			}
		}

		/* Here we need to resend some wifi info to BT */
		/* because bt is reset and loss of the info. */

		btc->btc_get(btc, BTC_GET_BL_WIFI_CONNECTED, &wifi_connected);
		/*  Re-Init */
		if ((coex_sta->bt_info_ext & BIT(1))) {
			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], BT ext info bit1 check, send wifi BW&Chnl to BT!!\n");
			BTC_TRACE(trace_buf);
			if (wifi_connected)
				type = BTC_MEDIA_CONNECT;
			else
				type = BTC_MEDIA_DISCONNECT;
			halbtc8822b1ant_update_wifi_ch_info(btc, type);
		}

		/* If Ignore_WLanAct && not SetUp_Link */
		if ((coex_sta->bt_info_ext & BIT(3)) &&
		    (!(coex_sta->bt_info_ext & BIT(2)))) {
			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], BT ext info bit3 check, set BT NOT to ignore Wlan active!!\n");
			BTC_TRACE(trace_buf);
			halbtc8822b1ant_ignore_wlan_act(btc, FC_EXCU, FALSE);
		}
	}

	halbtc8822b1ant_update_bt_link_info(btc);

	halbtc8822b1ant_run_coex(btc, BT_8822B_1ANT_RSN_BTINFO);
}

void ex_halbtc8822b1ant_wl_fwdbginfo_notify(struct btc_coexist *btc,
					    u8 *tmp_buf, u8 length)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	u8 i = 0;
	static u8 tmp_buf_pre[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
		    "[BTCoex], WiFi Fw Dbg info = %d %d %d %d %d %d (len = %d)\n",
		    tmp_buf[0], tmp_buf[1], tmp_buf[2], tmp_buf[3], tmp_buf[4],
		    tmp_buf[5], length);
	BTC_TRACE(trace_buf);

	if (tmp_buf[0] == 0x8) {
		for (i = 1; i <= 5; i++) {
			coex_sta->wl_fw_dbg_info[i] =
				(tmp_buf[i] >= tmp_buf_pre[i]) ?
					(tmp_buf[i] - tmp_buf_pre[i]) :
					(255 - tmp_buf_pre[i] + tmp_buf[i]);

			tmp_buf_pre[i] = tmp_buf[i];
		}
	}
}

void ex_halbtc8822b1ant_rx_rate_change_notify(struct btc_coexist *btc,
					      BOOLEAN is_data_frame,
					      u8 btc_rate_id)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct coex_dm_8822b_1ant *coex_dm = &btc->coex_dm_8822b_1ant;
	BOOLEAN wifi_connected = FALSE;

	btc->btc_get(btc, BTC_GET_BL_WIFI_CONNECTED, &wifi_connected);

	if (is_data_frame) {
		coex_sta->wl_rx_rate = btc_rate_id;

		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], rx_rate_change_notify data rate id = %d, RTS_Rate = %d\n",
			    coex_sta->wl_rx_rate, coex_sta->wl_rts_rx_rate);
		BTC_TRACE(trace_buf);
	} else {
		coex_sta->wl_rts_rx_rate = btc_rate_id;

		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], rts_rate_change_notify RTS rate id = %d, RTS_Rate = %d\n",
			    coex_sta->wl_rts_rx_rate, coex_sta->wl_rts_rx_rate);
		BTC_TRACE(trace_buf);
	}

	if (wifi_connected &&
	    (coex_dm->bt_status == BT_8822B_1ANT_BSTATUS_ACL_BUSY ||
	     coex_dm->bt_status == BT_8822B_1ANT_BSTATUS_ACL_SCO_BUSY ||
	     coex_dm->bt_status == BT_8822B_1ANT_BSTATUS_SCO_BUSY)) {
		if (coex_sta->wl_rx_rate == BTC_CCK_5_5 ||
		    coex_sta->wl_rx_rate == BTC_OFDM_6 ||
		    coex_sta->wl_rx_rate == BTC_MCS_0) {
			coex_sta->cck_lock_warn = TRUE;

			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], cck lock warning...\n");
			BTC_TRACE(trace_buf);
		} else if ((coex_sta->wl_rx_rate == BTC_CCK_1) ||
			   (coex_sta->wl_rx_rate == BTC_CCK_2) ||
			   (coex_sta->wl_rts_rx_rate == BTC_CCK_1) ||
			   (coex_sta->wl_rts_rx_rate == BTC_CCK_2)) {
			coex_sta->cck_lock = TRUE;

			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], cck locking...\n");
			BTC_TRACE(trace_buf);
		} else {
			coex_sta->cck_lock_warn = FALSE;
			coex_sta->cck_lock = FALSE;

			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], cck unlock...\n");
			BTC_TRACE(trace_buf);
		}
	} else {
		if (coex_dm->bt_status ==
		     BT_8822B_1ANT_BSTATUS_CON_IDLE ||
		    coex_dm->bt_status ==
		     BT_8822B_1ANT_BSTATUS_NCON_IDLE) {
			coex_sta->cck_lock_warn = FALSE;
			coex_sta->cck_lock = FALSE;
		}
	}
}

void ex_halbtc8822b1ant_tx_rate_change_notify(struct btc_coexist *btc,
					      u8 tx_rate, u8 tx_retry_ratio,
					      u8 macid)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;

	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
		    "[BTCoex], tx_rate_change_notify Tx_Rate = %d, Tx_Retry_Ratio = %d, macid =%d\n",
		    tx_rate, tx_retry_ratio, macid);
	BTC_TRACE(trace_buf);

	coex_sta->wl_tx_rate = tx_rate;
	coex_sta->wl_tx_retry_ratio = tx_retry_ratio;
	coex_sta->wl_tx_macid = macid;
}

void ex_halbtc8822b1ant_rf_status_notify(struct btc_coexist *btc, u8 type)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;

	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE, "[BTCoex], RF Status notify\n");
	BTC_TRACE(trace_buf);

	if (type == BTC_RF_ON) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], RF is turned ON!!\n");
		BTC_TRACE(trace_buf);
		btc->stop_coex_dm = FALSE;
		coex_sta->is_rf_state_off = FALSE;

	} else if (type == BTC_RF_OFF) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], RF is turned OFF!!\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_write_scbd(btc, BT_8822B_1ANT_SCBD_ACTIVE |
					   BT_8822B_1ANT_SCBD_ONOFF |
					   BT_8822B_1ANT_SCBD_SCAN |
					   BT_8822B_1ANT_SCBD_UNDERTEST,
					   FALSE);

		halbtc8822b1ant_tdma(btc, FC_EXCU, FALSE, 0);

		halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, FC_EXCU,
					     BT_8822B_1ANT_PHASE_WOFF);
		/* for test : s3 bt disppear , fail rate 1/600*/
		halbtc8822b1ant_ignore_wlan_act(btc, FC_EXCU, TRUE);

		btc->stop_coex_dm = TRUE;
		coex_sta->is_rf_state_off = TRUE;

		/* must place in the last step */
		halbtc8822b1ant_update_wifi_ch_info(btc, BTC_MEDIA_DISCONNECT);
	}
}

void ex_halbtc8822b1ant_halt_notify(struct btc_coexist *btc)
{
	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE, "[BTCoex], Halt notify\n");
	BTC_TRACE(trace_buf);

	ex_halbtc8822b1ant_media_status_notify(btc, BTC_MEDIA_DISCONNECT);

	halbtc8822b1ant_ignore_wlan_act(btc, FC_EXCU, TRUE);

	halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, FC_EXCU,
				     BT_8822B_1ANT_PHASE_WOFF);

	halbtc8822b1ant_write_scbd(btc, BT_8822B_1ANT_SCBD_ACTIVE |
				   BT_8822B_1ANT_SCBD_ONOFF |
				   BT_8822B_1ANT_SCBD_SCAN |
				   BT_8822B_1ANT_SCBD_UNDERTEST |
				   BT_8822B_1ANT_SCBD_RXGAIN,
				   FALSE);

	btc->stop_coex_dm = TRUE;

	/* must place in the last step */
	halbtc8822b1ant_update_wifi_ch_info(btc, BTC_MEDIA_DISCONNECT);
}

void ex_halbtc8822b1ant_pnp_notify(struct btc_coexist *btc, u8 pnp_state)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	struct wifi_link_info_8822b_1ant *wifi_link_info_ext =
					 &btc->wifi_link_info_8822b_1ant;
	static u8 pre_pnp_state;
	u8 phase;

	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE, "[BTCoex], Pnp notify\n");
	BTC_TRACE(trace_buf);

	if (pnp_state == BTC_WIFI_PNP_SLEEP ||
	    pnp_state == BTC_WIFI_PNP_SLEEP_KEEP_ANT) {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Pnp notify to SLEEP\n");
		BTC_TRACE(trace_buf);

		halbtc8822b1ant_write_scbd(btc, BT_8822B_1ANT_SCBD_ACTIVE |
					   BT_8822B_1ANT_SCBD_ONOFF |
					   BT_8822B_1ANT_SCBD_SCAN |
					   BT_8822B_1ANT_SCBD_UNDERTEST |
					   BT_8822B_1ANT_SCBD_RXGAIN,
					   FALSE);

		if (pnp_state == BTC_WIFI_PNP_SLEEP_KEEP_ANT) {
			if (wifi_link_info_ext->is_all_under_5g)
				phase = BT_8822B_1ANT_PHASE_5G;
			else
				phase = BT_8822B_1ANT_PHASE_2G;
		} else {
			phase = BT_8822B_1ANT_PHASE_WOFF;
		}
		halbtc8822b1ant_set_ant_path(btc, BTC_ANT_PATH_AUTO, FC_EXCU,
					     phase);

		btc->stop_coex_dm = TRUE;
	} else {
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Pnp notify to WAKE UP\n");
		BTC_TRACE(trace_buf);
		coex_sta->wl_pnp_wakeup_downcnt = 3;

		/*WoWLAN*/
		if (pre_pnp_state == BTC_WIFI_PNP_SLEEP_KEEP_ANT ||
		    pnp_state == BTC_WIFI_PNP_WOWLAN) {
			coex_sta->run_time_state = TRUE;
			btc->stop_coex_dm = FALSE;
			halbtc8822b1ant_run_coex(btc, BT_8822B_1ANT_RSN_PNP);
		}
	}

	pre_pnp_state = pnp_state;
}

void ex_halbtc8822b1ant_coex_dm_reset(struct btc_coexist *btc)
{
	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
		    "[BTCoex], *****************Coex DM Reset*****************\n");
	BTC_TRACE(trace_buf);

	halbtc8822b1ant_init_hw_config(btc, FALSE, FALSE);
	halbtc8822b1ant_init_coex_dm(btc);
}

void ex_halbtc8822b1ant_periodical(struct btc_coexist *btc)
{
	struct coex_sta_8822b_1ant *coex_sta = &btc->coex_sta_8822b_1ant;
	boolean bt_relink_finish = FALSE;

	BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
		    "[BTCoex], ==========================Periodical===========================\n");
	BTC_TRACE(trace_buf);

	if (!btc->auto_report)
		halbtc8822b1ant_query_bt_info(btc);

	halbtc8822b1ant_monitor_bt_ctr(btc);
	halbtc8822b1ant_monitor_wifi_ctr(btc);
	halbtc8822b1ant_update_wifi_link_info(btc,
					      BT_8822B_1ANT_RSN_PERIODICAL);
	halbtc8822b1ant_monitor_bt_enable(btc);

	if (coex_sta->bt_relink_downcount != 0) {
		coex_sta->bt_relink_downcount--;
		if (coex_sta->bt_relink_downcount == 0) {
			coex_sta->is_setup_link = FALSE;
			bt_relink_finish = TRUE;
		}
	}

	/* for 4-way, DHCP, EAPOL packet */
	if (coex_sta->specific_pkt_period_cnt > 0) {
		coex_sta->specific_pkt_period_cnt--;
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], Hi-Pri Task1 = %s\n",
			    (coex_sta->wifi_high_pri_task1 ? "Yes" : "No"));
		BTC_TRACE(trace_buf);
	}

	/*for A2DP glitch during connecting AP*/
	if (coex_sta->connect_ap_period_cnt > 0)
		coex_sta->connect_ap_period_cnt--;

	if (coex_sta->wl_pnp_wakeup_downcnt > 0) {
		coex_sta->wl_pnp_wakeup_downcnt--;
		BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
			    "[BTCoex], wl_pnp_wakeup_downcnt = %d!!\n",
			    coex_sta->wl_pnp_wakeup_downcnt);
		BTC_TRACE(trace_buf);
	}

	if (coex_sta->cnt_bt_reenable > 0) {
		coex_sta->cnt_bt_reenable--;
		if (coex_sta->cnt_bt_reenable == 0) {
			coex_sta->is_bt_reenable = FALSE;
			BTC_SPRINTF(trace_buf, BT_TMP_BUF_SIZE,
				    "[BTCoex], BT renable 30s finish!!\n");
			BTC_TRACE(trace_buf);
		}
	}

	if (halbtc8822b1ant_moniter_wifibt_status(btc) ||
	    bt_relink_finish || coex_sta->is_set_ps_state_fail)
		halbtc8822b1ant_run_coex(btc, BT_8822B_1ANT_RSN_PERIODICAL);
}

void ex_halbtc8822b1ant_dbg_control(struct btc_coexist *btc, u8 op_code,
				    u8 op_len, u8 *pdata)
{
}
#endif /*  #if(BTC_COEX_OFFLOAD == 1) */

#endif

#else

void ex_halbtc8822b1ant_switch_band_without_bt(struct btc_coexist *btc,
					       boolean wifi_only_5g)
{
	/* ant switch WL2G or WL5G*/
	if (wifi_only_5g)
		btc->btc_write_1byte_bitmask(btc, 0xcbd, 0x3, 1);
	else
		btc->btc_write_1byte_bitmask(btc, 0xcbd, 0x3, 2);
}

#endif /* #if (BT_SUPPORT == 1 && COEX_SUPPORT == 1) */
