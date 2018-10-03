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
#define _RTL8192EE_RECV_C_

/* #include <drv_types.h> */
#include <rtl8192e_hal.h>


s32 rtl8192ee_init_recv_priv(_adapter *padapter)
{
	struct recv_priv	*precvpriv = &padapter->recvpriv;
	s32	ret = _SUCCESS;


#ifdef PLATFORM_LINUX
	tasklet_init(&precvpriv->recv_tasklet,
		     (void(*)(unsigned long))rtl8192ee_recv_tasklet,
		     (unsigned long)padapter);

	tasklet_init(&precvpriv->irq_prepare_beacon_tasklet,
		     (void(*)(unsigned long))rtl8192ee_prepare_bcn_tasklet,
		     (unsigned long)padapter);
#endif


	return ret;
}

void rtl8192ee_free_recv_priv(_adapter *padapter)
{
	/* struct recv_priv	*precvpriv = &padapter->recvpriv; */


}
