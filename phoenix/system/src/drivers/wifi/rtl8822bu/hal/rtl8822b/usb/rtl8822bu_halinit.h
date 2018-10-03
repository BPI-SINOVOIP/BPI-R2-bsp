/******************************************************************************
 *
 * Copyright(c) 2015 Realtek Corporation. All rights reserved.
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
#ifndef _RTL8822BU_HALINIT_H_
#define _RTL8822BU_HALINIT_H_

#include <drv_types.h>		/* PADAPTER and etc. */

u32 rtl8822bu_hal_init(PADAPTER padapter);
u32 rtl8822bu_hal_deinit(PADAPTER padapter);
u32 rtl8822bu_inirp_init(PADAPTER padapter);
u32 rtl8822bu_inirp_deinit(PADAPTER padapter);
void rtl8822bu_interface_configure(PADAPTER padapter);

#endif /* _RTL8822BU_HALINIT_H_ */
