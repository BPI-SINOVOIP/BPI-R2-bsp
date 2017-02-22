/*
 * bur_comon.h
 *
 * common board information header for B&R boards
 *
 * Copyright (C) 2013 Hannes Petermaier <oe5hpm@oevsv.at>
 * Bernecker & Rainer Industrieelektronik GmbH - http://www.br-automation.com
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _BUR_COMMON_H_
#define _BUR_COMMON_H_

void blink(u32 blinks, u32 intervall, u32 pin);
void pmicsetup(u32 mpupll);
void enable_uart0_pin_mux(void);
void enable_i2c0_pin_mux(void);
void enable_board_pin_mux(void);
int board_eth_init(bd_t *bis);

#endif
