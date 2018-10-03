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

#ifndef _HALMAC_GPIO_88XX_H_
#define _HALMAC_GPIO_88XX_H_

#include "../halmac_api.h"
#include "../halmac_gpio_cmd.h"

#if HALMAC_88XX_SUPPORT

enum halmac_ret_status
halmac_pinmux_wl_led_mode_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_wlled_mode mode
);

void
halmac_pinmux_wl_led_sw_ctrl_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 on
);

void
halmac_pinmux_sdio_int_polarity_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 low_active
);

enum halmac_ret_status
halmac_pinmux_gpio_mode_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 gpio_id,
	IN u8 output
);

enum halmac_ret_status
halmac_pinmux_gpio_output_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 gpio_id,
	IN u8 high
);

enum halmac_ret_status
halmac_pinmux_pin_status_88xx(
	IN struct halmac_adapter *adapter,
	IN u8 pin_id,
	IN u8 *high
);

enum halmac_ret_status
halmac_pinmux_parser_88xx(
	IN struct halmac_adapter *adapter,
	IN const struct halmac_gpio_pimux_list *list,
	IN u32 size,
	IN u32 gpio_id,
	OUT u32 *cur_func
);

enum halmac_ret_status
halmac_pinmux_switch_88xx(
	IN struct halmac_adapter *adapter,
	IN const struct halmac_gpio_pimux_list *list,
	IN u32 size,
	IN u32 gpio_id,
	IN enum halmac_gpio_func gpio_func
);

enum halmac_ret_status
halmac_pinmux_record_88xx(
	IN struct halmac_adapter *adapter,
	IN enum halmac_gpio_func gpio_func,
	IN u8 val
);

#endif /* HALMAC_88XX_SUPPORT */

#endif/* _HALMAC_GPIO_88XX_H_ */
