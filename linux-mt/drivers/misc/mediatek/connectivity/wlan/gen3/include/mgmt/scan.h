/*
** Id: @(#)
*/

/*! \file   "scan.h"
    \brief

*/

/*
** Log: scan.h
**
** 04 21 2014 eason.tsai
** [ALPS01511962] [WFD][Case Fail]Device can't connect to another AP successfully after connect to WFD.
**	BSS descipt timeout set 10s , or it would always trigger full channel scan before connection  and
**	and connection fail
**
** 03 12 2014 eason.tsai
** [ALPS01070904] [Need Patch] [Volunteer Patch][MT6630][Driver]MT6630 Wi-Fi Patch
** revise for cfg80211 disconnect because of timeout
**
** 08 15 2013 cp.wu
** [BORA00002253] [MT6630 Wi-Fi][Driver][Firmware] Add NLO and timeout mechanism to SCN module
** enlarge  match_ssid_num to 16 for PNO support
**
** 08 09 2013 cp.wu
** [BORA00002253] [MT6630 Wi-Fi][Driver][Firmware] Add NLO and timeout mechanism to SCN module
** 1. integrate scheduled scan functionality
** 2. condition compilation for linux-3.4 & linux-3.8 compatibility
** 3. correct CMD queue access to reduce lock scope
**
** 04 30 2013 eason.tsai
** [BORA00002255] [MT6630 Wi-Fi][Driver] develop
** update 11ac channel setting
**
** 01 22 2013 cp.wu
** [BORA00002253] [MT6630 Wi-Fi][Driver][Firmware] Add NLO and timeout mechanism to SCN module
** .add driver side NLO state machine
**
** 01 22 2013 cp.wu
** [BORA00002253] [MT6630 Wi-Fi][Driver][Firmware] Add NLO and timeout mechanism to SCN module
** modification for ucBssIndex migration
**
** 01 17 2013 cm.chang
** [BORA00002149] [MT6630 Wi-Fi] Initial software development
** Use ucBssIndex to replace eNetworkTypeIndex
**
** 01 03 2013 cp.wu
** [BORA00002253] [MT6630 Wi-Fi][Driver][Firmware] Add NLO and timeout mechanism to SCN module
** carry timeout value and channel dwell time value to scan module
**
** 09 17 2012 cm.chang
** [BORA00002149] [MT6630 Wi-Fi] Initial software development
** Duplicate source from MT6620 v2.3 driver branch
** (Davinci label: MT6620_WIFI_Driver_V2_3_120913_1942_As_MT6630_Base)
*
* 01 16 2012 cp.wu
* [MT6620 Wi-Fi][Driver] API and behavior modification for preferred band configuration
* with corresponding network configuration
* add wlanSetPreferBandByNetwork() for glue layer to invoke for setting
* preferred band configuration corresponding to network type.
*
* 08 11 2011 cp.wu
* [WCXRP00000830] [MT6620 Wi-Fi][Firmware] Use MDRDY counter to detect empty channel for shortening scan time
* sparse channel detection:
* driver: collect sparse channel information with scan-done event

*
* 07 18 2011 cp.wu
* [WCXRP00000858] [MT5931][Driver][Firmware] Add support for scan to search for more than one SSID
* in a single scanning request
* add framework in driver domain for supporting new SCAN_REQ_V2 for more than 1 SSID
* support as well as uProbeDelay in NDIS 6.x driver model
*
* 06 27 2011 cp.wu
* [WCXRP00000815] [MT6620 Wi-Fi][Driver] allow single BSSID with multiple SSID settings
* to work around some tricky AP which use space character as hidden SSID
* allow to have a single BSSID with multiple SSID to be presented in scanning result
*
* 04 18 2011 terry.wu
* [WCXRP00000660] [MT6620 Wi-Fi][Driver] Remove flag CFG_WIFI_DIRECT_MOVED
* Remove flag CFG_WIFI_DIRECT_MOVED.
*
* 02 09 2011 wh.su
* [WCXRP00000433] [MT6620 Wi-Fi][Driver] Remove WAPI structure define for avoid P2P module
* with structure miss-align pointer issue
* always pre-allio WAPI related structure for align p2p module.
*
* 01 14 2011 yuche.tsai
* [WCXRP00000352] [Volunteer Patch][MT6620][Driver] P2P Statsion Record Client List Issue
* Fix compile error.
*
* 09 08 2010 cp.wu
* NULL
* use static memory pool for storing IEs of scanning result.
*
* 09 03 2010 kevin.huang
* NULL
* Refine #include sequence and solve recursive/nested #include issue
*
* 08 31 2010 kevin.huang
* NULL
* Use LINK LIST operation to process SCAN result
*
* 08 30 2010 cp.wu
* NULL
* eliminate klockwork errors
*
* 08 16 2010 cp.wu
* NULL
* add interface for RLM to trigger OBSS-SCAN.
*
* 08 12 2010 yuche.tsai
* NULL
* Add a functio prototype to find p2p descriptor of a bss descriptor directly.
*
* 08 11 2010 yuche.tsai
* NULL
* Add function prototype for return channel.
* modify data structure for scan specific device ID or TYPE. (Move from P2P Connection Settings to Scan Param)
*
* 08 05 2010 yuche.tsai
* NULL
* Check-in P2P Device Discovery Feature.
*
* 08 02 2010 yuche.tsai
* NULL
* P2P Group Negotiation Code Check in.
*
* 07 26 2010 yuche.tsai
*
* Add a option for channel time extension in scan abort command.
*
* 07 21 2010 yuche.tsai
*
* Add for P2P Scan Result Parsing & Saving.
*
* 07 19 2010 yuche.tsai
*
* Scan status "FIND" is used for P2P FSM find state.
*
* 07 19 2010 cp.wu
*
* [WPD00003833] [MT6620 and MT5931] Driver migration.
* SCN module is now able to handle multiple concurrent scanning requests
*
* 07 14 2010 cp.wu
*
* [WPD00003833] [MT6620 and MT5931] Driver migration.
* pass band with channel number information as scan parameter
*
* 07 14 2010 cp.wu
*
* [WPD00003833] [MT6620 and MT5931] Driver migration.
* remove timer in DRV-SCN.
*
* 07 09 2010 cp.wu
*
* 1) separate AIS_FSM state for two kinds of scanning. (OID triggered scan, and scan-for-connection)
* 2) eliminate PRE_BSS_DESC_T, Beacon/PrebResp is now parsed in single pass
* 3) implment DRV-SCN module, currently only accepts single scan request, other request
* will be directly dropped by returning BUSY
*
* 07 08 2010 cp.wu
*
* [WPD00003833] [MT6620 and MT5931] Driver migration - move to new repository.
*
* 07 01 2010 cp.wu
* [WPD00003833][MT6620 and MT5931] Driver migration
* add scan uninitialization procedure
*
* 07 01 2010 cp.wu
* [WPD00003833][MT6620 and MT5931] Driver migration
* implementation of DRV-SCN and related mailbox message handling.
*
* 06 25 2010 cp.wu
* [WPD00003833][MT6620 and MT5931] Driver migration
* modify Beacon/ProbeResp to complete parsing,
* because host software has looser memory usage restriction
*
* 06 17 2010 yuche.tsai
* [WPD00003839][MT6620 5931][P2P] Feature migration
* Add P2P related field in SCAN_PARAM_T.
*
* 06 14 2010 cp.wu
* [WPD00003833][MT6620 and MT5931] Driver migration
* saa_fsm.c is migrated.
*
* 06 14 2010 cp.wu
* [WPD00003833][MT6620 and MT5931] Driver migration
* add management dispatching function table.
*
* 06 14 2010 cp.wu
* [WPD00003833][MT6620 and MT5931] Driver migration
* restore utility function invoking via hem_mbox to direct calls
*
* 06 11 2010 cp.wu
* [WPD00003833][MT6620 and MT5931] Driver migration
* auth.c is migrated.
*
* 06 10 2010 cp.wu
* [WPD00003833][MT6620 and MT5931] Driver migration
* add buildable & linkable ais_fsm.c
*
* related reference are still waiting to be resolved
*
* 06 07 2010 cp.wu
* [WPD00003833][MT6620 and MT5931] Driver migration
* add aa_fsm.h, ais_fsm.h, bss.h, mib.h and scan.h.
*
* 05 12 2010 kevin.huang
* [BORA00000794][WIFISYS][New Feature]Power Management Support
* Add Power Management - Legacy PS-POLL support.
*
* 04 13 2010 kevin.huang
* [BORA00000663][WIFISYS][New Feature] AdHoc Mode Support
*
* Add new HW CH macro support
*
* 03 16 2010 kevin.huang
* [BORA00000663][WIFISYS][New Feature] AdHoc Mode Support
* Add AdHoc Mode
*
* 03 10 2010 kevin.huang
* [BORA00000654][WIFISYS][New Feature] CNM Module - Ch Manager Support
*
*  *  *  *  *  * Add Channel Manager for arbitration of JOIN and SCAN Req
*
* 02 26 2010 kevin.huang
* [BORA00000603][WIFISYS] [New Feature] AAA Module Support
* Modify scanBuildProbeReqFrameCommonIEs() to support P2P SCAN
*
* 02 23 2010 wh.su
* [BORA00000592][MT6620 Wi-Fi] Adding the security related code for driver
* refine the scan procedure, reduce the WPA and WAPI IE parsing, and move the parsing to the time for join.
*
* 02 23 2010 kevin.huang
* [BORA00000603][WIFISYS] [New Feature] AAA Module Support
* Add support scan channel 1~14 and update scan result's frequency infou1rwduu`wvpghlqg|n`slk+mpdkb
*
* 02 04 2010 kevin.huang
* [BORA00000603][WIFISYS] [New Feature] AAA Module Support
* Add AAA Module Support, Revise Net Type to Net Type Index for array lookup
*
* 01 27 2010 wh.su
* [BORA00000476][Wi-Fi][firmware] Add the security module initialize code
* add and fixed some security function.
*
* 01 07 2010 kevin.huang
* [BORA00000018]Integrate WIFI part into BORA for the 1st time
* [BORA00000018] Integrate WIFI part into BORA for the 1st time
*
* Simplify the process of Beacon during SCAN and remove redundant variable in PRE_BSS_DESC_T
*
* Dec 7 2009 mtk01088
* [BORA00000476] [Wi-Fi][firmware] Add the security module initialize code
* adding variable for wapi ap
*
* Dec 4 2009 mtk01088
* [BORA00000476] [Wi-Fi][firmware] Add the security module initialize code
* remove non-used secuirty variavle
*
* Dec 3 2009 mtk01461
* [BORA00000018] Integrate WIFI part into BORA for the 1st time
* Refine data structure of BSS_DESC_T and PRE_BSS_DESC_T
*
* Nov 24 2009 mtk01461
* [BORA00000018] Integrate WIFI part into BORA for the 1st time
* Add eNetType to rScanParam and revise MGMT Handler with Retain Status
*
* Nov 23 2009 mtk01461
* [BORA00000018] Integrate WIFI part into BORA for the 1st time
* Add ucAvailablePhyTypeSet to BSS_DESC_T
*
* Nov 20 2009 mtk01461
* [BORA00000018] Integrate WIFI part into BORA for the 1st time
* Add aucSrcAddress to SCAN_PARAM_T for P2P's Device Address
*
* Nov 19 2009 mtk01088
* [BORA00000476] [Wi-Fi][firmware] Add the security module initialize code
* adding the security related variable
*
* Nov 18 2009 mtk01088
* [BORA00000476] [Wi-Fi][firmware] Add the security module initialize code
* adding the security ie filed for scan parsing
*
* Nov 16 2009 mtk01461
* [BORA00000018] Integrate WIFI part into BORA for the 1st time
* Add scanSearchBssDescByPolicy()
*
* Nov 5 2009 mtk01461
* [BORA00000018] Integrate WIFI part into BORA for the 1st time
* Add function declarations of scan_fsm.c
*
* Oct 30 2009 mtk01461
* [BORA00000018] Integrate WIFI part into BORA for the 1st time
* Add scan.h to source control
**
*/

#ifndef _SCAN_H
#define _SCAN_H

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "gl_vendor.h"
#include "cnm_timer.h"
/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
/*! Maximum buffer size of SCAN list */
#define SCN_MAX_BUFFER_SIZE                 (CFG_MAX_NUM_BSS_LIST * ALIGN_4(sizeof(BSS_DESC_T)))

#define SCN_RM_POLICY_EXCLUDE_CONNECTED     BIT(0)	/* Remove SCAN result except the connected one. */
#define SCN_RM_POLICY_TIMEOUT               BIT(1)	/* Remove the timeout one */
#define SCN_RM_POLICY_OLDEST_HIDDEN         BIT(2)	/* Remove the oldest one with hidden ssid */
#define SCN_RM_POLICY_SMART_WEAKEST         BIT(3)	/* If there are more than half BSS which has the
							 * same ssid as connection setting, remove the
							 * weakest one from them
							 * Else remove the weakest one.
							 */
#define SCN_RM_POLICY_ENTIRE                BIT(4)	/* Remove entire SCAN result */

#define SCN_BSS_DESC_SAME_SSID_THRESHOLD    3	/* This is used by POLICY SMART WEAKEST,
						 * If exceed this value, remove weakest BSS_DESC_T
						 * with same SSID first in large network.
						 */
#if 1
#define SCN_BSS_DESC_REMOVE_TIMEOUT_SEC     30
#define SCN_BSS_DESC_STALE_SEC				10	/* 2.4G + 5G need 8.1s */
#else
#define SCN_BSS_DESC_REMOVE_TIMEOUT_SEC     5	/* Second. */
					      /* This is used by POLICY TIMEOUT,
					       * If exceed this value, remove timeout BSS_DESC_T.
					       */

#endif

#define SCN_PROBE_DELAY_MSEC                0

#define SCN_ADHOC_BSS_DESC_TIMEOUT_SEC      5	/* Second. */

#define SCN_NLO_NETWORK_CHANNEL_NUM         (4)

#define SCAN_DONE_DIFFERENCE                3
#define SCAN_DONE_TIMEOUT_THRESHOLD 3
/*----------------------------------------------------------------------------*/
/* MSG_SCN_SCAN_REQ                                                           */
/*----------------------------------------------------------------------------*/
#define SCAN_REQ_SSID_WILDCARD              BIT(0)
#define SCAN_REQ_SSID_P2P_WILDCARD          BIT(1)
#define SCAN_REQ_SSID_SPECIFIED             BIT(2)

/*----------------------------------------------------------------------------*/
/* Support Multiple SSID SCAN                                                 */
/*----------------------------------------------------------------------------*/
#define SCN_SSID_MAX_NUM                    CFG_SCAN_SSID_MAX_NUM
#define SCN_SSID_MATCH_MAX_NUM              CFG_SCAN_SSID_MATCH_MAX_NUM

#if CFG_SUPPORT_AGPS_ASSIST
#define SCN_AGPS_AP_LIST_MAX_NUM					32
#endif

#define SCN_BSS_JOIN_FAIL_THRESOLD				4
#define SCN_BSS_JOIN_FAIL_CNT_RESET_SEC				15
#define SCN_BSS_JOIN_FAIL_RESET_STEP				2

#if CFG_SUPPORT_BATCH_SCAN
/*----------------------------------------------------------------------------*/
/* SCAN_BATCH_REQ                                                             */
/*----------------------------------------------------------------------------*/
#define SCAN_BATCH_REQ_START                BIT(0)
#define SCAN_BATCH_REQ_STOP                 BIT(1)
#define SCAN_BATCH_REQ_RESULT               BIT(2)
#endif

#define SCAN_NLO_CHECK_SSID_ONLY    0x00000001
#define SCAN_NLO_DEFAULT_INTERVAL           30000

#define SWC_NUM_BSSID_THRESHOLD_DEFAULT 8
#define SWC_RSSI_WINDSIZE_DEFAULT 8
#define LOST_AP_WINDOW 16
#define MAX_CHANNEL_NUM_PER_BUCKETS 8

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
typedef enum _ENUM_SCAN_TYPE_T {
	SCAN_TYPE_PASSIVE_SCAN = 0,
	SCAN_TYPE_ACTIVE_SCAN,
	SCAN_TYPE_NUM
} ENUM_SCAN_TYPE_T, *P_ENUM_SCAN_TYPE_T;

typedef enum _ENUM_SCAN_STATE_T {
	SCAN_STATE_IDLE = 0,
	SCAN_STATE_SCANNING,
	SCAN_STATE_NUM
} ENUM_SCAN_STATE_T;

typedef enum _ENUM_FW_SCAN_STATE_T {
	FW_SCAN_STATE_IDLE = 0,	/* 0 */
	FW_SCAN_STATE_SCAN_START,	/* 1 */
	FW_SCAN_STATE_REQ_CHANNEL,	/* 2 */
	FW_SCAN_STATE_SET_CHANNEL,	/* 3 */
	FW_SCAN_STATE_DELAYED_ACTIVE_PROB_REQ,	/* 4 */
	FW_SCAN_STATE_ACTIVE_PROB_REQ,	/* 5 */
	FW_SCAN_STATE_LISTEN,	/* 6 */
	FW_SCAN_STATE_SCAN_DONE,	/* 7 */
	FW_SCAN_STATE_NLO_START,	/* 8 */
	FW_SCAN_STATE_NLO_HIT_CHECK,	/* 9 */
	FW_SCAN_STATE_NLO_STOP,	/* 10 */
	FW_SCAN_STATE_BATCH_START,	/* 11 */
	FW_SCAN_STATE_BATCH_CHECK,	/* 12 */
	FW_SCAN_STATE_BATCH_STOP,	/* 13 */
	FW_SCAN_STATE_NUM	/* 14 */
} ENUM_FW_SCAN_STATE_T;

typedef enum _ENUM_SCAN_CHANNEL_T {
	SCAN_CHANNEL_FULL = 0,
	SCAN_CHANNEL_2G4,
	SCAN_CHANNEL_5G,
	SCAN_CHANNEL_P2P_SOCIAL,
	SCAN_CHANNEL_SPECIFIED,
	SCAN_CHANNEL_NUM
} ENUM_SCAN_CHANNEL, *P_ENUM_SCAN_CHANNEL;

typedef struct _MSG_SCN_FSM_T {
	MSG_HDR_T rMsgHdr;	/* Must be the first member */
	UINT_32 u4Dummy;
} MSG_SCN_FSM_T, *P_MSG_SCN_FSM_T;

typedef enum _ENUM_PSCAN_STATE_T {
	PSCN_IDLE = 1,
	PSCN_SCANNING,
	PSCN_RESET,
	PSCAN_STATE_T_NUM
} ENUM_PSCAN_STATE_T;

/*----------------------------------------------------------------------------*/
/* BSS Descriptors                                                            */
/*----------------------------------------------------------------------------*/
struct _BSS_DESC_T {
	LINK_ENTRY_T rLinkEntry;

	UINT_8 aucBSSID[MAC_ADDR_LEN];
	UINT_8 aucSrcAddr[MAC_ADDR_LEN];	/* For IBSS, the SrcAddr is different from BSSID */

	BOOLEAN fgIsConnecting;	/* If we are going to connect to this BSS
				 * (JOIN or ROAMING to another BSS), don't
				 * remove this record from BSS List.
				 */
	BOOLEAN fgIsConnected;	/* If we have connected to this BSS (NORMAL_TR),
				 * don't removed this record from BSS list.
				 */

	BOOLEAN fgIsHiddenSSID;	/* When this flag is TRUE, means the SSID
				 * of this BSS is not known yet.
				 */
	UINT_8 ucSSIDLen;
	UINT_8 aucSSID[ELEM_MAX_LEN_SSID];

	OS_SYSTIME rUpdateTime;

	ENUM_BSS_TYPE_T eBSSType;

	UINT_16 u2CapInfo;

	UINT_16 u2BeaconInterval;
	UINT_16 u2ATIMWindow;

	UINT_16 u2OperationalRateSet;
	UINT_16 u2BSSBasicRateSet;
	BOOLEAN fgIsUnknownBssBasicRate;

	BOOLEAN fgIsERPPresent;
	BOOLEAN fgIsHTPresent;
	BOOLEAN fgIsVHTPresent;

	UINT_8 ucPhyTypeSet;	/* Available PHY Type Set of this BSS */

	UINT_8 ucChannelNum;

	ENUM_CHNL_EXT_T eSco;	/* Record bandwidth for association process
				   Some AP will send association resp by 40MHz BW */
	ENUM_CHANNEL_WIDTH_T eChannelWidth;	/*VHT operation ie */
	UINT_8 ucCenterFreqS1;
	UINT_8 ucCenterFreqS2;
	ENUM_BAND_T eBand;

	UINT_8 ucDTIMPeriod;

	BOOLEAN fgIsLargerTSF;	/* This BSS's TimeStamp is larger than us(TCL == 1 in RX_STATUS_T) */

	UINT_8 ucRCPI;

	UINT_8 ucWmmFlag;	/* A flag to indicate this BSS's WMM capability */

	/*! \brief The srbiter Search State will matched the scan result,
	   and saved the selected cipher and akm, and report the score,
	   for arbiter join state, join module will carry this target BSS
	   to rsn generate ie function, for gen wpa/rsn ie */
	UINT_32 u4RsnSelectedGroupCipher;
	UINT_32 u4RsnSelectedPairwiseCipher;
	UINT_32 u4RsnSelectedAKMSuite;

	UINT_16 u2RsnCap;

	RSN_INFO_T rRSNInfo;
	RSN_INFO_T rWPAInfo;
#if 1				/* CFG_SUPPORT_WAPI */
	WAPI_INFO_T rIEWAPI;
	BOOL fgIEWAPI;
#endif
	BOOL fgIERSN;
	BOOL fgIEWPA;

	/*! \brief RSN parameters selected for connection */
	/*! \brief The Select score for final AP selection,
	   0, no sec, 1,2,3 group cipher is WEP, TKIP, CCMP */
	UINT_8 ucEncLevel;

#if CFG_ENABLE_WIFI_DIRECT
	BOOLEAN fgIsP2PPresent;
	BOOLEAN fgIsP2PReport;	/* TRUE: report to upper layer */
	P_P2P_DEVICE_DESC_T prP2pDesc;

	UINT_8 aucIntendIfAddr[MAC_ADDR_LEN];	/* For IBSS, the SrcAddr is different from BSSID */
	/* UINT_8 ucDevCapabilityBitmap; *//* Device Capability Attribute. (P2P_DEV_CAPABILITY_XXXX) */
	/* UINT_8 ucGroupCapabilityBitmap; *//* Group Capability Attribute. (P2P_GROUP_CAPABILITY_XXXX) */

	LINK_T rP2pDeviceList;

/* P_LINK_T prP2pDeviceList; */

	/* For
	 *    1. P2P Capability.
	 *    2. P2P Device ID. ( in aucSrcAddr[] )
	 *    3. NOA   (TODO:)
	 *    4. Extend Listen Timing. (Probe Rsp)  (TODO:)
	 *    5. P2P Device Info. (Probe Rsp)
	 *    6. P2P Group Info. (Probe Rsp)
	 */
#endif

	BOOLEAN fgIsIEOverflow;	/* The received IE length exceed the maximum IE buffer size */
	UINT_16 u2RawLength;	/* The byte count of aucRawBuf[] */
	UINT_16 u2IELength;	/* The byte count of aucIEBuf[] */

	ULARGE_INTEGER u8TimeStamp;	/* Place u8TimeStamp before aucIEBuf[1] to force DW align */
	UINT_8 aucRawBuf[CFG_RAW_BUFFER_SIZE];
	UINT_8 aucIEBuf[CFG_IE_BUFFER_SIZE];
	UINT_8 ucJoinFailureCount;
	OS_SYSTIME rJoinFailTime;
};

typedef struct _SCAN_PARAM_T {	/* Used by SCAN FSM */
	/* Active or Passive */
	ENUM_SCAN_TYPE_T eScanType;

	/* Network Type */
	UINT_8 ucBssIndex;

	/* Specified SSID Type */
	UINT_8 ucSSIDType;
	UINT_8 ucSSIDNum;

	/* Length of Specified SSID */
	UINT_8 ucSpecifiedSSIDLen[SCN_SSID_MAX_NUM];

	/* Specified SSID */
	UINT_8 aucSpecifiedSSID[SCN_SSID_MAX_NUM][ELEM_MAX_LEN_SSID];

#if CFG_ENABLE_WIFI_DIRECT
	BOOLEAN fgFindSpecificDev;	/* P2P: Discovery Protocol */
	UINT_8 aucDiscoverDevAddr[MAC_ADDR_LEN];
	BOOLEAN fgIsDevType;
	P2P_DEVICE_TYPE_T rDiscoverDevType;

	/* TODO: Find Specific Device Type. */
#endif				/* CFG_SUPPORT_P2P */

	UINT_16 u2ChannelDwellTime;
	UINT_16 u2TimeoutValue;

	BOOLEAN fgIsObssScan;
	BOOLEAN fgIsScanV2;

	/* Run time flags */
	UINT_16 u2ProbeDelayTime;

	/* channel information */
	ENUM_SCAN_CHANNEL eScanChannel;
	UINT_8 ucChannelListNum;
	RF_CHANNEL_INFO_T arChnlInfoList[MAXIMUM_OPERATION_CHANNEL_LIST];

	/* Feedback information */
	UINT_8 ucSeqNum;

	/* Information Element */
	UINT_16 u2IELen;
	UINT_8 aucIE[MAX_IE_LENGTH];

} SCAN_PARAM_T, *P_SCAN_PARAM_T;

typedef struct _NLO_PARAM_T {	/* Used by SCAN FSM */
	SCAN_PARAM_T rScanParam;

	/* NLO */
	BOOLEAN fgStopAfterIndication;
	UINT_8 ucFastScanIteration;
	UINT_16 u2FastScanPeriod;
	UINT_16 u2SlowScanPeriod;

	/* Match SSID */
	UINT_8 ucMatchSSIDNum;
	UINT_8 ucMatchSSIDLen[SCN_SSID_MATCH_MAX_NUM];
	UINT_8 aucMatchSSID[SCN_SSID_MATCH_MAX_NUM][ELEM_MAX_LEN_SSID];

	UINT_8 aucCipherAlgo[SCN_SSID_MATCH_MAX_NUM];
	UINT_16 au2AuthAlgo[SCN_SSID_MATCH_MAX_NUM];
	UINT_8 aucChannelHint[SCN_SSID_MATCH_MAX_NUM][SCN_NLO_NETWORK_CHANNEL_NUM];
	P_BSS_DESC_T aprPendingBssDescToInd[SCN_SSID_MATCH_MAX_NUM];
} NLO_PARAM_T, *P_NLO_PARAM_T;

typedef struct _PSCN_PARAM_T {
	UINT_8 ucVersion;
	CMD_NLO_REQ rCurrentCmdNloReq;
	CMD_BATCH_REQ_T rCurrentCmdBatchReq;
	CMD_GSCN_REQ_T rCurrentCmdGscnReq;
	BOOLEAN fgNLOScnEnable;
	BOOLEAN fgBatchScnEnable;
	BOOLEAN fgGScnEnable;
	UINT_32 u4BasePeriod;	/*GSCAN_ATTRIBUTE_BASE_PERIOD */
} PSCN_PARAM_T, *P_PSCN_PARAM_T;

typedef struct _SCAN_INFO_T {
	ENUM_SCAN_STATE_T eCurrentState;	/* Store the STATE variable of SCAN FSM */

	OS_SYSTIME rLastScanCompletedTime;

	SCAN_PARAM_T rScanParam;
	NLO_PARAM_T rNloParam;

	UINT_32 u4NumOfBssDesc;

	UINT_8 aucScanBuffer[SCN_MAX_BUFFER_SIZE];

	LINK_T rBSSDescList;

	LINK_T rFreeBSSDescList;

	LINK_T rPendingMsgList;

	/* Sparse Channel Detection */
	BOOLEAN fgIsSparseChannelValid;
	RF_CHANNEL_INFO_T rSparseChannel;

	/* NLO scanning state tracking */
	BOOLEAN fgNloScanning;
#if CFG_SUPPORT_SCN_PSCN
	BOOLEAN fgPscnOnnning;
	BOOLEAN fgGScnConfigSet;
	BOOLEAN fgGScnParamSet;
	P_PSCN_PARAM_T prPscnParam;
	ENUM_PSCAN_STATE_T eCurrentPSCNState;
	TIMER_T rWaitForGscanResutsTimer;
	BOOLEAN fgGscnGetResWaiting;
#endif
	TIMER_T rScanDoneTimer;
	UINT_8 ucScanDoneTimeoutCnt;

} SCAN_INFO_T, *P_SCAN_INFO_T;

/* Incoming Mailbox Messages */
typedef struct _MSG_SCN_SCAN_REQ_T {
	MSG_HDR_T rMsgHdr;	/* Must be the first member */
	UINT_8 ucSeqNum;
	UINT_8 ucBssIndex;
	ENUM_SCAN_TYPE_T eScanType;
	UINT_8 ucSSIDType;	/* BIT(0) wildcard / BIT(1) P2P-wildcard / BIT(2) specific */
	UINT_8 ucSSIDLength;
	UINT_8 aucSSID[PARAM_MAX_LEN_SSID];
	UINT_16 u2ChannelDwellTime;	/* ms unit */
	UINT_16 u2TimeoutValue;	/* ms unit */
	ENUM_SCAN_CHANNEL eScanChannel;
	UINT_8 ucChannelListNum;
	RF_CHANNEL_INFO_T arChnlInfoList[MAXIMUM_OPERATION_CHANNEL_LIST];
	UINT_16 u2IELen;
	UINT_8 aucIE[MAX_IE_LENGTH];
} MSG_SCN_SCAN_REQ, *P_MSG_SCN_SCAN_REQ;

typedef struct _MSG_SCN_SCAN_REQ_V2_T {
	MSG_HDR_T rMsgHdr;	/* Must be the first member */
	UINT_8 ucSeqNum;
	UINT_8 ucBssIndex;
	ENUM_SCAN_TYPE_T eScanType;
	UINT_8 ucSSIDType;	/* BIT(0) wildcard / BIT(1) P2P-wildcard / BIT(2) specific */
	UINT_8 ucSSIDNum;
	P_PARAM_SSID_T prSsid;
	UINT_16 u2ProbeDelay;
	UINT_16 u2ChannelDwellTime;	/* In TU. 1024us. */
	UINT_16 u2TimeoutValue;	/* ms unit */
	ENUM_SCAN_CHANNEL eScanChannel;
	UINT_8 ucChannelListNum;
	RF_CHANNEL_INFO_T arChnlInfoList[MAXIMUM_OPERATION_CHANNEL_LIST];
	UINT_16 u2IELen;
	UINT_8 aucIE[MAX_IE_LENGTH];
} MSG_SCN_SCAN_REQ_V2, *P_MSG_SCN_SCAN_REQ_V2;

typedef struct _MSG_SCN_SCAN_CANCEL_T {
	MSG_HDR_T rMsgHdr;	/* Must be the first member */
	UINT_8 ucSeqNum;
	UINT_8 ucBssIndex;
	BOOLEAN fgIsChannelExt;
} MSG_SCN_SCAN_CANCEL, *P_MSG_SCN_SCAN_CANCEL;

typedef struct _tagOFFLOAD_NETWORK {
	UINT_8 aucSsid[ELEM_MAX_LEN_SSID];
	UINT_8 ucSsidLen;
	UINT_8 ucUnicastCipher;	/* ENUM_NLO_CIPHER_ALGORITHM */
	UINT_16 u2AuthAlgo;	/* ENUM_NLO_AUTH_ALGORITHM */
	UINT_8 aucChannelList[SCN_NLO_NETWORK_CHANNEL_NUM];
} OFFLOAD_NETWORK, *P_OFFLOAD_NETWORK;

typedef struct _MSG_SCN_NLO_REQ_T {
	MSG_HDR_T rMsgHdr;	/* Must be the first member */
	BOOLEAN fgStopAfterIndication;
	UINT_8 ucSeqNum;
	UINT_8 ucBssIndex;
	UINT_32 u4FastScanPeriod;
	UINT_32 u4FastScanIterations;
	UINT_32 u4SlowScanPeriod;
	UINT_32 u4NumOfEntries;
	OFFLOAD_NETWORK arNetwork[CFG_SCAN_SSID_MAX_NUM];
} MSG_SCN_NLO_REQ, *P_MSG_SCN_NLO_REQ;

typedef struct _MSG_SCN_NLO_CANCEL_T {
	MSG_HDR_T rMsgHdr;	/* Must be the first member */
	UINT_8 ucSeqNum;
	UINT_8 ucBssIndex;
} MSG_SCN_NLO_CANCEL, *P_MSG_SCN_NLO_CANCEL;

/* Outgoing Mailbox Messages */
typedef enum _ENUM_SCAN_STATUS_T {
	SCAN_STATUS_DONE = 0,
	SCAN_STATUS_CANCELLED,
	SCAN_STATUS_FAIL,
	SCAN_STATUS_BUSY,
	SCAN_STATUS_NUM
} ENUM_SCAN_STATUS, *P_ENUM_SCAN_STATUS;

typedef struct _MSG_SCN_SCAN_DONE_T {
	MSG_HDR_T rMsgHdr;	/* Must be the first member */
	UINT_8 ucSeqNum;
	UINT_8 ucBssIndex;
	ENUM_SCAN_STATUS eScanStatus;
} MSG_SCN_SCAN_DONE, *P_MSG_SCN_SCAN_DONE;

#if CFG_SUPPORT_AGPS_ASSIST
typedef enum {
	AGPS_PHY_A,
	AGPS_PHY_B,
	AGPS_PHY_G,
} AP_PHY_TYPE;

typedef struct _AGPS_AP_INFO_T {
	UINT_8 aucBSSID[MAC_ADDR_LEN];
	INT_16 i2ApRssi;	/* -127..128 */
	UINT_16 u2Channel;	/* 0..256 */
	AP_PHY_TYPE ePhyType;
} AGPS_AP_INFO_T, *P_AGPS_AP_INFO_T;

typedef struct _AGPS_AP_LIST_T {
	UINT_8 ucNum;
	AGPS_AP_INFO_T arApInfo[SCN_AGPS_AP_LIST_MAX_NUM];
} AGPS_AP_LIST_T, *P_AGPS_AP_LIST_T;
#endif

typedef enum _ENUM_NLO_STATUS_T {
	NLO_STATUS_FOUND = 0,
	NLO_STATUS_CANCELLED,
	NLO_STATUS_FAIL,
	NLO_STATUS_BUSY,
	NLO_STATUS_NUM
} ENUM_NLO_STATUS, *P_ENUM_NLO_STATUS;

typedef struct _MSG_SCN_NLO_DONE_T {
	MSG_HDR_T rMsgHdr;	/* Must be the first member */
	UINT_8 ucSeqNum;
	UINT_8 ucBssIndex;
	ENUM_NLO_STATUS eNloStatus;
} MSG_SCN_NLO_DONE, *P_MSG_SCN_NLO_DONE;

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

/*******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
/*----------------------------------------------------------------------------*/
/* Routines in scan.c                                                         */
/*----------------------------------------------------------------------------*/
VOID scnInit(IN P_ADAPTER_T prAdapter);

VOID scnUninit(IN P_ADAPTER_T prAdapter);

/* BSS-DESC Search */
P_BSS_DESC_T scanSearchBssDescByBssid(IN P_ADAPTER_T prAdapter, IN UINT_8 aucBSSID[]);

P_BSS_DESC_T
scanSearchBssDescByBssidAndSsid(IN P_ADAPTER_T prAdapter,
				IN UINT_8 aucBSSID[], IN BOOLEAN fgCheckSsid, IN P_PARAM_SSID_T prSsid);

P_BSS_DESC_T scanSearchBssDescByTA(IN P_ADAPTER_T prAdapter, IN UINT_8 aucSrcAddr[]);

P_BSS_DESC_T
scanSearchBssDescByTAAndSsid(IN P_ADAPTER_T prAdapter,
			     IN UINT_8 aucSrcAddr[], IN BOOLEAN fgCheckSsid, IN P_PARAM_SSID_T prSsid);

/* BSS-DESC Search - Alternative */
P_BSS_DESC_T
scanSearchExistingBssDesc(IN P_ADAPTER_T prAdapter,
			  IN ENUM_BSS_TYPE_T eBSSType, IN UINT_8 aucBSSID[], IN UINT_8 aucSrcAddr[]);

P_BSS_DESC_T
scanSearchExistingBssDescWithSsid(IN P_ADAPTER_T prAdapter,
				  IN ENUM_BSS_TYPE_T eBSSType,
				  IN UINT_8 aucBSSID[],
				  IN UINT_8 aucSrcAddr[], IN BOOLEAN fgCheckSsid, IN P_PARAM_SSID_T prSsid);

/* BSS-DESC Allocation */
P_BSS_DESC_T scanAllocateBssDesc(IN P_ADAPTER_T prAdapter);

/* BSS-DESC Removal */
VOID scanRemoveBssDescsByPolicy(IN P_ADAPTER_T prAdapter, IN UINT_32 u4RemovePolicy);

VOID scanRemoveBssDescByBssid(IN P_ADAPTER_T prAdapter, IN UINT_8 aucBSSID[]);

VOID scanRemoveBssDescByBandAndNetwork(IN P_ADAPTER_T prAdapter, IN ENUM_BAND_T eBand, IN UINT_8 ucBssIndex);

/* BSS-DESC State Change */
VOID scanRemoveConnFlagOfBssDescByBssid(IN P_ADAPTER_T prAdapter, IN UINT_8 aucBSSID[]);

/* BSS-DESC Insertion - ALTERNATIVE */
P_BSS_DESC_T scanAddToBssDesc(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb);

WLAN_STATUS scanProcessBeaconAndProbeResp(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSWRfb);

VOID
scanBuildProbeReqFrameCommonIEs(IN P_MSDU_INFO_T prMsduInfo,
				IN PUINT_8 pucDesiredSsid, IN UINT_32 u4DesiredSsidLen, IN UINT_16 u2SupportedRateSet);

WLAN_STATUS scanSendProbeReqFrames(IN P_ADAPTER_T prAdapter, IN P_SCAN_PARAM_T prScanParam);

VOID scanUpdateBssDescForSearch(IN P_ADAPTER_T prAdapter, IN P_BSS_DESC_T prBssDesc);

P_BSS_DESC_T scanSearchBssDescByPolicy(IN P_ADAPTER_T prAdapter, IN UINT_8 ucBssIndex);

WLAN_STATUS scanAddScanResult(IN P_ADAPTER_T prAdapter, IN P_BSS_DESC_T prBssDesc, IN P_SW_RFB_T prSwRfb);

VOID scanReportBss2Cfg80211(IN P_ADAPTER_T prAdapter, IN ENUM_BSS_TYPE_T eBSSType, IN P_BSS_DESC_T SpecificprBssDesc);
/*----------------------------------------------------------------------------*/
/* Routines in scan_fsm.c                                                     */
/*----------------------------------------------------------------------------*/
VOID scnFsmSteps(IN P_ADAPTER_T prAdapter, IN ENUM_SCAN_STATE_T eNextState);

/*----------------------------------------------------------------------------*/
/* Command Routines                                                           */
/*----------------------------------------------------------------------------*/
VOID scnSendScanReq(IN P_ADAPTER_T prAdapter);

VOID scnSendScanReqV2(IN P_ADAPTER_T prAdapter);

/*----------------------------------------------------------------------------*/
/* RX Event Handling                                                          */
/*----------------------------------------------------------------------------*/
VOID scnEventScanDone(IN P_ADAPTER_T prAdapter, IN P_EVENT_SCAN_DONE prScanDone, BOOLEAN fgIsNewVersion);

VOID scnEventNloDone(IN P_ADAPTER_T prAdapter, IN P_EVENT_NLO_DONE_T prNloDone);

/*----------------------------------------------------------------------------*/
/* Mailbox Message Handling                                                   */
/*----------------------------------------------------------------------------*/
VOID scnFsmMsgStart(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

VOID scnFsmMsgAbort(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

VOID scnFsmHandleScanMsg(IN P_ADAPTER_T prAdapter, IN P_MSG_SCN_SCAN_REQ prScanReqMsg);

VOID scnFsmHandleScanMsgV2(IN P_ADAPTER_T prAdapter, IN P_MSG_SCN_SCAN_REQ_V2 prScanReqMsg);

VOID scnFsmRemovePendingMsg(IN P_ADAPTER_T prAdapter, IN UINT_8 ucSeqNum, IN UINT_8 ucBssIndex);

VOID scnFsmNloMsgStart(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

VOID scnFsmNloMsgAbort(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

VOID scnFsmHandleNloMsg(IN P_ADAPTER_T prAdapter, IN P_MSG_SCN_NLO_REQ prNloReqMsg);

/*----------------------------------------------------------------------------*/
/* Mailbox Message Generation                                                 */
/*----------------------------------------------------------------------------*/
VOID
scnFsmGenerateScanDoneMsg(IN P_ADAPTER_T prAdapter,
			  IN UINT_8 ucSeqNum, IN UINT_8 ucBssIndex, IN ENUM_SCAN_STATUS eScanStatus);

/*----------------------------------------------------------------------------*/
/* Query for sparse channel                                                   */
/*----------------------------------------------------------------------------*/
BOOLEAN scnQuerySparseChannel(IN P_ADAPTER_T prAdapter, P_ENUM_BAND_T prSparseBand, PUINT_8 pucSparseChannel);

/*----------------------------------------------------------------------------*/
/* OID/IOCTL Handling                                                         */
/*----------------------------------------------------------------------------*/
BOOLEAN
scnFsmSchedScanRequest(IN P_ADAPTER_T prAdapter,
		       IN UINT_8 ucSsidNum,
		       IN P_PARAM_SSID_T prSsid, IN UINT_32 u4IeLength, IN PUINT_8 pucIe, IN UINT_16 u2Interval);

BOOLEAN scnFsmSchedScanStopRequest(IN P_ADAPTER_T prAdapter);

#if CFG_SUPPORT_PASSPOINT
P_BSS_DESC_T scanSearchBssDescByBssidAndLatestUpdateTime(IN P_ADAPTER_T prAdapter, IN UINT_8 aucBSSID[]);
#endif /* CFG_SUPPORT_PASSPOINT */

#if CFG_SUPPORT_AGPS_ASSIST
VOID scanReportScanResultToAgps(P_ADAPTER_T prAdapter);
#endif

BOOLEAN scnFsmPSCNAction(IN P_ADAPTER_T prAdapter, IN UINT_8 ucPscanAct);

BOOLEAN scnFsmPSCNSetParam(IN P_ADAPTER_T prAdapter, IN P_CMD_SET_PSCAN_PARAM prCmdPscnParam);

BOOLEAN scnFsmGSCNSetHotlist(IN P_ADAPTER_T prAdapter, IN P_CMD_SET_PSCAN_PARAM prCmdPscnParam);

#if 0

BOOLEAN scnFsmGSCNSetRssiSignificatn(IN P_ADAPTER_T prAdapter, IN P_CMD_SET_PSCAN_PARAM prCmdPscnParam);
#endif

BOOLEAN scnFsmPSCNAddSWCBssId(IN P_ADAPTER_T prAdapter, IN P_CMD_SET_PSCAN_ADD_SWC_BSSID prCmdPscnAddSWCBssId);

BOOLEAN scnFsmPSCNSetMacAddr(IN P_ADAPTER_T prAdapter, IN P_CMD_SET_PSCAN_MAC_ADDR prCmdPscnSetMacAddr);

#if 1
BOOLEAN scnSetGSCNParam(IN P_ADAPTER_T prAdapter, IN P_PARAM_WIFI_GSCAN_CMD_PARAMS prCmdGscnParam);

#else
BOOLEAN scnSetGSCNParam(IN P_ADAPTER_T prAdapter, IN P_CMD_GSCN_REQ_T prCmdGscnParam);

#endif

BOOLEAN
scnCombineParamsIntoPSCN(IN P_ADAPTER_T prAdapter,
			 IN P_CMD_NLO_REQ prCmdNloReq,
			 IN P_CMD_BATCH_REQ_T prCmdBatchReq,
			 IN P_CMD_GSCN_REQ_T prCmdGscnReq,
			 IN P_CMD_GSCN_SCN_COFIG_T prNewCmdGscnConfig,
			 IN BOOLEAN fgRemoveNLOfromPSCN,
			 IN BOOLEAN fgRemoveBatchSCNfromPSCN, IN BOOLEAN fgRemoveGSCNfromPSCN);

BOOLEAN scnFsmSetGSCNConfig(IN P_ADAPTER_T prAdapter, IN P_CMD_GSCN_SCN_COFIG_T prCmdGscnScnConfig);

BOOLEAN scnFsmGetGSCNResult(IN P_ADAPTER_T prAdapter, IN P_CMD_GET_GSCAN_RESULT_T prGetGscnScnResultCmd);

BOOLEAN
scnPSCNFsm(IN P_ADAPTER_T prAdapter,
	   ENUM_PSCAN_STATE_T eNextPSCNState,
	   IN P_CMD_NLO_REQ prCmdNloReq,
	   IN P_CMD_BATCH_REQ_T prCmdBatchReq,
	   IN P_CMD_GSCN_REQ_T prCmdGscnReq,
	   IN P_CMD_GSCN_SCN_COFIG_T prNewCmdGscnConfig,
	   IN BOOLEAN fgRemoveNLOfromPSCN,
	   IN BOOLEAN fgRemoveBatchSCNfromPSCN, IN BOOLEAN fgRemoveGSCNfromPSCN, IN BOOLEAN fgEnableGSCN);

VOID scnGscnGetResultReplyCheck(IN P_ADAPTER_T prAdapter);

VOID scnGscnGetResultReplyCheckTimeout(IN P_ADAPTER_T prAdapter, ULONG ulParamPtr);

VOID scnScanDoneTimeout(IN P_ADAPTER_T prAdapter, ULONG ulParamPtr);

#endif /* _SCAN_H */
