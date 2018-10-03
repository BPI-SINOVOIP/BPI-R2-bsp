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

#include "halmac_pcie_88xx.h"

#if HALMAC_88XX_SUPPORT

/**
 * halmac_init_pcie_cfg_88xx() -  init PCIe
 * @adapter : the adapter of halmac
 * Author : KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_init_pcie_cfg_88xx(
	IN struct halmac_adapter *adapter
)
{
	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (adapter->halmac_interface != HALMAC_INTERFACE_PCIE)
		return HALMAC_RET_WRONG_INTF;

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_deinit_pcie_cfg_88xx() - deinit PCIE
 * @adapter : the adapter of halmac
 * Author : KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_deinit_pcie_cfg_88xx(
	IN struct halmac_adapter *adapter
)
{
	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	if (adapter->halmac_interface != HALMAC_INTERFACE_PCIE)
		return HALMAC_RET_WRONG_INTF;

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_cfg_rx_aggregation_88xx_pcie() - config rx aggregation
 * @adapter : the adapter of halmac
 * @halmac_rx_agg_mode
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_cfg_rx_aggregation_88xx_pcie(
	IN struct halmac_adapter *adapter,
	IN struct halmac_rxagg_cfg *cfg
)
{
	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_reg_read_8_pcie_88xx() - read 1byte register
 * @adapter : the adapter of halmac
 * @offset : register offset
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
u8
halmac_reg_read_8_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset
)
{
	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	return PLTFM_REG_R8(offset);
}

/**
 * halmac_reg_write_8_pcie_88xx() - write 1byte register
 * @adapter : the adapter of halmac
 * @offset : register offset
 * @value : register value
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_reg_write_8_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u8 value
)
{
	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_REG_W8(offset, value);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_reg_read_16_pcie_88xx() - read 2byte register
 * @adapter : the adapter of halmac
 * @offset : register offset
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
u16
halmac_reg_read_16_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset
)
{
	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	return PLTFM_REG_R16(offset);
}

/**
 * halmac_reg_write_16_pcie_88xx() - write 2byte register
 * @adapter : the adapter of halmac
 * @offset : register offset
 * @value : register value
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_reg_write_16_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u16 value
)
{
	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_REG_W16(offset, value);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_reg_read_32_pcie_88xx() - read 4byte register
 * @adapter : the adapter of halmac
 * @offset : register offset
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
u32
halmac_reg_read_32_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset
)
{
	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	return PLTFM_REG_R32(offset);
}

/**
 * halmac_reg_write_32_pcie_88xx() - write 4byte register
 * @adapter : the adapter of halmac
 * @offset : register offset
 * @value : register value
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_reg_write_32_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u32 value
)
{
	if (halmac_api_validate(adapter) != HALMAC_RET_SUCCESS)
		return HALMAC_RET_API_INVALID;

	PLTFM_REG_W32(offset, value);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_cfg_tx_agg_align_pcie_88xx() -config sdio bus tx agg alignment
 * @adapter : the adapter of halmac
 * @enable : function enable(1)/disable(0)
 * @align_size : sdio bus tx agg alignment size (2^n, n = 3~11)
 * Author : Soar Tu
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_cfg_tx_agg_align_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u8	enable,
	IN u16	align_size
)
{
	return HALMAC_RET_NOT_SUPPORT;
}

/**
 * halmac_tx_allowed_pcie_88xx() - check tx status
 * @adapter : the adapter of halmac
 * @buf : tx packet, include txdesc
 * @size : tx packet size, include txdesc
 * Author : Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_tx_allowed_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size
)
{
	return HALMAC_RET_NOT_SUPPORT;
}

/**
 * halmac_reg_read_indirect_32_pcie_88xx() - read MAC reg by SDIO reg
 * @adapter : the adapter of halmac
 * @offset : register offset
 * Author : Soar
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
u32
halmac_reg_read_indirect_32_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset
)
{
	return 0xFFFFFFFF;
}

/**
 * halmac_reg_read_nbyte_pcie_88xx() - read n byte register
 * @adapter : the adapter of halmac
 * @offset : register offset
 * @size : register value size
 * @value : register value
 * Author : Soar
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_reg_read_nbyte_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u32 offset,
	IN u32 size,
	OUT u8 *value
)
{
	return HALMAC_RET_NOT_SUPPORT;
}

/**
 * halmac_set_bulkout_num_pcie_88xx() - inform bulk-out num
 * @adapter : the adapter of halmac
 * @num : usb bulk-out number
 * Author : KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_set_bulkout_num_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 num
)
{
	return HALMAC_RET_NOT_SUPPORT;
}

/**
 * halmac_get_sdio_tx_addr_pcie_88xx() - get CMD53 addr for the TX packet
 * @adapter : the adapter of halmac
 * @buf : tx packet, include txdesc
 * @size : tx packet size
 * @cmd53_addr : cmd53 addr value
 * Author : KaiYuan Chang/Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_get_sdio_tx_addr_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size,
	OUT u32 *cmd53_addr
)
{
	return HALMAC_RET_NOT_SUPPORT;
}

/**
 * halmac_get_usb_bulkout_id_pcie_88xx() - get bulk out id for the TX packet
 * @adapter : the adapter of halmac
 * @buf : tx packet, include txdesc
 * @size : tx packet size
 * @id : usb bulk-out id
 * Author : KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_get_usb_bulkout_id_pcie_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 *buf,
	IN u32 size,
	OUT u8 *id
)
{
	return HALMAC_RET_NOT_SUPPORT;
}

enum halmac_ret_status
halmac_mdio_write_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 addr,
	IN u16 data,
	IN u8 speed
)
{
	u8 tmp_u1b = 0;
	u32 cnt = 0;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	u8 real_addr = 0;

	HALMAC_REG_W16(REG_MDIO_V1, data);

	real_addr = (addr & 0x1F);
	HALMAC_REG_W8(REG_PCIE_MIX_CFG, real_addr);

	if (speed == HAL_INTF_PHY_PCIE_GEN1) {
		if (addr < 0x20)
			HALMAC_REG_W8(REG_PCIE_MIX_CFG + 3, 0x00);
		else
			HALMAC_REG_W8(REG_PCIE_MIX_CFG + 3, 0x01);
	} else if (speed == HAL_INTF_PHY_PCIE_GEN2) {
		if (addr < 0x20)
			HALMAC_REG_W8(REG_PCIE_MIX_CFG + 3, 0x02);
		else
			HALMAC_REG_W8(REG_PCIE_MIX_CFG + 3, 0x03);
	} else {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Error Speed !\n");
	}

	HALMAC_REG_W8(REG_PCIE_MIX_CFG,
		      HALMAC_REG_R8(REG_PCIE_MIX_CFG) | BIT_MDIO_WFLAG_V1);

	tmp_u1b = HALMAC_REG_R8(REG_PCIE_MIX_CFG) & BIT_MDIO_WFLAG_V1;
	cnt = 20;

	while (tmp_u1b && (cnt != 0)) {
		PLTFM_DELAY_US(10);
		tmp_u1b = HALMAC_REG_R8(REG_PCIE_MIX_CFG) & BIT_MDIO_WFLAG_V1;
		cnt--;
	}

	if (tmp_u1b) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]MDIO write fail!\n");
		return HALMAC_RET_FAIL;
	}

	return HALMAC_RET_SUCCESS;
}

u16
halmac_mdio_read_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 addr,
	IN u8 speed

)
{
	u16 ret = 0;
	u8 tmp_u1b = 0;
	u32 cnt = 0;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;
	u8 real_addr = 0;

	real_addr = (addr & 0x1F);
	HALMAC_REG_W8(REG_PCIE_MIX_CFG, real_addr);

	if (speed == HAL_INTF_PHY_PCIE_GEN1) {
		if (addr < 0x20)
			HALMAC_REG_W8(REG_PCIE_MIX_CFG + 3, 0x00);
		else
			HALMAC_REG_W8(REG_PCIE_MIX_CFG + 3, 0x01);
	} else if (speed == HAL_INTF_PHY_PCIE_GEN2) {
		if (addr < 0x20)
			HALMAC_REG_W8(REG_PCIE_MIX_CFG + 3, 0x02);
		else
			HALMAC_REG_W8(REG_PCIE_MIX_CFG + 3, 0x03);
	} else {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]Error Speed !\n");
	}

	HALMAC_REG_W8(REG_PCIE_MIX_CFG,
		      HALMAC_REG_R8(REG_PCIE_MIX_CFG) | BIT_MDIO_RFLAG_V1);

	tmp_u1b = HALMAC_REG_R8(REG_PCIE_MIX_CFG) & BIT_MDIO_RFLAG_V1;
	cnt = 20;

	while (tmp_u1b && (cnt != 0)) {
		PLTFM_DELAY_US(10);
		tmp_u1b = HALMAC_REG_R8(REG_PCIE_MIX_CFG) & BIT_MDIO_RFLAG_V1;
		cnt--;
	}

	if (tmp_u1b) {
		ret  = 0xFFFF;
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]MDIO read fail!\n");
	} else {
		ret = HALMAC_REG_R16(REG_MDIO_V1 + 2);
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Value-R = %x\n", ret);
	}

	return ret;
}

enum halmac_ret_status
halmac_dbi_write32_88xx(
	IN struct halmac_adapter *adapter,
	IN u16 addr,
	IN u32 data
)
{
	u8 tmp_u1b = 0;
	u32 cnt = 0;
	u16 write_addr = 0;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	HALMAC_REG_W32(REG_DBI_WDATA_V1, data);

	write_addr = ((addr & 0x0ffc) | (0x000F << 12));
	HALMAC_REG_W16(REG_DBI_FLAG_V1, write_addr);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Addr-W = %x\n", write_addr);

	HALMAC_REG_W8(REG_DBI_FLAG_V1 + 2, 0x01);
	tmp_u1b = HALMAC_REG_R8(REG_DBI_FLAG_V1 + 2);

	cnt = 20;
	while (tmp_u1b && (cnt != 0)) {
		PLTFM_DELAY_US(10);
		tmp_u1b = HALMAC_REG_R8(REG_DBI_FLAG_V1 + 2);
		cnt--;
	}

	if (tmp_u1b) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]DBI write fail!\n");
		return HALMAC_RET_FAIL;
	}

	return HALMAC_RET_SUCCESS;
}

u32
halmac_dbi_read32_88xx(
	IN struct halmac_adapter *adapter,
	IN u16 addr
)
{
	u16 read_addr = addr & 0x0ffc;
	u8 tmp_u1b = 0;
	u32 cnt = 0;
	u32 ret = 0;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	HALMAC_REG_W16(REG_DBI_FLAG_V1, read_addr);

	HALMAC_REG_W8(REG_DBI_FLAG_V1 + 2, 0x2);
	tmp_u1b = HALMAC_REG_R8(REG_DBI_FLAG_V1 + 2);

	cnt = 20;
	while (tmp_u1b && (cnt != 0)) {
		PLTFM_DELAY_US(10);
		tmp_u1b = HALMAC_REG_R8(REG_DBI_FLAG_V1 + 2);
		cnt--;
	}

	if (tmp_u1b) {
		ret  = 0xFFFF;
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]DBI read fail!\n");
	} else {
		ret = HALMAC_REG_R32(REG_DBI_RDATA_V1);
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Value-R = %x\n", ret);
	}

	return ret;
}

enum halmac_ret_status
halmac_dbi_write8_88xx(
	IN struct halmac_adapter *adapter,
	IN u16 addr,
	IN u8 data
)
{
	u8 tmp_u1b = 0;
	u32 cnt = 0;
	u16 write_addr = 0;
	u16 remainder = addr & (4 - 1);
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	HALMAC_REG_W8(REG_DBI_WDATA_V1 + remainder, data);

	write_addr = ((addr & 0x0ffc) | (BIT(0) << (remainder + 12)));

	HALMAC_REG_W16(REG_DBI_FLAG_V1, write_addr);

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Addr-W = %x\n", write_addr);

	HALMAC_REG_W8(REG_DBI_FLAG_V1 + 2, 0x01);

	tmp_u1b = HALMAC_REG_R8(REG_DBI_FLAG_V1 + 2);

	cnt = 20;
	while (tmp_u1b && (cnt != 0)) {
		PLTFM_DELAY_US(10);
		tmp_u1b = HALMAC_REG_R8(REG_DBI_FLAG_V1 + 2);
		cnt--;
	}

	if (tmp_u1b) {
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]DBI write fail!\n");
		return HALMAC_RET_FAIL;
	}

	return HALMAC_RET_SUCCESS;
}

u8
halmac_dbi_read8_88xx(
	IN struct halmac_adapter *adapter,
	IN u16 addr
)
{
	u16 read_addr = addr & 0x0ffc;
	u8 tmp_u1b = 0;
	u32 cnt = 0;
	u8 ret = 0;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	HALMAC_REG_W16(REG_DBI_FLAG_V1, read_addr);
	HALMAC_REG_W8(REG_DBI_FLAG_V1 + 2, 0x2);

	tmp_u1b = HALMAC_REG_R8(REG_DBI_FLAG_V1 + 2);

	cnt = 20;
	while (tmp_u1b && (cnt != 0)) {
		PLTFM_DELAY_US(10);
		tmp_u1b = HALMAC_REG_R8(REG_DBI_FLAG_V1 + 2);
		cnt--;
	}

	if (tmp_u1b) {
		ret  = 0xFF;
		PLTFM_MSG_PRINT(HALMAC_DBG_ERR, "[ERR]DBI read fail!\n");
	} else {
		ret = HALMAC_REG_R8(REG_DBI_RDATA_V1 + (addr & (4 - 1)));
		PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]Value-R = %x\n", ret);
	}

	return ret;
}

enum halmac_ret_status
halmac_trxdma_check_idle_88xx(
	IN struct halmac_adapter *adapter
)
{
	u8 tmp_u8 = 0;
	u32 tmp_u32 = 0;
	u32 cnt = 0;
	struct halmac_api *api = (struct halmac_api *)adapter->pHalmac_api;

	/* Stop Tx & Rx DMA */
	HALMAC_REG_W32(REG_RXPKT_NUM, HALMAC_REG_R32(REG_RXPKT_NUM) | BIT(18));
	HALMAC_REG_W16(REG_PCIE_CTRL,
		       HALMAC_REG_R16(REG_PCIE_CTRL) | ~(BIT(15) | BIT(8)));

	/* Stop FW */
	HALMAC_REG_W16(REG_SYS_FUNC_EN,
		       HALMAC_REG_R16(REG_SYS_FUNC_EN) & ~(BIT(10)));

	/* Check Tx DMA is idle */
	cnt = 20;
	while ((HALMAC_REG_R8(REG_SYS_CFG5) & BIT(2)) == BIT(2)) {
		PLTFM_DELAY_US(10);
		cnt--;
		if (cnt == 0) {
			PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[ERR]Chk tx idle\n");
			return HALMAC_RET_POWER_OFF_FAIL;
		}
	}

	/* Check Rx DMA is idle */
	cnt = 20;
	while ((HALMAC_REG_R32(REG_RXPKT_NUM) & BIT(17)) != BIT(17)) {
		PLTFM_DELAY_US(10);
		cnt--;
		if (cnt == 0) {
			PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[ERR]Chk rx idle\n");
			return HALMAC_RET_POWER_OFF_FAIL;
		}
	}

	return HALMAC_RET_SUCCESS;
}
#endif /* HALMAC_88XX_SUPPORT */
