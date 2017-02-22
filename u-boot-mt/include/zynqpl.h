/*
 * (C) Copyright 2012-2013, Xilinx, Michal Simek
 *
 * (C) Copyright 2012
 * Joe Hershberger <joe.hershberger@ni.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _ZYNQPL_H_
#define _ZYNQPL_H_

#include <xilinx.h>

extern int zynq_load(Xilinx_desc *desc, const void *image, size_t size);
extern int zynq_dump(Xilinx_desc *desc, const void *buf, size_t bsize);
extern int zynq_info(Xilinx_desc *desc);

#define XILINX_ZYNQ_7010	0x2
#define XILINX_ZYNQ_7015	0x1b
#define XILINX_ZYNQ_7020	0x7
#define XILINX_ZYNQ_7030	0xc
#define XILINX_ZYNQ_7045	0x11
#define XILINX_ZYNQ_7100	0x16

/* Device Image Sizes */
#define XILINX_XC7Z010_SIZE	16669920/8
#define XILINX_XC7Z015_SIZE	28085344/8
#define XILINX_XC7Z020_SIZE	32364512/8
#define XILINX_XC7Z030_SIZE	47839328/8
#define XILINX_XC7Z045_SIZE	106571232/8
#define XILINX_XC7Z100_SIZE	139330784/8

/* Descriptor Macros */
#define XILINX_XC7Z010_DESC(cookie) \
{ xilinx_zynq, devcfg, XILINX_XC7Z010_SIZE, NULL, cookie, "7z010" }

#define XILINX_XC7Z015_DESC(cookie) \
{ xilinx_zynq, devcfg, XILINX_XC7Z015_SIZE, NULL, cookie, "7z015" }

#define XILINX_XC7Z020_DESC(cookie) \
{ xilinx_zynq, devcfg, XILINX_XC7Z020_SIZE, NULL, cookie, "7z020" }

#define XILINX_XC7Z030_DESC(cookie) \
{ xilinx_zynq, devcfg, XILINX_XC7Z030_SIZE, NULL, cookie, "7z030" }

#define XILINX_XC7Z045_DESC(cookie) \
{ xilinx_zynq, devcfg, XILINX_XC7Z045_SIZE, NULL, cookie, "7z045" }

#define XILINX_XC7Z100_DESC(cookie) \
{ xilinx_zynq, devcfg, XILINX_XC7Z100_SIZE, NULL, cookie, "7z100" }

#endif /* _ZYNQPL_H_ */
