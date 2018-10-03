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
#define _HCI_HAL_INIT_C_

/* #include <drv_types.h> */
#include <rtl8814a_hal.h>


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


/* -------------------------------------------------------------------
 *
 *	EEPROM Content Parsing
 *
 * ------------------------------------------------------------------- */

static VOID
hal_ReadIDs_8814AE(
	IN	PADAPTER	Adapter,
	IN	u8			*PROMContent,
	IN	BOOLEAN		AutoloadFail
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

	if (!AutoloadFail) {
		/* VID, DID */
		pHalData->EEPROMVID = ReadLE2Byte(&PROMContent[EEPROM_VID_8814AE]);
		pHalData->EEPROMDID = ReadLE2Byte(&PROMContent[EEPROM_DID_8814AE]);
		pHalData->EEPROMSVID = ReadLE2Byte(&PROMContent[EEPROM_SVID_8814AE]);
		pHalData->EEPROMSMID = ReadLE2Byte(&PROMContent[EEPROM_SMID_8814AE]);

		/* Customer ID, 0x00 and 0xff are reserved for Realtek.		 */
		pHalData->EEPROMCustomerID = *(u8 *)&PROMContent[EEPROM_CustomID_8814];
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
hal_ReadEfusePCIeCap8814AE(
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

		RTW_INFO("Hal_ReadEfusePCIeCap8814AE(): PCIeCap = %#x\n", PCIeCap);

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

	rtw_hal_set_def_var(Adapter, HAL_DEF_PCI_ASPM_OSC, (u8 *)&AspmOscSupport);
	RTW_INFO("Hal_ReadEfusePCIeCap8814AE(): AspmOscSupport = %d\n", AspmOscSupport);
}

static VOID
hal_CustomizedBehavior_8814AE(
	PADAPTER			Adapter
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct led_priv	*pledpriv = &(Adapter->ledpriv);

	pledpriv->LedStrategy = SW_LED_MODE7; /* Default LED strategy. */
	pHalData->bLedOpenDrain = _TRUE;/* Support Open-drain arrangement for controlling the LED. Added by Roger, 2009.10.16. */

	switch (pHalData->CustomerID) {
	case RT_CID_DEFAULT:
		break;

	case RT_CID_CCX:
		/* pMgntInfo->IndicateByDeauth = _TRUE; */
		break;

	case RT_CID_WHQL:
		/* Adapter->bInHctTest = _TRUE; */
		break;

	default:
		/* RTW_INFO("Unkown hardware Type\n"); */
		break;
	}
	RTW_INFO("hal_CustomizedBehavior_8814AE(): RT Customized ID: 0x%02X\n", pHalData->CustomerID);

#if 0
	if (Adapter->bInHctTest) {
		pMgntInfo->PowerSaveControl.bInactivePs = FALSE;
		pMgntInfo->PowerSaveControl.bIPSModeBackup = FALSE;
		pMgntInfo->PowerSaveControl.bLeisurePs = FALSE;
		pMgntInfo->PowerSaveControl.bLeisurePsModeBackup = FALSE;
		pMgntInfo->keepAliveLevel = 0;
	}
#endif
}


static VOID
hal_CustomizeByCustomerID_8814AE(
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

	hal_CustomizedBehavior_8814AE(pAdapter);
}

/* [copy] from 8814AU */
static VOID
ReadLEDSetting_8814AE(
	IN	PADAPTER	Adapter,
	IN	u8		*PROMContent,
	IN	BOOLEAN		AutoloadFail
)
{
	struct led_priv *pledpriv = &(Adapter->ledpriv);

#ifdef CONFIG_SW_LED
	pledpriv->bRegUseLed = _TRUE;
#else /* HW LED */
	pledpriv->LedStrategy = HW_LED;
#endif /* CONFIG_SW_LED */
}

VOID
InitAdapterVariablesByPROM_8814AE(
	IN	PADAPTER	Adapter
)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Adapter);

	RTW_INFO("====> %s\n", __func__);

	hal_InitPGData_8814A(Adapter, pHalData->efuse_eeprom_data);

	/* Hal_EfuseParseIDCode8812A(Adapter, pHalData->efuse_eeprom_data); */
	hal_ReadPROMVersion8814A(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	hal_ReadIDs_8814AE(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	hal_config_macaddr(Adapter, pHalData->bautoload_fail_flag);
	hal_ReadTxPowerInfo8814A(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	hal_ReadBoardType8814A(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	hal_ReadEfusePCIeCap8814AE(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	hal_Read_TRX_antenna_8814A(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);

	/*  */
	/* Read Bluetooth co-exist and initialize */
	/*  */
	hal_EfuseParseBTCoexistInfo8814A(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);

	hal_ReadChannelPlan8814A(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	hal_EfuseParseXtal_8814A(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	hal_ReadThermalMeter_8814A(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	hal_ReadRemoteWakeup_8814A(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	hal_ReadAntennaDiversity8814A(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	hal_ReadRFEType_8814A(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	hal_ReadPowerTrackingType_8814A(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	ReadLEDSetting_8814AE(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);

	/* hal_ReadUsbModeSwitch_8814AU(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag); */
	hal_CustomizeByCustomerID_8814AE(Adapter);

	Hal_EfuseParseKFreeData_8814A(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);

	hal_GetRxGainOffset_8814A(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	RTW_INFO("%s <====\n", __func__);

}

static void hal_ReadPROMContent_8814A(
	IN PADAPTER		Adapter
)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Adapter);
	u8			eeValue;

	RTW_INFO("====> %s\n", __func__);

	/* check system boot selection */
	eeValue = rtw_read8(Adapter, REG_9346CR);
	pHalData->EepromOrEfuse		= (eeValue & BOOT_FROM_EEPROM) ? _TRUE : _FALSE;
	pHalData->bautoload_fail_flag	= (eeValue & EEPROM_EN) ? _FALSE : _TRUE;

	RTW_INFO("Boot from %s, Autoload %s !\n", (pHalData->EepromOrEfuse ? "EEPROM" : "EFUSE"),
		 (pHalData->bautoload_fail_flag ? "Fail" : "OK"));

	/* pHalData->EEType = IS_BOOT_FROM_EEPROM(Adapter) ? EEPROM_93C46 : EEPROM_BOOT_EFUSE; */

	InitAdapterVariablesByPROM_8814AE(Adapter);

	RTW_INFO("%s <====\n", __func__);
}

/* [copy] from 8814AU */
void ReadAdapterInfo8814AE(PADAPTER Adapter)
{
	/* For debug test now!!!!! */
	PHY_RFShadowRefresh(Adapter);

	Hal_InitEfuseVars_8814A(Adapter);

	/* Read all content in Efuse/EEPROM. */
	hal_ReadPROMContent_8814A(Adapter);

	/* We need to define the RF type after all PROM value is recognized. */
	ReadRFType8814A(Adapter);
}

void rtl8814ae_interface_configure(PADAPTER Adapter)
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

/* [refer] to DisableInterrupt8814AE() in win driver hal8814AEIsr.c */
VOID
DisableInterrupt8814AE(
	IN PADAPTER			Adapter
)
{
	struct dvobj_priv	*pdvobjpriv = adapter_to_dvobj(Adapter);

	rtw_write32(Adapter, REG_HIMR0_8814A, IMR_DISABLED_8814A);

	rtw_write32(Adapter, REG_HIMR1_8814A, IMR_DISABLED_8814A);	/* by tynli */

	pdvobjpriv->irq_enabled = 0;
}

/* [refer] to ClearInterrupt8814AE() in win driver hal8814AEIsr.c */
VOID
ClearInterrupt8814AE(
	IN PADAPTER			Adapter
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u32	tmp = 0;

	tmp = rtw_read32(Adapter, REG_HISR0_8814A);
	rtw_write32(Adapter, REG_HISR0_8814A, tmp);
	pHalData->IntArray[0] = 0;

	tmp = rtw_read32(Adapter, REG_HISR1_8814A);
	rtw_write32(Adapter, REG_HISR1_8814A, tmp);
	pHalData->IntArray[1] = 0;

	/* [check]: win driver there are no codes below: */
	tmp = rtw_read32(Adapter, REG_HSISR_8814A);
	rtw_write32(Adapter, REG_HSISR_8814A, tmp);
	pHalData->SysIntArray[0] = 0;
}


/* [refer] to EnableInterrupt8814AE() in win driver hal8814AEIsr.c */
VOID
EnableInterrupt8814AE(
	IN PADAPTER			Adapter
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct dvobj_priv	*pdvobjpriv = adapter_to_dvobj(Adapter);

	pdvobjpriv->irq_enabled = 1;

	rtw_write32(Adapter, REG_HIMR0_8814A, pHalData->IntrMask[0] & 0xFFFFFFFF);

	rtw_write32(Adapter, REG_HIMR1_8814A, pHalData->IntrMask[1] & 0xFFFFFFFF);

	rtw_write32(Adapter, REG_HSISR_8814A, pHalData->SysIntrMask[0] & 0xFFFFFFFF);
}

/* [copy] from win driver  in hal8814AEIsr.c */
VOID
LogInterruptHistory8814AE(
	IN	PADAPTER			Adapter
)
{
#if 0 /* there is no "InterruptLog" field.  */
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	if (pHalData->IntArray[0] & IMR_VODOK_8814A)
		pHalData->InterruptLog.nIMR_VODOK++;
	if (pHalData->IntArray[0] & IMR_VIDOK_8814A)
		pHalData->InterruptLog.nIMR_VIDOK++;
	if (pHalData->IntArray[0] & IMR_MGNTDOK_8814A)
		pHalData->InterruptLog.nIMR_MGNTDOK++;
	if (pHalData->IntArray[0] & IMR_BEDOK_8814A)
		pHalData->InterruptLog.nIMR_BEDOK++;
	if (pHalData->IntArray[0] & IMR_BKDOK_8814A)
		pHalData->InterruptLog.nIMR_BKDOK++;
	if (pHalData->IntArray[0] & IMR_ROK_8814A)
		pHalData->InterruptLog.nIMR_ROK++;

	if (pHalData->IntArray[0] & IMR_TXBCN0OK_8814A)
		pHalData->InterruptLog.nIMR_TBDOK++;
	if (pHalData->IntArray[0] & IMR_TXBCN0ERR_8814A)
		pHalData->InterruptLog.nIMR_TBDER++;
	if (pHalData->IntArray[0] & IMR_BCNDERR0_8814A)
		pHalData->InterruptLog.nIMR_BDOK++;
	if (pHalData->IntArray[1] & IMR_TXFOVW_8814A)
		pHalData->InterruptLog.nIMR_TXFOVW++;
	if (pHalData->IntArray[1] & IMR_RXFOVW_8814A)
		pHalData->InterruptLog.nIMR_RXFOVW++;
	if (pHalData->IntArray[0] & IMR_RDU_8814A)
		pHalData->InterruptLog.nIMR_RDU++;
#endif
}

/* [refer] to InterruptRecognized8814AE() in win driver hal8814AEIsr.c */
BOOLEAN
InterruptRecognized8814AE(
	IN	PADAPTER			Adapter
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	BOOLEAN			bRecognized = _FALSE;

	/* 2013.11.18 Glayrainx suggests that turn off IMR and */
	/* restore after cleaning ISR. */
	rtw_write32(Adapter, REG_HIMR0_8814A, 0);
	rtw_write32(Adapter, REG_HIMR1_8814A, 0);


	pHalData->IntArray[0] = rtw_read32(Adapter, REG_HISR0_8814A);
	pHalData->IntArray[0] &= pHalData->IntrMask[0];
	rtw_write32(Adapter, REG_HISR0_8814A, pHalData->IntArray[0]);

	/*if (pHalData->IntArray[0] & BIT(11)) {*/
		/* For HISR extension. Added by tynli. 2009.10.07. */
		pHalData->IntArray[1] = rtw_read32(Adapter, REG_HISR1_8814A);
		pHalData->IntArray[1] &= pHalData->IntrMask[1];
		rtw_write32(Adapter, REG_HISR1_8814A, pHalData->IntArray[1]);
	/*}*/

#if 0
	if (pHalData->IntArray[0] != 0/*   || pHalData->->IntArray[1] != 0*/)
		LogInterruptHistory8814AE(Adapter);
#endif

	if (((pHalData->IntArray[0]) & pHalData->IntrMask[0]) != 0 ||
	    ((pHalData->IntArray[1]) & pHalData->IntrMask[1]) != 0)
		bRecognized = _TRUE;

	/* restore IMR */
	rtw_write32(Adapter, REG_HIMR0_8814A, pHalData->IntrMask[0] & 0xFFFFFFFF);
	rtw_write32(Adapter, REG_HIMR1_8814A, pHalData->IntrMask[1] & 0xFFFFFFFF);

	return bRecognized;
}

VOID
UpdateInterruptMask8814AE(
	IN	PADAPTER		Adapter,
	IN	u32		AddMSR,	u32		AddMSR1,
	IN	u32		RemoveMSR, u32		RemoveMSR1
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

	DisableInterrupt8814AE(Adapter);

	if (AddMSR)
		pHalData->IntrMask[0] |= AddMSR;
	if (AddMSR1)
		pHalData->IntrMask[1] |= AddMSR1;

	if (RemoveMSR)
		pHalData->IntrMask[0] &= (~RemoveMSR);

	if (RemoveMSR1)
		pHalData->IntrMask[1] &= (~RemoveMSR1);

	EnableInterrupt8814AE(Adapter);
}

/* [copy] from win driver */
static VOID
HwConfigureRTL8814A(
	IN	PADAPTER			Adapter
)
{
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(Adapter);
	u32 regRRSR = 0;
	u16	tmpU2b = 0;


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

	case WIRELESS_MODE_AUTO:
	case WIRELESS_MODE_N_24G:
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
	rtw_write32(Adapter, REG_RRSR_8814A, regRRSR);

#if 0
	/*delete by ylb 20150423, to Fix Tx Low using FW V16
	Now we using FW RA,ARFB table is not used */

	/* ARFB table 9 for 11ac 5G 2SS */
	rtw_write32(Adapter, REG_ARFR0_8814A, 0x00000010);
	rtw_write32(Adapter, REG_ARFR0_8814A + 4, 0x3e0ff000);

	/* ARFB table 10 for 11ac 5G 1SS */
	rtw_write32(Adapter, REG_ARFR1_8814A, 0x00000010);
	rtw_write32(Adapter, REG_ARFR1_8814A + 4, 0x000ff000);
#endif

	/* Set SLOT time */
	rtw_write8(Adapter, REG_SLOT_8814A, 0x09);

	/* CF-End setting. */
	/* if(Adapter->MgntInfo.Reg8814AFPGAVerification) */ /* add by ylb 20130726 */
	rtw_write16(Adapter, REG_FWHW_TXQ_CTRL_8814A, 0x1F00); /* clear 0x420 Bit7(BIT_EN_RTY_BK) , add by ylb 20130726 to put retry Packet in next AMPDU, UDP TP will + 60Mbps */
	/* else */
	/* PlatformEFIOWrite2Byte(Adapter,REG_FWHW_TXQ_CTRL_8814A, 0x1F80); */


	/* Set retry limit */
	/* 3vivi, 20100928, especially for DTM, performance_ext, To avoid asoc too long to another AP more than 4.1 seconds. */
	/* 3we find retry 7 times maybe not enough, so we retry more than 7 times to pass DTM. */
#if 0
	if (pMgntInfo->bPSPXlinkMode) {
		pHalData->ShortRetryLimit = 3;
		pHalData->LongRetryLimit = 3;
		/* Set retry limit */
		rtw_write16(Adapter, REG_RETRY_LIMIT_8814A, 0x0303);
	} else
#endif
		rtw_write16(Adapter, REG_RETRY_LIMIT_8814A, 0x0707);

	/* BAR settings */
	rtw_write32(Adapter, REG_BAR_MODE_CTRL_8814A, 0x0201ffff);

	/* Set Data / Response auto rate fallack retry count */
	rtw_write32(Adapter, REG_DARFRC_8814A, 0x01000000);
	rtw_write32(Adapter, REG_DARFRC_8814A + 4, 0x07060504);
	rtw_write32(Adapter, REG_RARFRC_8814A, 0x01000000);
	rtw_write32(Adapter, REG_RARFRC_8814A + 4, 0x07060504);


	/* Beacon related, for rate adaptive */
	rtw_write8(Adapter, REG_ATIMWND_8814A, 0x2);

	rtw_write8(Adapter, REG_BCN_MAX_ERR_8814A, 0xff);


	/* 20100211 Joseph: Change original setting of BCN_CTRL(0x550) from */
	/* 0x1e(0x2c for test chip) ro 0x1f(0x2d for test chip). Set BIT0 of this register disable ATIM */
	/* function. Since we do not use HIGH_QUEUE anymore, ATIM function is no longer used. */
	/* Also, enable ATIM function may invoke HW Tx stop operation. This may cause ping failed */
	/* sometimes in long run test. So just disable it now. */
	/* PlatformAtomicExchange((pu4Byte)(&pHalData->RegBcnCtrlVal), 0x1d); */
	pHalData->RegBcnCtrlVal = 0x1d;
#ifdef CONFIG_CONCURRENT_MODE
	rtw_write16(Adapter, REG_BCN_CTRL_8814A, 0x1010);	/* For 2 PORT TSF SYNC */
#else
	rtw_write8(Adapter, REG_BCN_CTRL_8814A, (u8)(pHalData->RegBcnCtrlVal));
#endif

	/* Marked out by Bruce, 2010-09-09. */
	/* This register is configured for the 2nd Beacon (multiple BSSID). */
	/* We shall disable this register if we only support 1 BSSID. */
	/* vivi guess 92d also need this, also 92d now doesnot set this reg */
	rtw_write8(Adapter, REG_BCN_CTRL_1_8814A, 0);

	/* TBTT prohibit hold time. Suggested by designer TimChen. */
	rtw_write8(Adapter, REG_TBTT_PROHIBIT_8814A + 1, 0xff); /* 8 ms */

	/* 3 Note */
	rtw_write8(Adapter, REG_PIFS_8814A, 0);
	rtw_write8(Adapter, REG_AGGR_BREAK_TIME_8814A, 0x16);

	rtw_write16(Adapter, REG_NAV_PROT_LEN_8814A, 0x0040);
	/* Protection Ctrl */
	rtw_write16(Adapter, REG_PROT_MODE_CTRL_8814A, 0x08ff);

	if (!Adapter->registrypriv.wifi_spec) {
		/* For Rx TP. Suggested by SD1 Richard. Added by tynli. 2010.04.12. */
		rtw_write32(Adapter, REG_FAST_EDCA_CTRL_8814A, 0x03086666);
	} else {
		/* For WiFi WMM. suggested by timchen. Added by tynli.	 */
		rtw_write16(Adapter, REG_FAST_EDCA_CTRL_8814A, 0x0);
	}

#if (BOARD_TYPE == FPGA_2MAC)

	/* Set Spec SIFS (used in NAV) */
	rtw_write16(Adapter, REG_SPEC_SIFS_8814A, 0x1010);
	rtw_write16(Adapter, REG_MAC_SPEC_SIFS_8814A, 0x1010);

	/* Set SIFS for CCK */
	rtw_write16(Adapter, REG_SIFS_CTX_8814A, 0x1010);

	/* Set SIFS for OFDM */
	rtw_write16(Adapter, REG_SIFS_TRX_8814A, 0x1010);

#else

	/* Set Spec SIFS (used in NAV) */
	/* 92d all set these register 0x1010, check it later */
	rtw_write16(Adapter, REG_SPEC_SIFS_8814A, 0x100a);
	rtw_write16(Adapter, REG_MAC_SPEC_SIFS_8814A, 0x100a);

	/* Set SIFS for CCK */
	rtw_write16(Adapter, REG_SIFS_CTX_8814A, 0x100a);

	/* Set SIFS for OFDM */
	rtw_write16(Adapter, REG_SIFS_TRX_8814A, 0x100a);

#endif

	/* 3 Note Data sheet don't define */
	rtw_write8(Adapter, REG_SINGLE_AMPDU_CTRL_8814A, 0x80);

	/* Set MAC AGGR NUM.*/
	/* Suggest by Wilson: spec define AMPDU max. is 64. Should not set to 0x3F(un-limit).
	   If AMPUD pre-tx is disble. It is safe to set the 0x3E(62). */
#ifdef RTL8814AE_AMPDU_PRE_TX_OFF
	rtw_write8(Adapter, REG_MAX_AGGR_NUM_8814A, 0x3E);
	rtw_write8(Adapter, REG_RTS_MAX_AGGR_NUM_8814A, 0x3E);
#else
	rtw_write8(Adapter, REG_MAX_AGGR_NUM_8814A, 0x20);
	rtw_write8(Adapter, REG_RTS_MAX_AGGR_NUM_8814A, 0x20);
#endif

	/* #if (HAL_MAC_ENABLE == 0) */
	/* Set Multicast Address. 2009.01.07. by tynli. */
	rtw_write32(Adapter, REG_MAR_8814A, 0xffffffff);
	rtw_write32(Adapter, REG_MAR_8814A + 4, 0xffffffff);
	/* #endif */

	/* Reject all control frame - default value is 0 */
	rtw_write16(Adapter, REG_RXFLTMAP1_8814A, 0x0);

	/* 2010.09.08 hpfan */
	/* Since ADF is removed from RCR, ps-poll will not be indicate to driver, */
	/* RxFilterMap should mask ps-poll to gurantee AP mode can rx ps-poll. */
	tmpU2b = rtw_read16(Adapter, REG_RXFLTMAP1_8814A);
	tmpU2b  |= BIT10;
#ifdef CONFIG_BEAMFORMING
	/* NDPA packet subtype is 0x0101 */
	tmpU2b |= BIT5;
#endif /* CONFIG_BEAMFORMING */
	rtw_write16(Adapter, REG_RXFLTMAP1_8814A, tmpU2b);
	/* RTW_INFO("REG_RXFLTMAP1_8814A=0x%X\n", (u16)rtw_read16(Adapter,REG_RXFLTMAP1_8814A)); */

	pHalData->RegTxPause = rtw_read8(Adapter, REG_TXPAUSE);
	pHalData->RegFwHwTxQCtrl = rtw_read8(Adapter, REG_FWHW_TXQ_CTRL + 2);
	pHalData->RegReg542 = rtw_read8(Adapter, REG_TBTT_PROHIBIT + 2);
	pHalData->RegCR_1 = rtw_read8(Adapter, REG_CR_8814A + 1);
	RTW_INFO("%s: pHalData->RegCR_1 = 0x%2X\n", __func__, pHalData->RegCR_1);
}

static u32 _InitPowerOn_8814AE(PADAPTER Adapter)
{
	u32	status = _SUCCESS;
	u8	tmpU1b;
	u8 bMacPwrCtrlOn = _FALSE;

	rtw_hal_get_hwreg(Adapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if (bMacPwrCtrlOn == _TRUE) {
		RTW_INFO("%s, MacPwrCtrlOn have already = 1\n", __FUNCTION__);
		return _SUCCESS;
	}

	rtw_write8(Adapter, REG_RSV_CTRL, 0x0);

	/* Auto Power Down to CHIP-off State */
	tmpU1b = (rtw_read8(Adapter, REG_APS_FSMCO + 1) & (~BIT7));
	rtw_write8(Adapter, REG_APS_FSMCO + 1, tmpU1b);

	/* HW Power on sequence */
	if (IS_VENDOR_8814A_MP_CHIP(Adapter)) {
		if (!HalPwrSeqCmdParsing(Adapter, ~PWR_CUT_TESTCHIP_MSK, PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK, Rtl8814A_NIC_ENABLE_FLOW))
			return _FAIL;
	} else {
		if (!HalPwrSeqCmdParsing(Adapter, PWR_CUT_TESTCHIP_MSK, PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK, Rtl8814A_NIC_ENABLE_FLOW))
			return _FAIL;
	}

	/* Enable Power Down Interrupt */
	tmpU1b = (rtw_read8(Adapter, REG_APS_FSMCO) | BIT4);
	rtw_write8(Adapter, REG_APS_FSMCO, tmpU1b);

	/* Release MAC IO register reset */
	tmpU1b = rtw_read8(Adapter, REG_CR_8814A);
	tmpU1b = 0xff;
	rtw_write8(Adapter, REG_CR_8814A, tmpU1b);
	rtw_mdelay_os(2);

	tmpU1b = 0x7f;
	rtw_write8(Adapter, REG_HWSEQ_CTRL, tmpU1b);
	rtw_mdelay_os(2);

#if 0
	/* Need remove below furture, suggest by Jackie. */
	/* if 0xF0[24] =1 (LDO), need to set the 0x7C[6] to 1. */
	if (IS_HARDWARE_TYPE_8821E(Adapter)) {
		tmpU1b = rtw_read8(Adapter, REG_SYS_CFG + 3);
		if (tmpU1b & BIT0) { /* LDO mode. */
			tmpU1b = rtw_read8(Adapter, 0x7c);
			rtw_write8(Adapter, 0x7c, tmpU1b | BIT6);
		}
	}
#endif

	/* Add for wakeup online */
	tmpU1b = rtw_read8(Adapter, REG_SYS_CLKR);
	rtw_write8(Adapter, REG_SYS_CLKR, (tmpU1b | BIT3));
	tmpU1b = rtw_read8(Adapter, REG_GPIO_MUXCFG + 1);
	rtw_write8(Adapter, REG_GPIO_MUXCFG + 1, (tmpU1b & ~BIT4));


	/* Release MAC IO register reset */
	/* 9.	CR 0x100[7:0]	= 0xFF; */
	/* 10.	CR 0x101[1]	= 0x01; */ /* Enable SEC block */
	rtw_write16(Adapter, REG_CR_8814A, 0x2ff);

	bMacPwrCtrlOn = _TRUE;
	rtw_hal_set_hwreg(Adapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);

	return status;
}

/*
 *	Description:
 *		PCI configuration space write operation on RTL8814AE
 *
 *	Created by GW, 2013.12.25.
 *   */
static VOID
Hal_DBIWrite1Byte_8814AE(
	IN	PADAPTER	Adapter,
	IN	u2Byte		Addr,
	IN	u1Byte		Data
)
{
	u1Byte tmpU1b = 0, count = 0;
	u2Byte WriteAddr = 0, Remainder = Addr % 4;


	/* Write DBI 1Byte Data */
	WriteAddr = REG_DBI_WDATA_V1_8814A + Remainder;
	rtw_write8(Adapter, WriteAddr, Data);

	/* Write DBI 2Byte Address & Write Enable */
	WriteAddr = (Addr & 0xfffc) | (BIT0 << (Remainder + 12));
	rtw_write16(Adapter, REG_DBI_FLAG_V1_8814A, WriteAddr);

	/* Write DBI Write Flag */
	rtw_write8(Adapter, REG_DBI_FLAG_V1_8814A + 2, 0x1);

	tmpU1b = rtw_read8(Adapter, REG_DBI_FLAG_V1_8814A + 2);
	count = 0;
	while (tmpU1b && count < 20) {
		rtw_udelay_os(10);
		tmpU1b = rtw_read8(Adapter, REG_DBI_FLAG_V1_8814A + 2);
		count++;
	}
}

/*	Description:
 *		PCI configuration space read operation on RTL814AE
 *
 *	modify by gw from 8192EE
 *
 * [copy] from win driver */
static u1Byte
Hal_DBIRead1Byte_8814AE(
	IN	PADAPTER	Adapter,
	IN	u2Byte		Addr
)
{
	u2Byte ReadAddr = Addr & 0xfffc;
	u1Byte ret = 0, tmpU1b = 0, count = 0;

	rtw_write16(Adapter, REG_DBI_FLAG_V1_8814A, ReadAddr);
	rtw_write8(Adapter, REG_DBI_FLAG_V1_8814A + 2, 0x2);
	tmpU1b = rtw_read8(Adapter, REG_DBI_FLAG_V1_8814A + 2);
	count = 0;
	while (tmpU1b && count < 20) {
		rtw_udelay_os(10);
		tmpU1b = rtw_read8(Adapter, REG_DBI_FLAG_V1_8814A + 2);
		count++;
	}
	if (0 == tmpU1b) {
		ReadAddr = REG_DBI_RDATA_V1_8814A + Addr % 4;
		ret = rtw_read8(Adapter, ReadAddr);
	}

	return ret;
}

/* [copy] from win driver */
static VOID
EnableAspmBeforeBackDoor_8814AE(IN	PADAPTER Adapter)
{
	u4Byte tmp4Byte = 0, count = 0;
	u1Byte tmp1byte = 0;
	/* set 0x0 to 0x0663; */
	rtw_write16(Adapter, REG_MDIO_V1_8814A, 0x0663);
	tmp1byte = rtw_read8(Adapter, REG_PCIE_MIX_CFG_8814A);
	rtw_write8(Adapter, REG_PCIE_MIX_CFG_8814A, (tmp1byte | BIT5 | 0x0));

	tmp1byte = rtw_read8(Adapter, REG_PCIE_MIX_CFG_8814A);
	while ((tmp1byte & 0x60) && count < 20) {
		rtw_mdelay_os(20);
		count++;
	}

	/* set 0x04 to 0x7544; */
	count = 0;
	rtw_write16(Adapter, REG_MDIO_V1_8814A, 0x7544);
	tmp1byte = rtw_read8(Adapter, REG_PCIE_MIX_CFG_8814A);
	rtw_write8(Adapter, REG_PCIE_MIX_CFG_8814A, (tmp1byte | BIT5 | 0x04));

	tmp1byte = rtw_read8(Adapter, REG_PCIE_MIX_CFG_8814A);
	while ((tmp1byte & 0x60) && count < 20) {
		rtw_mdelay_os(20);
		count++;
	}

	/* set 0x06 to 0xb080; */
	count = 0;
	rtw_write16(Adapter, REG_MDIO_V1_8814A, 0xb080);
	tmp1byte = rtw_read8(Adapter, REG_PCIE_MIX_CFG_8814A);
	rtw_write8(Adapter, REG_PCIE_MIX_CFG_8814A, (tmp1byte | BIT5 | 0x06));

	tmp1byte = rtw_read8(Adapter, REG_PCIE_MIX_CFG_8814A);
	while ((tmp1byte & 0x60) && count < 20) {
		rtw_mdelay_os(20);
		count++;
	}

	/* set 0x07 to 0x4000; */
	count = 0;
	rtw_write16(Adapter, REG_MDIO_V1_8814A, 0x4000);
	tmp1byte = rtw_read8(Adapter, REG_PCIE_MIX_CFG_8814A);
	rtw_write8(Adapter, REG_PCIE_MIX_CFG_8814A, (tmp1byte | BIT5 | 0x07));

	tmp1byte = rtw_read8(Adapter, REG_PCIE_MIX_CFG_8814A);
	while ((tmp1byte & 0x60) && count < 20) {
		rtw_mdelay_os(20);
		count++;
	}

	/* set 0x08 to 0x9003; */
	count = 0;
	rtw_write16(Adapter, REG_MDIO_V1_8814A, 0x9003);
	tmp1byte = rtw_read8(Adapter, REG_PCIE_MIX_CFG_8814A);
	rtw_write8(Adapter, REG_PCIE_MIX_CFG_8814A, (tmp1byte | BIT5 | 0x08));

	tmp1byte = rtw_read8(Adapter, REG_PCIE_MIX_CFG_8814A);
	while ((tmp1byte & 0x60) && count < 20) {
		rtw_mdelay_os(20);
		count++;
	}

	/* set 0x09 to 0x0d03; */
	count = 0;
	rtw_write16(Adapter, REG_MDIO_V1_8814A, 0x0d03);
	tmp1byte = rtw_read8(Adapter, REG_PCIE_MIX_CFG_8814A);
	rtw_write8(Adapter, REG_PCIE_MIX_CFG_8814A, (tmp1byte | BIT5 | 0x09));

	tmp1byte = rtw_read8(Adapter, REG_PCIE_MIX_CFG_8814A);
	while ((tmp1byte & 0x60) && count < 20) {
		rtw_mdelay_os(20);
		count++;
	}

	/* set 0x0a to 0x4037; */
	count = 0;
	rtw_write16(Adapter, REG_MDIO_V1_8814A, 0x4037);
	tmp1byte = rtw_read8(Adapter, REG_PCIE_MIX_CFG_8814A);
	rtw_write8(Adapter, REG_PCIE_MIX_CFG_8814A, (tmp1byte | BIT5 | 0x0a));

	tmp1byte = rtw_read8(Adapter, REG_PCIE_MIX_CFG_8814A);
	while ((tmp1byte & 0x60) && count < 20) {
		rtw_mdelay_os(20);
		count++;
	}


}

/* [copy] from win driver */
static VOID
EnableAspmBackDoor_8814AE(IN PADAPTER Adapter)
{
	u1Byte tmp1byte = 0;

	/* 0x70f BIT7 is used to control L0S */
	/* 20100212 Tynli: Set register offset 0x70f in PCI configuration space to the value 0x23 */
	/* for all bridge suggested by SD1. Origianally this is only for INTEL. */
	/* 20100422 Joseph: Set PCI configuration space offset 0x70F to 0x93 to Enable L0s for all platform. */
	/* This is suggested by SD1 Glayrainx and for Lenovo's request. */
	/* 20120316 YJ: Use BIT31|value(read from 0x70C) intead of 0x93. */
	/* ====== Add for ASMP test adviced by Pisa============= */
	/* modifie by jackie liu, 20140704 */
	{
		tmp1byte = Hal_DBIRead1Byte_8814AE(Adapter, 0x70f);
		Hal_DBIWrite1Byte_8814AE(Adapter, 0x70f, (tmp1byte | BIT7 | BIT5 | BIT6));
	}


	/* 0x719 Bit3 is for L1,0x719 BIT4 is for clock request */
	/* 20100427 Joseph: Disable L1 for Toshiba AMD platform. If AMD platform do not contain */
	/* L1 patch, driver shall disable L1 backdoor. */
	/* 20120316 YJ: Use BIT11|BIT12|value(read from 0x718) intead of 0x1b. */

	/* modified by jackie liu, 20140704 */
	if (GET_HAL_DATA(Adapter)->bSupportBackDoor) {
		tmp1byte = Hal_DBIRead1Byte_8814AE(Adapter, 0x719);
		Hal_DBIWrite1Byte_8814AE(Adapter, 0x719, (tmp1byte | BIT3 | BIT4));
	}
}

/* [copy] from win driver */
static s32
LLT_table_init_8814AE(
	IN PADAPTER	Adapter
)
{
	struct registry_priv	*pregistrypriv = &Adapter->registrypriv;

	/* -------RTL8814A Total txbuf size = 192KB; Page size = 128bytes; Total Pages = 1536; */
	u2Byte		TotalPageNum = TOTAL_PGNUM_8814A;
	u2Byte		txpktPageNum = TXPKT_PGNUM_8814A;
	u2Byte		txpktbuf_bndy = TXPKT_PGNUM_8814A;

	/* Set reserved page number for each queue */
	u2Byte		hpq_pgnum = HPQ_PGNUM_8814A;
	u2Byte		npq_pgnum = NPQ_PGNUM_8814A;
	u2Byte		lpq_pgnum = LPQ_PGNUM_8814A;
	u2Byte		epq_pgnum = EPQ_PGNUM_8814A;
	u2Byte		pub_pgnum = PUB_PGNUM_8814A;
	u2Byte		bcn_pgnum = BCQ_PGNUM_8814A;

	u1Byte		tmp1byte = 0;
	s32			status = _SUCCESS;

	RTW_INFO("=====>LLT_table_init_8814AE\n");

	/* ------------Tx FIFO page number setting------------------ */
	/* Queue page number setting */ /* add by ylb 20120124 */
	rtw_write16(Adapter, REG_FIFOPAGE_INFO_1_8814A, hpq_pgnum);	/* High Queue */
	rtw_write16(Adapter, REG_FIFOPAGE_INFO_2_8814A, lpq_pgnum);		/* Low Queue */
	rtw_write16(Adapter, REG_FIFOPAGE_INFO_3_8814A, npq_pgnum);	/* Normal Queue */
	rtw_write16(Adapter, REG_FIFOPAGE_INFO_4_8814A, epq_pgnum);	/* Extra Queue */
	rtw_write16(Adapter, REG_FIFOPAGE_INFO_5_8814A, pub_pgnum);	/* Public Queue */

	if (IS_VENDOR_8814A_MP_CHIP(Adapter)) {
		rtw_write16(Adapter, REG_RXFF_PTR_8814A, RX_DMA_BOUNDARY_8814A);/*Rx Buffer Size 24K*/
	} else {
		rtw_write16(Adapter, REG_TRXFF_BNDY_8814A, RX_DMA_BOUNDARY_8814A);/*Rx Buffer Size 24K*/
	}

	/* Page number enable */
	/* Write 1 to set RQPN bit[79:0] value to page numbers for initialization */
	rtw_write8(Adapter, REG_RQPN_CTRL_2_8814A + 3, 0x80);

	if (rtw_read16(Adapter, REG_FIFOPAGE_INFO_5_8814A + 2) == 0x0000) { /* add by ylb 20130609 */
		RTW_INFO("=====>Load page Num fail in 0x22C bit31\n");
		rtw_write8(Adapter, REG_RQPN_CTRL_2_8814A + 3, 0x80);
	}

	if (!pregistrypriv->wifi_spec)
		txpktbuf_bndy = TX_PAGE_BOUNDARY_8814A;
	else		/* for WMM */
		txpktbuf_bndy = WMM_NORMAL_TX_PAGE_BOUNDARY_8814A;

	/* Set page boundary and header */
	rtw_write16(Adapter, REG_TXPKTBUF_BCNQ_BDNY_8814A, txpktbuf_bndy);
	rtw_write16(Adapter, REG_TXPKTBUF_BCNQ1_BDNY_8814A, txpktbuf_bndy);
	rtw_write16(Adapter, REG_MGQ_PGBNDY_8814A, txpktbuf_bndy);

	/* Set The head page of packet of Bcnq */
	rtw_write16(Adapter, REG_FIFOPAGE_CTRL_2_8814A, txpktbuf_bndy);
	/* The head page of packet of Bcnq1 */
	rtw_write16(Adapter, REG_FIFOPAGE_CTRL_2_8814A + 2, txpktbuf_bndy);

	rtw_write16(Adapter, REG_WMAC_LBK_BUF_HD_8814A, txpktbuf_bndy);

	/* Set Tx/Rx page size (Tx must be 128 Bytes, Rx can be 64,128,256,512,1024 bytes) */
	rtw_write8(Adapter, REG_PBP_8814A, 0x10);

	/* DRV_INFO_SZ */
#if 0 /* (OMNIPEEK_SNIFFER_ENABLED == 1) */
	rtw_write8(Adapter, REG_RX_DRVINFO_SZ_8814A, 0x5);
	tmp1byte = rtw_read8(Adapter, 0x7d5);
	rtw_write8(Adapter, 0x7d5, tmp1byte | BIT1);
#else
	rtw_write8(Adapter, REG_RX_DRVINFO_SZ_8814A, DRVINFO_SZ);
#endif

	status = InitLLTTable8814A(Adapter);

	return status;
}

static  u32
InitMAC_8814AE(
	IN	PADAPTER	Adapter
)
{
	u8	tmpU1b;
	u16	tmpU2b;
	struct recv_priv	*precvpriv = &Adapter->recvpriv;
	struct xmit_priv	*pxmitpriv = &Adapter->xmitpriv;
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(Adapter);

	RTW_INFO("=======>InitMAC_8814AE()\n");

	rtw_hal_power_on(Adapter);

	/* if(!pHalData->bMACFuncEnable)	 */
	{
		/* System init */
		/* 18.	LLT_table_init(Adapter); */
		if (LLT_table_init_8814AE(Adapter) == _FAIL)
			/* if(LLT_table_init_8814AE_v2(Adapter) == _FAIL) */
			/* if(InitLLT_Table88XX_V1(Adapter) == _FAIL) */
			return _FAIL;
	}

	/* Enable Host Interrupt */
	rtw_write32(Adapter, REG_HISR0_8814A, 0xffffffff);
	rtw_write32(Adapter, REG_HISR1_8814A, 0xffffffff);
	/* Enable FW Beamformer Interrupt. */
	rtw_write8(Adapter, REG_FWIMR_8814A + 3, (rtw_read8(Adapter, REG_FWIMR_8814A + 3) | BIT6));

	tmpU2b = rtw_read16(Adapter, REG_TRXDMA_CTRL_8814A);
	tmpU2b &= 0xf;
	if (Adapter->registrypriv.wifi_spec)
		tmpU2b |= 0xF9B1;
	else
		tmpU2b |= 0xF5B1;
	rtw_write16(Adapter, REG_TRXDMA_CTRL_8814A, tmpU2b);


	/* Reported Tx status from HW for rate adaptive. */
	/* 2009/12/03 MH This should be realtive to power on step 14. But in document V11  */
	/* still not contain the description.!!! */
	rtw_write8(Adapter, REG_FWHW_TXQ_CTRL_8814A + 1, 0x1F);

	/* Set RCR register */
	rtw_write32(Adapter, REG_RCR_8814A, pHalData->ReceiveConfig);
	rtw_write16(Adapter, REG_RXFLTMAP0_8814A, 0xFFFF); /*Receive all mgnt frame*/
	rtw_write16(Adapter, REG_RXFLTMAP2_8814A, 0xFFFF);

	/* Set TCR register */
	rtw_write32(Adapter, REG_TCR_8814A, pHalData->TransmitConfig);

	/*  */
	/* Set TX/RX descriptor physical address(from OS API). */
	/*  */
	rtw_write32(Adapter, REG_BCNQ_TXBD_DESA_8814A, (u64)pxmitpriv->tx_ring[BCN_QUEUE_INX].dma & DMA_BIT_MASK(32));
	rtw_write32(Adapter, REG_MGQ_TXBD_DESA_8814A, (u64)pxmitpriv->tx_ring[MGT_QUEUE_INX].dma & DMA_BIT_MASK(32));
	rtw_write32(Adapter, REG_VOQ_TXBD_DESA_8814A, (u64)pxmitpriv->tx_ring[VO_QUEUE_INX].dma & DMA_BIT_MASK(32));
	rtw_write32(Adapter, REG_VIQ_TXBD_DESA_8814A, (u64)pxmitpriv->tx_ring[VI_QUEUE_INX].dma & DMA_BIT_MASK(32));
	rtw_write32(Adapter, REG_BEQ_TXBD_DESA_8814A, (u64)pxmitpriv->tx_ring[BE_QUEUE_INX].dma & DMA_BIT_MASK(32));
	rtw_write32(Adapter, REG_BKQ_TXBD_DESA_8814A, (u64)pxmitpriv->tx_ring[BK_QUEUE_INX].dma & DMA_BIT_MASK(32));
	rtw_write32(Adapter, REG_HI0Q_TXBD_DESA_8814A, (u64)pxmitpriv->tx_ring[HIGH_QUEUE_INX].dma & DMA_BIT_MASK(32));
	rtw_write32(Adapter, REG_RXQ_RXBD_DESA_8814A, (u64)precvpriv->rx_ring[RX_MPDU_QUEUE].dma & DMA_BIT_MASK(32));

#ifdef CONFIG_64BIT_DMA
	/* 2009/10/28 MH For DMA 64 bits. We need to assign the high 32 bit address */
	/* for NIC HW to transmit data to correct path. */
	rtw_write32(Adapter, REG_BCNQ_TXBD_DESA_8814A + 4,
		    ((u64)pxmitpriv->tx_ring[BCN_QUEUE_INX].dma) >> 32);
	rtw_write32(Adapter, REG_MGQ_TXBD_DESA_8814A + 4,
		    ((u64)pxmitpriv->tx_ring[MGT_QUEUE_INX].dma) >> 32);
	rtw_write32(Adapter, REG_VOQ_TXBD_DESA_8814A + 4,
		    ((u64)pxmitpriv->tx_ring[VO_QUEUE_INX].dma) >> 32);
	rtw_write32(Adapter, REG_VIQ_TXBD_DESA_8814A + 4,
		    ((u64)pxmitpriv->tx_ring[VI_QUEUE_INX].dma) >> 32);
	rtw_write32(Adapter, REG_BEQ_TXBD_DESA_8814A + 4,
		    ((u64)pxmitpriv->tx_ring[BE_QUEUE_INX].dma) >> 32);
	rtw_write32(Adapter, REG_BKQ_TXBD_DESA_8814A + 4,
		    ((u64)pxmitpriv->tx_ring[BK_QUEUE_INX].dma) >> 32);
	rtw_write32(Adapter, REG_HI0Q_TXBD_DESA_8814A + 4,
		    ((u64)pxmitpriv->tx_ring[HIGH_QUEUE_INX].dma) >> 32);
	rtw_write32(Adapter, REG_RXQ_RXBD_DESA_8814A + 4,
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

	/* Reset the Read/Write point to 0 */
	rtw_write32(Adapter, REG_TSFTIMER_HCI_8814A, 0x3fffffff);

	/* Set Tx/Rx DMA Burst Size: Tx: 2048bytes(max.), Rx: 128 bytes(max.) */
	tmpU1b = rtw_read8(Adapter, REG_PCIE_CTRL_REG_8814A + 3);
	rtw_write8(Adapter, REG_PCIE_CTRL_REG_8814A + 3, tmpU1b | 0xF7);
	/* rtw_write8(Adapter,REG_PCIE_CTRL_REG_8814A+3, 0x77); */

	/* 20100318 Joseph: Reset interrupt migration setting when initialization. Suggested by SD1. */
	rtw_write32(Adapter, REG_INT_MIG_8814A, 0);
	pHalData->bInterruptMigration = _FALSE;

	/* 2009.10.19. Reset H2C protection register. by tynli. */
	rtw_write32(Adapter, REG_MCUTST_1_8814A, 0x0);

#if MP_DRIVER == 1
	if (Adapter->registrypriv.mp_mode == 1) {
		rtw_write32(Adapter, REG_MACID, 0x87654321);
		rtw_write32(Adapter, 0x0700, 0x87654321);
	}
#endif

	/* pic buffer descriptor mode: */
	/* ---- tx */
	rtw_write16(Adapter, REG_MGQ_TXBD_NUM_8814A, TX_DESC_NUM_8814A | ((RTL8814AE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_VOQ_TXBD_NUM_8814A, TX_DESC_NUM_8814A | ((RTL8814AE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_VIQ_TXBD_NUM_8814A, TX_DESC_NUM_8814A | ((RTL8814AE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_BEQ_TXBD_NUM_8814A, BE_QUEUE_TX_DESC_NUM_8814A | ((RTL8814AE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_VOQ_TXBD_NUM_8814A, TX_DESC_NUM_8814A | ((RTL8814AE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_BKQ_TXBD_NUM_8814A, TX_DESC_NUM_8814A | ((RTL8814AE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_HI0Q_TXBD_NUM_8814A, TX_DESC_NUM_8814A | ((RTL8814AE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_HI1Q_TXBD_NUM_8814A, TX_DESC_NUM_8814A | ((RTL8814AE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_HI2Q_TXBD_NUM_8814A, TX_DESC_NUM_8814A | ((RTL8814AE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_HI3Q_TXBD_NUM_8814A, TX_DESC_NUM_8814A | ((RTL8814AE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_HI4Q_TXBD_NUM_8814A, TX_DESC_NUM_8814A | ((RTL8814AE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_HI5Q_TXBD_NUM_8814A, TX_DESC_NUM_8814A | ((RTL8814AE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_HI6Q_TXBD_NUM_8814A, TX_DESC_NUM_8814A | ((RTL8814AE_SEG_NUM << 12) & 0x3000));
	rtw_write16(Adapter, REG_HI7Q_TXBD_NUM_8814A, TX_DESC_NUM_8814A | ((RTL8814AE_SEG_NUM << 12) & 0x3000));

	/* ---- rx. support 32 bits in linux */
	/* rtw_write16(Adapter, REG_RX_RXBD_NUM_8814A, RX_DESC_NUM_8814A |((RTL8814AE_SEG_NUM<<13 ) & 0x6000) |0x8000);  */ /* using 64bit */
	rtw_write16(Adapter, REG_RX_RXBD_NUM_8814A, RX_DESC_NUM_8814A | ((RTL8814AE_SEG_NUM << 13) & 0x6000)); /* using 32bit */

	/* ---- reset read/write point */
	rtw_write32(Adapter, REG_TSFTIMER_HCI_8814A, 0XFFFFFFFF); /* Reset read/write point */


#if (DISABLE_BB_RF == 1)
	rtw_write16(Adapter, REG_PCIE_MULTIFET_CTRL_8812, 0xF450);
#error "fix me!"
#endif

	RTW_INFO("InitMAC_8814AE() <====\n");

	return _SUCCESS;
}

/* [copy] from win driver */
static  VOID
EnableRF12vSwitchPowerSupply_8814AE(IN	PADAPTER Adapter)
{
	/* This is for fullsize card, because GPIO7 there is floating. */
	/* We should pull GPIO7 high to enable RF 1.2V Switch Power Supply */
	/* Sugguested by Jiaqi_liu, 20140708 */

	u1Byte u1tmp;

	/* set GPIO7 output high£º */
	/* 1. set 0x40[1:0] to 0, BIT_GPIOSEL=0, select pin as GPIO; */
	u1tmp = rtw_read8(Adapter, 0x40);
	rtw_write8(Adapter, 0x40, u1tmp & 0xFC);
	/* 2. set 0x44[31] to 0 */
	/* mode=0:data port; */
	/* mode=1 and BIT_GPIO_IO_SEL=0:interrupt mode; */
	u1tmp = rtw_read8(Adapter, 0x47);
	rtw_write8(Adapter, 0x47, u1tmp & 0x7F);
	/* 3. data mode */
	/* 3.1 set 0x44[23] to 1 */
	/* sel=0:input; */
	/* sel=1:output; */
	u1tmp = rtw_read8(Adapter, 0x46);
	rtw_write8(Adapter, 0x46, u1tmp | 0x80);
	/* 3.2 0x44[15] to 1 */
	/* output high value; */
	u1tmp = rtw_read8(Adapter, 0x45);
	rtw_write8(Adapter, 0x45, u1tmp | 0x80);
}

VOID
hal_carddisable_8814AE(
	IN	PADAPTER			Adapter
)
{
	u8	u1bTmp;
	u8 bMacPwrCtrlOn = _FALSE;
	u16	utemp, ori_fsmc0;

	rtw_hal_get_hwreg(Adapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if (bMacPwrCtrlOn == _FALSE)
		return  ;

#if 0
	ori_fsmc0 = utemp = rtw_read16(Adapter, REG_APS_FSMCO);
	rtw_write16(Adapter, REG_APS_FSMCO, utemp & ~0x8000);
#endif
	RTW_INFO("=====>%s\n", __FUNCTION__);

	/* pHalData->bMACFuncEnable = _FALSE; */

	/* stop rx : [copy] from 8814AU */
	rtw_write8(Adapter, REG_CR_8814A, 0x0);

	/* Run LPS WL RFOFF flow */
	HalPwrSeqCmdParsing(Adapter, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK, Rtl8814A_NIC_LPS_ENTER_FLOW);

	/* 0x1F[7:0] = 0		 */ /* turn off RF */
	/* rtw_write8(Adapter, REG_RF_CTRL_8812, 0x00); */

#if 0
	/*	==== Reset digital sequence   ======	 */
	if ((rtw_read8(Adapter, REG_MCUFWDL) & BIT7) && Adapter->bFWReady) /* 8051 RAM code */
		_8051Reset8812(Adapter);

	/* Reset MCU. Suggested by Filen. 2011.01.26. by tynli. */
	u1bTmp = rtw_read8(Adapter, REG_SYS_FUNC_EN + 1);
	rtw_write8(Adapter, REG_SYS_FUNC_EN + 1, (u1bTmp & (~BIT2)));

	/* MCUFWDL 0x80[1:0]=0				 */ /* reset MCU ready status */
	rtw_write8(Adapter, REG_MCUFWDL, 0x00);
#endif

	/* HW card disable configuration. */
	if (IS_VENDOR_8814A_MP_CHIP(Adapter))
		HalPwrSeqCmdParsing(Adapter, ~PWR_CUT_TESTCHIP_MSK, PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK, Rtl8814A_NIC_DISABLE_FLOW);
	else
		HalPwrSeqCmdParsing(Adapter, PWR_CUT_TESTCHIP_MSK, PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK, Rtl8814A_NIC_DISABLE_FLOW);

#if 0
	/* Reset MCU IO Wrapper */
	u1bTmp = rtw_read8(Adapter, REG_RSV_CTRL + 1);
	rtw_write8(Adapter, REG_RSV_CTRL + 1, (u1bTmp & (~BIT0)));
	u1bTmp = rtw_read8(Adapter, REG_RSV_CTRL + 1);
	rtw_write8(Adapter, REG_RSV_CTRL + 1, u1bTmp | BIT0);

	/* RSV_CTRL 0x1C[7:0] = 0x0E			 */ /* lock ISO/CLK/Power control register */
	rtw_write8(Adapter, REG_RSV_CTRL, 0x0e);
#endif

	Adapter->bFWReady = _FALSE;
	bMacPwrCtrlOn = _FALSE;
	rtw_hal_set_hwreg(Adapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);

#if 0
	if (ori_fsmc0 & 0x8000) {
		utemp = rtw_read16(Adapter, REG_APS_FSMCO);
		rtw_write16(Adapter, REG_APS_FSMCO, utemp | 0x8000);
	}
#endif
}

VOID
PowerOffAdapter8814AE(
	IN	PADAPTER			Adapter
)
{
	rtw_hal_power_off(Adapter);
}

/* [refer] to InitializeAdapter8814AE() in Hal8814AEGen.c */
static u32 rtl8814ae_hal_init(PADAPTER Adapter)
{
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	struct pwrctrl_priv		*pwrpriv = adapter_to_pwrctl(Adapter);
	u32	rtStatus = _SUCCESS;
	u8	tmpU1b, u1bRegCR;
	u32	i;
	BOOLEAN bSupportRemoteWakeUp;
	BOOLEAN bMACFuncEnable;
	u32	NavUpper = WiFiNavUpperUs;


	bMACFuncEnable = 0;

	/*  */
	/* No I/O if device has been surprise removed */
	/*  */
	if (rtw_is_surprise_removed(Adapter)) {
		RTW_INFO("rtl8814ae_hal_init(): bSurpriseRemoved!\n");
		return _SUCCESS;
	}

	Adapter->init_adpt_in_progress = _TRUE;
	RTW_INFO("=======>rtl8814ae_hal_init()\n");

	/* Check if MAC has already power on. by tynli. 2011.05.27. */
	tmpU1b = rtw_read8(Adapter, REG_SYS_CLKR + 1);	/* 0x0009 BIT3 is flag MAC Clock Enable */
	u1bRegCR = rtw_read8(Adapter, REG_CR_8814A);
	RTW_INFO(" power-on :REG_SYS_CLKR 0x09=0x%02x. REG_CR 0x100=0x%02x.\n", tmpU1b, u1bRegCR);
	if ((tmpU1b & BIT3) && (u1bRegCR != 0 && u1bRegCR != 0xEA)) {
		/* pHalData->bMACFuncEnable = TRUE; */
		bMACFuncEnable = TRUE;
		RTW_INFO(" MAC has already power on.\n");
	} else {
		bMACFuncEnable = FALSE;
		/* pHalData->bMACFuncEnable = FALSE;		 */
		/* Set FwPSState to ALL_ON mode to prevent from the I/O be return because of 32k */
		/* state which is set before sleep under wowlan mode. 2012.01.04. by tynli. */
		/* pHalData->FwPSState = FW_PS_STATE_ALL_ON_88E; */
		RTW_INFO(" MAC has not been powered on yet.\n");
	}

#if 1 /* [copy] from win driver */
#if 0
	if (bSupportRemoteWakeUp && Adapter->bWakeFromPnpSleep && pHalData->bMACFuncEnable) {
		if (WoWLANInitializeAdapter_8814AE(Adapter)) { /* change by ylb 20141230 for Review Init flow as 8812AE and 8192EE */
			pMgntInfo->init_adpt_in_progress = FALSE;
			return RT_STATUS_SUCCESS;
		}
	}
#endif

#if 0 /* [check] if this is MUST code. */
	/* add by ylb 20141230 for Review Init flow as 8812AE and 8192EE */
	if (CheckPcieDMAHang8814AE(Adapter)) {
		ResetPcieInterfaceDMA8814AE(Adapter, pHalData->bMACFuncEnable, FALSE);
		/* Release pHalData->bMACFuncEnable here because it will reset MAC in ResetPcieInterfaceDMA */
		/* function such that we need to allow LLT to be initialized in later flow. */
		/* pHalData->bMACFuncEnable = FALSE; */
		bMACFuncEnable = FALSE;
	}
#endif

#if 1
	/* Reset MAC/BB/RF status if it is not powered off before calling initialize Hw flow to prevent */
	/* from interface and MAC status mismatch. 2013.06.21, by tynli. Suggested by SD1 JackieLau. */
	/* if(pHalData->bMACFuncEnable) */
	if (bMACFuncEnable) {
		PowerOffAdapter8814AE(Adapter);
		/* pHalData->bMACFuncEnable = FALSE; */
		bMACFuncEnable = FALSE;
	}
#endif
#endif

	/*  */
	/* 1. MAC Initialize */
	/*  */
	rtStatus = InitMAC_8814AE(Adapter);

	if (rtStatus != _SUCCESS) {
		RTW_INFO("Init MAC failed\n");
		return rtStatus;
	}

#if 1 /* [copy] from win driver */
	/* Pull GPIO7 high to enable RF 1.2V Switch Power Supply for fullsize card, YP 20140708 */
	EnableRF12vSwitchPowerSupply_8814AE(Adapter);


	/* Forced the antenna b to wifi. */
	/* if(pHalData->EEPROMBluetoothCoexist ==FALSE) */
	{
		u1Byte tmp1Byte = 0;

		tmp1Byte = rtw_read8(Adapter, 0x64);
		rtw_write8(Adapter, 0x64, (tmp1Byte & 0xFB) | BIT2);

		tmp1Byte = rtw_read8(Adapter, 0x65);
		rtw_write8(Adapter, 0x65, (tmp1Byte & 0xFE) | BIT0);

		tmp1Byte = rtw_read8(Adapter, 0x4F);
		rtw_write8(Adapter, 0x4F, (tmp1Byte & 0xF6) | BIT3 | BIT0);
	}

#endif

#if 1 /* copy from win driver */
	pHalData->RxTag = 0;
	/* Release Pcie Interface Tx DMA. */
#ifdef USING_RX_TAG
	{
#if 0 /* 8814AE HAL driver has no this code. */
		/* Reset HCI to TRX DMA interface if PRESET or Retraining link rst signal is occurred. */
		u1Byte u1Tmp;
		u1Tmp = rtw_read8(Adapter, REG_PCIE_CTRL_REG_8814A + 2);
		rtw_write8(Adapter, REG_PCIE_CTRL_REG_8814A + 2, (u1Tmp | BIT4));
#endif
		/* Rxtag mode */
		rtw_write16(Adapter, REG_PCIE_CTRL_REG_8814A, 0x8000);
	}
	RTW_INFO("TXBD display mode is RxTag.\n");
#else
	rtw_write16(Adapter, REG_PCIE_CTRL_REG_8814A, 0x0000);
	RTW_INFO("TXBD display mode is TotalLen.\n");
#endif
#endif

#if HAL_FW_ENABLE
	if (Adapter->registrypriv.mp_mode == 0) {
		tmpU1b = rtw_read8(Adapter, REG_SYS_CFG);
		rtw_write8(Adapter, REG_SYS_CFG, tmpU1b & 0x7F);

		rtStatus = FirmwareDownload8814A(Adapter, _FALSE);

		if (rtStatus != _SUCCESS) {
			RTW_INFO("FwLoad failed\n");
			rtStatus = _SUCCESS;
			Adapter->bFWReady = _FALSE;
			pHalData->fw_ractrl = _FALSE;
		} else {
			RTW_INFO("FwLoad SUCCESSFULLY!!!\n");
			Adapter->bFWReady = _TRUE;
			pHalData->fw_ractrl = _TRUE;
		}
	}

#if 0 /* [check][copy] from win driver */
#if 1/* (ROM_DOWNLOAD_FW_ENABLE == 1)	 */
	if (!pHalData->bMACFuncEnable) {
		u2Byte			txpktbuf_bndy = TX_PAGE_BOUNDARY_8814A;
		PlatformEFIOWrite2Byte(Adapter, REG_TXPKTBUF_BCNQ_BDNY_8814A, txpktbuf_bndy);
		PlatformEFIOWrite2Byte(Adapter, REG_TXPKTBUF_BCNQ1_BDNY_8814A, txpktbuf_bndy);
		Adapter->bFWReady = TRUE;
	}
#endif

	FW_InitializeHALVars(Adapter);
	SIC_Init(Adapter);
#endif
#endif /* HAL_FW_ENABLE */

#ifdef RTL8814AE_AMPDU_PRE_TX_OFF
	/* RTW_INFO("AMPDU Burst Mode: %x\n", rtw_read8(Adapter, REG_SW_AMPDU_BURST_MODE_CTRL_8814A)); */
	rtw_write8(Adapter, REG_SW_AMPDU_BURST_MODE_CTRL_8814A, rtw_read8(Adapter, REG_SW_AMPDU_BURST_MODE_CTRL_8814A) & ~BIT(6));
	RTW_INFO("Turn off the AMPUD pre-TX, reg 0x%x = 0x%x\n",
		REG_SW_AMPDU_BURST_MODE_CTRL_8814A, rtw_read8(Adapter, REG_SW_AMPDU_BURST_MODE_CTRL_8814A));
#endif

	if (pHalData->AMPDUBurstMode)
		rtw_write8(Adapter, REG_SW_AMPDU_BURST_MODE_CTRL_8814A,  0x7F);
#if 0 /* [ToDo][copy] from win driver */
	else if (pHalData->bNewAMPDUBurstMode) { /* Old Early Mode and New Early Mode can not coexist */
		u2Byte temp  = 0;
		/* bit7:new early mode enable */
		/* bit2:AMPDU burst mode enable for AC_BE queue		 */
		rtw_write8(Adapter, REG_SW_AMPDU_BURST_MODE_CTRL_8814A,  0xC4);

		/* bit15~12:Select which one of queue is used in early mode. 4'b0001: VO;    4'b0010: VI;   4'b0100: BE;   4'b1000: BK; */
		/* bit5:Enable early mode. */
		temp = PlatformEFIORead2Byte(Adapter, REG_DBG_8814A);
		temp = temp & 0x0FFF;
		rtw_write16(Adapter, REG_DBG_8814A,  temp | BIT5 | BIT14);
	}
#endif

	pHalData->CurrentChannel = 0;/* set 0 to trigger switch correct channel */

	/*  */
	/* 2. Initialize MAC/PHY Config by MACPHY_reg.txt */
	/*  */
#if (HAL_MAC_ENABLE == 1)
	RTW_INFO("8814AE MAC Config Start!\n");
	rtStatus = PHY_MACConfig8814(Adapter);
	if (rtStatus != _SUCCESS) {
		RTW_INFO("8814AE MAC Config failed\n");
		return rtStatus;
	}
	RTW_INFO("8814AE MAC Config Finished!\n");

#if 0 /* [check] remove to sync with win driver */
	/* without this statement, RCR doesn't take effect so that it will show 'validate_recv_ctrl_frame fail'. */
	rtw_write32(Adapter, REG_RCR, rtw_read32(Adapter, REG_RCR) & ~(RCR_ADF));
#endif
#endif	/*  #if (HAL_MAC_ENABLE == 1) */

	/*  */
	/* 3. Initialize BB After MAC Config PHY_reg.txt, AGC_Tab.txt */
	/*  */
#if (HAL_BB_ENABLE == 1)
	RTW_INFO("BB Config Start!\n");
	rtStatus = PHY_BBConfig8814(Adapter);
	if (rtStatus != _SUCCESS) {
		RTW_INFO("BB Config failed\n");
		return rtStatus;
	}
	RTW_INFO("BB Config Finished!\n");
#endif	/*  #if (HAL_BB_ENABLE == 1) */

	/* [copy] from win driver */
	/* if(Adapter->MgntInfo.Reg8814AFPGAVerification) */
	{
		u2Byte RegVal = 0;
		RegVal = rtw_read8(Adapter, REG_TX_PTCL_CTRL_8814A + 1);
		rtw_write8(Adapter, REG_TX_PTCL_CTRL_8814A + 1, RegVal & ~BIT4); /* clear 0x521 Bit4(BIT_SIFS_BK_EN) , add by ylb 20130726 to AMPDU small Packet(only 10), UDP TP will + 100Mbps */
	}

	/*  */
	/* 4. Initiailze RF RAIO_A.txt RF RAIO_B.txt */
	/*  */
#if (HAL_RF_ENABLE == 1)
	RTW_INFO("RF Config started!\n");
	rtStatus = PHY_RFConfig8814A(Adapter);
	if (rtStatus != _SUCCESS) {
		RTW_INFO("RF Config failed\n");
		return rtStatus;
	}
	RTW_INFO("RF Config Finished!\n");

#endif	/*  #if (HAL_RF_ENABLE == 1) */

	/*---- Set CCK and OFDM Block "ON"----*/
	PHY_ConfigBB_8814A(Adapter);

	/* 3 Set Hardware(MAC default setting.) */
	HwConfigureRTL8814A(Adapter);

	/* secondary_cca20 and secondary_cca40 signal sent from PHY would be blocked. */
	rtw_write8(Adapter, REG_SECONDARY_CCA_CTRL_8814A, 
		rtw_read8(Adapter, REG_SECONDARY_CCA_CTRL_8814A) | 0x03);

	if (Adapter->registrypriv.channel <= 14)
		PHY_SwitchWirelessBand8814A(Adapter, BAND_ON_2_4G);
	else
		PHY_SwitchWirelessBand8814A(Adapter, BAND_ON_5G);

	/* 3 Set Wireless Mode */
	rtw_hal_set_chnl_bw(Adapter, Adapter->registrypriv.channel,
		CHANNEL_WIDTH_20, HAL_PRIME_CHNL_OFFSET_DONT_CARE, HAL_PRIME_CHNL_OFFSET_DONT_CARE);

	/* adjust EDCCA to avoid collision */
	/* Disable EDCCA setting by TH. It casue p2p find can only scan few p2p device.
		When wifi_spec=1, phydm will stop EDCCA (0x7f7f). BB kevin suggest not to set to 0xfe01. It let EDCCA
		to stop Tx easily. So, it affect the p2p to find other device. */
	/*if (Adapter->registrypriv.wifi_spec)
		rtw_write16(Adapter, rEDCCA_Jaguar, 0xfe01);*/

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
	rtw_write8(Adapter, REG_CR_8814A + 1, (rtw_read8(Adapter, REG_CR_8814A + 1) | BIT1));

	/* 2======================================================= */
	/* RF Power Save */
	/* 2======================================================= */
	/* Fix the bug that Hw/Sw radio off before S3/S4, the RF off action will not be executed */
	/* in MgntActSet_RF_State() after wake up, because the value of pHalData->eRFPowerState */
	/* is the same as eRfOff, we should change it to eRfOn after we config RF parameters. */
	/* Added by tynli. 2010.03.30. */
	pwrpriv->rf_pwrstate = rf_on;

	/* if(pPSC->bGpioRfSw) */

	/*	if(0)
		{
			tmpU1b = rtw_read8(Adapter, REG_GPIO_EXT_CTRL_8814A);
			pwrpriv->rfoff_reason |= (tmpU1b & BIT3) ? 0 : RF_CHANGE_BY_HW;
		}*/

	pwrpriv->rfoff_reason |= (pwrpriv->reg_rfoff) ? RF_CHANGE_BY_SW : 0;

	if (pwrpriv->rfoff_reason & RF_CHANGE_BY_HW)
		pwrpriv->b_hw_radio_off = _TRUE;

	if (pwrpriv->rfoff_reason > RF_CHANGE_BY_PS) {
		/* H/W or S/W RF OFF before sleep. */
		RTW_INFO("rtl8814ae_hal_init(): Turn off RF for RfOffReason(%d) ----------\n", pwrpriv->rfoff_reason);
		/* MgntActSet_RF_State(Adapter, rf_off, pwrpriv->rfoff_reason, _TRUE); */
	} else {
		pwrpriv->rf_pwrstate = rf_on;
		pwrpriv->rfoff_reason = 0;

		RTW_INFO("rtl8814ae_hal_init(): Turn on  ----------\n");

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

#if 0 /* temp delete by ylb 20130806 for init slow */
	EnableAspmBeforeBackDoor_8814AE(Adapter);
#endif
	EnableAspmBackDoor_8814AE(Adapter);
	/* [check] */
	/* EnableL1Off_8812E(Adapter); */

#if 0
	/* Init BT hw config. */
	if (IS_HARDWARE_TYPE_8812(Adapter)) {
		if (pHalData->RFEType == 1 || pHalData->RFEType == 5)
			PHY_SetBBReg(Adapter, r_ANTSEL_SW_Jaguar, 0x00000303, 0x0302);	/* 0x900[9,8,1,0] = b1110 */
	}

#ifdef CONFIG_BT_COEXIST
	rtw_btcoex_HAL_Initialize(Adapter, _FALSE);
#endif
#endif

	Adapter->init_adpt_in_progress = _FALSE;

#if (MP_DRIVER == 1)
	if (Adapter->registrypriv.mp_mode == 1) {
		Adapter->mppriv.channel = pHalData->CurrentChannel;
		MPT_InitializeAdapter(Adapter, Adapter->mppriv.channel);
	} else
#endif

		rtw_hal_set_hwreg(Adapter, HW_VAR_NAV_UPPER, ((u8 *)&NavUpper));

#if 0  /* [check] below is win driver */
	if (pHalData->SwBeaconType < HAL92CE_DEFAULT_BEACON_TYPE) /* The lowest Beacon Type that HW can support */
		pHalData->SwBeaconType = HAL92CE_DEFAULT_BEACON_TYPE;
#if (DEV_BUS_TYPE == RT_PCI_INTERFACE)
	/* Change PcieBus Work on 25G or 5G  by gw  20131225 */
	pMgntInfo->ForcePCIeRate =  pNdisCommon->RegForcePCIeRate;
	PcieBusOnCHANGETO25or50GHz_8814AE(Adapter) ;
#endif
#endif

	rtl8814_InitHalDm(Adapter);


	PHY_IQCalibrate_8814A_Init(&pHalData->odmpriv);

#if (HAL_BB_ENABLE == 1)
	PHY_SetRFEReg8814A(Adapter, _TRUE, pHalData->CurrentBandType);
#endif /* HAL_BB_ENABLE */

	/* this should be done by rf team using phydm code */
	/* PHY_IQCalibrate_8814A(&pHalData->odmpriv, _FALSE); */

#if 0 /* [check] below is win driver */
	MacIdInitializeMediaStatus(Adapter);

	PlatformEFIOWrite1Byte(Adapter, 0x6b7, 0x9);
	PlatformSetTimer(Adapter, &pHalData->CheckMacRxHangTimer, 100);
#endif

#if defined(CONFIG_CONCURRENT_MODE) || defined(CONFIG_TX_MCAST2UNI)

#ifdef CONFIG_CHECK_AC_LIFETIME
	/* Enable lifetime check for the four ACs */
	rtw_write8(Adapter, REG_LIFETIME_CTRL, 0x0F);
#endif /* CONFIG_CHECK_AC_LIFETIME */

#ifdef CONFIG_TX_MCAST2UNI
	rtw_write16(Adapter, REG_PKT_VO_VI_LIFE_TIME, 0x0400);	/* unit: 256us. 256ms */
	rtw_write16(Adapter, REG_PKT_BE_BK_LIFE_TIME, 0x0400);	/* unit: 256us. 256ms */
#else	/* CONFIG_TX_MCAST2UNI */
	rtw_write16(Adapter, REG_PKT_VO_VI_LIFE_TIME, 0x3000);	/* unit: 256us. 3s */
	rtw_write16(Adapter, REG_PKT_BE_BK_LIFE_TIME, 0x3000);	/* unit: 256us. 3s */
#endif /* CONFIG_TX_MCAST2UNI */
#endif /* CONFIG_CONCURRENT_MODE || CONFIG_TX_MCAST2UNI */


	/* enable tx DMA to drop the redundate data of packet */
	rtw_write16(Adapter, REG_TXDMA_OFFSET_CHK, (rtw_read16(Adapter, REG_TXDMA_OFFSET_CHK) | DROP_DATA_EN));

	pHalData->RegBcnCtrlVal = rtw_read8(Adapter, REG_BCN_CTRL);
	pHalData->RegTxPause = rtw_read8(Adapter, REG_TXPAUSE);
	pHalData->RegFwHwTxQCtrl = rtw_read8(Adapter, REG_FWHW_TXQ_CTRL + 2);
	pHalData->RegReg542 = rtw_read8(Adapter, REG_TBTT_PROHIBIT + 2);

	/* MAC Alan suggest to disable TSF 32bit toggle reset P2P NOA-0 */
	rtw_write32(Adapter, 0x528, rtw_read32(Adapter, 0x528) & 0xFFDFFFFF);
	/* MAC Alan suggest to disable TSF 32bit toggle reset P2P NOA-1,2 */
	rtw_write8(Adapter, 0x57B, rtw_read8(Adapter, 0x57B) & 0xCF);

#if 0
	if (pHalData->eRFPowerState == eRfOn) {
		if (pHalData->LedStrategy != SW_LED_MODE10)
			PHY_SetRFPathSwitch_8812(Adapter, pMgntInfo->bDefaultAntenna);	/* Wifi default use Main */

		if (pHalData->odmpriv.RFCalibrateInfo.bIQKInitialized)
			PHY_IQCalibrate_8812(Adapter, TRUE);
		else {
			PHY_IQCalibrate_8812(Adapter, FALSE);
			pHalData->odmpriv.RFCalibrateInfo.bIQKInitialized = TRUE;
		}
		ODM_TXPowerTrackingCheck(&pHalData->DM_OutSrc);
		PHY_LCCalibrate_8812(Adapter);
	}
#endif

	/*
		tmpU1b = EFUSE_Read1Byte(Adapter, 0x1FA);

		if(!(tmpU1b & BIT0))
		{
			PHY_SetRFReg(Adapter, ODM_RF_PATH_A, 0x15, 0x0F, 0x05);
			RTW_INFO("PA BIAS path A\n");
		}

		if(!(tmpU1b & BIT1) && is2T2R)
		{
			PHY_SetRFReg(Adapter, ODM_RF_PATH_B, 0x15, 0x0F, 0x05);
			RTW_INFO("PA BIAS path B\n");
		}
	*/


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
	RTW_INFO("rtl8814ae_hal_init()<========\n");


	return rtStatus;
}

/*
 * 2009/10/13 MH Acoording to documetn form Scott/Alfred....
 * This is based on version 8.1.
 *   */

/* [refer] to win driver : ShutdownAdapter8814AE() */
static u32 rtl8814ae_hal_deinit(PADAPTER Adapter)
{
	u8	u1bTmp = 0;
	u32	count = 0;
	u8	bSupportRemoteWakeUp = _FALSE;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	struct pwrctrl_priv		*pwrpriv = adapter_to_pwrctl(Adapter);


	RTW_INFO("====> %s()\n", __FUNCTION__);

	if (Adapter->bHaltInProgress == _TRUE) {
		RTW_INFO("====> Abort rtl8814ae_hal_deinit()\n");
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

	/* Without supporting WoWLAN or the driver is in awake (D0) state, we should */
	/* call PowerOffAdapter8188EE() to run the power sequence. 2009.04.23. by tynli. */
	if (!bSupportRemoteWakeUp) { /* ||!pMgntInfo->bPwrSaveState) */
		/* 2009/10/13 MH For power off test. */
		PowerOffAdapter8814AE(Adapter);
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
		if (rtStatus == _SUCCESS)
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
			Adapter->HalFunc.SetHwRegHandler(Adapter, HW_VAR_AID, 0);
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
			Adapter->HalFunc.SetHwRegHandler(Adapter, HW_VAR_FW_LPS_ACTION, (pu1Byte)(&bEnterFwLPS));
			pPSC->bEnterLPSDuringSuspend = TRUE;
		}

		PlatformAcquireSpinLock(Adapter, RT_TX_SPINLOCK);
		/* guangan, 2009/08/28, return TCB in busy queue to idle queue when resume from S3/S4. */
		for (QueueID = 0; QueueID < MAX_TX_QUEUE; QueueID++) {
			/* 2004.08.11, revised by rcnjko. */
			while (!RTIsListEmpty(&Adapter->TcbBusyQueue[QueueID])) {
				pTcb = (PRT_TCB)RTRemoveHeadList(&Adapter->TcbBusyQueue[QueueID]);
				ReturnTCB(Adapter, pTcb, _SUCCESS);
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

/* [copy] from 8814AU */
void SetHwReg8814AE(PADAPTER Adapter, u8 variable, u8 *val)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	DM_ODM_T		*podmpriv = &pHalData->odmpriv;

	switch (variable) {
	case HW_VAR_RXDMA_AGG_PG_TH:
		/* [support]? */
		break;
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
			rtw_write8(Adapter, REG_PCIE_HRPWM, ps_state);
		}
#endif
		break;
	case HW_VAR_AMPDU_MAX_TIME: {
#if 0 /* [support]? */
		u8	maxRate = *(u8 *)val;

		if (maxRate > MGN_VHT1SS_MCS9)
			rtw_write8(Adapter, REG_AMPDU_MAX_TIME_8814A, 0x70);
		else if (IS_HARDWARE_TYPE_8814A(Adapter))
			rtw_write8(Adapter, REG_AMPDU_MAX_TIME_8814A, 0xe0);
		else
			rtw_write8(Adapter, REG_AMPDU_MAX_TIME_8814A, 0x5e);
#endif
	}
	break;
	case HW_VAR_PCIE_STOP_TX_DMA:
		rtw_write16(Adapter, REG_PCIE_CTRL_REG, 0xff00);
		break;
	case HW_VAR_DM_IN_LPS:
		/* [support]? */
		break;
	default:
		SetHwReg8814A(Adapter, variable, val);
		break;
	}

}

void GetHwReg8814AE(PADAPTER Adapter, u8 variable, u8 *val)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	DM_ODM_T		*podmpriv = &pHalData->odmpriv;

	switch (variable) {
	default:
		GetHwReg8814A(Adapter, variable, val);
		break;
	}

}

/* [copy] from 8814AU
 *
 *	Description:
 *		Change default setting of specified variable.
 *   */
u8
SetHalDefVar8814AE(
	IN	PADAPTER				Adapter,
	IN	HAL_DEF_VARIABLE		eVariable,
	IN	PVOID					pValue
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u8			bResult = _SUCCESS;

	switch (eVariable) {
	default:
		SetHalDefVar8814A(Adapter, eVariable, pValue);
		break;
	}

	return bResult;
}

/* [copy] from 8814AU
 *
 *	Description:
 *		Query setting of specified variable.
 *   */
u8
GetHalDefVar8814AE(
	IN	PADAPTER				Adapter,
	IN	HAL_DEF_VARIABLE		eVariable,
	IN	PVOID					pValue
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u8			bResult = _SUCCESS;

	switch (eVariable) {
	default:
		GetHalDefVar8814A(Adapter, eVariable, pValue);
		break;
	}

	return bResult;
}

/* [refer] to win driver : InitializeVariables8814AE() and 8814AU */
static void rtl8814ae_init_default_value(_adapter *padapter)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);


	InitDefaultValue8814A(padapter);

	pHalData->CurrentWirelessMode = WIRELESS_MODE_AUTO;
	pHalData->bDefaultAntenna = 1;

	/*  */
	/* Set TCR-Transmit Control Register. The value is set in InitializeAdapter8190Pci() */
	/*  */
	/* YJ,mod,111212 */
	/* pHalData->TransmitConfig = CFENDFORM | BIT12 | BIT13; */
	pHalData->TransmitConfig = CFENDFORM | BIT15;

	/*  */
	/* Set RCR-Receive Control Register . The value is set in InitializeAdapter8190Pci(). */
	/*  */
	pHalData->ReceiveConfig = (\
				   RCR_APP_MIC		|
				   RCR_APP_ICV			|
				   RCR_APP_PHYST_RXFF	|
				   RCR_VHT_DACK	|
				   RCR_HTC_LOC_CTRL	|
				   RCR_AMF				|
				   /* RCR_ACF				|	 */
		/* RCR_ADF				|	 */ /* Note: This bit controls the PS-Poll packet filter. */
				   /* RCR_AICV			| */
#if (1 == RTL8814A_RX_PACKET_INCLUDE_CRC)
				   RCR_ACRC32			|
#endif
				   RCR_CBSSID_BCN		|
				   RCR_CBSSID_DATA	|
				   RCR_AB				|
				   RCR_AM				|
				   RCR_APM			|
#ifdef CONFIG_RX_PACKET_APPEND_FCS
				   RCR_APPFCS			|
#endif
				   0);

	/*  */
	/* Set Interrupt Mask Register */
	/*  */
	pHalData->IntrMaskDefault[0]	= (u32)(\
						IMR_PSTIMEOUT_8814A		|
		/* IMR_GTINT4_8814A			|	 */ /* [check]: win driver enable */
						IMR_GTINT3_8814A			|
						IMR_TXBCN0ERR_8814A		|
						IMR_TXBCN0OK_8814A		|
						IMR_BCNDMAINT0_8814A	|
						IMR_HSISR_IND_ON_INT_8814A	|
						IMR_C2HCMD_8814A		|
		/* IMR_CPWM2_8814A			|	 */ /* [check]: win driver enable */
						/* IMR_CPWM_8814A			| */
						IMR_HIGHDOK_8814A		|
						IMR_MGNTDOK_8814A		|
						IMR_BKDOK_8814A			|
						IMR_BEDOK_8814A			|
						IMR_VIDOK_8814A			|
						IMR_VODOK_8814A			|
						IMR_RDU_8814A			|
						IMR_ROK_8814A			|
						0);
	pHalData->IntrMaskDefault[1]	= (u32)(\
						IMR_MCUERR_8814A	|
						IMR_RXERR_8814A		|
						IMR_TXERR_8814A		|
						IMR_RXFOVW_8814A	|
						IMR_TXFOVW_8814A	|
						0);

	/* 2012/03/27 hpfan Add for win8 DTM DPC ISR test */
	pHalData->IntrMaskReg[0]	=	(u32)(\
						IMR_RDU_8814A		|
						IMR_PSTIMEOUT_8814A	|
						0);
	pHalData->IntrMaskReg[1]	=	(u32)(\
						IMR_C2HCMD_8814A	|
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

	if (IS_HARDWARE_TYPE_8814AE(padapter)) {
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

void rtl8814ae_set_hal_ops(_adapter *padapter)
{
	struct hal_ops	*pHalFunc = &padapter->HalFunc;


	pHalFunc->hal_power_on = _InitPowerOn_8814AE;
	pHalFunc->hal_power_off = hal_carddisable_8814AE;

	pHalFunc->hal_init = &rtl8814ae_hal_init;
	pHalFunc->hal_deinit = &rtl8814ae_hal_deinit;

	pHalFunc->inirp_init = &rtl8814ae_init_desc_ring;
	pHalFunc->inirp_deinit = &rtl8814ae_free_desc_ring;
	pHalFunc->irp_reset = &rtl8814ae_reset_desc_ring;

	pHalFunc->init_xmit_priv = &rtl8814ae_init_xmit_priv;
	pHalFunc->free_xmit_priv = &rtl8814ae_free_xmit_priv;

	pHalFunc->init_recv_priv = &rtl8814ae_init_recv_priv;
	pHalFunc->free_recv_priv = &rtl8814ae_free_recv_priv;
#ifdef CONFIG_SW_LED
	pHalFunc->InitSwLeds = &rtl8814ae_InitSwLeds;
	pHalFunc->DeInitSwLeds = &rtl8814ae_DeInitSwLeds;
#else /* case of hw led or no led */
	pHalFunc->InitSwLeds = NULL;
	pHalFunc->DeInitSwLeds = NULL;
#endif/* CONFIG_SW_LED */

	pHalFunc->init_default_value = &rtl8814ae_init_default_value;
	pHalFunc->intf_chip_configure = &rtl8814ae_interface_configure;
	pHalFunc->read_adapter_info = &ReadAdapterInfo8814AE;

	pHalFunc->enable_interrupt = &EnableInterrupt8814AE;
	pHalFunc->disable_interrupt = &DisableInterrupt8814AE;
	pHalFunc->interrupt_handler = &rtl8814ae_interrupt;

	pHalFunc->SetHwRegHandler = &SetHwReg8814AE;
	pHalFunc->GetHwRegHandler = &GetHwReg8814AE;
	pHalFunc->GetHalDefVarHandler = &GetHalDefVar8814AE;
	pHalFunc->SetHalDefVarHandler = &SetHalDefVar8814AE;

	pHalFunc->hal_xmit = &rtl8814ae_hal_xmit;
	pHalFunc->mgnt_xmit = &rtl8814ae_mgnt_xmit;
	pHalFunc->hal_xmitframe_enqueue = &rtl8814ae_hal_xmitframe_enqueue;

#ifdef CONFIG_HOSTAPD_MLME
	pHalFunc->hostap_mgnt_xmit_entry = &rtl8814ae_hostap_mgnt_xmit_entry;
#endif

#ifdef CONFIG_XMIT_THREAD_MODE
	pHalFunc->xmit_thread_handler = &rtl8814ae_xmit_buf_handler;
#endif
#ifdef RTL8814AE_SW_BCN
	pHalFunc->fw_correct_bcn = &rtl8814_fw_update_beacon_cmd;
#else
	pHalFunc->fw_correct_bcn = NULL;
#endif
	rtl8814_set_hal_ops(pHalFunc);

}
