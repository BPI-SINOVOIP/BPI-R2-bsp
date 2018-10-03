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

#include "halmac_gpio_8821c.h"
#include "../halmac_gpio_88xx.h"

#if HALMAC_8821C_SUPPORT

const struct halmac_gpio_pimux_list PIMUX_LIST_GPIO0_8821C[] = {
	HALMAC_GPIO0_BT_GPIO0_8821C,
	HALMAC_GPIO0_BT_ACT_8821C,
	HALMAC_GPIO0_WL_ACT_8821C,
	HALMAC_GPIO0_WLMAC_DBG_GPIO0_8821C,
	HALMAC_GPIO0_WLPHY_DBG_GPIO0_8821C,
	HALMAC_GPIO0_BT_DBG_GPIO0_8821C,
	HALMAC_GPIO0_SW_IO_8821C
};

const struct halmac_gpio_pimux_list PIMUX_LIST_GPIO1_8821C[] = {
	HALMAC_GPIO1_BT_GPIO1_8821C,
	HALMAC_GPIO1_BT_3DD_SYNC_A_8821C,
	HALMAC_GPIO1_WL_CK_8821C,
	HALMAC_GPIO1_BT_CK_8821C,
	HALMAC_GPIO1_WLMAC_DBG_GPIO1_8821C,
	HALMAC_GPIO1_WLPHY_DBG_GPIO1_8821C,
	HALMAC_GPIO1_BT_DBG_GPIO1_8821C,
	HALMAC_GPIO1_SW_IO_8821C
};

const struct halmac_gpio_pimux_list PIMUX_LIST_GPIO2_8821C[] = {
	HALMAC_GPIO2_BT_GPIO2_8821C,
	HALMAC_GPIO2_WL_STATE_8821C,
	HALMAC_GPIO2_BT_STATE_8821C,
	HALMAC_GPIO2_WLMAC_DBG_GPIO2_8821C,
	HALMAC_GPIO2_WLPHY_DBG_GPIO2_8821C,
	HALMAC_GPIO2_BT_DBG_GPIO2_8821C,
	HALMAC_GPIO2_RFE_CTRL_5_8821C,
	HALMAC_GPIO2_SW_IO_8821C
};

const struct halmac_gpio_pimux_list PIMUX_LIST_GPIO3_8821C[] = {
	HALMAC_GPIO3_BT_GPIO3_8821C,
	HALMAC_GPIO3_WL_PRI_8821C,
	HALMAC_GPIO3_BT_PRI_8821C,
	HALMAC_GPIO3_WLMAC_DBG_GPIO3_8821C,
	HALMAC_GPIO3_WLPHY_DBG_GPIO3_8821C,
	HALMAC_GPIO3_BT_DBG_GPIO3_8821C,
	HALMAC_GPIO3_RFE_CTRL_4_8821C,
	HALMAC_GPIO3_SW_IO_8821C
};

const struct halmac_gpio_pimux_list PIMUX_LIST_GPIO4_8821C[] = {
	HALMAC_GPIO4_BT_SPI_D0_8821C,
	HALMAC_GPIO4_WL_SPI_D0_8821C,
	HALMAC_GPIO4_SDIO_INT_8821C,
	HALMAC_GPIO4_JTAG_TRST_8821C,
	HALMAC_GPIO4_DBG_GNT_WL_8821C,
	HALMAC_GPIO4_WLMAC_DBG_GPIO4_8821C,
	HALMAC_GPIO4_WLPHY_DBG_GPIO4_8821C,
	HALMAC_GPIO4_BT_DBG_GPIO4_8821C,
	HALMAC_GPIO4_SW_IO_8821C
};

const struct halmac_gpio_pimux_list PIMUX_LIST_GPIO5_8821C[] = {
	HALMAC_GPIO5_BT_SPI_D1_8821C,
	HALMAC_GPIO5_WL_SPI_D1_8821C,
	HALMAC_GPIO5_JTAG_TDI_8821C,
	HALMAC_GPIO5_DBG_GNT_BT_8821C,
	HALMAC_GPIO5_WLMAC_DBG_GPIO5_8821C,
	HALMAC_GPIO5_WLPHY_DBG_GPIO5_8821C,
	HALMAC_GPIO5_BT_DBG_GPIO5_8821C,
	HALMAC_GPIO5_SW_IO_8821C
};

const struct halmac_gpio_pimux_list PIMUX_LIST_GPIO6_8821C[] = {
	HALMAC_GPIO6_BT_SPI_D2_8821C,
	HALMAC_GPIO6_WL_SPI_D2_8821C,
	HALMAC_GPIO6_EEDO_8821C,
	HALMAC_GPIO6_JTAG_TDO_8821C,
	HALMAC_GPIO6_BT_3DD_SYNC_B_8821C,
	HALMAC_GPIO6_BT_GPIO18_8821C,
	HALMAC_GPIO6_SIN_8821C,
	HALMAC_GPIO6_WLMAC_DBG_GPIO6_8821C,
	HALMAC_GPIO6_WLPHY_DBG_GPIO6_8821C,
	HALMAC_GPIO6_BT_DBG_GPIO6_8821C,
	HALMAC_GPIO6_SW_IO_8821C
};

const struct halmac_gpio_pimux_list PIMUX_LIST_GPIO7_8821C[] = {
	HALMAC_GPIO7_BT_SPI_D3_8821C,
	HALMAC_GPIO7_WL_SPI_D3_8821C,
	HALMAC_GPIO7_EEDI_8821C,
	HALMAC_GPIO7_JTAG_TMS_8821C,
	HALMAC_GPIO7_BT_GPIO16_8821C,
	HALMAC_GPIO7_SOUT_8821C,
	HALMAC_GPIO7_WLMAC_DBG_GPIO7_8821C,
	HALMAC_GPIO7_WLPHY_DBG_GPIO7_8821C,
	HALMAC_GPIO7_BT_DBG_GPIO7_8821C,
	HALMAC_GPIO7_SW_IO_8821C
};

const struct halmac_gpio_pimux_list PIMUX_LIST_GPIO8_8821C[] = {
	HALMAC_GPIO8_WL_EXT_WOL_8821C,
	HALMAC_GPIO8_WL_LED_8821C,
	HALMAC_GPIO8_SW_IO_8821C
};

const struct halmac_gpio_pimux_list PIMUX_LIST_GPIO9_8821C[] = {
	HALMAC_GPIO9_DIS_WL_N_8821C,
	HALMAC_GPIO9_WL_EXT_WOL_8821C,
	HALMAC_GPIO9_USCTS0_8821C,
	HALMAC_GPIO9_SW_IO_8821C
};

const struct halmac_gpio_pimux_list PIMUX_LIST_GPIO10_8821C[] = {
	HALMAC_GPIO10_SW_IO_8821C
};

const struct halmac_gpio_pimux_list PIMUX_LIST_GPIO11_8821C[] = {
	HALMAC_GPIO11_DIS_BT_N_8821C,
	HALMAC_GPIO11_USOUT0_8821C,
	HALMAC_GPIO11_SW_IO_8821C
};

const struct halmac_gpio_pimux_list PIMUX_LIST_GPIO12_8821C[] = {
	HALMAC_GPIO12_USIN0_8821C,
	HALMAC_GPIO12_SW_IO_8821C
};

const struct halmac_gpio_pimux_list PIMUX_LIST_GPIO13_8821C[] = {
	HALMAC_GPIO13_BT_WAKE_8821C,
	HALMAC_GPIO13_SW_IO_8821C
};

const struct halmac_gpio_pimux_list PIMUX_LIST_GPIO14_8821C[] = {
	HALMAC_GPIO14_UART_WAKE_8821C,
	HALMAC_GPIO14_SW_IO_8821C
};

const struct halmac_gpio_pimux_list PIMUX_LIST_GPIO15_8821C[] = {
	HALMAC_GPIO15_EXT_XTAL_8821C,
	HALMAC_GPIO15_SW_IO_8821C
};

static enum halmac_ret_status
halmac_get_pinmux_list_8821c(
	IN struct halmac_adapter *adapter,
	IN enum halmac_gpio_func gpio_func,
	OUT const struct halmac_gpio_pimux_list **list,
	OUT u32 *list_size,
	OUT u32 *gpio_id
);

static enum halmac_ret_status
halmac_chk_pinmux_valid_8821c(
	IN struct halmac_adapter *adapter,
	IN enum halmac_gpio_func gpio_func
);

/**
 * halmac_pinmux_get_func_8821c() -get current gpio status
 * @adapter : the adapter of halmac
 * @gpio_func : gpio function
 * @enable : function is enable(1) or disable(0)
 * Author : Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_pinmux_get_func_8821c(
	IN struct halmac_adapter *adapter,
	IN enum halmac_gpio_func gpio_func,
	OUT u8 *enable
)
{
	u32 list_size;
	u32 cur_func;
	u32 gpio_id;
	enum halmac_ret_status status;
	const struct halmac_gpio_pimux_list *list = NULL;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	status = halmac_get_pinmux_list_8821c(
			adapter, gpio_func, &list, &list_size, &gpio_id);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	status = halmac_pinmux_parser_88xx(
			adapter, list, list_size, gpio_id, &cur_func);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	switch (gpio_func) {
	case HALMAC_GPIO_FUNC_WL_LED:
		*enable = (cur_func == HALMAC_WL_LED) ? 1 : 0;
		break;
	case HALMAC_GPIO_FUNC_SDIO_INT:
		*enable = (cur_func == HALMAC_SDIO_INT) ? 1 : 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_0:
	case HALMAC_GPIO_FUNC_SW_IO_1:
	case HALMAC_GPIO_FUNC_SW_IO_2:
	case HALMAC_GPIO_FUNC_SW_IO_3:
	case HALMAC_GPIO_FUNC_SW_IO_4:
	case HALMAC_GPIO_FUNC_SW_IO_5:
	case HALMAC_GPIO_FUNC_SW_IO_6:
	case HALMAC_GPIO_FUNC_SW_IO_7:
	case HALMAC_GPIO_FUNC_SW_IO_8:
	case HALMAC_GPIO_FUNC_SW_IO_9:
	case HALMAC_GPIO_FUNC_SW_IO_10:
	case HALMAC_GPIO_FUNC_SW_IO_11:
	case HALMAC_GPIO_FUNC_SW_IO_12:
	case HALMAC_GPIO_FUNC_SW_IO_13:
	case HALMAC_GPIO_FUNC_SW_IO_14:
	case HALMAC_GPIO_FUNC_SW_IO_15:
		*enable = (cur_func == HALMAC_SW_IO) ? 1 : 0;
		break;
	default:
		*enable = 0;
		return HALMAC_RET_GET_PINMUX_ERR;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_pinmux_set_func_8821c() -set gpio function
 * @adapter : the adapter of halmac
 * @gpio_func : gpio function
 * Author : Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_pinmux_set_func_8821c(
	IN struct halmac_adapter *adapter,
	IN enum halmac_gpio_func gpio_func
)
{
	u32 list_size;
	u32 gpio_id;
	enum halmac_ret_status status;
	const struct halmac_gpio_pimux_list *list = NULL;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);
	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]func name : %d\n", gpio_func);

	status = halmac_chk_pinmux_valid_8821c(adapter, gpio_func);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	status = halmac_get_pinmux_list_8821c(
			adapter, gpio_func, &list, &list_size, &gpio_id);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	status = halmac_pinmux_switch_88xx(
			adapter, list, list_size, gpio_id, gpio_func);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	status = halmac_pinmux_record_88xx(
			adapter, gpio_func, 1);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_pinmux_free_func_8821c() -free locked gpio function
 * @adapter : the adapter of halmac
 * @gpio_func : gpio function
 * Author : Ivan Lin
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
halmac_pinmux_free_func_8821c(
	IN struct halmac_adapter *adapter,
	IN enum halmac_gpio_func gpio_func
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s ===>\n", __func__);

	switch (gpio_func) {
	case HALMAC_GPIO_FUNC_SW_IO_0:
		adapter->pinmux_info.sw_io_0 = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_1:
		adapter->pinmux_info.sw_io_1 = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_2:
		adapter->pinmux_info.sw_io_2 = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_3:
		adapter->pinmux_info.sw_io_3 = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_4:
	case HALMAC_GPIO_FUNC_SDIO_INT:
		adapter->pinmux_info.sw_io_4 = 0;
		adapter->pinmux_info.sdio_int = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_5:
		adapter->pinmux_info.sw_io_5 = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_6:
		adapter->pinmux_info.sw_io_6 = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_7:
		adapter->pinmux_info.sw_io_7 = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_8:
	case HALMAC_GPIO_FUNC_WL_LED:
		adapter->pinmux_info.sw_io_8 = 0;
		adapter->pinmux_info.wl_led = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_9:
		adapter->pinmux_info.sw_io_9 = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_10:
		adapter->pinmux_info.sw_io_10 = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_11:
		adapter->pinmux_info.sw_io_11 = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_12:
		adapter->pinmux_info.sw_io_12 = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_13:
		adapter->pinmux_info.sw_io_13 = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_14:
		adapter->pinmux_info.sw_io_14 = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_15:
		adapter->pinmux_info.sw_io_15 = 0;
		break;
	default:
		return HALMAC_RET_SWITCH_CASE_ERROR;
	}

	PLTFM_MSG_PRINT(HALMAC_DBG_TRACE, "[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_get_pinmux_list_8821c(
	IN struct halmac_adapter *adapter,
	IN enum halmac_gpio_func gpio_func,
	OUT const struct halmac_gpio_pimux_list **list,
	OUT u32 *list_size,
	OUT u32 *gpio_id
)
{
	switch (gpio_func) {
	case HALMAC_GPIO_FUNC_SW_IO_0:
		*list = PIMUX_LIST_GPIO0_8821C;
		*list_size = ARRAY_SIZE(PIMUX_LIST_GPIO0_8821C);
		*gpio_id = HALMAC_GPIO0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_1:
		*list = PIMUX_LIST_GPIO1_8821C;
		*list_size = ARRAY_SIZE(PIMUX_LIST_GPIO1_8821C);
		*gpio_id = HALMAC_GPIO1;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_2:
		*list = PIMUX_LIST_GPIO2_8821C;
		*list_size = ARRAY_SIZE(PIMUX_LIST_GPIO2_8821C);
		*gpio_id = HALMAC_GPIO2;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_3:
		*list = PIMUX_LIST_GPIO3_8821C;
		*list_size = ARRAY_SIZE(PIMUX_LIST_GPIO3_8821C);
		*gpio_id = HALMAC_GPIO3;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_4:
	case HALMAC_GPIO_FUNC_SDIO_INT:
		*list = PIMUX_LIST_GPIO4_8821C;
		*list_size = ARRAY_SIZE(PIMUX_LIST_GPIO4_8821C);
		*gpio_id = HALMAC_GPIO4;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_5:
		*list = PIMUX_LIST_GPIO5_8821C;
		*list_size = ARRAY_SIZE(PIMUX_LIST_GPIO5_8821C);
		*gpio_id = HALMAC_GPIO5;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_6:
		*list = PIMUX_LIST_GPIO6_8821C;
		*list_size = ARRAY_SIZE(PIMUX_LIST_GPIO6_8821C);
		*gpio_id = HALMAC_GPIO6;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_7:
		*list = PIMUX_LIST_GPIO7_8821C;
		*list_size = ARRAY_SIZE(PIMUX_LIST_GPIO7_8821C);
		*gpio_id = HALMAC_GPIO7;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_8:
	case HALMAC_GPIO_FUNC_WL_LED:
		*list = PIMUX_LIST_GPIO8_8821C;
		*list_size = ARRAY_SIZE(PIMUX_LIST_GPIO8_8821C);
		*gpio_id = HALMAC_GPIO8;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_9:
		*list = PIMUX_LIST_GPIO9_8821C;
		*list_size = ARRAY_SIZE(PIMUX_LIST_GPIO9_8821C);
		*gpio_id = HALMAC_GPIO9;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_10:
		*list = PIMUX_LIST_GPIO10_8821C;
		*list_size = ARRAY_SIZE(PIMUX_LIST_GPIO10_8821C);
		*gpio_id = HALMAC_GPIO10;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_11:
		*list = PIMUX_LIST_GPIO11_8821C;
		*list_size = ARRAY_SIZE(PIMUX_LIST_GPIO11_8821C);
		*gpio_id = HALMAC_GPIO11;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_12:
		*list = PIMUX_LIST_GPIO12_8821C;
		*list_size = ARRAY_SIZE(PIMUX_LIST_GPIO12_8821C);
		*gpio_id = HALMAC_GPIO12;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_13:
		*list = PIMUX_LIST_GPIO13_8821C;
		*list_size = ARRAY_SIZE(PIMUX_LIST_GPIO13_8821C);
		*gpio_id = HALMAC_GPIO13;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_14:
		*list = PIMUX_LIST_GPIO14_8821C;
		*list_size = ARRAY_SIZE(PIMUX_LIST_GPIO14_8821C);
		*gpio_id = HALMAC_GPIO14;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_15:
		*list = PIMUX_LIST_GPIO15_8821C;
		*list_size = ARRAY_SIZE(PIMUX_LIST_GPIO15_8821C);
		*gpio_id = HALMAC_GPIO15;
		break;
	default:
		return HALMAC_RET_SWITCH_CASE_ERROR;
	}

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
halmac_chk_pinmux_valid_8821c(
	IN struct halmac_adapter *adapter,
	IN enum halmac_gpio_func gpio_func
)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	switch (gpio_func) {
	case HALMAC_GPIO_FUNC_SW_IO_0:
		if (adapter->pinmux_info.sw_io_0 == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_1:
		if (adapter->pinmux_info.sw_io_1 == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_2:
		if (adapter->pinmux_info.sw_io_2 == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_3:
		if (adapter->pinmux_info.sw_io_3 == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_4:
	case HALMAC_GPIO_FUNC_SDIO_INT:
		if ((adapter->pinmux_info.sw_io_4 == 1) ||
		    (adapter->pinmux_info.sdio_int == 1))
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_5:
		if (adapter->pinmux_info.sw_io_5 == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_6:
		if (adapter->pinmux_info.sw_io_6 == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_7:
		if (adapter->pinmux_info.sw_io_7 == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_8:
	case HALMAC_GPIO_FUNC_WL_LED:
		if ((adapter->pinmux_info.sw_io_8 == 1) ||
		    (adapter->pinmux_info.wl_led == 1))
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_9:
		if (adapter->pinmux_info.sw_io_9 == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_10:
		if (adapter->pinmux_info.sw_io_10 == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_11:
		if (adapter->pinmux_info.sw_io_11 == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_12:
		if (adapter->pinmux_info.sw_io_12 == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_13:
		if (adapter->pinmux_info.sw_io_13 == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_14:
		if (adapter->pinmux_info.sw_io_14 == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_15:
		if (adapter->pinmux_info.sw_io_15 == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	default:
		return HALMAC_RET_SWITCH_CASE_ERROR;
	}

	return status;
}
#endif /* HALMAC_8821C_SUPPORT */

