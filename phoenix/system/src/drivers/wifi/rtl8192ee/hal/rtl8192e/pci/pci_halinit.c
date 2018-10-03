/******************************************************************************
 *
 * Copyright(c) 2012 - 2017 Realtek Corporation.
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
#define _HCI_HAL_INIT_C_

/* #include <drv_types.h> */
#include <rtl8192e_hal.h>

/* For Two MAC FPGA verify we must disable all MAC/BB/RF setting */
#define FPGA_UNKNOWN		0
#define FPGA_2MAC			1
#define FPGA_PHY			2
#define ASIC					3
#define BOARD_TYPE			ASIC

#if BOARD_TYPE == FPGA_2MAC
#else /* FPGA_PHY and ASIC */
	#define FPGA_RF_UNKOWN	0
	#define FPGA_RF_8225		1
	#define FPGA_RF_0222D		2
	#define FPGA_RF				FPGA_RF_0222D
#endif


u8 NORMAL_PAGE_NUM_8192EE[] = {0xe9, 0x08, 0x08, 0x08, 0x02};
u8 WMM_PAGE_NUM_8192EE[]    = {0x30, 0x20, 0x20, 0x20, 0x00};

/* -------------------------------------------------------------------
 *
 *	EEPROM Content Parsing
 *
 * ------------------------------------------------------------------- */

static VOID
Hal_ReadPROMContent_BT_8821A(
	IN PADAPTER	Adapter
)
{

#if MP_DRIVER == 1
	if (Adapter->registrypriv.mp_mode == 1)
		EFUSE_ShadowMapUpdate(Adapter, EFUSE_BT, _FALSE);
#endif

}

static VOID
hal_ReadIDs_8192EE(
	IN	PADAPTER	Adapter,
	IN	u8			*PROMContent,
	IN	BOOLEAN		AutoloadFail
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

	if (!AutoloadFail) {
		/* VID, DID */
		pHalData->EEPROMVID = ReadLE2Byte(&PROMContent[EEPROM_VID_8192EE]);
		pHalData->EEPROMDID = ReadLE2Byte(&PROMContent[EEPROM_DID_8192EE]);
		pHalData->EEPROMSVID = ReadLE2Byte(&PROMContent[EEPROM_SVID_8192EE]);
		pHalData->EEPROMSMID = ReadLE2Byte(&PROMContent[EEPROM_SMID_8192EE]);

		/* Customer ID, 0x00 and 0xff are reserved for Realtek.		 */
		pHalData->EEPROMCustomerID = *(u8 *)&PROMContent[EEPROM_CustomID_8192E];
		if (pHalData->EEPROMCustomerID == 0xFF)
			pHalData->EEPROMCustomerID = EEPROM_Default_CustomerID_8188E;
		pHalData->EEPROMSubCustomerID = EEPROM_Default_SubCustomerID;
	} else {
		pHalData->EEPROMVID		= 0;
		pHalData->EEPROMDID		= 0;
		pHalData->EEPROMSVID		= 0;
		pHalData->EEPROMSMID		= 0;

		/* Customer ID, 0x00 and 0xff are reserved for Realtek. */
		pHalData->EEPROMCustomerID	= EEPROM_Default_CustomerID;
		pHalData->EEPROMSubCustomerID	= EEPROM_Default_SubCustomerID;
	}

	RTW_INFO("VID = 0x%04X, DID = 0x%04X\n", pHalData->EEPROMVID, pHalData->EEPROMDID);
	RTW_INFO("SVID = 0x%04X, SMID = 0x%04X\n", pHalData->EEPROMSVID, pHalData->EEPROMSMID);
}

static VOID
Hal_ReadEfusePCIeCap8192EE(
	IN	PADAPTER	Adapter,
	IN	u8			*PROMContent,
	IN	BOOLEAN	AutoloadFail
)
{
	u8	AspmOscSupport = RT_PCI_ASPM_OSC_IGNORE;
	u16	PCIeCap = 0;

	if (!AutoloadFail) {
		PCIeCap = PROMContent[EEPROM_PCIE_DEV_CAP_01] |
			  (PROMContent[EEPROM_PCIE_DEV_CAP_02] << 8);

		RTW_INFO("Hal_ReadEfusePCIeCap8192EE(): PCIeCap = %#x\n", PCIeCap);

		/*  */
		/* <Roger_Notes> We shall take L0S/L1 accept latency into consideration for ASPM Configuration policy, 2013.03.27. */
		/* L1 accept Latency: 0x8d from PCI CFG space offset 0x75 */
		/* L0S accept Latency: 0x80 from PCI CFG space offset 0x74 */
		/*  */
		if (PCIeCap == 0x8d80)
			AspmOscSupport |= RT_PCI_ASPM_OSC_ENABLE;
		else
			AspmOscSupport |= RT_PCI_ASPM_OSC_DISABLE;
	}

	/* rtw_hal_set_def_var(Adapter, HAL_DEF_PCI_ASPM_OSC, (u8 *)&AspmOscSupport); */
	RTW_INFO("Hal_ReadEfusePCIeCap92EE(): AspmOscSupport = %d\n", AspmOscSupport);
}

static VOID
hal_CustomizedBehavior_8192EE(
	PADAPTER			Adapter
)
{
#ifdef CONFIG_RTW_SW_LED
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct led_priv	*pledpriv = &(Adapter->ledpriv);

	pledpriv->LedStrategy = SW_LED_MODE7; /* Default LED strategy. */
	pHalData->bLedOpenDrain = _TRUE;/* Support Open-drain arrangement for controlling the LED. Added by Roger, 2009.10.16. */

	switch (pHalData->CustomerID) {
	case RT_CID_DEFAULT:
		break;

	default:
		/* RTW_INFO("Unkown hardware Type\n"); */
		break;
	}
	RTW_INFO("hal_CustomizedBehavior_8192EE(): RT Customized ID: 0x%02X\n", pHalData->CustomerID);

#if 0
	if (Adapter->bInHctTest) {
		pMgntInfo->PowerSaveControl.bInactivePs = FALSE;
		pMgntInfo->PowerSaveControl.bIPSModeBackup = FALSE;
		pMgntInfo->PowerSaveControl.bLeisurePs = FALSE;
		pMgntInfo->PowerSaveControl.bLeisurePsModeBackup = FALSE;
		pMgntInfo->keepAliveLevel = 0;
	}
#endif
#endif
}

static VOID
hal_CustomizeByCustomerID_8192EE(
	IN	PADAPTER		pAdapter
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);

	/* If the customer ID had been changed by registry, do not cover up by EEPROM. */
	if (pHalData->CustomerID == RT_CID_DEFAULT) {
		switch (pHalData->EEPROMCustomerID) {
		default:
			pHalData->CustomerID = RT_CID_DEFAULT;
			break;

		}
	}
	RTW_INFO("MGNT Customer ID: 0x%2x\n", pHalData->CustomerID);

	hal_CustomizedBehavior_8192EE(pAdapter);
}


static VOID
InitAdapterVariablesByPROM_8192EE(
	IN	PADAPTER	Adapter
)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Adapter);

	/* RTW_INFO("InitAdapterVariablesByPROM_8821E()!!\n");  */

	hal_InitPGData_8192E(Adapter, pHalData->efuse_eeprom_data);
	Hal_EfuseParseIDCode8192E(Adapter, pHalData->efuse_eeprom_data);

	Hal_ReadPROMVersion8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	hal_ReadIDs_8192EE(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	hal_config_macaddr(Adapter, pHalData->bautoload_fail_flag);
	Hal_ReadPowerSavingMode8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_ReadTxPowerInfo8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_ReadBoardType8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_ReadEfusePCIeCap8192EE(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);

	/*  */
	/* Read Bluetooth co-exist and initialize */
	/*  */
	Hal_EfuseParseBTCoexistInfo8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);

	Hal_ReadChannelPlan8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_EfuseParseXtal_8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_ReadThermalMeter_8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_ReadPAType_8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_ReadAmplifierType_8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_ReadRFEType_8192E(Adapter, pHalData->efuse_eeprom_data,  pHalData->bautoload_fail_flag);
	Hal_ReadAntennaDiversity8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	hal_CustomizeByCustomerID_8192EE(Adapter);

	Hal_EfuseParseKFreeData_8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
}


static void Hal_ReadPROMContent_8192E(
	IN PADAPTER		Adapter
)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Adapter);
	u8			eeValue;

	/* check system boot selection */
	eeValue = rtw_read8(Adapter, REG_SYS_EEPROM_CTRL);
	pHalData->EepromOrEfuse		= (eeValue & EEPROMSEL) ? _TRUE : _FALSE;
	pHalData->bautoload_fail_flag	= (eeValue & EEPROM_EN) ? _FALSE : _TRUE;

	RTW_INFO("Boot from %s, Autoload %s !\n", (pHalData->EepromOrEfuse ? "EEPROM" : "EFUSE"),
		 (pHalData->bautoload_fail_flag ? "Fail" : "OK"));

	/* pHalData->EEType = IS_BOOT_FROM_EEPROM(Adapter) ? EEPROM_93C46 : EEPROM_BOOT_EFUSE; */

	InitAdapterVariablesByPROM_8192EE(Adapter);
}

static u8 ReadAdapterInfo8192EE(PADAPTER Adapter)
{
	/* Read all content in Efuse/EEPROM. */
	Hal_ReadPROMContent_8192E(Adapter);

	/* We need to define the RF type after all PROM value is recognized. */
	hal_ReadRFType_8192E(Adapter);

	return _SUCCESS;
}


void rtl8192ee_interface_configure(PADAPTER Adapter)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct dvobj_priv	*pdvobjpriv = adapter_to_dvobj(Adapter);
	struct pwrctrl_priv	*pwrpriv = dvobj_to_pwrctl(pdvobjpriv);


	/* close ASPM for AMD defaultly */
	pdvobjpriv->const_amdpci_aspm = 0;

	/* ASPM PS mode. */
	/* 0 - Disable ASPM, 1 - Enable ASPM without Clock Req, */
	/* 2 - Enable ASPM with Clock Req, 3- Alwyas Enable ASPM with Clock Req, */
	/* 4-  Always Enable ASPM without Clock Req. */
	/* set defult to rtl8188ee:3 RTL8192E:2 */
	pdvobjpriv->const_pci_aspm = 0;

	/* Setting for PCI-E device */
	pdvobjpriv->const_devicepci_aspm_setting = 0x03;

	/* Setting for PCI-E bridge */
	pdvobjpriv->const_hostpci_aspm_setting = 0x03;

	/* In Hw/Sw Radio Off situation. */
	/* 0 - Default, 1 - From ASPM setting without low Mac Pwr, */
	/* 2 - From ASPM setting with low Mac Pwr, 3 - Bus D3 */
	/* set default to RTL8192CE:0 RTL8192SE:2 */
	pdvobjpriv->const_hwsw_rfoff_d3 = 0;

	/* This setting works for those device with backdoor ASPM setting such as EPHY setting. */
	/* 0: Not support ASPM, 1: Support ASPM, 2: According to chipset. */
	pdvobjpriv->const_support_pciaspm = 1;

	pwrpriv->reg_rfoff = 0;
	pwrpriv->rfoff_reason = 0;

	pHalData->bL1OffSupport = _FALSE;
}

VOID
DisableInterrupt8192EE(
	IN PADAPTER			Adapter
)
{
	struct dvobj_priv	*pdvobjpriv = adapter_to_dvobj(Adapter);

	rtw_write32(Adapter, REG_HIMR0_8192E, IMR_DISABLED_8192E);

	rtw_write32(Adapter, REG_HIMR1_8192E, IMR_DISABLED_8192E);	/* by tynli */

	pdvobjpriv->irq_enabled = 0;
}

VOID
ClearInterrupt8192EE(
	IN PADAPTER			Adapter
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u32	tmp = 0;

	tmp = rtw_read32(Adapter, REG_HISR0_8192E);
	rtw_write32(Adapter, REG_HISR0_8192E, tmp);
	pHalData->IntArray[0] = 0;

	tmp = rtw_read32(Adapter, REG_HISR1_8192E);
	rtw_write32(Adapter, REG_HISR1_8192E, tmp);
	pHalData->IntArray[1] = 0;

}


VOID
EnableInterrupt8192EE(
	IN PADAPTER			Adapter
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct dvobj_priv	*pdvobjpriv = adapter_to_dvobj(Adapter);

	pdvobjpriv->irq_enabled = 1;

	rtw_write32(Adapter, REG_HIMR0_8192E, pHalData->IntrMask[0] & 0xFFFFFFFF);

	rtw_write32(Adapter, REG_HIMR1_8192E, pHalData->IntrMask[1] & 0xFFFFFFFF);

}

BOOLEAN
InterruptRecognized8192EE(
	IN	PADAPTER			Adapter
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	BOOLEAN			bRecognized = _FALSE;

	/* 2013.11.18 Glayrainx suggests that turn off IMR and */
	/* restore after cleaning ISR. */

	rtw_write32(Adapter, REG_HIMR0_8192E, 0);
	rtw_write32(Adapter, REG_HIMR1_8192E, 0);

	pHalData->IntArray[0] = rtw_read32(Adapter, REG_HISR0_8192E);
	pHalData->IntArray[0] &= pHalData->IntrMask[0];
	rtw_write32(Adapter, REG_HISR0_8192E, pHalData->IntArray[0]);

	/* For HISR extension. Added by tynli. 2009.10.07. */
	pHalData->IntArray[1] = rtw_read32(Adapter, REG_HISR1_8192E);
	pHalData->IntArray[1] &= pHalData->IntrMask[1];
	rtw_write32(Adapter, REG_HISR1_8192E, pHalData->IntArray[1]);

	if (((pHalData->IntArray[0]) & pHalData->IntrMask[0]) != 0 ||
	    ((pHalData->IntArray[1]) & pHalData->IntrMask[1]) != 0)
		bRecognized = _TRUE;

	/* restore IMR */
	rtw_write32(Adapter, REG_HIMR0_8192E, pHalData->IntrMask[0] & 0xFFFFFFFF);
	rtw_write32(Adapter, REG_HIMR1_8192E, pHalData->IntrMask[1] & 0xFFFFFFFF);

	return bRecognized;
}

VOID
UpdateInterruptMask8192EE(
	IN	PADAPTER		Adapter,
	IN	u32		AddMSR,	u32		AddMSR1,
	IN	u32		RemoveMSR, u32		RemoveMSR1
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

	DisableInterrupt8192EE(Adapter);

	if (AddMSR)
		pHalData->IntrMask[0] |= AddMSR;
	if (AddMSR1)
		pHalData->IntrMask[1] |= AddMSR1;

	if (RemoveMSR)
		pHalData->IntrMask[0] &= (~RemoveMSR);

	if (RemoveMSR1)
		pHalData->IntrMask[1] &= (~RemoveMSR1);

	EnableInterrupt8192EE(Adapter);
}

static VOID
HwConfigureRTL8192EE(
	IN	PADAPTER			Adapter
)
{

	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u32	regRRSR = 0;


	/* 1 This part need to modified according to the rate set we filtered!! */
	/*  */
	/* Set RRSR, RATR, and BW_OPMODE registers */
	/*  */
	switch (pHalData->CurrentWirelessMode) {
	case WIRELESS_MODE_B:
		regRRSR = RATE_ALL_CCK;
		break;
	case WIRELESS_MODE_A:
		regRRSR = RATE_ALL_OFDM_AG;
		break;
	case WIRELESS_MODE_G:
		regRRSR = RATE_ALL_CCK | RATE_ALL_OFDM_AG;
		break;
	case WIRELESS_MODE_UNKNOWN:
	case WIRELESS_MODE_AUTO:
	case WIRELESS_MODE_N_24G:
	case WIRELESS_MODE_AC_24G:
		regRRSR = RATE_ALL_CCK | RATE_ALL_OFDM_AG;
		break;
	case WIRELESS_MODE_N_5G:
	case WIRELESS_MODE_AC_5G:
		regRRSR = RATE_ALL_OFDM_AG;
		break;
	default:
		break;
	}

	/* Init value for RRSR. */
	rtw_write32(Adapter, REG_RRSR, regRRSR);

	/* TODO_8192EE */
#if 0
	/* ARFB table 9 for 11ac 5G 2SS */
#if (HAL_MAC_ENABLE == 0)
	rtw_write32(Adapter, REG_ARFR0, 0x00000010);
#endif
#endif
	/* TODO_8192EE */


	/* TODO_8192EE */
#if 0
	/* ARFB table 10 for 11ac 5G 1SS */
#if (HAL_MAC_ENABLE == 0)
	rtw_write32(Adapter, REG_ARFR2, 0x00000010);
#endif
#endif
	/* TODO_8192EE */
	/* Configure rate id */
	/*	bgn 40M 2ss RATEID_IDX_BGN_40M_2SS */
	rtw_write32(Adapter, REG_ARFR0_8192E, 0x0ffff015);

	/*	bgn 20M 2ss BN  */
	rtw_write32(Adapter, REG_ARFR2_8192E, 0x0f8ff0ff);

	/* Set SLOT time Reg518 0x9 */
	PlatformEFIOWrite1Byte(Adapter, REG_SLOT, 0x09);

	/* 0x420[7] = 0 , enable retry AMPDU in new AMPD not singal MPDU. */
	rtw_write16(Adapter, REG_FWHW_TXQ_CTRL, 0x1F80);


	/* Set retry limit */
	/* 3vivi, 20100928, especially for DTM, performance_ext, To avoid asoc too long to another AP more than 4.1 seconds. */
	/* 3we find retry 7 times maybe not enough, so we retry more than 7 times to pass DTM. */
	/* if(pMgntInfo->bPSPXlinkMode) */
	/* { */
	/*	pHalData->ShortRetryLimit = 3; */
	/*	pHalData->LongRetryLimit = 3;	 */
	/* Set retry limit */
	/*	rtw_write16(Adapter,REG_RL, 0x0303); */
	/* } */
	/* else */
	rtw_write16(Adapter, REG_RL, (RL_VAL_AP<<8) | RL_VAL_AP);

	/* BAR settings */
	rtw_write32(Adapter, REG_BAR_MODE_CTRL, 0x0201ffff);


	/* Set Data / Response auto rate fallack retry count */
	rtw_write32(Adapter, REG_DARFRC, 0x01000000);
	rtw_write32(Adapter, REG_DARFRC + 4, 0x07060504);
	rtw_write32(Adapter, REG_RARFRC, 0x01000000);
	rtw_write32(Adapter, REG_RARFRC + 4, 0x07060504);

	/* Beacon related, for rate adaptive */
	rtw_write8(Adapter, REG_ATIMWND, 0x2);

	rtw_write8(Adapter, REG_BCN_MAX_ERR, 0xff);


	/* 20100211 Joseph: Change original setting of BCN_CTRL(0x550) from */
	/* 0x1e(0x2c for test chip) ro 0x1f(0x2d for test chip). Set BIT0 of this register disable ATIM */
	/* function. Since we do not use HIGH_QUEUE anymore, ATIM function is no longer used. */
	/* Also, enable ATIM function may invoke HW Tx stop operation. This may cause ping failed */
	/* sometimes in long run test. So just disable it now. */

#ifdef CONFIG_CONCURRENT_MODE
	rtw_write16(Adapter, REG_BCN_CTRL, 0x1010);	/* For 2 PORT TSF SYNC */
#else
	rtw_write8(Adapter, REG_BCN_CTRL, 0x11);
#endif

	/* Marked out by Bruce, 2010-09-09. */
	/* This register is configured for the 2nd Beacon (multiple BSSID). */
	/* We shall disable this register if we only support 1 BSSID. */
	/* vivi guess 92d also need this, also 92d now doesnot set this reg */
	PlatformEFIOWrite1Byte(Adapter, REG_BCN_CTRL_1, 0);

	/* BCN_CTRL1 Reg551 0x10 */

	/* TBTT setup time */
	rtw_write8(Adapter, REG_TBTT_PROHIBIT, TBTT_PROHIBIT_SETUP_TIME);

	/* TBTT hold time: 0x540[19:8] */
	rtw_write8(Adapter, REG_TBTT_PROHIBIT + 1, TBTT_PROHIBIT_HOLD_TIME_STOP_BCN & 0xFF);
	rtw_write8(Adapter, REG_TBTT_PROHIBIT + 2,
		(rtw_read8(Adapter, REG_TBTT_PROHIBIT + 2) & 0xF0) | (TBTT_PROHIBIT_HOLD_TIME_STOP_BCN >> 8));

	rtw_write8(Adapter, REG_PIFS, 0);
	rtw_write8(Adapter, REG_AGGR_BREAK_TIME, 0x16);

	/* AGGR_BK_TIME Reg51A 0x16 */

	rtw_write16(Adapter, REG_NAV_PROT_LEN, 0x0040);
	rtw_write16(Adapter, REG_PROT_MODE_CTRL, 0x08ff);

	if (!Adapter->registrypriv.wifi_spec) {
		/* For Rx TP. Suggested by SD1 Richard. Added by tynli. 2010.04.12. */
		rtw_write32(Adapter, REG_FAST_EDCA_CTRL, 0x03086666);
	} else {
		/* For WiFi WMM. suggested by timchen. Added by tynli.	 */
		rtw_write32(Adapter, REG_FAST_EDCA_CTRL, 0x0);
	}

	/* ACKTO for IOT issue. */
	PlatformEFIOWrite1Byte(Adapter, REG_ACKTO, 0x40);

#if DISABLE_BB_RF

	/* 0x50 for 80MHz clock */
	rtw_write8(Adapter, REG_USTIME_TSF, 0x50);
	rtw_write8(Adapter, REG_USTIME_EDCA, 0x50);

	/* Set Spec SIFS (used in NAV) */
	rtw_write16(Adapter, REG_SPEC_SIFS, 0x1010);
	rtw_write16(Adapter, REG_MAC_SPEC_SIFS, 0x1010);

	/* Set SIFS for CCK */
	rtw_write16(Adapter, REG_SIFS_CTX, 0x1010);

	/* Set SIFS for OFDM */
	rtw_write16(Adapter, REG_SIFS_TRX, 0x1010);

	/* PIFS */
	rtw_write8(Adapter, REG_PIFS, 0);

	/* Protection Ctrl */
	rtw_write16(Adapter, REG_PROT_MODE_CTRL, 0x08ff);

	/* BAR settings */
	rtw_write32(Adapter, REG_BAR_MODE_CTRL, 0x0001ffff);

	/* ACKTO for IOT issue. */
	rtw_write8(Adapter, REG_ACKTO, 0x40);
#else
	/* Set Spec SIFS Reg428 Reg63A 0x100a */
	rtw_write16(Adapter, REG_SPEC_SIFS, 0x100a);
	rtw_write16(Adapter, REG_MAC_SPEC_SIFS, 0x100a);

	/* Set SIFS for CCK Reg514 0x100a */
	rtw_write16(Adapter, REG_SIFS_CTX, 0x100a);

	/* Set SIFS for OFDM Reg516 0x100a */
	rtw_write16(Adapter, REG_SIFS_TRX, 0x100a);

	/* Protection Ctrl Reg4C8 0x08ff */

	/* BAR settings Reg4CC 0x01ffff */

	/* PIFS Reg512 0x1C */

	/* ACKTO for IOT issue Reg640 0x80 */
#endif

	rtw_write8(Adapter, REG_SINGLE_AMPDU_CTRL, 0x80);

	rtw_write8(Adapter, REG_RX_PKT_LIMIT, 0x20);

	rtw_write16(Adapter, REG_MAX_AGGR_NUM, 0x1717);

	/* #if (OMNIPEEK_SNIFFER_ENABLED == 1) */
	/* For sniffer parsing legacy rate bandwidth information
	*	phy_set_bb_reg(Adapter, 0x834, BIT26, 0x0);
	* #endif */

	rtw_write32(Adapter, REG_MAR, 0xffffffff);
	rtw_write32(Adapter, REG_MAR + 4, 0xffffffff);

	/* Reject all control frame - default value is 0 */
	/* rtw_write16(Adapter,REG_RXFLTMAP1,0x0); */

}

static u32 _InitPowerOn_8192EE(PADAPTER padapter)
{
	u32	status = _SUCCESS;
	u8	value8, bMacPwrCtrlOn = _FALSE;
	u32	value32;

	RTW_INFO("==>%s\n", __FUNCTION__);


	/* TODO_8192EE */
	rtw_write8(padapter, REG_RSV_CTRL, 0x0);

	/* Auto Power Down to CHIP-off State */
	value8 = (rtw_read8(padapter, REG_APS_FSMCO + 1) & (~BIT7));
	rtw_write8(padapter, REG_APS_FSMCO + 1, value8);

	/* TODO_8192EE */


	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if (bMacPwrCtrlOn == _TRUE)
		return status;

	value32 = rtw_read32(padapter, REG_SYS_CFG1_8192E);
	if (value32 & BIT_SPSLDO_SEL) {
		/* LDO */
		rtw_write8(padapter, REG_LDO_SWR_CTRL, 0xC3);
	} else	{
		/* SPS */
		/* 0x7C [6] = 1¡¦b0 (IC default, 0x83) */
		/* 0x14[23:20]=b¡¦0101 (raise 1.2V voltage) */
		value8 = rtw_read8(padapter, 0x16);
		rtw_write8(padapter, 0x16, value8 | BIT4 | BIT6);

		/* u32 voltage = (rtw_read32(padapter,0x14)& 0xFF0FFFFF )|(0x05<<20); */
		/* rtw_write32(padapter,0x14,voltage); */

		rtw_write8(padapter, REG_LDO_SWR_CTRL, 0x83);
	}

	/* adjust xtal/afe before enable PLL, suggest by SD1-Scott */
	Hal_CrystalAFEAdjust(padapter);

	/* HW Power on sequence */
	if (!HalPwrSeqCmdParsing(padapter, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK, Rtl8192E_NIC_ENABLE_FLOW))
		return _FAIL;

	/* TODO_8192EE */
#if 0
	/* Enable Power Down Interrupt */
	tmpU1b = (rtw_read8(padapter, REG_APS_FSMCO) | BIT4);
	rtw_write8(padapter, REG_APS_FSMCO, tmpU1b);
#endif

	/* TODO_8192EE */

	/* Release MAC IO register reset */
	value8 = rtw_read8(padapter, REG_CR);
	value8 = 0xff;
	rtw_write8(padapter, REG_CR, value8);
	rtw_mdelay_os(2);

	value8 = 0x7f;
	rtw_write8(padapter, REG_HWSEQ_CTRL, value8);
	rtw_mdelay_os(2);

	/* Add for wakeup online */
	value8 = PlatformEFIORead1Byte(padapter, REG_SYS_CLKR);
	PlatformEFIOWrite1Byte(padapter, REG_SYS_CLKR, (value8 | BIT3));
	value8 = rtw_read8(padapter, REG_GPIO_MUXCFG + 1);
	rtw_write8(padapter, REG_GPIO_MUXCFG + 1, (value8 & ~BIT4));

	/* Release MAC IO register reset */
	/* 9.	CR 0x100[7:0]	= 0xFF; */
	/* 10.	CR 0x101[1]	= 0x01; */ /* Enable SEC block */
	rtw_write16(padapter, REG_CR, 0x2ff);

	bMacPwrCtrlOn = _TRUE;
	rtw_hal_set_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);

	return status;
}

static u8
_RQPN_Init_8192EE(
	IN  PADAPTER	Adapter,
	OUT u32		*pNPQRQPNVaule,
	OUT u32		*pRQPNValue
)
{
	u8			i, numNQ, numPubQ, numHQ, numLQ, numEQ;
	u8			llt_page_num;
	u8			*numPageArray;

	if (Adapter->registrypriv.wifi_spec)
		numPageArray = WMM_PAGE_NUM_8192EE;
	else
		numPageArray = NORMAL_PAGE_NUM_8192EE;

	numHQ   = numPageArray[1];
	numLQ   = numPageArray[2];
	numNQ   = numPageArray[3];
	numEQ   = numPageArray[4];

	llt_page_num = TX_TOTAL_PAGE_NUMBER_8192E - 1; /* Reserve 1 page at the boundary for safety */
	numPubQ = llt_page_num - (numHQ + numLQ + numNQ + numEQ);

	*pNPQRQPNVaule = _EPQ(numEQ) | _NPQ(numNQ);
	*pRQPNValue = _HPQ(numHQ) | _LPQ(numLQ) | _PUBQ(numPubQ) | LD_RQPN;

	/* Set whether Tx page size excludes the public queue */
	if (Adapter->registrypriv.wifi_spec)
		*pRQPNValue |= 0x0b000000; /* BIT_EPQ_PUBLIC_DIS|BIT_LPQ_PUBLIC_DIS|BIT_HPQ_PUBLIC_DIS	 */

	return TX_PAGE_BOUNDARY_8192E;
}

static s32
LLT_table_init_8192EE(
	IN	PADAPTER	Adapter,
	IN	u8			txpktbuf_bndy,
	IN	u32			RQPN,
	IN	u32			RQPN_NPQ
)
{
	u16	i, maxPage = 255;
	s32	status = _SUCCESS;
	u8	tmp1byte = 0, testcnt = 0;

	/* NPQ MUST be set before RQPN */
	rtw_write32(Adapter, REG_RQPN_NPQ, RQPN_NPQ);

	/* Set reserved page for each queue */
	/* 11.	RQPN */
	rtw_write32(Adapter, REG_RQPN, RQPN);

	_InitTxBufferBoundary_8192E(Adapter, txpktbuf_bndy);

	/* 15.	WMAC_LBK_BF_HD 0x45D[7:0] =  0xF6			 */ /* WMAC_LBK_BF_HD */
	PlatformEFIOWrite1Byte(Adapter, REG_WMAC_LBK_BUF_HD_8192E, txpktbuf_bndy);

	/* Set Tx/Rx page size (Tx must be 128 Bytes, Rx can be 64,128,256,512,1024 bytes) */
	/* 16.	PBP [7:0] = 0x11								 */ /* TRX page size */
	/*		This register is obseleted. Page size is configured in tx buffer descriptor */
	rtw_write8(Adapter, REG_PBP, 0x31);

	/* 17.	DRV_INFO_SZ = 0x04 */
	rtw_write8(Adapter, REG_RX_DRVINFO_SZ, DRVINFO_SZ);
	tmp1byte = rtw_read8(Adapter, REG_AUTO_LLT + 2);
	PlatformEFIOWrite1Byte(Adapter, REG_AUTO_LLT + 2, tmp1byte | BIT0);

	while (tmp1byte & BIT0) {
		tmp1byte = rtw_read8(Adapter, REG_AUTO_LLT + 2);
		rtw_udelay_os(10);
		testcnt++;
		if (testcnt > 10)
			break;

	}

	return _SUCCESS;
}

static  u32
InitMAC_8192EE(
	IN	PADAPTER	Adapter
)
{
	u8	tmpU1b;
	u16	tmpU2b;
	struct recv_priv	*precvpriv = &Adapter->recvpriv;
	struct xmit_priv	*pxmitpriv = &Adapter->xmitpriv;
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(Adapter);

	RTW_INFO("=======>InitMAC_8192EE()\n");

	rtw_hal_power_on(Adapter);

	/* if(!pHalData->bMACFuncEnable)	 */
	{
		/* System init */
		/* 18.	LLT_table_init(Adapter); */
		u32	RQPN = 0x80EB0808;
		u32	RQPN_NPQ = 0;
		u8	TX_PAGE_BOUNDARY =
			_RQPN_Init_8192EE(Adapter, &RQPN_NPQ, &RQPN);

		if (LLT_table_init_8192EE(Adapter, TX_PAGE_BOUNDARY, RQPN, RQPN_NPQ) == _FAIL)
			return _FAIL;
	}
	/* _InitQueuePriority_8192E(Adapter);		 */

	/* Enable Host Interrupt */
	rtw_write32(Adapter, REG_HISR0_8192E, 0xffffffff);
	rtw_write32(Adapter, REG_HISR1_8192E, 0xffffffff);

	tmpU2b = rtw_read16(Adapter, REG_TRXDMA_CTRL);
	tmpU2b &= 0xf;
	if (Adapter->registrypriv.wifi_spec)
		tmpU2b |= 0xF9B1;
	else
		tmpU2b |= 0xC5A0;	/* HIQ->HPQ, MGQ->EPQ, BKQ->LPQ, BEQ->LPQ, VIQ->NPQ, VOQ->NPQ */
	rtw_write16(Adapter, REG_TRXDMA_CTRL, tmpU2b);


	/* Reported Tx status from HW for rate adaptive. */
	/* 2009/12/03 MH This should be realtive to power on step 14. But in document V11  */
	/* still not contain the description.!!! */
	rtw_write8(Adapter, REG_FWHW_TXQ_CTRL + 1, 0x1F);

	/* Set RCR register */
	rtw_hal_set_hwreg(Adapter, HW_VAR_RCR, (u8 *)&pHalData->ReceiveConfig);
	rtw_write16(Adapter, REG_RXFLTMAP2, 0xFFFF);

	/* Set TCR register */
	rtw_write32(Adapter, REG_TCR, pHalData->TransmitConfig);

	/*  */
	/* Set TX/RX descriptor physical address(from OS API). */
	/*  */
	rtw_write32(Adapter, REG_BCNQ_TXBD_DESA_8192E, (u64)pxmitpriv->tx_ring[BCN_QUEUE_INX].dma & DMA_BIT_MASK(32));
	rtw_write32(Adapter, REG_MGQ_TXBD_DESA_8192E, (u64)pxmitpriv->tx_ring[MGT_QUEUE_INX].dma & DMA_BIT_MASK(32));
	rtw_write32(Adapter, REG_VOQ_TXBD_DESA_8192E, (u64)pxmitpriv->tx_ring[VO_QUEUE_INX].dma & DMA_BIT_MASK(32));
	rtw_write32(Adapter, REG_VIQ_TXBD_DESA_8192E, (u64)pxmitpriv->tx_ring[VI_QUEUE_INX].dma & DMA_BIT_MASK(32));
	rtw_write32(Adapter, REG_BEQ_TXBD_DESA_8192E, (u64)pxmitpriv->tx_ring[BE_QUEUE_INX].dma & DMA_BIT_MASK(32));
	rtw_write32(Adapter, REG_BKQ_TXBD_DESA_8192E, (u64)pxmitpriv->tx_ring[BK_QUEUE_INX].dma & DMA_BIT_MASK(32));
	rtw_write32(Adapter, REG_HI0Q_TXBD_DESA_8192E, (u64)pxmitpriv->tx_ring[HIGH_QUEUE_INX].dma & DMA_BIT_MASK(32));
	rtw_write32(Adapter, REG_RXQ_RXBD_DESA_8192E, (u64)precvpriv->rx_ring[RX_MPDU_QUEUE].dma & DMA_BIT_MASK(32));

#ifdef CONFIG_64BIT_DMA
	/* 2009/10/28 MH For DMA 64 bits. We need to assign the high 32 bit address */
	/* for NIC HW to transmit data to correct path. */
	rtw_write32(Adapter, REG_BCNQ_TXBD_DESA_8192E+4,
		    ((u64)pxmitpriv->tx_ring[BCN_QUEUE_INX].dma) >> 32);
	rtw_write32(Adapter, REG_MGQ_TXBD_DESA_8192E+4,
		    ((u64)pxmitpriv->tx_ring[MGT_QUEUE_INX].dma) >> 32);
	rtw_write32(Adapter, REG_VOQ_TXBD_DESA_8192E+4,
		    ((u64)pxmitpriv->tx_ring[VO_QUEUE_INX].dma) >> 32);
	rtw_write32(Adapter, REG_VIQ_TXBD_DESA_8192E+4,
		    ((u64)pxmitpriv->tx_ring[VI_QUEUE_INX].dma) >> 32);
	rtw_write32(Adapter, REG_BEQ_TXBD_DESA_8192E+4,
		    ((u64)pxmitpriv->tx_ring[BE_QUEUE_INX].dma) >> 32);
	rtw_write32(Adapter, REG_BKQ_TXBD_DESA_8192E+4,
		    ((u64)pxmitpriv->tx_ring[BK_QUEUE_INX].dma) >> 32);
	rtw_write32(Adapter, REG_HI0Q_TXBD_DESA_8192E+4,
		    ((u64)pxmitpriv->tx_ring[HIGH_QUEUE_INX].dma) >> 32);
	rtw_write32(Adapter, REG_RXQ_RXBD_DESA_8192E+4,
		    ((u64)precvpriv->rx_ring[RX_MPDU_QUEUE].dma) >> 32);

	/* 2009/10/28 MH If RX descriptor address is not equal to zero. We will enable */
	/* DMA 64 bit functuion. */
	/* Note: We never saw thd consition which the descripto address are divided into */
	/* 4G down and 4G upper seperate area. */
	if (((u64)precvpriv->rx_ring[RX_MPDU_QUEUE].dma) >> 32 != 0) {
		/* RTW_INFO("RX_DESC_HA=%08lx\n", ((u64)priv->rx_ring_dma[RX_MPDU_QUEUE])>>32); */
		RTW_INFO("Enable DMA64 bit\n");

		/* Check if other descriptor address is zero and abnormally be in 4G lower area. */
		if (((u64)pxmitpriv->tx_ring[MGT_QUEUE_INX].dma) >> 32)
			RTW_INFO("MGNT_QUEUE HA=0\n");

		PlatformEnableDMA64(Adapter);
	} else
		RTW_INFO("Enable DMA32 bit\n");
#endif

	/* pci buffer descriptor mode: Reset the Read/Write point to 0 */
	PlatformEFIOWrite4Byte(Adapter, REG_TSFTIMER_HCI_8192E, 0x3fffffff);

	tmpU1b = rtw_read8(Adapter, REG_PCIE_CTRL_REG_8192E+3);
	rtw_write8(Adapter, REG_PCIE_CTRL_REG_8192E+3, (tmpU1b | 0xF7));

	/* 20100318 Joseph: Reset interrupt migration setting when initialization. Suggested by SD1. */
	rtw_write32(Adapter, REG_INT_MIG, 0);
	pHalData->bInterruptMigration = _FALSE;

	/* 2009.10.19. Reset H2C protection register. by tynli. */
	rtw_write32(Adapter, REG_MCUTST_1, 0x0);

#if MP_DRIVER == 1
	if (Adapter->registrypriv.mp_mode == 1) {
		rtw_write32(Adapter, REG_MACID, 0x87654321);
		rtw_write32(Adapter, 0x0700, 0x87654321);
	}
#endif

	/* pic buffer descriptor mode: */
	/* ---- tx */
	rtw_write16(Adapter, REG_MGQ_TXBD_NUM_8192E, TX_DESC_NUM_92E | ((RTL8192EE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_VOQ_TXBD_NUM_8192E, TX_DESC_NUM_92E | ((RTL8192EE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_VIQ_TXBD_NUM_8192E, TX_DESC_NUM_92E | ((RTL8192EE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_BEQ_TXBD_NUM_8192E, TX_DESC_NUM_92E | ((RTL8192EE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_VOQ_TXBD_NUM_8192E, TX_DESC_NUM_92E | ((RTL8192EE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_BKQ_TXBD_NUM_8192E, TX_DESC_NUM_92E | ((RTL8192EE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_HI0Q_TXBD_NUM_8192E, TX_DESC_NUM_92E | ((RTL8192EE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_HI1Q_TXBD_NUM_8192E, TX_DESC_NUM_92E | ((RTL8192EE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_HI2Q_TXBD_NUM_8192E, TX_DESC_NUM_92E | ((RTL8192EE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_HI3Q_TXBD_NUM_8192E, TX_DESC_NUM_92E | ((RTL8192EE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_HI4Q_TXBD_NUM_8192E, TX_DESC_NUM_92E | ((RTL8192EE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_HI5Q_TXBD_NUM_8192E, TX_DESC_NUM_92E | ((RTL8192EE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_HI6Q_TXBD_NUM_8192E, TX_DESC_NUM_92E | ((RTL8192EE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_HI7Q_TXBD_NUM_8192E, TX_DESC_NUM_92E | ((RTL8192EE_SEG_NUM << 12) & 0x3000));
	/* ---- rx. support 32 bits in linux
	* 8192EE_TODO	 */
	/* rtw_write16(Adapter, REG_RX_RXBD_NUM_8192E, RX_DESC_NUM_92E |((RTL8192EE_SEG_NUM<<13 ) & 0x6000) |0x8000);  */ /* using 64bit */
	rtw_write16(Adapter, REG_RX_RXBD_NUM_8192E, RX_DESC_NUM_92E | ((RTL8192EE_SEG_NUM << 13) & 0x6000)); /* using 32bit
 * 8192EE_TODO	 */
	/* ---- reset read/write point */
	rtw_write32(Adapter, REG_TSFTIMER_HCI_8192E, 0XFFFFFFFF); /* Reset read/write point */


	rtw_write8(Adapter, 0x577, 0x03);

	/* for Crystal 40 Mhz setting */
	rtw_write8(Adapter, REG_AFE_CTRL4_8192E, 0x2A);
	rtw_write8(Adapter, REG_AFE_CTRL4_8192E+1, 0x00);
	rtw_write8(Adapter, REG_AFE_CTRL2_8192E, 0x83);

	/* Forced the antenna b to wifi. */
	if (pHalData->EEPROMBluetoothCoexist == FALSE) {
		u1Byte tmp1Byte = 0;

		tmp1Byte = rtw_read8(Adapter, 0x64);
		rtw_write8(Adapter, 0x64, (tmp1Byte & 0xFB) & ~BIT2);

		tmp1Byte = rtw_read8(Adapter, 0x65);
		rtw_write8(Adapter, 0x65, (tmp1Byte & 0xFE) | BIT0);

		tmp1Byte = rtw_read8(Adapter, 0x4F);
		rtw_write8(Adapter, 0x4F, (tmp1Byte & 0xF6) | BIT3 | BIT0);
	}

	pHalData->RxTag = 0;
	/* Release Pcie Interface Tx DMA. */
#if defined(USING_RX_TAG)
	rtw_write16(Adapter, REG_PCIE_CTRL_REG_8192E, 0x8000);
#else
	rtw_write16(Adapter, REG_PCIE_CTRL_REG_8192E, 0x0000);
#endif


	RTW_INFO("InitMAC_8192EE() <====\n");

	return _SUCCESS;
}
/* TODO_8192EE */
#if 0
u16
hal_MDIORead_8192EE(
	IN	PADAPTER	Adapter,
	IN	u8			Addr
)
{
	u16	ret = 0;

	u8	tmpU1b = 0, count = 0;

	rtw_write8(Adapter, REG_MDIO_CTL_8812, Addr | BIT6);
	tmpU1b = rtw_read8(Adapter, REG_MDIO_CTL_8812) & BIT6 ;
	count = 0;
	while (tmpU1b && count < 20) {
		rtw_udelay_os(10);
		tmpU1b = rtw_read8(Adapter, REG_MDIO_CTL_8812) & BIT6;
		count++;
	}
	if (0 == tmpU1b)
		ret = rtw_read16(Adapter, REG_MDIO_RDATA_8812);

	return ret;
}

VOID
hal_MDIOWrite_8812E(
	IN	PADAPTER	Adapter,
	IN	u8			Addr,
	IN	u16			Data
)
{
	u8	tmpU1b = 0, count = 0;

	rtw_write16(Adapter, REG_MDIO_WDATA_8812, Data);
	rtw_write8(Adapter, REG_MDIO_CTL_8812, Addr | BIT5);
	tmpU1b = rtw_read8(Adapter, REG_MDIO_CTL_8812) & BIT5 ;
	count = 0;
	while (tmpU1b && count < 20) {
		rtw_udelay_os(10);
		tmpU1b = rtw_read8(Adapter, REG_MDIO_CTL_8812) & BIT5;
		count++;
	}
}

#endif
/* TODO_8192EE */


static u32 rtl8192ee_hal_init(PADAPTER Adapter)
{
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	struct pwrctrl_priv		*pwrpriv = adapter_to_pwrctl(Adapter);
	struct registry_priv  *registry_par = &Adapter->registrypriv;
	u32	rtStatus = _SUCCESS;
	u8	tmpU1b, u1bRegCR;
	u32	i;
	BOOLEAN bSupportRemoteWakeUp, is2T2R = TRUE;
	u32	NavUpper = WiFiNavUpperUs;


	/*  */
	/* No I/O if device has been surprise removed */
	/*  */
	if (rtw_is_surprise_removed(Adapter)) {
		RTW_INFO("rtl8812ae_hal_init(): bSurpriseRemoved!\n");
		return _SUCCESS;
	}

	Adapter->init_adpt_in_progress = _TRUE;
	RTW_INFO("=======>rtl8192ee_hal_init()\n");

	tmpU1b = rtw_read8(Adapter, REG_SYS_CLKR + 1);
	u1bRegCR = rtw_read8(Adapter, REG_CR);
	RTW_INFO(" power-on :REG_CR 0x100=0x%02x.\n", u1bRegCR);
	if ((tmpU1b & BIT3) && (u1bRegCR != 0 && u1bRegCR != 0xEA)) {
		/* pHalData->bMACFuncEnable = TRUE; */
		RTW_INFO(" MAC has already power on.\n");
	} else {
		/* pHalData->bMACFuncEnable = FALSE;		 */
		/* Set FwPSState to ALL_ON mode to prevent from the I/O be return because of 32k */
		/* state which is set before sleep under wowlan mode. 2012.01.04. by tynli. */
		/* pHalData->FwPSState = FW_PS_STATE_ALL_ON_88E; */
	}

	/*  */
	/* 1. MAC Initialize */
	/*  */
	rtStatus = InitMAC_8192EE(Adapter);
	if (rtStatus != _SUCCESS) {
		RTW_INFO("Init MAC failed\n");
		return rtStatus;
	}

#if HAL_FW_ENABLE
	if (Adapter->registrypriv.mp_mode == 0) {
		tmpU1b = rtw_read8(Adapter, REG_SYS_CFG);
		rtw_write8(Adapter, REG_SYS_CFG, tmpU1b & 0x7F);

		rtStatus = FirmwareDownload8192E(Adapter, _FALSE);

		if (rtStatus != _SUCCESS) {
			RTW_INFO("FwLoad failed\n");
			rtStatus = _SUCCESS;
			pHalData->bFWReady = _FALSE;
			pHalData->fw_ractrl = _FALSE;
		} else {
			RTW_INFO("FwLoad SUCCESSFULLY!!!\n");
			pHalData->bFWReady = _TRUE;
			pHalData->fw_ractrl = _TRUE;
		}
	}
#endif

	pHalData->current_channel = 0;/* set 0 to trigger switch correct channel	 */

	/*  */
	/* 2. Initialize MAC/PHY Config by MACPHY_reg.txt */
	/*  */
#if (HAL_MAC_ENABLE == 1)
	RTW_INFO("8192ee MAC Config Start!\n");
	rtStatus = PHY_MACConfig8192E(Adapter);
	if (rtStatus != _SUCCESS) {
		RTW_INFO("8192ee MAC Config failed\n");
		return rtStatus;
	}
	RTW_INFO("8192ee MAC Config Finished!\n");


	/* TODO_8192EE */
	/* remove to sync with windows driver */
	/* rtw_write32(Adapter,REG_RCR, rtw_read32(Adapter, REG_RCR)&~(RCR_ADF) );
	* TODO_8192EE	 */
#endif	/*  #if (HAL_MAC_ENABLE == 1) */

	/*  */
	/* 3. Initialize BB After MAC Config PHY_reg.txt, AGC_Tab.txt */
	/*  */
#if (HAL_BB_ENABLE == 1)
	RTW_INFO("BB Config Start!\n");
	rtStatus = PHY_BBConfig8192E(Adapter);
	if (rtStatus != _SUCCESS) {
		RTW_INFO("BB Config failed\n");
		return rtStatus;
	}
	RTW_INFO("BB Config Finished!\n");
#endif	/*  #if (HAL_BB_ENABLE == 1) */

	/*  */
	/* 4. Initiailze RF RAIO_A.txt RF RAIO_B.txt */
	/*  */
#if (HAL_RF_ENABLE == 1)
	RTW_INFO("RF Config started!\n");
	rtStatus = PHY_RFConfig8192E(Adapter);
	if (rtStatus != _SUCCESS) {
		RTW_INFO("RF Config failed\n");
		return rtStatus;
	}
	RTW_INFO("RF Config Finished!\n");


#endif	/*  #if (HAL_RF_ENABLE == 1) */


	/* TODO_8192EE */
	/* Port from Windows Driver */
	pHalData->RfRegChnlVal[0] = phy_query_rf_reg(Adapter, 0, RF_CHNLBW, bRFRegOffsetMask);
	pHalData->RfRegChnlVal[1] = phy_query_rf_reg(Adapter, 1, RF_CHNLBW, bRFRegOffsetMask);
	/* pHalData->backupRF0x1A = (u4Byte)phy_query_rf_reg(Adapter, RF_PATH_A, RF_RX_G1, bRFRegOffsetMask);		 */
	pHalData->RfRegChnlVal[0] = ((pHalData->RfRegChnlVal[0] & 0xfffff3ff) | BIT10 | BIT11);
	phy_set_rf_reg(Adapter, RF_PATH_A, RF_CHNLBW, bRFRegOffsetMask, pHalData->RfRegChnlVal[0]);
	phy_set_rf_reg(Adapter, RF_PATH_B, RF_CHNLBW, bRFRegOffsetMask, pHalData->RfRegChnlVal[0]);

	/*---- Set CCK and OFDM Block "ON"----*/

	/*---- Set CCK and OFDM Block "ON"----*/
	phy_set_bb_reg(Adapter, rFPGA0_RFMOD, bCCKEn, 0x1);
	phy_set_bb_reg(Adapter, rFPGA0_RFMOD, bOFDMEn, 0x1);

	/* Must set this, otherwise the rx sensitivity will be very pool. Maddest */
	phy_set_rf_reg(Adapter, RF_PATH_A, 0xB1, bRFRegOffsetMask, 0x54418);


	/* 3 Set Hardware(MAC default setting.) */
	HwConfigureRTL8192EE(Adapter);

	/* TODO_8192EE
	 *	if(Adapter->registrypriv.channel <= 14)
	 *		PHY_SwitchWirelessBand8812(Adapter, BAND_ON_2_4G);
	 *	else
	 *		PHY_SwitchWirelessBand8812(Adapter, BAND_ON_5G);
	 * TODO_8192EE */
	rtw_hal_set_chnl_bw(Adapter, Adapter->registrypriv.channel,
		CHANNEL_WIDTH_20, HAL_PRIME_CHNL_OFFSET_DONT_CARE, HAL_PRIME_CHNL_OFFSET_DONT_CARE);

	/* We should set pHalData->bMACFuncEnable to TURE after LLT initialize and Fw download */
	/* and before GPIO detect. by tynli. 2011.05.27. */
	/* pHalData->bMACFuncEnable = _TRUE; */

	/* 3Security related */
	/* ----------------------------------------------------------------------------- */
	/* Set up security related. 070106, by rcnjko: */
	/* 1. Clear all H/W keys. */
	/* ----------------------------------------------------------------------------- */
	invalidate_cam_all(Adapter);

	/* Joseph debug: MAC_SEC_EN need to be set */
	rtw_write8(Adapter, REG_CR + 1, (rtw_read8(Adapter, REG_CR + 1) | BIT1));

	/* 2======================================================= */
	/* RF Power Save */
	/* 2======================================================= */
	/* Fix the bug that Hw/Sw radio off before S3/S4, the RF off action will not be executed */
	/* in MgntActSet_RF_State() after wake up, because the value of pHalData->eRFPowerState */
	/* is the same as eRfOff, we should change it to eRfOn after we config RF parameters. */
	/* Added by tynli. 2010.03.30. */
	pwrpriv->rf_pwrstate = rf_on;

	pwrpriv->rfoff_reason |= (pwrpriv->reg_rfoff) ? RF_CHANGE_BY_SW : 0;

	if (pwrpriv->rfoff_reason & RF_CHANGE_BY_HW)
		pwrpriv->b_hw_radio_off = _TRUE;

	if (pwrpriv->rfoff_reason > RF_CHANGE_BY_PS) {
		/* H/W or S/W RF OFF before sleep. */
		RTW_INFO("InitializeAdapter8188EE(): Turn off RF for RfOffReason(%d) ----------\n", pwrpriv->rfoff_reason);
		/* MgntActSet_RF_State(Adapter, rf_off, pwrpriv->rfoff_reason, _TRUE); */
	} else {
		pwrpriv->rf_pwrstate = rf_on;
		pwrpriv->rfoff_reason = 0;

		RTW_INFO("InitializeAdapter8812E(): Turn on  ----------\n");

		/* LED control */
		rtw_led_control(Adapter, LED_CTL_POWER_ON);

	}

	/* Fix the bug that when the system enters S3/S4 then tirgger HW radio off, after system */
	/* wakes up, the scan OID will be set from upper layer, but we still in RF OFF state and scan */
	/* list is empty, such that the system might consider the NIC is in RF off state and will wait */
	/* for several seconds (during this time the scan OID will not be set from upper layer anymore) */
	/* even though we have already HW RF ON, so we tell the upper layer our RF state here. */
	/* Added by tynli. 2010.04.01. */
	/* DrvIFIndicateCurrentPhyStatus(Adapter); */

	if (Adapter->registrypriv.hw_wps_pbc) {
		tmpU1b = rtw_read8(Adapter, GPIO_IO_SEL);
		tmpU1b &= ~(HAL_8192C_HW_GPIO_WPS_BIT);
		rtw_write8(Adapter, GPIO_IO_SEL, tmpU1b);	/* enable GPIO[2] as input mode */
	}

	/*  */
	/* Execute TX power tracking later */
	/*  */

	/* We must set MAC address after firmware download. HW do not support MAC addr */
	/* autoload now. */

	pHalData->pci_backdoor_ctrl = registry_par->pci_aspm_config;

	rtw_pci_aspm_config(Adapter);


#ifdef CONFIG_BT_COEXIST
	/* _InitBTCoexist(Adapter); */
#endif
#ifdef CONFIG_BT_COEXIST
	/* YiWei 20140624 , Fix 8192eu mailbox BT info no response issue
	* reduce I2C clock rate to 156KHz (default 1.25Mhz) */
	if (pHalData->EEPROMBluetoothCoexist == 1)
		rtw_write8(Adapter, rPMAC_TxPacketNum, rtw_read8(Adapter, rPMAC_TxPacketNum) | 0x7);
	/* Init BT hw config. */
	rtw_btcoex_HAL_Initialize(Adapter, _FALSE);
#endif

	rtl8192e_InitHalDm(Adapter);

	Adapter->init_adpt_in_progress = _FALSE;

#ifdef CONFIG_CHECK_AC_LIFETIME
	/* Enable lifetime check for the four ACs */
	rtw_write8(Adapter, REG_LIFETIME_CTRL, rtw_read8(Adapter, REG_LIFETIME_CTRL) | 0x0f);
#endif /* CONFIG_CHECK_AC_LIFETIME */

#if defined(CONFIG_CONCURRENT_MODE) || defined(CONFIG_TX_MCAST2UNI)
#ifdef CONFIG_TX_MCAST2UNI
	/* DHCWIFI-55: 20160426, Janet */
	rtw_write16(Adapter, REG_PKT_VO_VI_LIFE_TIME, 0x3000);	/* unit: 256us. 3s */
	rtw_write16(Adapter, REG_PKT_BE_BK_LIFE_TIME, 0x3000);	/* unit: 256us. 3s */
#else	/* CONFIG_TX_MCAST2UNI */
	rtw_write16(Adapter, REG_PKT_VO_VI_LIFE_TIME, 0x3000);	/* unit: 256us. 3s */
	rtw_write16(Adapter, REG_PKT_BE_BK_LIFE_TIME, 0x3000);	/* unit: 256us. 3s */
#endif /* CONFIG_TX_MCAST2UNI */
#endif /* CONFIG_CONCURRENT_MODE || CONFIG_TX_MCAST2UNI */


	/* enable tx DMA to drop the redundate data of packet */
	rtw_write16(Adapter, REG_TXDMA_OFFSET_CHK, (rtw_read16(Adapter, REG_TXDMA_OFFSET_CHK) | DROP_DATA_EN));

	/* EnableInterrupt8188EE(Adapter); */

#if (MP_DRIVER == 1)
	if (Adapter->registrypriv.mp_mode == 1) {
		Adapter->mppriv.channel = pHalData->current_channel;
		MPT_InitializeAdapter(Adapter, Adapter->mppriv.channel);
	} else
#endif
	{
		if (pwrpriv->rf_pwrstate == rf_on) {
			pHalData->bNeedIQK = _TRUE;
			if (pHalData->bIQKInitialized)
				halrf_iqk_trigger(&pHalData->odmpriv, _TRUE);
				/*phy_iq_calibrate_8192e(padapter, _TRUE);*/
			else {
				/*phy_iq_calibrate_8192e(padapter, _FALSE);*/
				halrf_iqk_trigger(&pHalData->odmpriv, _FALSE);
				pHalData->bIQKInitialized = _TRUE;
			}
			odm_txpowertracking_check(&pHalData->odmpriv);
		}
	}

	efuse_OneByteRead(Adapter, 0x1FA, &tmpU1b ,_FALSE);

	if (!(tmpU1b & BIT0)) {
		phy_set_rf_reg(Adapter, RF_PATH_A, 0x15, 0x0F, 0x05);
		RTW_INFO("PA BIAS path A\n");
	}

	is2T2R = IS_2T2R(pHalData->version_id);
	if (!(tmpU1b & BIT1) && is2T2R) {
		phy_set_rf_reg(Adapter, RF_PATH_B, 0x15, 0x0F, 0x05);
		RTW_INFO("PA BIAS path B\n");
	}

	/* Fixed LDPC rx hang issue. */
	{
		u4Byte	tmp4Byte = rtw_read32(Adapter, REG_SYS_SWR_CTRL1_8192E);
		rtw_write8(Adapter, REG_SYS_SWR_CTRL2_8192E, 0x75);
		tmp4Byte = (tmp4Byte & 0xfff00fff) | (0x7E << 12);
		rtw_write32(Adapter, REG_SYS_SWR_CTRL1_8192E, tmp4Byte);
	}
	PHY_SetRFEReg_8192E(Adapter);

	/* For 11n logo 5.2.3 AP & STA association and Throughput Honoring NAV */
	if (Adapter->registrypriv.wifi_spec == 1)
		NavUpper = 0;

	rtw_hal_set_hwreg(Adapter, HW_VAR_NAV_UPPER, ((u8 *)&NavUpper));

#ifdef CONFIG_XMIT_ACK
	/* ack for xmit mgmt frames. */
	rtw_write32(Adapter, REG_FWHW_TXQ_CTRL, rtw_read32(Adapter, REG_FWHW_TXQ_CTRL) | BIT(12));
#endif /* CONFIG_XMIT_ACK */

#ifdef CONFIG_HIGH_CHAN_SUPER_CALIBRATION
	rtw_hal_set_chnl_bw(Adapter, 13,
		CHANNEL_WIDTH_20, HAL_PRIME_CHNL_OFFSET_DONT_CARE, HAL_PRIME_CHNL_OFFSET_DONT_CARE);

	PHY_SpurCalibration_8192E(Adapter);

	rtw_hal_set_chnl_bw(Adapter, Adapter->registrypriv.channel,
		CHANNEL_WIDTH_20, HAL_PRIME_CHNL_OFFSET_DONT_CARE, HAL_PRIME_CHNL_OFFSET_DONT_CARE);
#endif

	if (Adapter->registrypriv.wifi_spec == 1) {
		/* For 11n logo 5.2.29 Traffic Differentition in Single BSS with WMM STA */
		phy_set_bb_reg(Adapter, rOFDM0_ECCAThreshold, bMaskDWord, 0x00fe0301);
	}

	/*{
		RTW_INFO("===== Start Dump Reg =====");
		for(i = 0 ; i <= 0xeff ; i+=4)
		{
			if(i%16==0)
				RTW_INFO("\n%04x: ",i);
			RTW_INFO("0x%08x ",rtw_read32(Adapter, i));
		}
		RTW_INFO("\n ===== End Dump Reg =====\n");
	}*/


	RTW_INFO("read[REG_MGQ_TXBD_DESA_8192E] = %x\n", rtw_read32(Adapter, REG_MGQ_TXBD_DESA_8192E));

	return rtStatus;
}

/*
 * 2009/10/13 MH Acoording to documetn form Scott/Alfred....
 * This is based on version 8.1.
 *   */




VOID
hal_poweroff_8192ee(
	IN	PADAPTER			Adapter
)
{
	u8 u1bTmp;
	u8 bMacPwrCtrlOn = _FALSE;
	HAL_DATA_TYPE		*pHalData	= GET_HAL_DATA(Adapter);

	rtw_hal_get_hwreg(Adapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if (bMacPwrCtrlOn == _FALSE)
		return ;
	/* pHalData->bMACFuncEnable = _FALSE; */

	/* Run LPS WL RFOFF flow */
	HalPwrSeqCmdParsing(Adapter, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK, Rtl8192E_NIC_LPS_ENTER_FLOW);

	/* 0x1F[7:0] = 0		 */ /* turn off RF */
	/* rtw_write8(Adapter, REG_RF_CTRL_8812, 0x00); */

	/*	==== Reset digital sequence   ======	 */
	if ((rtw_read8(Adapter, REG_MCUFWDL) & BIT7) && pHalData->bFWReady) /* 8051 RAM code */
		_8051Reset8192E(Adapter);

	/* Reset MCU. Suggested by Filen. 2011.01.26. by tynli. */
	u1bTmp = rtw_read8(Adapter, REG_SYS_FUNC_EN + 1);
	rtw_write8(Adapter, REG_SYS_FUNC_EN + 1, (u1bTmp & (~BIT2)));

	/* MCUFWDL 0x80[1:0]=0				 */ /* reset MCU ready status */
	rtw_write8(Adapter, REG_MCUFWDL, 0x00);

	/* HW card disable configuration. */
	HalPwrSeqCmdParsing(Adapter, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK, Rtl8192E_NIC_DISABLE_FLOW);

	/* Reset MCU IO Wrapper */
	u1bTmp = rtw_read8(Adapter, REG_RSV_CTRL + 1);
	rtw_write8(Adapter, REG_RSV_CTRL + 1, (u1bTmp & (~BIT0)));
	u1bTmp = rtw_read8(Adapter, REG_RSV_CTRL + 1);
	rtw_write8(Adapter, REG_RSV_CTRL + 1, u1bTmp | BIT0);

	/* RSV_CTRL 0x1C[7:0] = 0x0E			 */ /* lock ISO/CLK/Power control register */
	rtw_write8(Adapter, REG_RSV_CTRL, 0x0e);

	bMacPwrCtrlOn = _FALSE;
	rtw_hal_set_hwreg(Adapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);

	pHalData->bFWReady = _FALSE;

}

VOID
PowerOffAdapter8192EE(
	IN	PADAPTER			Adapter
)
{
	rtw_hal_power_off(Adapter);
}

static u32 rtl8192ee_hal_deinit(PADAPTER Adapter)
{
	u8	u1bTmp = 0;
	u32	count = 0;
	u8	bSupportRemoteWakeUp = _FALSE;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	struct pwrctrl_priv		*pwrpriv = adapter_to_pwrctl(Adapter);


	RTW_INFO("====> %s()\n", __FUNCTION__);

	if (Adapter->bHaltInProgress == _TRUE) {
		RTW_INFO("====> Abort rtl8812ae_hal_deinit()\n");
		return _FAIL;
	}

	Adapter->bHaltInProgress = _TRUE;

	/*  */
	/* No I/O if device has been surprise removed */
	/*  */
	if (rtw_is_surprise_removed(Adapter)) {
		Adapter->bHaltInProgress = _FALSE;
		return _SUCCESS;
	}

	RT_SET_PS_LEVEL(pwrpriv, RT_RF_OFF_LEVL_HALT_NIC);

	/* Without supporting WoWLAN or the driver is in awake (D0) state, we should	 */
	if (!bSupportRemoteWakeUp) { /* ||!pMgntInfo->bPwrSaveState) */
		/* 2009/10/13 MH For power off test. */
		PowerOffAdapter8192EE(Adapter);
	}
#if 0
	else {
		s32	rtStatus = _SUCCESS;

		/* -------------------------------------------------------- */
		/* 3 <1> Prepare for configuring wowlan related infomations */
		/* -------------------------------------------------------- */

		/* Clear Fw WoWLAN event. */
		rtw_write8(Adapter, REG_MCUTST_WOWLAN, 0x0);

#if (USE_SPECIFIC_FW_TO_SUPPORT_WOWLAN == 1)
		SetFwRelatedForWoWLAN8812A(Adapter, _TRUE);
#endif

		SetWoWLANCAMEntry8812(Adapter);
		PlatformEFIOWrite1Byte(Adapter, REG_WKFMCAM_NUM_8812, pPSC->WoLPatternNum);

		/* Dynamically adjust Tx packet boundary for download reserved page packet. */
		rtStatus = DynamicRQPN8812AE(Adapter, 0xE0, 0x3, 0x80c20d0d); /* reserve 30 pages for rsvd page */
		if (rtStatus == RT_STATUS_SUCCESS)
			pHalData->bReInitLLTTable = _TRUE;

		/* -------------------------------------------------------- */
		/* 3 <2> Set Fw releted H2C cmd. */
		/* -------------------------------------------------------- */

		/* Set WoWLAN related security information. */
		SetFwGlobalInfoCmd_8812(Adapter);

		HalDownloadRSVDPage8812E(Adapter, _TRUE);


		/* Just enable AOAC related functions when we connect to AP. */
		if (pMgntInfo->mAssoc && pMgntInfo->OpMode == RT_OP_MODE_INFRASTRUCTURE) {
			/* Set Join Bss Rpt H2C cmd and download RSVD page. */
			/* Fw will check media status to send null packet and perform WoWLAN LPS. */
			Adapter->hal_func.set_hw_reg_handler(Adapter, HW_VAR_AID, 0);
			MacIdIndicateSpecificMediaStatus(Adapter, MAC_ID_STATIC_FOR_DEFAULT_PORT, RT_MEDIA_CONNECT);

			HalSetFWWoWlanMode8812(Adapter, _TRUE);
			/* Enable Fw Keep alive mechanism. */
			HalSetFwKeepAliveCmd8812(Adapter, _TRUE);

			/* Enable disconnect decision control. */
			SetFwDisconnectDecisionCtrlCmd_8812(Adapter, _TRUE);
		}

		/* -------------------------------------------------------- */
		/* 3 <3> Hw Configutations */
		/* -------------------------------------------------------- */

		/* Wait untill Rx DMA Finished before host sleep. FW Pause Rx DMA may happens when received packet doing dma.  */ /* YJ,add,111101 */
		rtw_write8(Adapter, REG_RXDMA_CONTROL_8812A, BIT2);

		u1bTmp = rtw_read8(Adapter, REG_RXDMA_CONTROL_8812A);
		count = 0;
		while (!(u1bTmp & BIT(1)) && (count++ < 100)) {
			rtw_udelay_os(10); /* 10 us */
			u1bTmp = rtw_read8(Adapter, REG_RXDMA_CONTROL_8812A);
		}
		RTW_INFO("HaltAdapter8812E(): 222 Wait untill Rx DMA Finished before host sleep. count=%d\n", count);

		rtw_write8(Adapter, REG_APS_FSMCO + 1, 0x0);

		PlatformClearPciPMEStatus(Adapter);

		/* tynli_test for normal chip wowlan. 2010.01.26. Suggested by Sd1 Isaac and designer Alfred. */
		rtw_write8(Adapter, REG_SYS_CLKR, (rtw_read8(Adapter, REG_SYS_CLKR) | BIT3));

		/* prevent 8051 to be reset by PERST# */
		rtw_write8(Adapter, REG_RSV_CTRL, 0x20);
		rtw_write8(Adapter, REG_RSV_CTRL, 0x60);
	}

	/* For wowlan+LPS+32k. */
	if (bSupportRemoteWakeUp && Adapter->bEnterPnpSleep) {
		BOOLEAN		bEnterFwLPS = TRUE;
		u1Byte		QueueID;
		PRT_TCB	pTcb;


		/* Set the WoWLAN related function control enable. */
		/* It should be the last H2C cmd in the WoWLAN flow. 2012.02.10. by tynli. */
		SetFwRemoteWakeCtrlCmd_8723B(Adapter, 1);

		/* Stop Pcie Interface Tx DMA. */
		PlatformEFIOWrite1Byte(Adapter, REG_PCIE_CTRL_REG_8723B + 1, 0xff);

		/* Wait for TxDMA idle. */
		count = 0;
		do {
			u1bTmp = PlatformEFIORead1Byte(Adapter, REG_PCIE_CTRL_REG_8723B);
			PlatformSleepUs(10); /* 10 us */
			count++;
		} while ((u1bTmp != 0) && (count < 100));

		/* Set Fw to enter LPS mode.		 */
		if (pMgntInfo->mAssoc &&
		    pMgntInfo->OpMode == RT_OP_MODE_INFRASTRUCTURE &&
		    (pPSC->WoWLANLPSLevel > 0)) {
			Adapter->hal_func.set_hw_reg_handler(Adapter, HW_VAR_FW_LPS_ACTION, (pu1Byte)(&bEnterFwLPS));
			pPSC->bEnterLPSDuringSuspend = TRUE;
		}

		PlatformAcquireSpinLock(Adapter, RT_TX_SPINLOCK);
		/* guangan, 2009/08/28, return TCB in busy queue to idle queue when resume from S3/S4. */
		for (QueueID = 0; QueueID < MAX_TX_QUEUE; QueueID++) {
			/* 2004.08.11, revised by rcnjko. */
			while (!RTIsListEmpty(&Adapter->TcbBusyQueue[QueueID])) {
				pTcb = (PRT_TCB)RTRemoveHeadList(&Adapter->TcbBusyQueue[QueueID]);
				ReturnTCB(Adapter, pTcb, RT_STATUS_SUCCESS);
			}
		}

		PlatformReleaseSpinLock(Adapter, RT_TX_SPINLOCK);

		if (pHalData->HwROFEnable) {
			u1bTmp = PlatformEFIORead1Byte(Adapter, REG_HSISR_8812 + 3);
			PlatformEFIOWrite1Byte(Adapter, REG_HSISR_8812 + 3, u1bTmp | BIT1);
		}
	}
#endif

	RTW_INFO("%s() <====\n", __FUNCTION__);

	Adapter->bHaltInProgress = _FALSE;


	return _SUCCESS;
}


static u16
hal_mdio_read_8192ee(PADAPTER Adapter, u8 Addr)
{
	u2Byte ret = 0;
	u1Byte tmpU1b = 0, count = 0;

	rtw_write8(Adapter, REG_PCIE_MIX_CFG_8192E, Addr | BIT6);
	tmpU1b = rtw_read8(Adapter, REG_PCIE_MIX_CFG_8192E)&BIT6;
	count = 0;
	while (tmpU1b && count < 20) {
		rtw_udelay_os(10);
		tmpU1b = rtw_read8(Adapter, REG_PCIE_MIX_CFG_8192E) & BIT6;
		count++;
	}
	if (tmpU1b == 0)
		ret = rtw_read16(Adapter, REG_MDIO_V1_8192E);

	return ret;
}


static VOID
hal_mdio_write_8192ee(PADAPTER Adapter, u8 Addr, u16 Data)
{
	u1Byte tmpU1b = 0, count = 0;

	rtw_write16(Adapter, REG_MDIO_V1_8192E, Data);
	rtw_write8(Adapter, REG_PCIE_MIX_CFG_8192E, Addr | BIT5);
	tmpU1b = rtw_read8(Adapter, REG_PCIE_MIX_CFG_8192E) & BIT5;
	count = 0;
	while (tmpU1b && count < 20) {
		rtw_udelay_os(10);
		tmpU1b = rtw_read8(Adapter, REG_PCIE_MIX_CFG_8192E) & BIT5;
		count++;
	}
}


static void hal_dbi_write_8192ee(PADAPTER Adapter, u16 Addr, u8 Data)
{
	u1Byte tmpU1b = 0, count = 0;
	u2Byte WriteAddr = 0, Remainder = Addr % 4;

	/* Write DBI 1Byte Data */
	WriteAddr = REG_DBI_WDATA_V1_8192E + Remainder;
	rtw_write8(Adapter, WriteAddr, Data);

	/* Write DBI 2Byte Address & Write Enable */
	WriteAddr = (Addr & 0xfffc) | (BIT0 << (Remainder + 12));
	rtw_write16(Adapter, REG_DBI_FLAG_V1_8192E, WriteAddr);

	/* Write DBI Write Flag */
	rtw_write8(Adapter, REG_DBI_FLAG_V1_8192E + 2, 0x1);

	tmpU1b = rtw_read8(Adapter, REG_DBI_FLAG_V1_8192E + 2);
	count = 0;
	while (tmpU1b && count < 20) {
		rtw_udelay_os(10);
		tmpU1b = rtw_read8(Adapter, REG_DBI_FLAG_V1_8192E + 2);
		count++;
	}
}

static u8 hal_dbi_read_8192ee(PADAPTER Adapter, u16 Addr)
{
	u16 ReadAddr = Addr & 0xfffc;
	u8 ret = 0, tmpU1b = 0, count = 0;

	rtw_write16(Adapter, REG_DBI_FLAG_V1_8192E, ReadAddr);
	rtw_write8(Adapter, REG_DBI_FLAG_V1_8192E + 2, 0x2);
	tmpU1b = rtw_read8(Adapter, REG_DBI_FLAG_V1_8192E + 2);
	count = 0;
	while (tmpU1b && count < 20) {
		rtw_udelay_os(10);
		tmpU1b = rtw_read8(Adapter, REG_DBI_FLAG_V1_8192E + 2);
		count++;
	}
	if (tmpU1b == 0) {
		ReadAddr = REG_DBI_RDATA_V1_8192E + Addr % 4;
		ret = rtw_read8(Adapter, ReadAddr);
	}

	return ret;
}

u8 SetHwReg8192EE(PADAPTER Adapter, u8 variable, u8 *val)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u8 ret = _SUCCESS;

	switch (variable) {
	case HW_VAR_SET_RPWM:
#ifdef CONFIG_LPS_LCLK
		{
			u8	ps_state = *((u8 *)val);
			/* rpwm value only use BIT0(clock bit) ,BIT6(Ack bit), and BIT7(Toggle bit) for 88e. */
			/* BIT0 value - 1: 32k, 0:40MHz. */
			/* BIT6 value - 1: report cpwm value after success set, 0:do not report. */
			/* BIT7 value - Toggle bit change. */
			/* modify by Thomas. 2012/4/2. */
			ps_state = ps_state & 0xC1;
			/* RTW_INFO("##### Change RPWM value to = %x for switch clk #####\n",ps_state); */
			rtw_write8(Adapter, REG_PCIE_HRPWM_8192EE, ps_state);
		}
#endif
		break;
	case HW_VAR_PCIE_STOP_TX_DMA:
		rtw_write16(Adapter, REG_PCIE_CTRL_REG, 0xff00);
		break;

	case HW_VAR_NAV_UPPER: {
		u32 usNavUpper = *((u32 *)val);

		if (usNavUpper > HAL_NAV_UPPER_UNIT_8192E * 0xFF) {
			break;
		}

		/* The value of ((usNavUpper + HAL_NAV_UPPER_UNIT_8192E - 1) / HAL_NAV_UPPER_UNIT_8192E)
		 * is getting the upper integer. */

		usNavUpper = (usNavUpper + HAL_NAV_UPPER_UNIT_8192E - 1) / HAL_NAV_UPPER_UNIT_8192E;

		rtw_write8(Adapter, REG_NAV_UPPER, (u8)usNavUpper);
	}
	break;

		case HW_VAR_DBI:
		{
			u16 *pCmd;

			pCmd = (u16 *)val;
			hal_dbi_write_8192ee(Adapter, pCmd[0], (u8)pCmd[1]);
			break;
		}
		case HW_VAR_MDIO:
		{
			u16 *pCmd;

			pCmd = (u16 *)val;
			hal_mdio_write_8192ee(Adapter, (u8)pCmd[0], pCmd[1]);
			break;
		}
	default:
		ret = SetHwReg8192E(Adapter, variable, val);
		break;
	}

	return ret;
}

void GetHwReg8192EE(PADAPTER Adapter, u8 variable, u8 *val)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	/* DM_ODM_T 		*podmpriv = &pHalData->odmpriv; */

	switch (variable) {
	case HW_VAR_DBI:
		*val = hal_dbi_read_8192ee(Adapter, *((u16 *)(val)));
		break;
	case HW_VAR_MDIO:
		*((u16 *)(val)) = hal_mdio_read_8192ee(Adapter, *val);
		break;

	case HW_VAR_L1OFF_NIC_SUPPORT:
		{
		u8 l1off;

		l1off = hal_dbi_read_8192ee(Adapter, 0x160);
		if (l1off & (BIT2|BIT3))
			*val = _TRUE;
		else
			*val = _FALSE;
		}
		break;
	case HW_VAR_L1OFF_CAPABILITY:
		{
		u8 l1off;

		l1off = hal_dbi_read_8192ee(Adapter, 0x15c);
		if (l1off & (BIT2|BIT3))
			*val = _TRUE;
		else
			*val = _FALSE;
		}
		break;
	default:
		GetHwReg8192E(Adapter, variable, val);
		break;
	}

}

/*
 *	Description:
 *		Change default setting of specified variable.
 *   */
u8
SetHalDefVar8192EE(
	IN	PADAPTER				Adapter,
	IN	HAL_DEF_VARIABLE		eVariable,
	IN	PVOID					pValue
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u8			bResult = _SUCCESS;

	switch (eVariable) {
	case HAL_DEF_PCI_SUUPORT_L1_BACKDOOR:
		pHalData->bSupportBackDoor = *((PBOOLEAN)pValue);
		break;
	default:
		SetHalDefVar8192E(Adapter, eVariable, pValue);
		break;
	}

	return bResult;
}

/*
 *	Description:
 *		Query setting of specified variable.
 *   */
u8
GetHalDefVar8192EE(
	IN	PADAPTER				Adapter,
	IN	HAL_DEF_VARIABLE		eVariable,
	IN	PVOID					pValue
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u8			bResult = _SUCCESS;

	switch (eVariable) {
	case HW_VAR_MAX_RX_AMPDU_FACTOR:
		*((u32 *)pValue) = MAX_AMPDU_FACTOR_64K; /* MAX_AMPDU_FACTOR_64K; */
		break;

	case HAL_DEF_PCI_SUUPORT_L1_BACKDOOR:
		*((PBOOLEAN)pValue) = pHalData->bSupportBackDoor;
		break;

	case HAL_DEF_PCI_AMD_L1_SUPPORT:
		*((PBOOLEAN)pValue) = _TRUE;/* Support L1 patch on AMD platform in default, added by Roger, 2012.04.30. */
		break;

	default:
		GetHalDefVar8192E(Adapter, eVariable, pValue);
		break;
	}

	return bResult;
}

static void rtl8192ee_init_default_value(_adapter *padapter)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);


	rtl8192e_init_default_value(padapter);

	pHalData->CurrentWirelessMode = WIRELESS_MODE_AUTO;
	pHalData->bDefaultAntenna = 1;

	/*  */
	/* Set TCR-Transmit Control Register. The value is set in InitializeAdapter8190Pci() */
	/*
	* TODO_8192ee */
#if 0/* original */
	pHalData->TransmitConfig = CFENDFORM | BIT12 | BIT13;
#else
	pHalData->TransmitConfig = CFENDFORM | BIT15;
#endif
	/* TODO_8192ee	 */

	/*  */
	/* Set RCR-Receive Control Register . The value is set in InitializeAdapter8190Pci(). */
	/*
	* TODO_8192ee */
#if 0
	/* pHalData->ReceiveConfig = (\ */
	/* RCR_APPFCS			| */
	/* RCR_APP_MIC			| */
	/* RCR_APP_ICV			| */
	/* RCR_APP_PHYST_RXFF	|   */
	/* RCR_VHT_DACK		| */
	/* RCR_HTC_LOC_CTRL	| */
	/* RCR_AMF				| */
	/* RCR_ACF				|	 */
	/* RCR_ADF				|	  Note: This bit controls the PS-Poll packet filter. */
	/* RCR_AICV			| */
	/* RCR_ACRC32			| */
	/* RCR_AB				| */
	/* RCR_AM				| */
	/* RCR_APM				| */
	/* 0); */
#endif

	/* TODO_8192ee. original */

	pHalData->ReceiveConfig = (\
				   /* RCR_APPFCS			| */
				   RCR_APP_MIC			|
				   RCR_APP_ICV			|
				   RCR_APP_PHYST_RXFF	|
				   RCR_VHT_DACK	|
				   RCR_HTC_LOC_CTRL	|
				   RCR_AMF				|
				   RCR_CBSSID_DATA			|
				   RCR_CBSSID_BCN			|
				   /* RCR_ACF				| */
		/* RCR_ADF				|	// Note: This bit controls the PS-Poll packet filter. */
				   RCR_AB				|
				   RCR_AM				|
				   RCR_APM				|
				   0);

#if (1 == RTL8192E_RX_PACKET_INCLUDE_CRC)
	pHalData->ReceiveConfig |= ACRC32;
#endif

	/* TODO_8192ee */

	/*  */
	/* Set Interrupt Mask Register */
	/*  */
	pHalData->IntrMaskDefault[0]	= (u32)(\
						IMR_PSTIMEOUT_8192E |
						/* IMR_GTINT4_8812			| */
						IMR_GTINT3_8192E			|
						IMR_TXBCN0ERR_8192E		|
						IMR_TXBCN0OK_8192E		|
						IMR_BCNDMAINT0_8192E	|
						IMR_HSISR_IND_ON_INT_8192E	|
						IMR_C2HCMD_8192E		|
						/* IMR_CPWM_8192E			| */
						IMR_HIGHDOK_8192E		|
						IMR_MGNTDOK_8192E		|
						IMR_BKDOK_8192E			|
						IMR_BEDOK_8192E			|
						IMR_VIDOK_8192E			|
						IMR_VODOK_8192E			|
						IMR_RDU_8192E			|
						IMR_ROK_8192E			|
						0);
	pHalData->IntrMaskDefault[1]	= (u32)(\
						IMR_RXFOVW_8192E		|
						IMR_TXFOVW_8192E		|
						0);

	/* 2012/03/27 hpfan Add for win8 DTM DPC ISR test */
	pHalData->IntrMaskReg[0]	=	(u32)(\
						IMR_RDU_8192E		|
						IMR_PSTIMEOUT_8192E	|
						0);
	pHalData->IntrMaskReg[1]	=	(u32)(\
						IMR_C2HCMD_8192E		|
						0);

	/* if (padapter->bUseThreadHandleInterrupt) */
	/* { */
	/*	pHalData->IntrMask[0] =pHalData->IntrMaskReg[0]; */
	/*	pHalData->IntrMask[1] =pHalData->IntrMaskReg[1]; */
	/* } */
	/* else */
	{
		pHalData->IntrMask[0] = pHalData->IntrMaskDefault[0];
		pHalData->IntrMask[1] = pHalData->IntrMaskDefault[1];
	}

	if (IS_HARDWARE_TYPE_8812E(padapter)) {
		pHalData->SysIntrMask[0] = (u32)(\
						 HSIMR_PDN_INT_EN		|
						 HSIMR_RON_INT_EN		|
						 0);
	} else {
		pHalData->SysIntrMask[0] = (u32)(\
						 HSIMR_GPIO12_0_INT_EN			|
						 HSIMR_GPIO9_INT_EN				|
						 0);
	}
}

void rtl8192ee_set_hal_ops(_adapter *padapter)
{
	struct hal_ops	*pHalFunc = &padapter->hal_func;


	pHalFunc->hal_power_on = _InitPowerOn_8192EE;
	pHalFunc->hal_power_off = hal_poweroff_8192ee;
	pHalFunc->hal_init = &rtl8192ee_hal_init;
	pHalFunc->hal_deinit = &rtl8192ee_hal_deinit;

	pHalFunc->inirp_init = &rtl8192ee_init_desc_ring;
	pHalFunc->inirp_deinit = &rtl8192ee_free_desc_ring;
	pHalFunc->irp_reset = &rtl8192ee_reset_desc_ring;

	pHalFunc->init_xmit_priv = &rtl8192ee_init_xmit_priv;
	pHalFunc->free_xmit_priv = &rtl8192ee_free_xmit_priv;

	pHalFunc->init_recv_priv = &rtl8192ee_init_recv_priv;
	pHalFunc->free_recv_priv = &rtl8192ee_free_recv_priv;
#ifdef CONFIG_RTW_SW_LED
	pHalFunc->InitSwLeds = &rtl8192ee_InitSwLeds;
	pHalFunc->DeInitSwLeds = &rtl8192ee_DeInitSwLeds;
#endif/* CONFIG_RTW_SW_LED */

	pHalFunc->init_default_value = &rtl8192ee_init_default_value;
	pHalFunc->intf_chip_configure = &rtl8192ee_interface_configure;
	pHalFunc->read_adapter_info = &ReadAdapterInfo8192EE;

	pHalFunc->enable_interrupt = &EnableInterrupt8192EE;
	pHalFunc->disable_interrupt = &DisableInterrupt8192EE;
	pHalFunc->interrupt_handler = &rtl8192ee_interrupt;

	pHalFunc->set_hw_reg_handler = &SetHwReg8192EE;
	pHalFunc->GetHwRegHandler = &GetHwReg8192EE;
	pHalFunc->get_hal_def_var_handler = &GetHalDefVar8192EE;
	pHalFunc->SetHalDefVarHandler = &SetHalDefVar8192EE;

	pHalFunc->hal_xmit = &rtl8192ee_hal_xmit;
	pHalFunc->mgnt_xmit = &rtl8192ee_mgnt_xmit;
	pHalFunc->hal_xmitframe_enqueue = &rtl8192ee_hal_xmitframe_enqueue;

#ifdef CONFIG_HOSTAPD_MLME
	pHalFunc->hostap_mgnt_xmit_entry = &rtl8192ee_hostap_mgnt_xmit_entry;
#endif

#ifdef CONFIG_XMIT_THREAD_MODE
	pHalFunc->xmit_thread_handler = &rtl8192ee_xmit_buf_handler;
#endif
	pHalFunc->hal_set_l1ssbackdoor_handler = rtw_pci_aspm_config_l1off_general;

	rtl8192e_set_hal_ops(pHalFunc);

}
