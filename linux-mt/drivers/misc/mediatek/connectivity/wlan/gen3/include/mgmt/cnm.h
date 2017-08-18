/*
** Id: //Department/DaVinci/BRANCHES/MT6620_WIFI_DRIVER_V2_3/include/mgmt/cnm.h#1
*/

/*! \file   "cnm.h"
    \brief
*/

/*
** Log: cnm.h
**
** 07 19 2013 yuche.tsai
** [BORA00002398] [MT6630][Volunteer Patch] P2P Driver Re-Design for Multiple BSS support
** Code update for P2P.
**
** 05 07 2013 cm.chang
** [BORA00002149] [MT6630 Wi-Fi] Initial software development
** Provide macro for TXM to query if BSS is CH_GRANTED
**
** 01 30 2013 yuche.tsai
** [BORA00002398] [MT6630][Volunteer Patch] P2P Driver Re-Design for Multiple BSS support
** Code first update.
**
** 01 21 2013 cm.chang
** [BORA00002149] [MT6630 Wi-Fi] Initial software development
** 1. Create rP2pDevInfo structure
** 2. Support 80/160 MHz channel bandwidth for channel privilege
**
** 01 17 2013 cm.chang
** [BORA00002149] [MT6630 Wi-Fi] Initial software development
** Use ucBssIndex to replace eNetworkTypeIndex
**
** 09 17 2012 cm.chang
** [BORA00002149] [MT6630 Wi-Fi] Initial software development
** Duplicate source from MT6620 v2.3 driver branch
** (Davinci label: MT6620_WIFI_Driver_V2_3_120913_1942_As_MT6630_Base)
 *
 * 06 23 2011 cp.wu
 * [WCXRP00000798] [MT6620 Wi-Fi][Firmware] Follow-ups for WAPI frequency offset workaround in firmware SCN module
 * change parameter name from PeerAddr to BSSID
 *
 * 06 20 2011 cp.wu
 * [WCXRP00000798] [MT6620 Wi-Fi][Firmware] Follow-ups for WAPI frequency offset workaround in firmware SCN module
 * 1. specify target's BSSID when requesting channel privilege.
 * 2. pass BSSID information to firmware domain
 *
 * 04 12 2011 cm.chang
 * [WCXRP00000634] [MT6620 Wi-Fi][Driver][FW] 2nd BSS will not support 40MHz bandwidth for concurrency
 * .
 *
 * 03 10 2011 cm.chang
 * [WCXRP00000358] [MT6620 Wi-Fi][Driver] Provide concurrent information for each module
 * Add some functions to let AIS/Tethering or AIS/BOW be the same channel
 *
 * 01 12 2011 cm.chang
 * [WCXRP00000358] [MT6620 Wi-Fi][Driver] Provide concurrent information for each module
 * Provide function to decide if BSS can be activated or not
 *
 * 12 07 2010 cm.chang
 * [WCXRP00000239] MT6620 Wi-Fi][Driver][FW] Merge concurrent branch back to maintrunk
 * 1. BSSINFO include RLM parameter
 * 2. free all sta records when network is disconnected
 *
 * 08 24 2010 cm.chang
 * NULL
 * Support RLM initail channel of Ad-hoc, P2P and BOW
 *
 * 07 19 2010 cm.chang
 *
 * Set RLM parameters and enable CNM channel manager
 *
 * 07 13 2010 cm.chang
 *
 * Rename MSG_CH_RELEASE_T to MSG_CH_ABORT_T
 *
 * 07 08 2010 cp.wu
 *
 * [WPD00003833] [MT6620 and MT5931] Driver migration - move to new repository.
 *
 * 07 08 2010 cm.chang
 * [WPD00003841][LITE Driver] Migrate RLM/CNM to host driver
 * Rename MID_MNY_CNM_CH_RELEASE to MID_MNY_CNM_CH_ABORT
 *
 * 07 01 2010 cm.chang
 * [WPD00003841][LITE Driver] Migrate RLM/CNM to host driver
 * Need bandwidth info when requesting channel privilege
 *
 * 07 01 2010 cm.chang
 * [WPD00003841][LITE Driver] Migrate RLM/CNM to host driver
 * Modify CNM message handler for new flow
 *
 * 05 12 2010 kevin.huang
 * [BORA00000794][WIFISYS][New Feature]Power Management Support
 * Add Power Management - Legacy PS-POLL support.
 *
 * 05 05 2010 cm.chang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * Add a new function to send abort message
 *
 * 03 16 2010 kevin.huang
 * [BORA00000663][WIFISYS][New Feature] AdHoc Mode Support
 * Add AdHoc Mode
 *
 * 03 10 2010 kevin.huang
 * [BORA00000654][WIFISYS][New Feature] CNM Module - Ch Manager Support
 * Add Channel Manager for arbitration of JOIN and SCAN Req
 *
 * 02 08 2010 cm.chang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * Support partial part about cmd basic configuration
 *
 * Nov 18 2009 mtk01104
 * [BORA00000018] Integrate WIFI part into BORA for the 1st time
 * Add prototype of cnmFsmEventInit()
 *
 * Nov 2 2009 mtk01104
 * [BORA00000018] Integrate WIFI part into BORA for the 1st time
 *
**
*/

#ifndef _CNM_H
#define _CNM_H

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

typedef enum _ENUM_CH_REQ_TYPE_T {
	CH_REQ_TYPE_JOIN,
	CH_REQ_TYPE_P2P_LISTEN,
	CH_REQ_TYPE_OFFCHNL_TX,
	CH_REQ_TYPE_GO_START_BSS,
	CH_REQ_TYPE_NUM
} ENUM_CH_REQ_TYPE_T, *P_ENUM_CH_REQ_TYPE_T;

typedef struct _MSG_CH_REQ_T {
	MSG_HDR_T rMsgHdr;	/* Must be the first member */
	UINT_8 ucBssIndex;
	UINT_8 ucTokenID;
	UINT_8 ucPrimaryChannel;
	ENUM_CHNL_EXT_T eRfSco;
	ENUM_BAND_T eRfBand;
	ENUM_CHANNEL_WIDTH_T eRfChannelWidth;	/* To support 80/160MHz bandwidth */
	UINT_8 ucRfCenterFreqSeg1;	/* To support 80/160MHz bandwidth */
	UINT_8 ucRfCenterFreqSeg2;	/* To support 80/160MHz bandwidth */
	ENUM_CH_REQ_TYPE_T eReqType;
	UINT_32 u4MaxInterval;	/* In unit of ms */
} MSG_CH_REQ_T, *P_MSG_CH_REQ_T;

typedef struct _MSG_CH_ABORT_T {
	MSG_HDR_T rMsgHdr;	/* Must be the first member */
	UINT_8 ucBssIndex;
	UINT_8 ucTokenID;
} MSG_CH_ABORT_T, *P_MSG_CH_ABORT_T;

typedef struct _MSG_CH_GRANT_T {
	MSG_HDR_T rMsgHdr;	/* Must be the first member */
	UINT_8 ucBssIndex;
	UINT_8 ucTokenID;
	UINT_8 ucPrimaryChannel;
	ENUM_CHNL_EXT_T eRfSco;
	ENUM_BAND_T eRfBand;
	ENUM_CHANNEL_WIDTH_T eRfChannelWidth;	/* To support 80/160MHz bandwidth */
	UINT_8 ucRfCenterFreqSeg1;	/* To support 80/160MHz bandwidth */
	UINT_8 ucRfCenterFreqSeg2;	/* To support 80/160MHz bandwidth */
	ENUM_CH_REQ_TYPE_T eReqType;
	UINT_32 u4GrantInterval;	/* In unit of ms */
} MSG_CH_GRANT_T, *P_MSG_CH_GRANT_T;

typedef struct _MSG_CH_REOCVER_T {
	MSG_HDR_T rMsgHdr;	/* Must be the first member */
	UINT_8 ucBssIndex;
	UINT_8 ucTokenID;
	UINT_8 ucPrimaryChannel;
	ENUM_CHNL_EXT_T eRfSco;
	ENUM_BAND_T eRfBand;
	ENUM_CHANNEL_WIDTH_T eRfChannelWidth;	/* To support 80/160MHz bandwidth */
	UINT_8 ucRfCenterFreqSeg1;	/* To support 80/160MHz bandwidth */
	UINT_8 ucRfCenterFreqSeg2;	/* To support 80/160MHz bandwidth */
	ENUM_CH_REQ_TYPE_T eReqType;
} MSG_CH_RECOVER_T, *P_MSG_CH_RECOVER_T;

typedef struct _CNM_INFO_T {
	BOOLEAN fgChGranted;
	UINT_8 ucBssIndex;
	UINT_8 ucTokenID;
} CNM_INFO_T, *P_CNM_INFO_T;

#if CFG_ENABLE_WIFI_DIRECT
/* Moved from p2p_fsm.h */
typedef struct _DEVICE_TYPE_T {
	UINT_16 u2CategoryId;	/* Category ID */
	UINT_8 aucOui[4];	/* OUI */
	UINT_16 u2SubCategoryId;	/* Sub Category ID */
} __KAL_ATTRIB_PACKED__ DEVICE_TYPE_T, *P_DEVICE_TYPE_T;
#endif

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/
#define CNM_CH_GRANTED_FOR_BSS(_prAdapter, _ucBssIndex) \
	((_prAdapter)->rCnmInfo.fgChGranted && \
	 (_prAdapter)->rCnmInfo.ucBssIndex == (_ucBssIndex))

/*******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
VOID cnmInit(P_ADAPTER_T prAdapter);

VOID cnmUninit(P_ADAPTER_T prAdapter);

VOID cnmChMngrRequestPrivilege(P_ADAPTER_T prAdapter, P_MSG_HDR_T prMsgHdr);

VOID cnmChMngrAbortPrivilege(P_ADAPTER_T prAdapter, P_MSG_HDR_T prMsgHdr);

VOID cnmChMngrHandleChEvent(P_ADAPTER_T prAdapter, P_WIFI_EVENT_T prEvent);

BOOLEAN
cnmPreferredChannel(P_ADAPTER_T prAdapter, P_ENUM_BAND_T prBand, PUINT_8 pucPrimaryChannel, P_ENUM_CHNL_EXT_T prBssSCO);

BOOLEAN cnmAisInfraChannelFixed(P_ADAPTER_T prAdapter, P_ENUM_BAND_T prBand, PUINT_8 pucPrimaryChannel);

VOID cnmAisInfraConnectNotify(P_ADAPTER_T prAdapter);

BOOLEAN cnmAisIbssIsPermitted(P_ADAPTER_T prAdapter);

BOOLEAN cnmP2PIsPermitted(P_ADAPTER_T prAdapter);

BOOLEAN cnmBowIsPermitted(P_ADAPTER_T prAdapter);

BOOLEAN cnmBss40mBwPermitted(P_ADAPTER_T prAdapter, UINT_8 ucBssIndex);

BOOLEAN cnmBss80mBwPermitted(P_ADAPTER_T prAdapter, UINT_8 ucBssIndex);

UINT_8 cnmGetBssMaxBw(P_ADAPTER_T prAdapter, UINT_8 ucBssIndex);

P_BSS_INFO_T cnmGetBssInfoAndInit(P_ADAPTER_T prAdapter, ENUM_NETWORK_TYPE_T eNetworkType, BOOLEAN fgIsP2pDevice);

VOID cnmFreeBssInfo(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo);
#if CFG_SUPPORT_CHNL_CONFLICT_REVISE
BOOLEAN cnmAisDetectP2PChannel(P_ADAPTER_T prAdapter, P_ENUM_BAND_T prBand, PUINT_8 pucPrimaryChannel);
#endif
/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/
#ifndef _lint
/* We don't have to call following function to inspect the data structure.
 * It will check automatically while at compile time.
 * We'll need this to guarantee the same member order in different structures
 * to simply handling effort in some functions.
 */
static __KAL_INLINE__ VOID cnmMsgDataTypeCheck(VOID)
{
	DATA_STRUCT_INSPECTING_ASSERT(OFFSET_OF(MSG_CH_GRANT_T, rMsgHdr) == 0);

	DATA_STRUCT_INSPECTING_ASSERT(OFFSET_OF(MSG_CH_GRANT_T, rMsgHdr) == OFFSET_OF(MSG_CH_RECOVER_T, rMsgHdr));

	DATA_STRUCT_INSPECTING_ASSERT(OFFSET_OF(MSG_CH_GRANT_T, ucBssIndex) == OFFSET_OF(MSG_CH_RECOVER_T, ucBssIndex));

	DATA_STRUCT_INSPECTING_ASSERT(OFFSET_OF(MSG_CH_GRANT_T, ucTokenID) == OFFSET_OF(MSG_CH_RECOVER_T, ucTokenID));

	DATA_STRUCT_INSPECTING_ASSERT(OFFSET_OF(MSG_CH_GRANT_T, ucPrimaryChannel) ==
				     OFFSET_OF(MSG_CH_RECOVER_T, ucPrimaryChannel));

	DATA_STRUCT_INSPECTING_ASSERT(OFFSET_OF(MSG_CH_GRANT_T, eRfSco) == OFFSET_OF(MSG_CH_RECOVER_T, eRfSco));

	DATA_STRUCT_INSPECTING_ASSERT(OFFSET_OF(MSG_CH_GRANT_T, eRfBand) == OFFSET_OF(MSG_CH_RECOVER_T, eRfBand));

	DATA_STRUCT_INSPECTING_ASSERT(OFFSET_OF(MSG_CH_GRANT_T, eReqType) == OFFSET_OF(MSG_CH_RECOVER_T, eReqType));

}
#endif /* _lint */

#endif /* _CNM_H */
