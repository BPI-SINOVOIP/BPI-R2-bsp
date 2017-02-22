/*
 *  Copyright (C) 2012 Samsung Electronics
 *  Lukasz Majewski <l.majewski@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __G_DOWNLOAD_H_
#define __G_DOWNLOAD_H_

#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
int g_dnl_bind_fixup(struct usb_device_descriptor *, const char *);
int g_dnl_register(const char *s);
void g_dnl_unregister(void);
void g_dnl_set_serialnumber(char *);

#endif /* __G_DOWNLOAD_H_ */
