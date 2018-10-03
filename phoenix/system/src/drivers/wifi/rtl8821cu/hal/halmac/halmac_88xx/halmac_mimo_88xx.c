/******************************************************************************
 *
 * Copyright(c) 2016 - 2017 Realtek Corporation. All rights reserved.
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
 ******************************************************************************/

#include "halmac_mimo_88xx.h"
#include "halmac_88xx_cfg.h"
#include "halmac_common_88xx.h"
#include "halmac_init_88xx.h"

#if HALMAC_88XX_SUPPORT

static void
halmac_cfg_mu_bfee_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_cfg_mumimo_para *param
);

static void
halmac_cfg_mu_bfer_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_cfg_mumimo_para *param
);

static enum halmac_fw_snding_cmd_construct_state
halmac_query_fw_snding_curr_state_88xx(
	IN struct halmac_adapter *adapter
);

static enum halmac_ret_status
halmac_transition_fw_snding_state_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_fw_snding_cmd_construct_state dest_state
);

static u8
halmac_snding_pkt_chk_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *pkt
);

/**
 * halmac_cfg_txbf_88xx() - enable/disable specific user's txbf
 * @adapter : the adapter of halmac
 * @userid : su bfee userid = 0 or 1 to apply TXBF
 * @bw : the sounding bandwidth
 * @txbf_en : 0: disable TXBF, 1: enable TXBF
 * Author : chunchu
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_cfg_txbf_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 userid,
	IN enum halmac_bw bw,
	IN u8 txbf_en
)
{
	u16 tmp42c = 0;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (txbf_en) {
		switch (bw) {
		case HALMAC_BW_80:
			tmp42c |= BIT_R_TXBF0_80M;
		case HALMAC_BW_40:
			tmp42c |= BIT_R_TXBF0_40M;
		case HALMAC_BW_20:
			tmp42c |= BIT_R_TXBF0_20M;
			break;
		default:
			return HALMAC_RET_INVALID_SOUNDING_SETTING;
		}
	}

	switch (userid) {
	case 0:
		tmp42c |= HALMAC_REG_R16(REG_TXBF_CTRL) &
			~(BIT_R_TXBF0_20M | BIT_R_TXBF0_40M | BIT_R_TXBF0_80M);
		HALMAC_REG_W16(REG_TXBF_CTRL, tmp42c);
		break;
	case 1:
		tmp42c |= HALMAC_REG_R16(REG_TXBF_CTRL + 2) &
			~(BIT_R_TXBF0_20M | BIT_R_TXBF0_40M | BIT_R_TXBF0_80M);
		HALMAC_REG_W16(REG_TXBF_CTRL + 2, tmp42c);
		break;
	default:
		return HALMAC_RET_INVALID_SOUNDING_SETTING;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_cfg_mumimo_88xx() -config mumimo
 * @adapter : the adapter of halmac
 * @param : parameters to configure MU PPDU Tx/Rx
 * Author : chunchu
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_cfg_mumimo_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_cfg_mumimo_para *param
)
{
	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (param->role == HAL_BFEE)
		halmac_cfg_mu_bfee_88xx(adapter, param);
	else
		halmac_cfg_mu_bfer_88xx(adapter, param);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

static void
halmac_cfg_mu_bfee_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_cfg_mumimo_para *param
)
{
	u8 mu_tbl_sel;
	u8 tmp14c0;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	tmp14c0 = HALMAC_REG_R8(REG_MU_TX_CTL) & ~BIT_MASK_R_MU_TABLE_VALID;
	HALMAC_REG_W8(REG_MU_TX_CTL, (tmp14c0 | BIT(0) | BIT(1)) & ~(BIT(7)));

	/*config GID valid table and user position table*/
	mu_tbl_sel = HALMAC_REG_R8(REG_MU_TX_CTL + 1) &
						~(BIT(0) | BIT(1) | BIT(2));

	HALMAC_REG_W8(REG_MU_TX_CTL + 1, mu_tbl_sel);
	HALMAC_REG_W32(REG_MU_STA_GID_VLD, param->given_gid_tab[0]);
	HALMAC_REG_W32(REG_MU_STA_USER_POS_INFO, param->given_user_pos[0]);
	HALMAC_REG_W32(REG_MU_STA_USER_POS_INFO + 4, param->given_user_pos[1]);

	HALMAC_REG_W8(REG_MU_TX_CTL + 1, mu_tbl_sel | 1);
	HALMAC_REG_W32(REG_MU_STA_GID_VLD, param->given_gid_tab[1]);
	HALMAC_REG_W32(REG_MU_STA_USER_POS_INFO, param->given_user_pos[2]);
	HALMAC_REG_W32(REG_MU_STA_USER_POS_INFO + 4, param->given_user_pos[3]);
}

static void
halmac_cfg_mu_bfer_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_cfg_mumimo_para *param
)
{
	u8 i;
	u8 idx;
	u8 id0;
	u8 id1;
	u8 gid;
	u8 mu_tbl_sel;
	u8 mu_tbl_valid = 0;
	u32 gid_valid[6] = {0};
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (param->mu_tx_en == _FALSE) {
		HALMAC_REG_W8(REG_MU_TX_CTL,
			      HALMAC_REG_R8(REG_MU_TX_CTL) & ~(BIT(7)));
		return;
	}

	for (idx = 0; idx < 15; idx++) {
		if (idx < 5) {
			/*grouping_bitmap bit0~4, MU_STA0 with MUSTA1~5*/
			id0 = 0;
			id1 = (u8)(idx + 1);
		} else if (idx < 9) {
			/*grouping_bitmap bit5~8, MU_STA1 with MUSTA2~5*/
			id0 = 1;
			id1 = (u8)(idx - 3);
		} else if (idx < 12) {
			/*grouping_bitmap bit9~11, MU_STA2 with MUSTA3~5*/
			id0 = 2;
			id1 = (u8)(idx - 6);
		} else if (idx < 14) {
			/*grouping_bitmap bit12~13, MU_STA3 with MUSTA4~5*/
			id0 = 3;
			id1 = (u8)(idx - 8);
		} else {
			/*grouping_bitmap bit14, MU_STA4 with MUSTA5*/
			id0 = 4;
			id1 = (u8)(idx - 9);
		}
		if (param->grouping_bitmap & BIT(idx)) {
			/*Pair 1*/
			gid = (idx << 1) + 1;
			gid_valid[id0] |= (BIT(gid));
			gid_valid[id1] |= (BIT(gid));
			/*Pair 2*/
			gid += 1;
			gid_valid[id0] |= (BIT(gid));
			gid_valid[id1] |= (BIT(gid));
		} else {
			/*Pair 1*/
			gid = (idx << 1) + 1;
			gid_valid[id0] &= ~(BIT(gid));
			gid_valid[id1] &= ~(BIT(gid));
			/*Pair 2*/
			gid += 1;
			gid_valid[id0] &= ~(BIT(gid));
			gid_valid[id1] &= ~(BIT(gid));
		}
	}

	/*set MU STA GID valid TABLE*/
	mu_tbl_sel = HALMAC_REG_R8(REG_MU_TX_CTL + 1) &
						~(BIT(0) | BIT(1) | BIT(2));
	for (idx = 0; idx < 6; idx++) {
		HALMAC_REG_W8(REG_MU_TX_CTL + 1, idx | mu_tbl_sel);
		HALMAC_REG_W32(REG_MU_STA_GID_VLD, gid_valid[idx]);
	}

	/*To validate the sounding successful MU STA and enable MU TX*/
	for (i = 0; i < 6; i++) {
		if (param->sounding_sts[i] == _TRUE)
			mu_tbl_valid |= BIT(i);
	}
	HALMAC_REG_W8(REG_MU_TX_CTL, mu_tbl_valid | BIT(7));
}

/**
 * halmac_cfg_sounding_88xx() - configure general sounding
 * @adapter : the adapter of halmac
 * @role : driver's role, BFer or BFee
 * @rate : set ndpa tx rate if driver is BFer, or set csi response rate if driver is BFee
 * Author : chunchu
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_cfg_sounding_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_snd_role role,
	IN enum halmac_data_rate rate
)
{
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	switch (role) {
	case HAL_BFER:
		HALMAC_REG_W32(REG_TXBF_CTRL, HALMAC_REG_R32(REG_TXBF_CTRL) |
			       BIT_R_ENABLE_NDPA | BIT_USE_NDPA_PARAMETER |
			       BIT_R_EN_NDPA_INT | BIT_DIS_NDP_BFEN);
		HALMAC_REG_W8(REG_NDPA_RATE, rate);
		HALMAC_REG_W8(REG_NDPA_OPT_CTRL,
			      HALMAC_REG_R8(REG_NDPA_OPT_CTRL) &
					    ~(BIT(0) | BIT(1)));
		HALMAC_REG_W8(REG_SND_PTCL_CTRL + 1, 0x2 | BIT(7));
		HALMAC_REG_W8(REG_SND_PTCL_CTRL + 2, 0x2);
		break;
	case HAL_BFEE:
		HALMAC_REG_W8(REG_SND_PTCL_CTRL, 0xDB);
		HALMAC_REG_W8(REG_SND_PTCL_CTRL + 3, 0x26);
		HALMAC_REG_W8(REG_BBPSF_CTRL + 3, HALMAC_OFDM54 | BIT(6));
		HALMAC_REG_W16(REG_RRSR, HALMAC_REG_R16(REG_RRSR) | BIT(rate));
		HALMAC_REG_W8(REG_RXFLTMAP1,
			      HALMAC_REG_R8(REG_RXFLTMAP1) & ~BIT(4));
		HALMAC_REG_W8(REG_RXFLTMAP4,
			      HALMAC_REG_R8(REG_RXFLTMAP4) & ~BIT(4));
		break;
	default:
		return HALMAC_RET_INVALID_SOUNDING_SETTING;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_del_sounding_88xx() - reset general sounding
 * @adapter : the adapter of halmac
 * @role : driver's role, BFer or BFee
 * Author : chunchu
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_del_sounding_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_snd_role role
)
{
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	switch (role) {
	case HAL_BFER:
		HALMAC_REG_W8(REG_TXBF_CTRL + 3, 0);
		break;
	case HAL_BFEE:
		HALMAC_REG_W8(REG_SND_PTCL_CTRL, 0);
		break;
	default:
		return HALMAC_RET_INVALID_SOUNDING_SETTING;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_su_bfee_entry_init_88xx() - config SU beamformee's registers
 * @adapter : the adapter of halmac
 * @userid : SU bfee userid = 0 or 1 to be added
 * @paid : partial AID of this bfee
 * Author : chunchu
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_su_bfee_entry_init_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 userid,
	IN u16 paid
)
{
	u16 tmp42c = 0;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	switch (userid) {
	case 0:
		tmp42c = HALMAC_REG_R16(REG_TXBF_CTRL) &
				~(BIT_MASK_R_TXBF0_AID | BIT_R_TXBF0_20M |
				BIT_R_TXBF0_40M | BIT_R_TXBF0_80M);
		HALMAC_REG_W16(REG_TXBF_CTRL, tmp42c | paid);
		HALMAC_REG_W16(REG_ASSOCIATED_BFMEE_SEL, paid);
		break;
	case 1:
		tmp42c = HALMAC_REG_R16(REG_TXBF_CTRL + 2) &
				~(BIT_MASK_R_TXBF1_AID | BIT_R_TXBF0_20M |
				BIT_R_TXBF0_40M | BIT_R_TXBF0_80M);
		HALMAC_REG_W16(REG_TXBF_CTRL + 2, tmp42c | paid);
		HALMAC_REG_W16(REG_ASSOCIATED_BFMEE_SEL + 2, paid | BIT(9));
		break;
	default:
		return HALMAC_RET_INVALID_SOUNDING_SETTING;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_su_bfee_entry_init_88xx() - config SU beamformer's registers
 * @adapter : the adapter of halmac
 * @param : parameters to configure SU BFER entry
 * Author : chunchu
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_su_bfer_entry_init_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_su_bfer_init_para *param
)
{
	u16 mac_addr_h;
	u32 mac_addr_l;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	mac_addr_l = rtk_le32_to_cpu(
				param->bfer_address.Address_L_H.Address_Low);
	mac_addr_h = rtk_le16_to_cpu(
				param->bfer_address.Address_L_H.Address_High);

	switch (param->userid) {
	case 0:
		HALMAC_REG_W32(REG_ASSOCIATED_BFMER0_INFO, mac_addr_l);
		HALMAC_REG_W16(REG_ASSOCIATED_BFMER0_INFO + 4, mac_addr_h);
		HALMAC_REG_W16(REG_ASSOCIATED_BFMER0_INFO + 6, param->paid);
		HALMAC_REG_W16(REG_TX_CSI_RPT_PARAM_BW20, param->csi_para);
		break;
	case 1:
		HALMAC_REG_W32(REG_ASSOCIATED_BFMER1_INFO, mac_addr_l);
		HALMAC_REG_W16(REG_ASSOCIATED_BFMER1_INFO + 4, mac_addr_h);
		HALMAC_REG_W16(REG_ASSOCIATED_BFMER1_INFO + 6, param->paid);
		HALMAC_REG_W16(REG_TX_CSI_RPT_PARAM_BW20 + 2, param->csi_para);
		break;
	default:
		return HALMAC_RET_INVALID_SOUNDING_SETTING;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_mu_bfee_entry_init_88xx() - config MU beamformee's registers
 * @adapter : the adapter of halmac
 * @param : parameters to configure MU BFEE entry
 * Author : chunchu
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_mu_bfee_entry_init_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_mu_bfee_init_para *param
)
{
	u16 tmp168x = 0;
	u16 tmp14c0;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	tmp168x |= param->paid | BIT(9);
	HALMAC_REG_W16((0x1680 + param->userid * 2), tmp168x);

	tmp14c0 = HALMAC_REG_R16(REG_MU_TX_CTL) & ~(BIT(8) | BIT(9) | BIT(10));
	HALMAC_REG_W16(REG_MU_TX_CTL, tmp14c0 | ((param->userid - 2) << 8));
	HALMAC_REG_W32(REG_MU_STA_GID_VLD, 0);
	HALMAC_REG_W32(REG_MU_STA_USER_POS_INFO, param->user_position_l);
	HALMAC_REG_W32(REG_MU_STA_USER_POS_INFO + 4, param->user_position_h);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_mu_bfer_entry_init_88xx() - config MU beamformer's registers
 * @adapter : the adapter of halmac
 * @param : parameters to configure MU BFER entry
 * Author : chunchu
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_mu_bfer_entry_init_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_mu_bfer_init_para *param
)
{
	u16 tmp1680 = 0;
	u16 mac_addr_h;
	u32 mac_addr_l;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	mac_addr_l = rtk_le32_to_cpu(
				param->bfer_address.Address_L_H.Address_Low);
	mac_addr_h = rtk_le16_to_cpu(
				param->bfer_address.Address_L_H.Address_High);

	HALMAC_REG_W32(REG_ASSOCIATED_BFMER0_INFO, mac_addr_l);
	HALMAC_REG_W16(REG_ASSOCIATED_BFMER0_INFO + 4, mac_addr_h);
	HALMAC_REG_W16(REG_ASSOCIATED_BFMER0_INFO + 6, param->paid);
	HALMAC_REG_W16(REG_TX_CSI_RPT_PARAM_BW20, param->csi_para);

	tmp1680 = HALMAC_REG_R16(0x1680) & 0xC000;
	tmp1680 |= param->my_aid | (param->csi_length_sel << 12);
	HALMAC_REG_W16(0x1680, tmp1680);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_su_bfee_entry_del_88xx() - reset SU beamformee's registers
 * @adapter : the adapter of halmac
 * @userid : the SU BFee userid to be deleted
 * Author : chunchu
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_su_bfee_entry_del_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 userid
)
{
	u16 value16;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	switch (userid) {
	case 0:
		value16 = HALMAC_REG_R16(REG_TXBF_CTRL);
		value16 &= ~(BIT_MASK_R_TXBF0_AID | BIT_R_TXBF0_20M |
					BIT_R_TXBF0_40M | BIT_R_TXBF0_80M);
		HALMAC_REG_W16(REG_TXBF_CTRL, value16);
		HALMAC_REG_W16(REG_ASSOCIATED_BFMEE_SEL, 0);
		break;
	case 1:
		value16 = HALMAC_REG_R16(REG_TXBF_CTRL + 2);
		value16 &= ~(BIT_MASK_R_TXBF1_AID | BIT_R_TXBF0_20M |
					BIT_R_TXBF0_40M | BIT_R_TXBF0_80M);
		HALMAC_REG_W16(REG_TXBF_CTRL + 2, value16);
		HALMAC_REG_W16(REG_ASSOCIATED_BFMEE_SEL + 2, 0);
		break;
	default:
		return HALMAC_RET_INVALID_SOUNDING_SETTING;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_su_bfee_entry_del_88xx() - reset SU beamformer's registers
 * @adapter : the adapter of halmac
 * @userid : the SU BFer userid to be deleted
 * Author : chunchu
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_su_bfer_entry_del_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 userid
)
{
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	switch (userid) {
	case 0:
		HALMAC_REG_W32(REG_ASSOCIATED_BFMER0_INFO, 0);
		HALMAC_REG_W32(REG_ASSOCIATED_BFMER0_INFO + 4, 0);
		break;
	case 1:
		HALMAC_REG_W32(REG_ASSOCIATED_BFMER1_INFO, 0);
		HALMAC_REG_W32(REG_ASSOCIATED_BFMER1_INFO + 4, 0);
		break;
	default:
		return HALMAC_RET_INVALID_SOUNDING_SETTING;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_mu_bfee_entry_del_88xx() - reset MU beamformee's registers
 * @adapter : the adapter of halmac
 * @userid : the MU STA userid to be deleted
 * Author : chunchu
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_mu_bfee_entry_del_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 userid
)
{
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	HALMAC_REG_W16(0x1680 + userid * 2, 0);
	HALMAC_REG_W8(REG_MU_TX_CTL,
		      HALMAC_REG_R8(REG_MU_TX_CTL) & ~(BIT(userid - 2)));

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_mu_bfer_entry_del_88xx() -reset MU beamformer's registers
 * @adapter : the adapter of halmac
 * Author : chunchu
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_mu_bfer_entry_del_88xx(
	IN struct halmac_adapter *adapter
)
{
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	HALMAC_REG_W32(REG_ASSOCIATED_BFMER0_INFO, 0);
	HALMAC_REG_W32(REG_ASSOCIATED_BFMER0_INFO + 4, 0);
	HALMAC_REG_W16(0x1680, 0);
	HALMAC_REG_W8(REG_MU_TX_CTL, 0);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_cfg_csi_rate_88xx() - config CSI frame Tx rate
 * @adapter : the adapter of halmac
 * @rssi : rssi in decimal value
 * @cur_rate : current CSI frame rate
 * @fixrate_en : enable to fix CSI frame in VHT rate, otherwise legacy OFDM rate
 * @new_rate : API returns the final CSI frame rate
 * Author : chunchu
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_cfg_csi_rate_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 rssi,
	IN u8 cur_rate,
	IN u8 fixrate_en,
	OUT u8 *new_rate
)
{
	u32 csi_cfg;
	u16 cur_rrsr;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

#if HALMAC_8821C_SUPPORT
	if (adapter->chip_id == HALMAC_CHIP_ID_8821C) {
		if (fixrate_en) {
			csi_cfg = HALMAC_REG_R32(REG_BBPSF_CTRL) &
							~BITS_WMAC_CSI_RATE;
			HALMAC_REG_W32(REG_BBPSF_CTRL,
				       csi_cfg | BIT_CSI_FORCE_RATE_EN |
				       BIT_CSI_RSC(1) |
				       BIT_WMAC_CSI_RATE(HALMAC_VHT_NSS1_MCS3));
			*new_rate = HALMAC_VHT_NSS1_MCS3;
			return HALMAC_RET_SUCCESS;
		}
	}
	csi_cfg = HALMAC_REG_R32(REG_BBPSF_CTRL) & ~BITS_WMAC_CSI_RATE &
							~BIT_CSI_FORCE_RATE_EN;
#else
	csi_cfg = HALMAC_REG_R32(REG_BBPSF_CTRL) & ~BITS_WMAC_CSI_RATE;
#endif

	cur_rrsr = HALMAC_REG_R16(REG_RRSR);

	if (rssi >= 40) {
		if (cur_rate != HALMAC_OFDM54) {
			HALMAC_REG_W16(REG_RRSR, cur_rrsr | BIT(HALMAC_OFDM54));
			HALMAC_REG_W32(REG_BBPSF_CTRL, csi_cfg |
				       BIT_WMAC_CSI_RATE(HALMAC_OFDM54));
		}
		*new_rate = HALMAC_OFDM54;
	} else {
		if (cur_rate != HALMAC_OFDM24) {
			HALMAC_REG_W16(REG_RRSR, cur_rrsr &
				       ~(BIT(HALMAC_OFDM54)));
			HALMAC_REG_W32(REG_BBPSF_CTRL, csi_cfg |
				       BIT_WMAC_CSI_RATE(HALMAC_OFDM24));
		}
		*new_rate = HALMAC_OFDM24;
	}

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_fw_snding_88xx() - fw sounding control
 * @adapter : the adapter of halmac
 * @su_info :
 *	su0_en : enable/disable fw sounding
 *	pSu0_ndpa_pkt : ndpa pkt, shall include txdesc
 *	su0_pkt_sz : ndpa pkt size, shall include txdesc
 * @mu_info : currently not in use, input NULL is acceptable
 * @period : sounding period, unit is 5ms
 * Author : Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_fw_snding_88xx(
	IN struct halmac_adapter *adapter,
	IN struct halmac_su_snding_info *su_info,
	IN struct halmac_mu_snding_info *mu_info,
	IN u8 period
)
{
	u8 h2c_buf[HALMAC_H2C_CMD_SIZE_88XX] = { 0 };
	u16 seq_num;
	struct halmac_h2c_header_info hdr_info;
	enum halmac_cmd_process_status *proc_status =
			&adapter->halmac_state.fw_snding_set.process_status;
	enum halmac_ret_status status;

	if (adapter->chip_id == HALMAC_CHIP_ID_8821C)
		return HALMAC_RET_NOT_SUPPORT;

	if (halmac_fw_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_NO_DLFW;

	if (adapter->fw_version.h2c_version < 9)
		return HALMAC_RET_FW_NO_SUPPORT;

	if (*proc_status == HALMAC_CMD_PROCESS_SENDING) {
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Wait event(snd)\n");
		return HALMAC_RET_BUSY_STATE;
	}

	if (su_info->su0_en == 1) {
		if (!su_info->pSu0_ndpa_pkt)
			return HALMAC_RET_NULL_POINTER;

		if (su_info->su0_pkt_sz > (u32)HALMAC_TX_PAGE_SIZE_88XX -
		    adapter->hw_cfg_info.txdesc_size)
			return HALMAC_RET_DATA_SIZE_INCORRECT;

		if (halmac_snding_pkt_chk_88xx(
				adapter, su_info->pSu0_ndpa_pkt) == _FALSE)
			return HALMAC_RET_TXDESC_SET_FAIL;

		if (halmac_query_fw_snding_curr_state_88xx(adapter) !=
					HALMAC_FW_SNDING_CMD_CONSTRUCT_IDLE) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Not idle(snd)\n");
			return HALMAC_RET_ERROR_STATE;
		}

		status = halmac_download_rsvd_page_88xx(
			adapter,
			adapter->txff_allocation.rsvd_h2c_static_info_pg_bndy +
			HALMAC_SU0_SNDING_PKT_OFFSET_88XX,
			su_info->pSu0_ndpa_pkt, su_info->su0_pkt_sz);
		if (status != HALMAC_RET_SUCCESS) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]dl rsvd page\n");
			return status;
		}

		FW_SNDING_SET_SU0(h2c_buf, 1);
		FW_SNDING_SET_PERIOD(h2c_buf, period);
		FW_SNDING_SET_NDPA0_HEAD_PG(
			h2c_buf,
			adapter->txff_allocation.rsvd_h2c_static_info_pg_bndy +
			HALMAC_SU0_SNDING_PKT_OFFSET_88XX -
			adapter->txff_allocation.rsvd_pg_bndy);
	} else {
		if (halmac_query_fw_snding_curr_state_88xx(adapter) !=
					HALMAC_FW_SNDING_CMD_CONSTRUCT_SNDING) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Not snd(snd)\n");
			return HALMAC_RET_ERROR_STATE;
		}
		FW_SNDING_SET_SU0(h2c_buf, 0);
	}

	*proc_status = HALMAC_CMD_PROCESS_SENDING;

	hdr_info.sub_cmd_id = SUB_CMD_ID_FW_SNDING;
	hdr_info.content_size = 8;
	hdr_info.ack = _TRUE;
	halmac_set_h2c_pkt_hdr_88xx(adapter, h2c_buf, &hdr_info, &seq_num);
	adapter->halmac_state.fw_snding_set.seq_num = seq_num;

	status = halmac_send_h2c_pkt_88xx(adapter, h2c_buf,
					  HALMAC_H2C_CMD_SIZE_88XX);
	if (status != HALMAC_RET_SUCCESS) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]send h2c\n");
		halmac_reset_feature_88xx(adapter, HALMAC_FEATURE_FW_SNDING);
		return status;
	}

	if (halmac_transition_fw_snding_state_88xx(
				adapter,
				(su_info->su0_en == 1) ?
				HALMAC_FW_SNDING_CMD_CONSTRUCT_SNDING :
				HALMAC_FW_SNDING_CMD_CONSTRUCT_IDLE)
				!= HALMAC_RET_SUCCESS)
		return HALMAC_RET_ERROR_STATE;

	return HALMAC_RET_SUCCESS;
}

static u8
halmac_snding_pkt_chk_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *pkt
)
{
	u8 data_rate;

	if (GET_TX_DESC_NDPA(pkt) == 0) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]txdesc ndpa = 0\n");
		return _FALSE;
	}

	data_rate = (u8)GET_TX_DESC_DATARATE(pkt);
	if (!(data_rate >= HALMAC_VHT_NSS2_MCS0 &&
	      data_rate <= HALMAC_VHT_NSS2_MCS9)) {
		if (!(data_rate >= HALMAC_MCS8 &&
		      data_rate <= HALMAC_MCS15)) {
			PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]txdesc rate\n");
			return _FALSE;
		}
	}

	if (GET_TX_DESC_NAVUSEHDR(pkt) == 0) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]txdesc navusehdr = 0\n");
		return _FALSE;
	}

	if (GET_TX_DESC_USE_RATE(pkt) == 0) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]txdesc userate = 0\n");
		return _FALSE;
	}

	return _TRUE;
}

static enum halmac_fw_snding_cmd_construct_state
halmac_query_fw_snding_curr_state_88xx(
	IN struct halmac_adapter *adapter
)
{
	return adapter->halmac_state.fw_snding_set.fw_snding_cmd_construct_state;
}

enum halmac_ret_status
halmac_get_h2c_ack_fw_snding_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
)
{
	u8 seq_num = 0;
	u8 fw_rc;
	struct halmac_fw_snding_state_set *state =
		&adapter->halmac_state.fw_snding_set;
	enum halmac_cmd_process_status proc_status;

	seq_num = (u8)H2C_ACK_HDR_GET_H2C_SEQ(buf);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Seq num:h2c->%d c2h->%d\n",
			state->seq_num, seq_num);
	if (seq_num != state->seq_num) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR,
				"[ERR]Seq num mismatch:h2c->%d c2h->%d\n",
				state->seq_num, seq_num);
		return HALMAC_RET_SUCCESS;
	}

	if (state->process_status != HALMAC_CMD_PROCESS_SENDING) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]not sending(snd)\n");
		return HALMAC_RET_SUCCESS;
	}

	fw_rc = (u8)H2C_ACK_HDR_GET_H2C_RETURN_CODE(buf);
	state->fw_return_code = fw_rc;

	if ((enum halmac_h2c_return_code)fw_rc == HALMAC_H2C_RETURN_SUCCESS) {
		proc_status = HALMAC_CMD_PROCESS_DONE;
		state->process_status = proc_status;
		PLTFM_EVENT_INDICATION(HALMAC_FEATURE_FW_SNDING, proc_status,
				       NULL, 0);
	} else {
		proc_status = HALMAC_CMD_PROCESS_ERROR;
		state->process_status = proc_status;
		PLTFM_EVENT_INDICATION(HALMAC_FEATURE_FW_SNDING, proc_status,
				       &fw_rc, 1);
	}

	return HALMAC_RET_SUCCESS;
}

enum halmac_ret_status
halmac_query_fw_snding_status_88xx(
	IN struct halmac_adapter *adapter,
	OUT enum halmac_cmd_process_status *proc_status,
	INOUT u8 *data,
	INOUT u32 *size
)
{
	struct halmac_fw_snding_state_set *state =
					&adapter->halmac_state.fw_snding_set;

	*proc_status = state->process_status;

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_transition_fw_snding_state_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_fw_snding_cmd_construct_state dest_state
)
{
	struct halmac_fw_snding_state_set *state =
					&adapter->halmac_state.fw_snding_set;

	if ((state->fw_snding_cmd_construct_state != HALMAC_FW_SNDING_CMD_CONSTRUCT_IDLE) &&
	    (state->fw_snding_cmd_construct_state != HALMAC_FW_SNDING_CMD_CONSTRUCT_SNDING))
		return HALMAC_RET_ERROR_STATE;

	if (dest_state == HALMAC_FW_SNDING_CMD_CONSTRUCT_IDLE) {
		if (state->fw_snding_cmd_construct_state == HALMAC_FW_SNDING_CMD_CONSTRUCT_IDLE)
			return HALMAC_RET_ERROR_STATE;
	} else if (dest_state == HALMAC_FW_SNDING_CMD_CONSTRUCT_SNDING) {
		if (state->fw_snding_cmd_construct_state == HALMAC_FW_SNDING_CMD_CONSTRUCT_SNDING)
			return HALMAC_RET_ERROR_STATE;
	}

	state->fw_snding_cmd_construct_state = dest_state;

	return HALMAC_RET_SUCCESS;
}
#endif /* HALMAC_88XX_SUPPORT */
