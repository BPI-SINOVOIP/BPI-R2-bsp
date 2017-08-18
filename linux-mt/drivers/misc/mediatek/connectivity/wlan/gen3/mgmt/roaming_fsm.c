/*
** Id:
*/

/*! \file   "roaming_fsm.c"
    \brief  This file defines the FSM for Roaming MODULE.

    This file defines the FSM for Roaming MODULE.
*/

/*
** Log: roaming_fsm.c
**
** 09 03 2013 tsaiyuan.hsu
** [BORA00002775] MT6630 unified MAC ROAMING
** 1. modify roaming fsm.
** 2. add roaming control.
**
** 01 22 2013 cp.wu
** [BORA00002253] [MT6630 Wi-Fi][Driver][Firmware] Add NLO and timeout mechanism to SCN module
** modification for ucBssIndex migration
**
** 09 17 2012 cm.chang
** [BORA00002149] [MT6630 Wi-Fi] Initial software development
** Duplicate source from MT6620 v2.3 driver branch
** (Davinci label: MT6620_WIFI_Driver_V2_3_120913_1942_As_MT6630_Base)
 *
 * 11 24 2011 wh.su
 * [WCXRP00001078] [MT6620 Wi-Fi][Driver] Adding the mediatek log improment support : XLOG
 * Adjust code for DBG and CONFIG_XLOG.
 *
 * 11 11 2011 wh.su
 * [WCXRP00001078] [MT6620 Wi-Fi][Driver] Adding the mediatek log improment support : XLOG
 * modify the xlog related code.
 *
 * 11 02 2011 wh.su
 * [WCXRP00001078] [MT6620 Wi-Fi][Driver] Adding the mediatek log improment support : XLOG
 * adding the code for XLOG.
 *
 * 08 31 2011 tsaiyuan.hsu
 * [WCXRP00000931] [MT5931 Wi-Fi][DRV/FW] add swcr to disable roaming from driver
 * remove obsolete code.
 *
 * 08 15 2011 tsaiyuan.hsu
 * [WCXRP00000931] [MT5931 Wi-Fi][DRV/FW] add swcr to disable roaming from driver
 * add swcr in driver reg, 0x9fxx0000, to disable roaming .
 *
 * 03 16 2011 tsaiyuan.hsu
 * [WCXRP00000517] [MT6620 Wi-Fi][Driver][FW] Fine Tune Performance of Roaming
 * remove obsolete definition and unused variables.
 *
 * 02 26 2011 tsaiyuan.hsu
 * [WCXRP00000391] [MT6620 Wi-Fi][FW] Add Roaming Support
 * not send disassoc or deauth to leaving AP so as to improve performace of roaming.
 *
 * 01 27 2011 tsaiyuan.hsu
 * [WCXRP00000392] [MT6620 Wi-Fi][Driver] Add Roaming Support
 * add roaming fsm
 * 1. not support 11r, only use strength of signal to determine roaming.
 * 2. not enable CFG_SUPPORT_ROAMING until completion of full test.
 * 3. in 6620, adopt work-around to avoid sign extension problem of cck of hw
 * 4. assume that change of link quality in smooth way.
 *
 * 01 27 2011 tsaiyuan.hsu
 * [WCXRP00000392] [MT6620 Wi-Fi][Driver] Add Roaming Support
 * add roaming fsm
 * 1. not support 11r, only use strength of signal to determine roaming.
 * 2. not enable CFG_SUPPORT_ROAMING until completion of full test.
 * 3. in 6620, adopt work-around to avoid sign extension problem of cck of hw
 * 4. assume that change of link quality in smooth way.
 *
*/

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "precomp.h"

#if CFG_SUPPORT_ROAMING
/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

#if DBG
/*lint -save -e64 Type mismatch */
static PUINT_8 apucDebugRoamingState[ROAMING_STATE_NUM] = {
	(PUINT_8) DISP_STRING("ROAMING_STATE_IDLE"),
	(PUINT_8) DISP_STRING("ROAMING_STATE_DECISION"),
	(PUINT_8) DISP_STRING("ROAMING_STATE_DISCOVERY"),
	(PUINT_8) DISP_STRING("ROAMING_STATE_ROAM")
};

/*lint -restore */
#endif /* DBG */

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

/*******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

/*----------------------------------------------------------------------------*/
/*!
* @brief Initialize the value in ROAMING_FSM_INFO_T for ROAMING FSM operation
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID roamingFsmInit(IN P_ADAPTER_T prAdapter)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	P_CONNECTION_SETTINGS_T prConnSettings;

	DBGLOG(ROAMING, LOUD, "->roamingFsmInit(): Current Time = %ld\n", kalGetTimeTick());

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);
	prConnSettings = &(prAdapter->rWifiVar.rConnSettings);

	/* 4 <1> Initiate FSM */
	prRoamingFsmInfo->fgIsEnableRoaming = prConnSettings->fgIsEnableRoaming;
	prRoamingFsmInfo->eCurrentState = ROAMING_STATE_IDLE;
	prRoamingFsmInfo->rRoamingDiscoveryUpdateTime = 0;
}				/* end of roamingFsmInit() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Uninitialize the value in AIS_FSM_INFO_T for AIS FSM operation
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID roamingFsmUninit(IN P_ADAPTER_T prAdapter)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;

	DBGLOG(ROAMING, LOUD, "->roamingFsmUninit(): Current Time = %ld\n", kalGetTimeTick());

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	prRoamingFsmInfo->eCurrentState = ROAMING_STATE_IDLE;

}				/* end of roamingFsmUninit() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Send commands to firmware
*
* @param [IN P_ADAPTER_T]       prAdapter
*        [IN P_ROAMING_PARAM_T] prParam
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID roamingFsmSendCmd(IN P_ADAPTER_T prAdapter, IN P_CMD_ROAMING_TRANSIT_T prTransit)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	WLAN_STATUS rStatus;

	DBGLOG(ROAMING, LOUD, "->roamingFsmSendCmd(): Current Time = %ld\n", kalGetTimeTick());

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	rStatus = wlanSendSetQueryCmd(prAdapter,	/* prAdapter */
				      CMD_ID_ROAMING_TRANSIT,	/* ucCID */
				      TRUE,	/* fgSetQuery */
				      FALSE,	/* fgNeedResp */
				      FALSE,	/* fgIsOid */
				      NULL,	/* pfCmdDoneHandler */
				      NULL,	/* pfCmdTimeoutHandler */
				      sizeof(CMD_ROAMING_TRANSIT_T),	/* u4SetQueryInfoLen */
				      (PUINT_8) prTransit,	/* pucInfoBuffer */
				      NULL,	/* pvSetQueryBuffer */
				      0	/* u4SetQueryBufferLen */
	    );

	/* ASSERT(rStatus == WLAN_STATUS_PENDING); */

}				/* end of roamingFsmSendCmd() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Update the recent time when ScanDone occurred
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID roamingFsmScanResultsUpdate(IN P_ADAPTER_T prAdapter)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	/* Check Roaming Conditions */
	do {
		if (!(prRoamingFsmInfo->fgIsEnableRoaming))
			return;
	} while (0);

	DBGLOG(ROAMING, LOUD, "->roamingFsmScanResultsUpdate(): Current Time = %ld\n", kalGetTimeTick());

	GET_CURRENT_SYSTIME(&prRoamingFsmInfo->rRoamingDiscoveryUpdateTime);

}				/* end of roamingFsmScanResultsUpdate() */

/*----------------------------------------------------------------------------*/
/*!
* @brief The Core FSM engine of ROAMING for AIS Infra.
*
* @param [IN P_ADAPTER_T]          prAdapter
*        [IN ENUM_ROAMING_STATE_T] eNextState Enum value of next AIS STATE
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID roamingFsmSteps(IN P_ADAPTER_T prAdapter, IN ENUM_ROAMING_STATE_T eNextState)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	ENUM_ROAMING_STATE_T ePreviousState;
	BOOLEAN fgIsTransition = (BOOLEAN) FALSE;

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	do {

		/* Do entering Next State */
#if DBG
		DBGLOG(ROAMING, STATE, "TRANSITION: [%s] -> [%s]\n",
					apucDebugRoamingState[prRoamingFsmInfo->eCurrentState],
					apucDebugRoamingState[eNextState]);
#else
		DBGLOG(ROAMING, STATE, "[%d] TRANSITION: [%d] -> [%d]\n",
					DBG_ROAMING_IDX, prRoamingFsmInfo->eCurrentState, eNextState);
#endif
		/* NOTE(Kevin): This is the only place to change the eCurrentState(except initial) */
		ePreviousState = prRoamingFsmInfo->eCurrentState;
		prRoamingFsmInfo->eCurrentState = eNextState;

		fgIsTransition = (BOOLEAN) FALSE;

		/* Do tasks of the State that we just entered */
		switch (prRoamingFsmInfo->eCurrentState) {
			/* NOTE(Kevin): we don't have to rearrange the sequence of following
			 * switch case. Instead I would like to use a common lookup table of array
			 * of function pointer to speed up state search.
			 */
		case ROAMING_STATE_IDLE:
		case ROAMING_STATE_DECISION:
			break;

		case ROAMING_STATE_DISCOVERY:
			{
				OS_SYSTIME rCurrentTime;

				GET_CURRENT_SYSTIME(&rCurrentTime);
				if (CHECK_FOR_TIMEOUT
				    (rCurrentTime, prRoamingFsmInfo->rRoamingDiscoveryUpdateTime,
				     SEC_TO_SYSTIME(ROAMING_DISCOVERY_TIMEOUT_SEC))) {
					DBGLOG(ROAMING, LOUD, "roamingFsmSteps: DiscoveryUpdateTime Timeout");
					aisFsmRunEventRoamingDiscovery(prAdapter, TRUE);
				} else {
					DBGLOG(ROAMING, LOUD, "roamingFsmSteps: DiscoveryUpdateTime Updated");
					aisFsmRunEventRoamingDiscovery(prAdapter, FALSE);
				}
			}
			break;

		case ROAMING_STATE_ROAM:
			break;

		default:
			ASSERT(0);	/* Make sure we have handle all STATEs */
		}
	} while (fgIsTransition);

}				/* end of roamingFsmSteps() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Transit to Decision state after join completion
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID roamingFsmRunEventStart(IN P_ADAPTER_T prAdapter)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	ENUM_ROAMING_STATE_T eNextState;
	P_BSS_INFO_T prAisBssInfo;
	CMD_ROAMING_TRANSIT_T rTransit;

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	/* Check Roaming Conditions */
	do {
		if (!(prRoamingFsmInfo->fgIsEnableRoaming))
			return;
	} while (0);

	prAisBssInfo = prAdapter->prAisBssInfo;
	if (prAisBssInfo->eCurrentOPMode != OP_MODE_INFRASTRUCTURE)
		return;

	DBGLOG(ROAMING, EVENT, "EVENT-ROAMING START: Current Time = %ld\n", kalGetTimeTick());

	/* IDLE, ROAM -> DECISION */
	/* Errors as DECISION, DISCOVERY -> DECISION */
	if (!(prRoamingFsmInfo->eCurrentState == ROAMING_STATE_IDLE
	      || prRoamingFsmInfo->eCurrentState == ROAMING_STATE_ROAM))
		return;

	eNextState = ROAMING_STATE_DECISION;
	if (eNextState != prRoamingFsmInfo->eCurrentState) {
		rTransit.u2Event = ROAMING_EVENT_START;
		rTransit.u2Data = prAisBssInfo->ucBssIndex;
		roamingFsmSendCmd(prAdapter, (P_CMD_ROAMING_TRANSIT_T) & rTransit);

		/* Step to next state */
		roamingFsmSteps(prAdapter, eNextState);
	}
}				/* end of roamingFsmRunEventStart() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Transit to Discovery state when deciding to find a candidate
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID roamingFsmRunEventDiscovery(IN P_ADAPTER_T prAdapter, IN UINT_32 u4Param)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	ENUM_ROAMING_STATE_T eNextState;

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	/* Check Roaming Conditions */
	do {
		if (!(prRoamingFsmInfo->fgIsEnableRoaming))
			return;
	} while (0);

	DBGLOG(ROAMING, EVENT, "EVENT-ROAMING DISCOVERY: Current Time = %ld\n", kalGetTimeTick());

	/* DECISION -> DISCOVERY */
	/* Errors as IDLE, DISCOVERY, ROAM -> DISCOVERY */
	if (prRoamingFsmInfo->eCurrentState != ROAMING_STATE_DECISION)
		return;

	eNextState = ROAMING_STATE_DISCOVERY;
	/* DECISION -> DISCOVERY */
	if (eNextState != prRoamingFsmInfo->eCurrentState) {
		P_BSS_INFO_T prAisBssInfo;
		P_BSS_DESC_T prBssDesc;

		/* sync. rcpi with firmware */
		prAisBssInfo = prAdapter->prAisBssInfo;
		prBssDesc = scanSearchBssDescByBssid(prAdapter, prAisBssInfo->aucBSSID);
		if (prBssDesc)
			prBssDesc->ucRCPI = (UINT_8) (u4Param & 0xff);

		roamingFsmSteps(prAdapter, eNextState);
	}

}				/* end of roamingFsmRunEventDiscovery() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Transit to Roam state after Scan Done
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID roamingFsmRunEventRoam(IN P_ADAPTER_T prAdapter)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	ENUM_ROAMING_STATE_T eNextState;
	CMD_ROAMING_TRANSIT_T rTransit;

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	/* Check Roaming Conditions */
	do {
		if (!(prRoamingFsmInfo->fgIsEnableRoaming))
			return;
	} while (0);

	DBGLOG(ROAMING, EVENT, "EVENT-ROAMING ROAM: Current Time = %ld\n", kalGetTimeTick());

	/* IDLE, ROAM -> DECISION */
	/* Errors as IDLE, DECISION, ROAM -> ROAM */
	if (prRoamingFsmInfo->eCurrentState != ROAMING_STATE_DISCOVERY)
		return;

	eNextState = ROAMING_STATE_ROAM;
	/* DISCOVERY -> ROAM */
	if (eNextState != prRoamingFsmInfo->eCurrentState) {
		rTransit.u2Event = ROAMING_EVENT_ROAM;
		roamingFsmSendCmd(prAdapter, (P_CMD_ROAMING_TRANSIT_T) & rTransit);

		/* Step to next state */
		roamingFsmSteps(prAdapter, eNextState);
	}

}				/* end of roamingFsmRunEventRoam() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Transit to Decision state as being failed to find out any candidate
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID roamingFsmRunEventFail(IN P_ADAPTER_T prAdapter, IN UINT_32 u4Param)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	ENUM_ROAMING_STATE_T eNextState;
	CMD_ROAMING_TRANSIT_T rTransit;

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	/* Check Roaming Conditions */
	do {
		if (!(prRoamingFsmInfo->fgIsEnableRoaming))
			return;
	} while (0);

	DBGLOG(ROAMING, EVENT, "EVENT-ROAMING FAIL: reason %x Current Time = %ld\n", u4Param, kalGetTimeTick());

	/* IDLE, ROAM -> DECISION */
	/* Errors as IDLE, DECISION, DISCOVERY -> DECISION */
	if (prRoamingFsmInfo->eCurrentState != ROAMING_STATE_ROAM)
		return;

	eNextState = ROAMING_STATE_DECISION;
	/* ROAM -> DECISION */
	if (eNextState != prRoamingFsmInfo->eCurrentState) {
		rTransit.u2Event = ROAMING_EVENT_FAIL;
		rTransit.u2Data = (UINT_16) (u4Param & 0xffff);
		roamingFsmSendCmd(prAdapter, (P_CMD_ROAMING_TRANSIT_T) & rTransit);

		/* Step to next state */
		roamingFsmSteps(prAdapter, eNextState);
	}

}				/* end of roamingFsmRunEventFail() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Transit to Idle state as beging aborted by other moduels, AIS
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID roamingFsmRunEventAbort(IN P_ADAPTER_T prAdapter)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	ENUM_ROAMING_STATE_T eNextState;
	CMD_ROAMING_TRANSIT_T rTransit;

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	do {
		if (!(prRoamingFsmInfo->fgIsEnableRoaming))
			return;
	} while (0);

	DBGLOG(ROAMING, TRACE, "EVENT-ROAMING ABORT: Current Time = %ld\n", kalGetTimeTick());

	eNextState = ROAMING_STATE_IDLE;
	/* IDLE, DECISION, DISCOVERY, ROAM -> IDLE */
	if (eNextState != prRoamingFsmInfo->eCurrentState) {
		rTransit.u2Event = ROAMING_EVENT_ABORT;
		roamingFsmSendCmd(prAdapter, (P_CMD_ROAMING_TRANSIT_T) & rTransit);

		/* Step to next state */
		roamingFsmSteps(prAdapter, eNextState);
	}

}				/* end of roamingFsmRunEventAbort() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Process events from firmware
*
* @param [IN P_ADAPTER_T]       prAdapter
*        [IN P_ROAMING_PARAM_T] prParam
*
* @return none
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS roamingFsmProcessEvent(IN P_ADAPTER_T prAdapter, IN P_CMD_ROAMING_TRANSIT_T prTransit)
{
	DBGLOG(ROAMING, LOUD, "ROAMING Process Events: Current Time = %ld\n", kalGetTimeTick());

	if (ROAMING_EVENT_DISCOVERY == prTransit->u2Event)
		roamingFsmRunEventDiscovery(prAdapter, prTransit->u2Data);

	return WLAN_STATUS_SUCCESS;
}

#endif
