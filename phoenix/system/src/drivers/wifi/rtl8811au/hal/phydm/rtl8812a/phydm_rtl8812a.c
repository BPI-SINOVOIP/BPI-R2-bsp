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

/* ************************************************************
 * include files
 * ************************************************************ */

#include "mp_precomp.h"

#include "../phydm_precomp.h"

#if (RTL8812A_SUPPORT == 1)

#if (defined(CONFIG_PATH_DIVERSITY))
void
odm_update_tx_path_8812a(struct PHY_DM_STRUCT *p_dm_odm, u8 path)
{
	struct _ODM_PATH_DIVERSITY_	*p_dm_path_div = &p_dm_odm->dm_path_div;

	if (p_dm_path_div->resp_tx_path != path) {
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("Need to Update Tx path\n"));

		if (path == ODM_RF_PATH_A) {
			odm_set_bb_reg(p_dm_odm, 0x80c, 0xFFF0, 0x111); /* Tx by Reg */
			odm_set_bb_reg(p_dm_odm, 0x6d8, BIT(7) | BIT6, 1); /* Resp Tx by Txinfo */
		} else {
			odm_set_bb_reg(p_dm_odm, 0x80c, 0xFFF0, 0x222); /* Tx by Reg */
			odm_set_bb_reg(p_dm_odm, 0x6d8, BIT(7) | BIT6, 2); /* Resp Tx by Txinfo */
		}
	}
	p_dm_path_div->resp_tx_path = path;
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("path=%s\n", (path == ODM_RF_PATH_A) ? "ODM_RF_PATH_A" : "ODM_RF_PATH_B"));
}


void
odm_path_diversity_init_8812a(
	struct PHY_DM_STRUCT	*p_dm_odm
)
{
	u32	i;
	struct _ODM_PATH_DIVERSITY_	*p_dm_path_div = &p_dm_odm->dm_path_div;

	odm_set_bb_reg(p_dm_odm, 0x80c, BIT(29), 1); /* Tx path from Reg */
	odm_set_bb_reg(p_dm_odm, 0x80c, 0xFFF0, 0x111); /* Tx by Reg */
	odm_set_bb_reg(p_dm_odm, 0x6d8, BIT(7) | BIT6, 1); /* Resp Tx by Txinfo */
	odm_update_tx_path_8812a(p_dm_odm, ODM_RF_PATH_A);

	for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) {
		p_dm_path_div->path_sel[i] = 1; /* TxInfo default at path-A */
	}
}



void
odm_path_diversity_8812a(
	struct PHY_DM_STRUCT	*p_dm_odm
)
{
	u32	i, rssi_avg_a = 0, rssi_avg_b = 0, local_min_rssi, min_rssi = 0xFF;
	u8	tx_resp_path = 0, target_path;
	struct _ODM_PATH_DIVERSITY_	*p_dm_path_div = &p_dm_odm->dm_path_div;
	struct sta_info	*p_entry;


	ODM_RT_TRACE(p_dm_odm, ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("Odm_PathDiversity_8812A() =>\n"));

	for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) {
		p_entry = p_dm_odm->p_odm_sta_info[i];
		if (IS_STA_VALID(p_entry)) {
			/* 2 Caculate RSSI per path */
			rssi_avg_a = (p_dm_path_div->path_a_cnt[i] != 0) ? (p_dm_path_div->path_a_sum[i] / p_dm_path_div->path_a_cnt[i]) : 0;
			rssi_avg_b = (p_dm_path_div->path_b_cnt[i] != 0) ? (p_dm_path_div->path_b_sum[i] / p_dm_path_div->path_b_cnt[i]) : 0;
			target_path = (rssi_avg_a == rssi_avg_b) ? p_dm_path_div->resp_tx_path : ((rssi_avg_a >= rssi_avg_b) ? ODM_RF_PATH_A : ODM_RF_PATH_B);
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("mac_id=%d, path_a_sum=%d, path_a_cnt=%d\n", i, p_dm_path_div->path_a_sum[i], p_dm_path_div->path_a_cnt[i]));
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("mac_id=%d, path_b_sum=%d, path_b_cnt=%d\n", i, p_dm_path_div->path_b_sum[i], p_dm_path_div->path_b_cnt[i]));
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("mac_id=%d, rssi_avg_a= %d, rssi_avg_b= %d\n", i, rssi_avg_a, rssi_avg_b));

			/* 2 Select Resp Tx path */
			local_min_rssi = (rssi_avg_a > rssi_avg_b) ? rssi_avg_b : rssi_avg_a;
			if (local_min_rssi < min_rssi) {
				min_rssi = local_min_rssi;
				tx_resp_path = target_path;
			}

			/* 2 Select Tx DESC */
			if (target_path == ODM_RF_PATH_A)
				p_dm_path_div->path_sel[i] = 1;
			else
				p_dm_path_div->path_sel[i] = 2;

			ODM_RT_TRACE(p_dm_odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("Tx from TxInfo, target_path=%s\n",
				(target_path == ODM_RF_PATH_A) ? "ODM_RF_PATH_A" : "ODM_RF_PATH_B"));
			ODM_RT_TRACE(p_dm_odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("p_dm_path_div->path_sel[%d] = %d\n", i, p_dm_path_div->path_sel[i]));

		}
		p_dm_path_div->path_a_cnt[i] = 0;
		p_dm_path_div->path_a_sum[i] = 0;
		p_dm_path_div->path_b_cnt[i] = 0;
		p_dm_path_div->path_b_sum[i] = 0;
	}

	/* 2 Update Tx path */
	odm_update_tx_path_8812a(p_dm_odm, tx_resp_path);

}


#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
void
odm_set_tx_path_by_tx_info_8812a(
	struct PHY_DM_STRUCT		*p_dm_odm,
	u8			*p_desc,
	u8			mac_id
)
{
	struct _ODM_PATH_DIVERSITY_	*p_dm_path_div = &p_dm_odm->dm_path_div;

	if ((p_dm_odm->support_ic_type != ODM_RTL8812) || (!(p_dm_odm->support_ability & ODM_BB_PATH_DIV)))
		return;

	SET_TX_DESC_TX_ANT_8812(p_desc, p_dm_path_div->path_sel[mac_id]);
}
#else/* (DM_ODM_SUPPORT_TYPE == ODM_AP) */
void
odm_set_tx_path_by_tx_info_8812a(
	struct PHY_DM_STRUCT		*p_dm_odm
)
{

}
#endif

#endif /* CONFIG_PATH_DIVERSITY */
#endif /* #if (RTL8812A_SUPPORT == 1) */
